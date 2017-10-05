#include "plrsController.h"
#include "plrsModuleInput.h"

#include <cstdlib>
#include <unistd.h>
#include <dlfcn.h>
#include <iostream>

#include <map>
#include <string>

using namespace std;


plrsController::plrsController( ConfigParser* m) : cparser(m){

    Print( "Constructing polaris controller\n", DEBUG);

    bool found = false;

    state = NUL;    // initial default state

    pthread_mutex_init( &mux_state, 0);
    pthread_mutex_init( &mux_fsm, 0);
    pthread_mutex_init( &mux_buffer, 0);
    pthread_mutex_init( &mux_cmd, 0);
    pthread_mutex_init( &mux_time, 0);
    pthread_mutex_init( &mux_flag, 0);

    Module ctrl;
    //ctrl.buffer = circular_buffer< void* >();
    //ctrl.buffer_cmd = circular_buffer<string>();
    ctrl.fsm = 0;
    ctrl.thread = 0;
    module.push_back( ctrl);
    module_table[ "ctrl" ] = 0;

    if( !cparser->GetBool("/cmdl/disable-input", false) ){
        InsModule( new plrsModuleInput( this ) );
    }

    start_time = 0;

    max_run_time = cparser->GetInt("/cmdl/time", &found);    // obtains daq running time.
    if( found){
        Print( "Maximum run-time is " + cparser->GetString("/cmdl/time") + " seconds\n", DEBUG);
    }
    else{
        Print( "Maximum run-time is 1 week\n", DEBUG);
        max_run_time = 604800;
    }

    stop_flag = false;
}

plrsController::~plrsController(){

    if( GetState()!=ERROR )
        while( !ChangeState(END) ); // wait for state to update.

    for( int i=module.size()-1; i>0; --i){
        if( module[i].fsm != 0 ){
            RmModule(i);
        }
    }
    Print( "Good bye!\n", INFO);
}



void plrsController::StateLoop(){

    Print( "Loading modules...\n", INFO);
    LoadModules();
    Print( "Modules loaded\n\n", DETAIL);

    DAQSTATE nxt;

    while( 1 ){

        if( GetState()==END )
            break;
        else if( GetState()==ERROR ){
            ChangeState( ERROR );
            break;
        }
        
        nxt = GetNextState();

        switch( GetState() ){

            case NUL :
                if( nxt==NUL )
                    break;

                else if( nxt==INIT ){
                    Print( "Initializing modules...\n", INFO);
                    if ( !ChangeState( INIT ) ){
                        Print( "Error initializing modules\n\n", ERR);
                        SetState( ERROR );
                    }
                    else{
                        Print( "Modules initialized\n\n", INFO);
                    }
                }
                else{
                    SetState( ERROR );
                    Print( "Invalid state transition from NULL to "+GetStateName( nxt )+"\n\n", ERR);
                }
                break;

            case INIT :
                if( nxt==INIT )
                    break;

                else if( nxt==CONFIG ){
                    Print( "Configuring modules...\n", INFO);

                    if( !ChangeState( CONFIG ) ){
                        Print( "Error configuring modules\n\n", ERR);
                        SetState( ERROR );
                    }
                    else{
                        Print( "Modules configured\n\n", INFO);
                    }
                }
                else if( nxt==END ){
                    Print( "Cleaning up modules...\n", INFO);
                    if( !ChangeState( END ) ){
                        Print( "Error cleaning up modules\n\n", ERR);
                        SetState( ERROR );
                    }
                    else{
                        Print( "Modules cleaned up\n\n", INFO);
                    }
                }
                else{
                    SetState( ERROR );
                    Print( "Invalid state transition from INIT to "+GetStateName( nxt )+"\n\n", ERR);
                }
                break;

            case CONFIG :
                if( nxt==CONFIG )
                    break;

                else if( nxt==RUN ){
                    start_time = GetTimeStamp();
                    Print( "Run starting ...\n", INFO);
                    if( !ChangeState( RUN ) ){
                        Print( "Error starting run\n\n", ERR);
                        SetState( ERROR );
                    }
                    else{
                        Print( "Run started\n\n", INFO);
                    }
                }
                else if( nxt==INIT ){
                    Print( "Unconfiguring modules...\n", INFO);
                    if( !ChangeState( INIT ) ){
                        Print( "Error unconfiguring modules\n\n", ERR);
                        SetState( ERROR );
                    }
                    else{
                        Print( "Modules unconfigured\n\n", INFO);
                    }
                }
                else{
                    Print( "Invalid state transition from CONFIG to "+GetStateName( nxt )+"\n\n", ERR);
                    SetState( ERROR );
                }
                break;

            case RUN :
                if( GetNextState()==RUN )
                    break;

                else if( GetNextState()==CONFIG ){
                    Print( "Stopping run...\n", INFO);
                    if( !ChangeState( CONFIG ) ){
                        Print( "Error stopping run\n\n", ERR);
                        SetState( ERROR );
                    }
                    else{
                        Print( "Run stopped\n\n", INFO);
                    }
                }
                else{
                    Print( "Invalid state transition from RUN to "+GetStateName( nxt )+"\n\n", ERR);
                    SetState( ERROR );
                }
                break;

            case END :
                if( !ChangeState( END )){
                    Print( "Error cleaning up\n\n", ERR);
                    SetState( ERROR );
                }
                break;

            case ERROR:
                ChangeState( ERROR );
                break;

            default:
                break;
        }
        CommandHandler();
        sched_yield();
    }
}



