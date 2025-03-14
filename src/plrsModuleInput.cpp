#include "plrsModuleInput.h"


#include <unistd.h>



plrsModuleInput::plrsModuleInput( plrsController* c) : plrsStateMachine(c){
}



plrsModuleInput::~plrsModuleInput(){
}



void plrsModuleInput::Initialize(){

    Print( "Initializing\n", DETAIL);

    if( GetState()!=ERROR )
        SetStatus( INIT );
    else{
        SetStatus( ERROR );
        return;
    }
    while( GetState()==INIT ){
        IOHandler();
        usleep(500*1000);
    }
}



void plrsModuleInput::Deinitialize(){
    Print( "Deinitializing...\n", DETAIL);
}



void plrsModuleInput::Configure(){
    if( GetState()!=ERROR )
        SetStatus( CONFIG );
    else{
        SetStatus( ERROR );
        return;
    }
    Print( "Configuring...\n", DETAIL);

    while( GetState()==CONFIG ){
        IOHandler();
        usleep(500*1000);
    }
}


void plrsModuleInput::Deconfigure(){
    Print( "Deconfiguring\n", DETAIL);
}


void plrsModuleInput::PreRun(){
    Print( "Running...\n", DETAIL);
}



void plrsModuleInput::Run(){
    IOHandler();
    usleep(200*1000);
}


void plrsModuleInput::Idle(){
    IOHandler();
    usleep(200*1000);
}



void plrsModuleInput::PostRun(){
    Print( "Ending run...\n", DETAIL);
}



void plrsModuleInput::IOHandler(){
    string input = getstr();
    if( input!="" ){
        Print( "User typed "+input+"\n", DETAIL);
        SendUserCommand( input );
        input = "";
    }
}





