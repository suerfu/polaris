#include "plrsModuleGraphics.h"

#include <unistd.h>


plrsModuleGraphics::plrsModuleGraphics( plrsController* c ) : plrsStateMachine( c ){

    now = 0;
    last_update = 0;
}



plrsModuleGraphics::~plrsModuleGraphics(){}



void plrsModuleGraphics::Configure(){
    refresh_rate = cparser->GetInt("refresh_ms", 1000);
    refresh_rate *= 1000;
}


void plrsModuleGraphics::Clear(){}



void plrsModuleGraphics::Run(){

    void* rdo = PullFromBuffer();

    if( rdo==0 ){
        usleep(10000);
        return;
    }

    Process( rdo );

    now = ctrl->GetMSTimeStamp();
    if( now-last_update > refresh_rate ){
        Clear();
        Draw( rdo );
        last_update = now;
    }

    PushToBuffer( addr_nxt, rdo);
}



/*
void plrsModuleGraphics::CommandHandler(){

    plrsCommand cmd = PullCommand();
    string s = cmd.cmd;

    if( s=="pause" || s=="p" ){
        pause = !pause;
        return;
    }
}
*/
