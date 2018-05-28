#ifndef PTCONVERTER_H
    #define PTCONVERTER_H 1

#include <string>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

/// This module processes incoming data and pass it onto the next module.

/// Mainly it does two things: conversion ffom ADC count and averaging.

class PTConverter : public plrsStateMachine{

public:

    PTConverter( plrsController* c);    //!< Constructor.

    virtual ~PTConverter();  //!< Destructor

protected:

    string GetModuleName(){ return "processor";}

    virtual void Configure();

    virtual void Run();

private:

    int adc_res;

    float adc_vpref;

    int adc_navg;

    int addr_prev;

    float VoltToTorr( float);
        // Converts gauge reading in voltage to pressure in mbar

    float SPIToDegree( int );
};

extern "C" PTConverter* create_PTConverter( plrsController* c);

extern "C" void destroy_PTConverter( PTConverter* p );


#endif
