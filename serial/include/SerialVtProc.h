#ifndef RANDOMWALKRECORDER_H
    #define RANDOMWALKRECORDER_H 1

#include <string>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

/// This module processes incoming data and pass it onto the next module.

/// Mainly it does two things: conversion ffom ADC count and averaging.

class SerialVtProc : public plrsStateMachine{

public:

    SerialVtProc( plrsController* c);    //!< Constructor.

    virtual ~SerialVtProc();  //!< Destructor

protected:

    string GetModuleName(){ return "processor";}

    virtual void Configure();

    virtual void Run();

private:

    int adc_res;

    float adc_vpref;

    int adc_navg;

    int addr_prev;

};

extern "C" SerialVtProc* create_SerialVtProc( plrsController* c);

extern "C" void destroy_SerialVtProc( SerialVtProc* p );


#endif
