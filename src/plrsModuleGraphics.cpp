#include "plrsModuleGraphics.h"

#include <unistd.h>


plrsModuleGraphics::plrsModuleGraphics( plrsController* c ) : plrsStateMachine( c ){
    pause = false;
}



plrsModuleGraphics::~plrsModuleGraphics(){}



void plrsModuleGraphics::Configure(){
    refresh_rate = cparser->GetInt("refresh_ms", 1000);
    refresh_rate *= 1000;
}


void plrsModuleGraphics::Clear(){}



void plrsModuleGraphics::Run(){

    void* rdo = 0;

    uint32_t now = 0;
    uint32_t last_update = 0;

    while( GetState()==RUN ){

        rdo = PullFromBuffer();
 
        if( rdo==0 ){
            usleep(10000);
            continue;
        }

        Process( rdo );

        if( !pause ){
            now = ctrl->GetMSTimeStamp();
            if( now-last_update > refresh_rate ){
                Clear();
                Draw( rdo );
                last_update = now;
            }
        }

        PushToBuffer( addr_nxt, rdo);
        rdo = 0;
        
        usleep( 10000 );
        CommandHandler();
    }
}


void plrsModuleGraphics::CommandHandler(){

    string s = PullCommand();

    if( s=="pause" || s=="p" ){
        pause = !pause;
        return;
    }
}
