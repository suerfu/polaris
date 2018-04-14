#include "plrsModuleGraphics.h"


plrsModuleGraphics::plrsModuleGraphics( plrsController* c ) : plrsStateMachine( c ){
    rdo = 0;
    now = 0;
    last_update = 0;
    pause = false;
    refresh_rate = cparser->GetInt("refresh_rate", 1);
}



plrsModuleGraphics::~plrsModuleGraphics(){}



void plrsModuleGraphics::Configure(){}



void plrsModuleGraphics::UnConfigure(){}



void plrsModuleGraphics::CleanUp(){}



void plrsModuleGraphics::Clear(){}



void plrsModuleGraphics::PreEvent(){}



void plrsModuleGraphics::PostEvent(){}
    // can be used to update the graphics if necessary

void plrsModuleGraphics::Event(){

    rdo = PullFromBuffer( RUN );
 
    if( rdo!=0){
        Process( rdo );
        if( !pause ){
            // if pause is not true, check refresh_rate and redraw.
            now = ctrl->GetTimeStamp();
            if( now-last_update > refresh_rate ){
                Clear();
                Draw( rdo );
                last_update = now;
            }
        }
        PushToBuffer( addr_nxt, rdo);
            // regardless of pause or not, should give the resources to next module.
        rdo = 0;
            // set to 0 to avoid segmentation fault.
    }
}



void plrsModuleGraphics::CommandHandler(){

    string s = PullCommand();

    if( s=="pause" || s=="p" ){
        pause = !pause;
        return;
    }
}
