#include "plrsStateMachine.h"

#include <sstream>
#include <unistd.h>

//pthread_mutex_t plrsStateMachine::mutex_fasm = PTHREAD_MUTEX_INITIALIZER;
    // section of codes must be protected with mutex, especially the constructor.



string GetStateName( DAQSTATE state ){

    switch( state ){
        case NUL : return "NULL  ";
        case INIT : return "INIT  ";
        case CONFIG : return "CONFIG";
        case RUN : return "RUN   ";
        case END : return "END   ";
        case ERROR : return "ERROR ";
        default : return "";
    }
}



plrsStateMachine::plrsStateMachine( plrsController* h ) : state(NUL), status(NUL), mutex_fasm(PTHREAD_MUTEX_INITIALIZER), ctrl(h){
    cparser = h->GetConfigParser();
    addr_nxt = -1;
}



plrsStateMachine::~plrsStateMachine(){
}



DAQSTATE plrsStateMachine::GetState(){
    DAQSTATE r;

    pthread_mutex_lock( &mutex_fasm);
        r = state; 
    pthread_mutex_unlock( &mutex_fasm);

    return r;
}



DAQSTATE plrsStateMachine::GetStatus(){
    DAQSTATE r;

    pthread_mutex_lock( &mutex_fasm);
        r = status;
    pthread_mutex_unlock( &mutex_fasm);

    return r;
}



void plrsStateMachine::SetState(DAQSTATE s){

    pthread_mutex_lock( &mutex_fasm);
    	if(state!=ERROR){
	    	state = s;
        }
    pthread_mutex_unlock( &mutex_fasm);
}

void plrsStateMachine::SetStatus(DAQSTATE s){

    pthread_mutex_lock( &mutex_fasm);
    	if( status!=ERROR)
	    	status = s;
    pthread_mutex_unlock( &mutex_fasm);
}
// Set status will succeed unless there is error


DAQSTATE plrsStateMachine::WaitForState( DAQSTATE s, int max_try, int poll_rate){

    if( s==ERROR){
        while( GetState()!=ERROR )
            usleep(1000);
        return ERROR;
    }

    else{
    	for( int i=0; i<max_try; i++){
	    	if( GetState()==s)
                return s;
            sched_yield();
        }
	}

	return ERROR;
}



int plrsStateMachine::PushToBuffer( unsigned int i, void* data ){
    return ctrl->PushToBuffer( i, data);
}



void* plrsStateMachine::PullFromBuffer(){
    return ctrl->PullFromBuffer( ID);
}



void plrsStateMachine::PushCommand( unsigned int i, string c){
    return ctrl->PushCommand( i, c);
}



string plrsStateMachine::PullCommand(){
    return ctrl->PullCommand( ID );
}



void plrsStateMachine::EventLoop(){

    bool ok_to_exit = false;

    bool running = false;
    bool configured = false;
    bool initialized = false;

    while( !ok_to_exit ){

        // state machine evolution: switch condition on current module's status
        // when global state changes, take certain actions and change status as well.
        switch( GetStatus() ){

            case NUL :

                switch( GetState() ){

                    case NUL :
                        break;

                    case INIT :
                        Initialize();
                        initialized = true;

                        if( GetStatus()!=ERROR ){
                            SetStatus( INIT );
                        }
                        else{
                            Print("failed to initialize\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;

                    case ERROR :    // error from other modules.
                        SetStatus( ERROR );
                        ok_to_exit = true;
                        break;

                    default :
                        Print( "invalid state transition from NULL to "+GetStateName(GetState())+"\n", ERR);
                        SetStatus( ERROR );
                        ok_to_exit = true;
                        break;
                }
                break;

            case INIT :

                switch( GetState() ){

                    case INIT :
                        break;

                    case CONFIG :

                        module_table = ctrl->GetModuleTable();
                        ConfigDataFlow(); 
                        Configure();
                        configured = true;

                        if( GetStatus()!=ERROR ){
                            SetStatus(CONFIG);
                        }
                        else{
                            Print( "failed to configure\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;

                    case END : 
                        CleanUp();
                        initialized = false;

                        ok_to_exit = true;
                        SetStatus( END );
                        break;

                    case ERROR : 
                        SetStatus( ERROR );
                        break;

                    default : 
                        Print( "invalid state transition from INIT to "+GetStateName(GetState())+"\n", ERR);
                        SetStatus( ERROR );
                        break;
                }
                break;

            case CONFIG :

                switch( GetState() ){

                    case CONFIG :
                        break;

                    case INIT : 
                        UnConfigure();
                        configured = false;

                        if( GetStatus()!=ERROR){
                            SetStatus( INIT );
                        }
                        else{
                            Print( "failed to unconfigure\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;

                    case RUN :
                        PreRun();
                        if( GetStatus()!=ERROR ){
                            SetStatus( RUN );
                            running = true;
                        }
                        else{
                            Print( "error while starting run\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;

                    case ERROR :
                    default :
                        SetStatus( ERROR );
                        break;
                }
                break;

            case RUN :
                while( GetState()==RUN && GetStatus()!=ERROR ){
                    Run();
                    sched_yield();
                }
                if( GetStatus()==ERROR )
                    break;
                
                running = false;

                switch( GetState() ){

                    case CONFIG :
                        PostRun();
                        if( GetStatus()!=ERROR ){
                            SetStatus( CONFIG );
                        }
                        else{
                            Print( "error finishing the run\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;

                    case ERROR :
                    default :
                        PostRun();
                        UnConfigure();
                        CleanUp();
                        SetStatus( ERROR );
                        break;
                }
                break;

            case END :
                ok_to_exit = true;
                SetStatus( END );
                break;

            case ERROR :    // if module itself is in error state
                ok_to_exit = true;
                WaitForState( ERROR );  // wait for controller to realize
                if( running )
                    PostRun();
                if( configured)
                    UnConfigure();
                if( initialized)
                    CleanUp();
                break;

            default:
                Print( "invalid state transition from "+GetStateName(GetStatus())+" to "+GetStateName(GetState())+"\n", ERR);
                ok_to_exit = true;
                SetStatus( ERROR );
                break;
        }

        sched_yield();
    }

    return;
}



void plrsStateMachine::Initialize(){;}



void plrsStateMachine::ConfigDataFlow(){

    string self = GetModuleName();
    string nxt = "";
    nxt = cparser->GetString( "/module/"+self+"/next_module" );

    // next destination in data flow is explicitly specified.
    if( nxt!="" ){
        addr_nxt = ctrl->GetIDByName( nxt );
        if( addr_nxt>=0 ){
            stringstream ss;
            ss << this->GetModuleName() + " configured to send data to " << ctrl->GetNameByID(addr_nxt) << "\n";
            Print( ss.str(), DETAIL);
            return;
        }
        else{
            Print( "invalid next_module. "+nxt+" not registered.\n", ERR);
        }
    }

    if( addr_nxt<0 || nxt=="" ){
        Print( "setting up data loopback.\n", ERR);
        addr_nxt = ctrl->GetIDByName( this->GetModuleName());
    }
}

void plrsStateMachine::UnConfigure(){;}



void plrsStateMachine::CleanUp(){;}



void plrsStateMachine::GetModuleTable(){
    module_table = ctrl->GetModuleTable();
}


void plrsStateMachine::PreRun(){;}


void plrsStateMachine::PostRun(){;}
