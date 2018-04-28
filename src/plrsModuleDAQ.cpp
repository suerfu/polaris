#include "plrsModuleDAQ.h"

#include <sstream>

#include <unistd.h>


extern "C" plrsModuleDAQ* create_plrsModuleDAQ( plrsController* c ){ return new plrsModuleDAQ(c);}


extern "C" void destroy_plrsModuleDAQ( plrsModuleDAQ* p ){ delete p;}



plrsModuleDAQ::plrsModuleDAQ( plrsController* c) : plrsStateMachine(c){
    cparser = ctrl->GetConfigParser();
}



plrsModuleDAQ::~plrsModuleDAQ(){}



void plrsModuleDAQ::Initialize(){
    Print( "initializing\n", DETAIL);
}


//! Called at the end of program to clean up allocated memories.
void plrsModuleDAQ::Deinitialize(){
    Print( "cleaning up...\n", DETAIL);
}


void plrsModuleDAQ::Configure(){
    Print("configuring...\n", DETAIL);
}



void plrsModuleDAQ::Deconfigure(){
    Print("unconfiguring...\n", DETAIL);
}



void plrsModuleDAQ::PreRun(){
    Print("starting DAQ...\n", DETAIL);
}



void plrsModuleDAQ::Event(){
    sleep(1);
}



void plrsModuleDAQ::PostRun(){
    Print("stopping DAQ...\n", DETAIL);
}




