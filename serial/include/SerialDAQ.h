#ifndef SERIALDAQ_H
    #define SERIALDAQ_H 1

#include <fstream>

#include "serialport.h"

#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


class SerialDAQ : public plrsModuleDAQ {

public:

    SerialDAQ( plrsController* ctrl);

    ~SerialDAQ();

protected:

    void Configure();

    void UnConfigure();

    void CleanUp();

    void PreRun();

    void PostRun();

    void PreEvent();

    void Event();

    void PostEvent();

private:

    serialport port;

    int buff_size;

    int sampling_period;
};


extern "C" SerialDAQ* create_SerialDAQ( plrsController* c);

extern "C" void destroy_SerialDAQ( SerialDAQ* p );

#endif
