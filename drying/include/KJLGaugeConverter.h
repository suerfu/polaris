#ifndef KJLGAUGECONVERTER_H
    #define KJLGAUGECONVERTER_H 1

#include <string>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

/// This module processes incoming data and pass it onto the next module.

/// Mainly it does two things: conversion ffom ADC count and averaging.

class KJLGaugeConverter : public plrsStateMachine{

public:

    KJLGaugeConverter( plrsController* c);    //!< Constructor.

    virtual ~KJLGaugeConverter();  //!< Destructor

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


};

extern "C" KJLGaugeConverter* create_KJLGaugeConverter( plrsController* c);

extern "C" void destroy_KJLGaugeConverter( KJLGaugeConverter* p );


#endif
