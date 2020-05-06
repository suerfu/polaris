#include "plrsModuleDAQ.h"

#include <sstream>

#include <unistd.h>


extern "C" plrsModuleDAQ* create_plrsModuleDAQ( plrsController* c ){ return new plrsModuleDAQ(c);}


extern "C" void destroy_plrsModuleDAQ( plrsModuleDAQ* p ){ delete p;}



plrsModuleDAQ::plrsModuleDAQ( plrsController* c) : plrsStateMachine(c){ }



plrsModuleDAQ::~plrsModuleDAQ(){}



void plrsModuleDAQ::Initialize(){
    Print( "Initialize() called.\n", ERR);
}



void plrsModuleDAQ::Deinitialize(){
    Print( "Deinitialize called.\n", ERR);
}



void plrsModuleDAQ::Configure(){
    Print("Configure() called.\n", ERR);
}


void plrsModuleDAQ::Deconfigure(){
    Print("Deconfigure() called.\n", ERR);
}


void plrsModuleDAQ::PreRun(){
    Print("PreRun() called.\n", ERR);
}


void plrsModuleDAQ::PostRun(){
    Print("PostRun() called.\n", ERR);
}


void plrsModuleDAQ::Run(){
    Print("Run() called (inside loop).\n", ERR);
    sleep(1);
}



