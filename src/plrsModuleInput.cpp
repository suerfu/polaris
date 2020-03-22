#include "plrsModuleInput.h"


#include <unistd.h>



plrsModuleInput::plrsModuleInput( plrsController* c) : plrsStateMachine(c){
}



plrsModuleInput::~plrsModuleInput(){
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
        usleep(500*1000);
    }
}



void plrsModuleInput::Deinitialize(){
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
        usleep(500*1000);
    }
}



void plrsModuleInput::ConfigDataFlow(){;}



void plrsModuleInput::Deconfigure(){
    Print( "unconfiguring\n", DETAIL);
}



void plrsModuleInput::PreRun(){
    Print( "runing...\n", DETAIL);
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
    Print( "ending run...\n", DETAIL);
}



void plrsModuleInput::IOHandler(){
    string input = getstr();
    if( input!="" ){
        Print( "typed "+input+"\n", DETAIL);
        SendUserCommand( input );
        input = "";
    }
}





