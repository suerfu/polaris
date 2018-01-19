#include "plrsModuleDAQ.h"

#include <sstream>


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
void plrsModuleDAQ::CleanUp(){
    Print( "cleaning up...\n", DETAIL);
}


void plrsModuleDAQ::Configure(){
    Print("configuring...\n", DETAIL);
}



void plrsModuleDAQ::UnConfigure(){
    Print("unconfiguring...\n", DETAIL);
}


/*
void plrsModuleDAQ::PreRun(){
    StartDAQ();
}


void plrsModuleDAQ::PostRun(){
    StopDAQ();
}


void plrsModuleDAQ::StartDAQ(){
    Print( "running...\n", DETAIL);
}


void plrsModuleDAQ::StopDAQ(){
    Print( "ending run...\n", DETAIL);
}
*/
