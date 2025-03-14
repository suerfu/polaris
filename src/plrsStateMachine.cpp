#include "plrsStateMachine.h"

#include <sstream>
#include <unistd.h>



string GetStateName( DAQSTATE state ){

    switch( state ){
        case NUL : return "NULL  ";
        case INIT : return "INIT  ";
        case CONFIG : return "CONFIG";
        case RUN : return "RUN   ";
        case RUN_PAUSE : return "PAUSE ";
        case END : return "END   ";
        case ERROR : return "ERROR ";
        default : return "UNKNOWN";
    }
}



plrsStateMachine::plrsStateMachine( plrsController* h ) :  module_name("tbd"), state(NUL), status(NUL), ctrl(h) {
    cparser = h->GetConfigParser();
    //addr_prv = -1;
    //addr_nxt = -1;
}



plrsStateMachine::~plrsStateMachine(){}



DAQSTATE plrsStateMachine::GetState(){ return state;}



DAQSTATE plrsStateMachine::GetStatus(){ return status;}



void plrsStateMachine::SetState(DAQSTATE s){ state = s;}



void plrsStateMachine::SetStatus(DAQSTATE s){ status = s;}



DAQSTATE plrsStateMachine::WaitForState( DAQSTATE s, int max_try){
    if( s==ERROR){
        while( GetState()!=ERROR )
            usleep(1000000);
        return ERROR;
    }
    else{
    	for( int i=0; i<max_try; i++){
	    	if( GetState()==s)
                return s;
            usleep(1000000);
        }
	}
	return ERROR;
}



int plrsStateMachine::PushToBuffer( unsigned int i, void* data ){ return ctrl->PushToBuffer( i, data);}



void* plrsStateMachine::PullFromBuffer(){ return ctrl->PullFromBuffer( ID);}



void* plrsStateMachine::PullFromBuffer( DAQSTATE st){
    void* p = 0;
    while( p==0 && GetState()==st ){
        p = PullFromBuffer();
        sched_yield();
    }
    return p;
}



void plrsStateMachine::PushCommand( unsigned int i, string c){
    plrsCommand cmd;
    cmd.cmd = c;
    cmd.from = ID;
    return ctrl->PushCommand( i, cmd);
}



void plrsStateMachine::PushCommand( unsigned int i, plrsCommand cmd){
    cmd.from = ID;
    return ctrl->PushCommand( i, cmd);
}



void plrsStateMachine::SendUserCommand( string in){

    string dir, par;

    size_t found = in.find( "/" );

    if( found != string::npos ){

        size_t found2 = in.find( "/", found+1);

        if( found2 != string::npos ){
            dir = in.substr( found+1, found2-found-1);
            par = in.substr( found2+1, string::npos);
        }
    }

    if( dir == "" )
        PushCommand( 0, in);

    else{
        int id = ctrl->GetIDByName( dir );
        if( id != -1 ){
            PushCommand( id, par);
            stringstream st;
            st << "Sending command " << par << " to module " + dir + " ID " << id << "\n";
            Print( st.str(), ERR);
        }
    }
}



plrsCommand plrsStateMachine::PullCommand(){
    return ctrl->PullCommand( ID );
}



void plrsStateMachine::SendPauseReq(){
    PushCommand( 0, "pause");
}


void plrsStateMachine::ClearPauseReq(){
    PushCommand( 0, "resume");
}