unsigned int plrsController::GetTimeStamp(){
    unsigned int t;
    pthread_mutex_lock( &mux_time);
        t = time(0);    // mark Better to use a robust and system-independent way.
    pthread_mutex_unlock( &mux_time);
    return t;
}



ConfigParser* plrsController::GetConfigParser(){
    return cparser;
}



void plrsController::LoadModules(){

    Print( "Searching config file for list of modules...\n", DEBUG);
    std::map< string, vector<string> > mdl = cparser->GetListOfParameters("/module/");
        // mdl contains module names such as /module/daq/, /module/recorder/

    std::map< string, vector<string> >::iterator itr;
    bool found, enabled;
    for( itr=mdl.begin(); itr!=mdl.end(); itr++){
        found = enabled = false;

        enabled = cparser->GetBool( itr->first+"enable", &found);
        if( found && enabled ){

            string libname = cparser->GetString( itr->first+"lib");
            if( libname=="" ){
                Print( "Error: "+cparser->GetString( itr->first)+" no library name is specified", ERR);
                ChangeState( ERROR );
                return;
            }

            string fcnname = cparser->GetString( itr->first+"fcn");
            if( fcnname=="" ){
                Print( "Error: no function name in library " + libname + "\n", ERR);
                ChangeState( ERROR );
                return;
            }

            InsModule( libname, fcnname);
        }
    }

    if( GetState()==ERROR ) return;

    string to_print = "Following modules are enabled :\n";
    string sp = "           ";
    for( unsigned int i=0; i<module.size(); ++i){
        if( module[i].fsm!=0 ){
            //s.push_back( module[i].fsm->GetModuleName() );
            to_print += sp + " |-"+module[i].fsm->GetModuleName()+"\n";
        }
    }
	to_print += "\n";
    Print( to_print, INFO);

    // maybe add essential modules.
}


// Insert module from library.

