//! A sample polaris module to illustrate how to implement a custom DAQ module.

#ifndef RANDOMWALKDAQ_H
    #define RANDOMWALKDAQ_H 1

#include <fstream>

#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"

/*!
This is a sample polaris module that reads random numbers from /dev/urandom.
This object also shows how user should implement his/her own class.

A few things to pay attention to:

1) Every module should inherit from plrsStateMachine.

2) The inherited class must implement a number of functions specifying the behavior of the module at different phases of DAQ. These functions includes:
    Configure()   - obtain user parameters and configure hardware.
    PreRun()      - actions needed immediately before the run.
    Run()         - actual acquisition loop (for DAQ) and writing data (for recorder).
    PostRun()     - actions needed immediately after the run.
    Deconfigure() - free system resources.

3) Also optionally one can implement the following two functions as well:
    Initialize()
    Deinitialize()
*/

class RandomWalkDAQ : public plrsStateMachine {

public:

    RandomWalkDAQ( plrsController* ctrl);

    ~RandomWalkDAQ();

protected:

    void Configure();

    void Deconfigure();

    void PreRun();

    void Run();

private:

    ifstream file;
        //!< Unix file to obtain system random variable.

    unsigned int start_time;
        //!< Timestamp for the start of DAQ

    int sample_intv;
        //!< Intervals between samples

    int buff_depth;
        //!< Number of pointers to data buffer.

        //!< In this example, each pointer will be pointing to a single integer.
        //!< In actual applications, it should point to a block of memory.
};

//! The following syntax is needed to obtain a C++ object from a C code (dll).
//! DLL library expects a C function, but polaris object is implemented as C++

extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c ){ return new RandomWalkDAQ(c);}


//! The following syntax is needed to obtain a C++ object from a C code (dll).
//! DLL library expects a C function, but polaris object is implemented as C++

extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p ){ delete p;}


#endif
