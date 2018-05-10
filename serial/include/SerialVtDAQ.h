#ifndef SERIALVTDAQ_H
    #define SERIALVTDAQ_H 1

// system libraries
#include <fstream>
#include <chrono>

// driver
#include "serialport.h"

// polaris-specific
#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


/// SerialVtDAQ is a module for reading data from serial port.
/// It uses a simple protocol for ADC:
///     /adc/{freq N, on, off}
/// freq parameter will change the sampling frequency N, which is given as interval in ms between sampling.
/// on and off will turn the adc on and off
/// By default the hardware device has echo enabled, so it is necessary to mark messages sent to the device by delimiters ( #, /, (space)) so that echoed messages can be identified.

class SerialVtDAQ : public plrsModuleDAQ {

public:

    SerialVtDAQ( plrsController* ctrl);
        //!< Constructor

    ~SerialVtDAQ();
        //!< Destructor

protected:

    void Configure();
        //!< Configure method should open the serial port and configure the device for data-taking.

    void Deconfigure();
        //!< Close serial port.

    void PreRun();
        //!< Instruct hardware to start data taking. 

    void Run();

    void PostRun();
        //!< Instruct hardware to stop.

    void PreEvent();

    void Event();
        //!< Main function in event loop. This function will first get writable memory from circular buffer.
        //!< Then it reads data from hardware and put a null-terminating character at the end to make it a c-string.
        //!< If the data contains markers that marks data sent to hardware ( and then echoed back), discard the data.
        //!< Finally cast the read data into int and send to next module.

    void PostEvent();

    int ReadADC( int, int);
private:

    serialport port;

    int buff_depth;

    int samp_interval;

    int sampling_period;

    int start_time;

    vector<int> adc_pchannels;
    vector<int> adc_nchannels;
};


/// creator function for loading the module.
extern "C" SerialVtDAQ* create_SerialVtDAQ( plrsController* c ){ return new SerialVtDAQ(c);}


/// destructor function for releasing the module.
extern "C" void destroy_SerialVtDAQ( SerialVtDAQ* p ){ delete p;}


#endif