void plrsController::InsModule( const string& lib, const string& fcn ){

    Module mod;
    char *error;

    // check whether a library has been opened or not.
    // if opened, get its handle; if not, open and insert the handle.
    //
    mod.handle = LibraryOpen( lib );
    if(  mod.handle==0 ){
        mod.handle = dlopen( lib.c_str(), RTLD_NOW | RTLD_GLOBAL);
        Print( "Opening library "+lib+"...\n", DETAIL);
        if ( !mod.handle ) {
            Print( "Error: cannot open library "+lib+" - "+string(dlerror())+"\n", ERR);
            ChangeState(ERROR);
            return;
        }
        dlerror();
    }

    typedef plrsStateMachine* (*creator) (plrsController*);
    typedef void (*destroyer) (plrsStateMachine*);

    mod.helper_creator = (creator) dlsym( mod.handle, ("create_"+fcn).c_str());
    if ((error = dlerror()) != 0)  {
        Print( "cannot find creator function create_"+fcn+'\t'+string(error)+"\n", ERR);
        ChangeState(ERROR);
        return;
    }

    mod.helper_destroyer = (destroyer) dlsym( mod.handle, ("destroy_"+fcn).c_str());
    if ( (error=dlerror()) != 0 )  {
        Print( "cannot find destroyer function destroy_"+fcn+'\t'+string(error)+"\n", ERR);
        ChangeState(ERROR);
        return;
    }

    // initialize aspects of the module.

    pthread_mutex_lock( &mux_buffer);
    pthread_mutex_lock( &mux_cmd);
    pthread_mutex_lock( &mux_fsm);

        mod.libname = lib;
        mod.fcnname = fcn;

        mod.buffer = circular_buffer< void* >();
        mod.buffer_cmd = circular_buffer< string >();

        plrsStateMachine* new_fsm = mod.helper_creator( this );
        mod.fsm = new_fsm;

        pthread_t* nthrd = new pthread_t;
        mod.thread = nthrd;

        module.push_back( mod );

        int id = module.size()-1;
        new_fsm->SetID( id );
        module_table[ new_fsm->GetModuleName()] = id;

        pthread_create( nthrd, 0, LaunchStateMachine, new_fsm );

    pthread_mutex_unlock( &mux_fsm);
    pthread_mutex_unlock( &mux_cmd);
    pthread_mutex_unlock( &mux_buffer);

    Print( "Module "+new_fsm->GetModuleName()+" inserted\n", DETAIL);
}



void plrsController::InsModule( plrsStateMachine* p ){
    Module mod;

    mod.handle = 0;
    mod.libname = mod.fcnname = "";
    mod.helper_creator = 0;
    mod.helper_destroyer = 0;
        // since pointer already acquired, add dummy variable.

    pthread_mutex_lock( &mux_buffer);
    pthread_mutex_lock( &mux_cmd);
    pthread_mutex_lock( &mux_fsm);

        mod.fsm = p;

        mod.buffer = circular_buffer< void* >();
        mod.buffer_cmd = circular_buffer<string>();

        pthread_t* new_thread = new pthread_t;
        mod.thread = new_thread;
        module.push_back( mod);

        int id = module.size()-1;
        p->SetID( id );
        module_table[ p->GetModuleName()] = id;

        pthread_create( new_thread, 0, LaunchStateMachine, p);

    pthread_mutex_unlock( &mux_fsm);
    pthread_mutex_unlock( &mux_cmd);
    pthread_mutex_unlock( &mux_buffer);
}




void plrsController::RmModule( unsigned int ID){
 
    string libname = "";
    void* handle_to_lib = 0;

    pthread_mutex_lock( &mux_fsm);

        if( ID<module.size() ){

            if( module[ID].fsm != 0 ){
    
                Print("Terminating thread " + module[ID].fsm->GetModuleName() + "...\n", DETAIL);
                pthread_join( *module[ID].thread, 0);

                libname = module[ID].libname;
                handle_to_lib = module[ID].handle;

                if( module[ID].handle != 0 ){
                    module[ID].helper_destroyer( module[ID].fsm );

                    module[ID].handle = 0;
                    module[ID].libname = module[ID].fcnname = "";
                    module[ID].helper_creator = 0;
                    module[ID].helper_destroyer = 0;
                    module[ID].fsm = 0;

                }
                else{
                    delete module[ID].fsm;
                    module[ID].fsm = 0;
                }
            }
        }
    pthread_mutex_unlock( &mux_fsm);

    module_table.erase( GetNameByID(ID) );

    if( handle_to_lib!=0 && libname!="" ){
        if( LibraryOpen( libname )==0 ){
            dlclose( handle_to_lib );
            Print( "Closing library " + libname + "...\n", INFO);
        }
    }

}



void* LaunchStateMachine( void* p){

    plrsStateMachine* ptr = (plrsStateMachine*)(p);
    ptr->EventLoop();

    return 0;
}
// as soon as new thread is created, this function is called.



int plrsController::GetIDByName( const string& s){

    int id = 0;

    pthread_mutex_lock( &mux_fsm);
        map< string, int>::iterator itr = module_table.find( s );
        if( itr==module_table.end() ){
            id = -1;
            Print( "Failed to find module "+s+"\n", ERR);
        }
        else{
            id = module_table[s];
        }
    pthread_mutex_unlock( &mux_fsm);

    return id;
}



