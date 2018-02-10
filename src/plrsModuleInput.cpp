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
        usleep(500*1000);
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
        usleep(200*1000);
    }
}



void plrsModuleInput::PostRun(){
    Print( "ending run...\n", DETAIL);
}



void plrsModuleInput::IOHandler(){
    string input = getstr();
    if( input!="" ){
        Print( "typed "+input+"\n", INFO);
        SendUserCommand( input );
        input = "";
    }
}



void plrsModuleInput::SendUserCommand( string in){

    if( in=="quit" || in=="q" || in=="print" )
        PushCommand( 0, in);

    string dir, par;

    size_t found = in.find( "/" );

    if( found != string::npos ){

        size_t found2 = in.find( "/", found+1);

        if( found2 != string::npos ){
            dir = in.substr( found+1, found2-found-1);
            par = in.substr( found2+1, string::npos);
        }
    }

    if( dir == "" ){
        for( unsigned int i=0; i<module_table.size(); ++i){
            PushCommand( i, in);
        }
    }
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


