#include "plrsModuleInput.h"


pthread_mutex_t mux_module_input = PTHREAD_MUTEX_INITIALIZER;


// InputHandler will probe for user input through a while loop.
void* InputHandler( void* c){
    string* str = (string*) c;  // str should be the member data of IO module.
    string temp_input = "";

    while(1){   // begin input acquisition loop
        if( *str=="" ){    // ready for new command
        
            cin >> temp_input;

            if( cin.good() ){
                pthread_mutex_lock( &mux_module_input );
                    *str = temp_input;
                pthread_mutex_unlock( &mux_module_input );
            }
            else{
                cin.ignore( 1024, '\n');
                cin.clear();
            }
        }
    }
    return 0;
}


plrsModuleInput::plrsModuleInput( plrsController* c) : plrsStateMachine(c){
    input = "";
    pthread_create( &thread_input, 0, InputHandler, (void*)(&input) );
}


plrsModuleInput::~plrsModuleInput(){
    pthread_cancel( thread_input );
}


void plrsModuleInput::Initialize(){

    Print( "initializing\n", DETAIL);

    if( GetState()!=ERROR )
        SetStatus( INIT );
    else{
        SetStatus( ERROR );
        return;
    }
    while( GetState()==INIT ){
        IOHandler();
        sched_yield();
    }
}


void plrsModuleInput::CleanUp(){
    Print( "cleaning up...\n", DETAIL);
}


void plrsModuleInput::Configure(){
    if( GetState()!=ERROR )
        SetStatus( CONFIG );
    else{
        SetStatus( ERROR );
        return;
    }
    Print( "configuring...\n", DETAIL);

    while( GetState()==CONFIG ){
        IOHandler();
        sched_yield();
    }
}


void plrsModuleInput::ConfigDataFlow(){;}


void plrsModuleInput::UnConfigure(){
    Print( "unconfiguring\n", DETAIL);
}


void plrsModuleInput::PreRun(){
    Print( "runing...\n", DETAIL);
}


void plrsModuleInput::Run(){
    while( GetState()==RUN ){
        IOHandler();
        sched_yield();
    }
}


void plrsModuleInput::PostRun(){
    Print( "ending run...\n", DETAIL);
}


void plrsModuleInput::IOHandler(){
    pthread_mutex_lock( &mux_module_input);
    if( input!="" ){
        Print( "typed "+input+"\n", INFO);
        SendUserCommand( input );
        input = "";
    }
    pthread_mutex_unlock( &mux_module_input);
}

void plrsModuleInput::SendUserCommand( string in){

    if( in=="quit" || in=="q" || in=="print" )
        PushCommand( 0, in);

    else{
        for( unsigned int i=0; i<module_table.size(); ++i){
            PushCommand( i, in);
            //Print("Pushing command "+in+" to "+ctrl->GetNameByID(i)+"\n", DEBUG);
        }
    }

}