void plrsStateMachine::EventLoop(){

    bool ok_to_exit = false;

    bool running = false;
    bool configured = false;
    bool initialized = false;

    bool invalid_transition = false;

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
                            Print("Failed to initialize\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;
                    case ERROR :    // error from other modules.
                        SetStatus( ERROR );
                        break;
                    default :
                        invalid_transition = true;
                        break;
                }
                break;

            case INIT :

                switch( GetState() ){
                    case INIT :
                        break;
                    case CONFIG :
                        module_table = ctrl->GetModuleTable();
                        //ConfigDataFlow(); 
                        Configure();
                        configured = true;
                        if( GetStatus()!=ERROR ){
                            SetStatus(CONFIG);
                        }
                        else{
                            Print( "Failed to configure\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;
                    case END : 
                        Deinitialize();
                        initialized = false;
                        SetStatus( END );
                        break;
                    case ERROR : 
                        SetStatus( ERROR );
                        break;
                    default : 
                        invalid_transition = true;
                        break;
                }
                break;

            case CONFIG :
                switch( GetState() ){
                    case CONFIG :
                        break;
                    case INIT : 
                        Deconfigure();
                        configured = false;
                        if( GetStatus()!=ERROR){
                            SetStatus( INIT );
                        }
                        else{
                            Print( "Failed to deconfigure\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;
                    case RUN :
                        PreRun();
                        if( GetStatus()!=ERROR ){
                            SetStatus( RUN );
                        }
                        else{
                            Print( "Error while starting run\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;
                    case ERROR :
                        SetStatus( ERROR );
                        break;
                    default :
                        invalid_transition = true;
                        break;
                }
                break;

            case RUN :
                running = true;
                RunLoop();
                Print(GetModuleName()+" run finished\n", INFO);
                running = false;

                if( GetStatus()==ERROR )
                    break;

                switch( GetState() ){
                    case CONFIG :
                        PostRun();
                        if( GetStatus()!=ERROR ){
                            SetStatus( CONFIG );
                        }
                        else{
                            Print( "Error finishing the run\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;
                    case ERROR :
                        SetStatus( ERROR );
                        break;
                    case RUN :
                        break;
                    /*
                    case RUN_PAUSE :
                        SetStatus( RUN_PAUSE );
                        Print( GetModuleName()+" pausing...\n", INFO);
                        break;
                    */
                    default :
                        invalid_transition = true;
                        break;
                }
                break;
            /*
            case RUN_PAUSE :
                if( GetStatus()==ERROR )
                    break;

                switch( GetState() ){
                    case CONFIG :
                        PostRun();
                        if( GetStatus()!=ERROR ){
                            SetStatus( CONFIG );
                        }
                        else{
                            Print( "Error finishing the run\n", ERR);
                            SetStatus( ERROR );
                        }
                        break;
                    case ERROR :
                        SetStatus( ERROR );
                        break;
                    case RUN :
                        Print( GetModuleName()+" resuming...\n", INFO);
                        break;
                    case RUN_PAUSE :
                        break;
                    default :
                        invalid_transition = true;
                        break;
                }
                break;
            */

            case END :
                ok_to_exit = true;
                SetStatus( END );
                break;

            case ERROR :    // if module itself is in error state
                ok_to_exit = true;
                WaitForState( ERROR );  // wait for controller to realize
                if( running )
                    PostRun();
                if( configured )
                    Deconfigure();
                if( initialized )
                    Deinitialize();
                break;

            default:
                invalid_transition = true;
                SetStatus( ERROR );
                break;
        }

        if( invalid_transition ){
            Print( "Invalid state transition from "+GetStateName(GetStatus())+" to "+GetStateName(GetState())+"\n", ERR);
            invalid_transition = false;
        }
        usleep(100000);
    }

    return;
}



// Currently this method is not used.
// The data flow should be established manually.
/*
void plrsStateMachine::ConfigDataFlow(){

    string self = GetModuleName();

    string nxt = "";
    nxt = cparser->GetString( "/module/"+self+"/next_module" );
    if( nxt=="" )
        nxt = cparser->GetString( "/module/"+self+"/next" );

    string prv = "";
    prv = cparser->GetString( "/module/"+self+"/prev_module" );
    if( prv=="" )
        prv = cparser->GetString( "/module/"+self+"/prev" );

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
            Print( "Invalid next_module. "+nxt+" not registered.\n", ERR);
        }
    }

    if( addr_nxt<0 || nxt=="" ){
        Print( "Setting up data loopback.\n", ERR);
        addr_nxt = ctrl->GetIDByName( this->GetModuleName());
    }
}
*/


void plrsStateMachine::GetModuleTable(){
    module_table = ctrl->GetModuleTable();
}



void plrsStateMachine::RunLoop(){

    while( ( GetState()==RUN || GetState()==RUN_PAUSE ) && GetStatus()!=ERROR ){

        if( GetState()==RUN ){
            if( GetStatus()==RUN_PAUSE ){
                Resume();
                SetStatus( RUN );
            }

            Run();

        }
        else{
            if( GetStatus()==RUN ){
                Pause();
                SetStatus( RUN_PAUSE );
            }

            Idle();
        }

        CommandHandler();
        sched_yield();
    }
}



void plrsStateMachine::Idle(){
    usleep(100000);
}