string plrsController::GetNameByID( const int a){
    string s;
    pthread_mutex_lock( &mux_fsm);
        map< string, int>::iterator itr;
        for( itr = module_table.begin(); itr!=module_table.end(); ++itr){
            if( itr->second==a)
                s = itr->first;
        }
    pthread_mutex_unlock( &mux_fsm);
    return s;
}

bool plrsController::ChangeState( DAQSTATE s, unsigned int wait_time){

    if( GetState()==ERROR )
        s = ERROR;

    stringstream strm;
    strm << "Controller changing state to " << GetStateName(s) << "...\n";
    Print( strm.str(), DEBUG);

    // ask all modules to change state
    pthread_mutex_lock( &mux_fsm);
    	for( unsigned int i=0; i<module.size(); ++i ){
            if( module[i].fsm!=0 )
	    		module[i].fsm->SetState( s );
        }
    pthread_mutex_unlock( &mux_fsm);

    unsigned int time = GetTimeStamp();
    bool consistent = true;
    bool error = false;

    while( GetTimeStamp()-time<wait_time ){
        consistent = CheckStateConsistency();
        if( consistent ){
            SetState( s );
            PrintState( DETAIL);
            Print( "Controller state successfully changed\n", DEBUG);
            return true;
        }

        error = CheckErrorFlag();
        if( error ){
            PrintState( ERR );
            return false;
        }

        CommandHandler();
        sched_yield();
    }

    Print( "Time out error\n", ERR);
    PrintState();

    return false;
}



void plrsController::PrintState( VERBOSITY v ){

    stringstream ss;

    int len = 4;    // length of longest module name

    pthread_mutex_lock( &mux_fsm);
        for( unsigned int i=0; i<module.size(); i++)
            if( module[i].fsm!=0 ){
                int l = int(module[i].fsm->GetModuleName().length());
                len = len > l ? len : l;
            }
    pthread_mutex_unlock( &mux_fsm);

    pthread_mutex_lock( &mux_fsm);
        for( unsigned int i=0; i<module.size(); i++){
            if( module[i].fsm==0 )
                ss << "\n\t\t| ctrl" << string(len-4, ' ') << " | ------ | " << GetStateName( GetState() ) << " |\n";
            else{
                int l = module[i].fsm->GetModuleName().length();
                ss << "\t\t| " << module[i].fsm->GetModuleName() << string(len-l, ' ') << " | " << GetStateName( module[i].fsm->GetStatus());
                ss << " | " << GetStateName( module[i].fsm->GetState()) << " |\n";
            }
        }
    pthread_mutex_unlock( &mux_fsm);

    ss << "\n";
    Print( ss.str(), v);
}



// The flow of state machine is controlled here.
DAQSTATE plrsController::GetNextState( ){

    if( CheckErrorFlag() )
        return ERROR;

    switch( GetState() ){
        case NUL :
            return INIT;

        case INIT :
            if( CheckStopFlag() )
                return END;
            else
                return CONFIG;

        case CONFIG :
            if( CheckStopFlag() )
                return  INIT;
            else
                return  RUN;

        case RUN :
            if( CheckStopFlag() )
                return  CONFIG;
            else
                return  RUN;

        case END :
            return  END;

        case ERROR :
            return  ERROR;

             //irrecoverable from error
        default :
            Print( "Controller unrecognized state.\n", ERR);
            break;
    }

    return GetState();
}


void plrsController::SetState( DAQSTATE st){

    if( GetState()==ERROR )
        return;

    pthread_mutex_lock( &mux_state);
        state = st;
    pthread_mutex_unlock( &mux_state);
}


DAQSTATE plrsController::GetState(){

    DAQSTATE s;

    pthread_mutex_lock( &mux_state);
        s = state;
    pthread_mutex_unlock( &mux_state);

    return s;
}



bool plrsController::CheckStateConsistency(){

    bool consistent = true;

    pthread_mutex_lock( &mux_fsm);
    for( unsigned int i=0; i<module.size(); i++){
        if( module[i].fsm!=0 )
            if( module[i].fsm->GetState()!=module[i].fsm->GetStatus() ){
                consistent = false;
                break;
            }
    }
    pthread_mutex_unlock( &mux_fsm);

    return consistent;

}



bool plrsController::CheckErrorFlag(){

    bool error = false;

    if( GetState()==ERROR )
        return true;

    pthread_mutex_lock( &mux_fsm);
    for( unsigned int i=0; i<module.size(); i++){
        if( module[i].fsm!=0 )
            if( module[i].fsm->GetStatus()==ERROR ){
                pthread_mutex_lock( &mux_flag);
                    error = true;
                pthread_mutex_unlock( &mux_flag);
                break;
            }
    }
    pthread_mutex_unlock( &mux_fsm);

    return error;
}



bool plrsController::CheckStopFlag(){

    bool stop = false;

    // if stop flag has been set before, return it.
    //
    pthread_mutex_lock( &mux_flag);
        if( stop_flag )
            stop = true;
    pthread_mutex_unlock( &mux_flag);

    if( stop ){
        return true;
    }

    // check existing error
    //
    pthread_mutex_lock( &mux_fsm);
    for( unsigned int i=0; i<module.size(); i++){
        if( module[i].fsm!=0 )
            if( module[i].fsm->GetStatus()==ERROR ){
                pthread_mutex_lock( &mux_flag);
                    stop = stop_flag = true;
                pthread_mutex_unlock( &mux_flag);
                SetState( ERROR );
                break;
            }
    }
    pthread_mutex_unlock( &mux_fsm);

    // time out only possible during run.
    //
    if( GetState()==RUN ){
        if ( GetTimeStamp()-start_time > max_run_time ) {
            Print( "Maximum run time has passed\n", DEBUG);
            pthread_mutex_lock( &mux_flag);
                stop = stop_flag = true;
            pthread_mutex_unlock( &mux_flag);
        }
    }

    return stop;
}



int plrsController::PushToBuffer( unsigned int i, void* data){
    int s = -1;

    pthread_mutex_lock( &mux_buffer);
        if( i<module.size() ){
            module[i].buffer.push_back(data);
            s = module[i].buffer.size();
        }
    pthread_mutex_unlock( &mux_buffer );

    return s;
}



void* plrsController::PullFromBuffer( unsigned int i){
    void* p = 0;

    pthread_mutex_lock( &mux_buffer);
        if( i<module.size() ){
            if( (!module[i].buffer.empty()) ){
        	    p = module[i].buffer.front();
    	        module[i].buffer.pop_front();
            }
        }
    pthread_mutex_unlock( &mux_buffer);

	return p;
}



void plrsController::PushCommand( int i, string c){
    pthread_mutex_lock( &mux_cmd );
        if( i>=0 && i<int( module.size() ) ){
        	module[i].buffer_cmd.push_back( c );
        }
    pthread_mutex_unlock( &mux_cmd);
}



string plrsController::PullCommand( int i){
    string cmd;

    pthread_mutex_lock( &mux_cmd);
    	if( i>=0 && i<int( module.size() ) ){
            if( (!module[i].buffer_cmd.empty()) ){
        	    cmd = module[i].buffer_cmd.front();
    	        module[i].buffer_cmd.pop_front();
            }
            else cmd = "";
        }
        else
            cmd = "";
    pthread_mutex_unlock( &mux_cmd);

	return cmd;
}



void plrsController::CommandHandler(){
    string cmd = PullCommand(0);

    if( cmd=="quit" || cmd=="q" ){
        pthread_mutex_lock( &mux_flag );
            stop_flag = true;
            Print( "Stop signal received from user\n", INFO);
        pthread_mutex_unlock( &mux_flag );
    }

    else if( cmd=="max-evt"){
        pthread_mutex_lock( &mux_flag );
            stop_flag = true;
        pthread_mutex_unlock( &mux_flag );    
    }

    else if( cmd=="print" ){
        PrintState( ERR );
    }
}



map< string, int > plrsController::GetModuleTable(){
    map< string, int > tab;

    pthread_mutex_lock( &mux_fsm);
        tab = module_table;
    pthread_mutex_unlock( &mux_fsm);

    return tab;
}



void* plrsController::LibraryOpen( const string& libname ){

    void* handle_return = 0;

    vector< Module >::iterator itr;
    pthread_mutex_lock( &mux_fsm);
        for( itr = module.begin(); itr!=module.end(); ++itr){
            if( itr->libname==libname ){
                if( itr->handle!=0 ){
                    handle_return = itr->handle;
                    break;
                }
            }
        }
    pthread_mutex_unlock( &mux_fsm);

    return handle_return;

}
