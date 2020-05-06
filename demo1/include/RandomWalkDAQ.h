//! A sample polaris module to illustrate how to implement a custom DAQ module.

/*!
This is a sample polaris module that reads random numbers from /dev/urandom and write them to file.

A few things to pay attention to:

1) Every module should inherit from plrsStateMachine.

2) Custom behavior is achieved by implementing a number of base functions:
    Configure()   - obtain user parameters and configure hardware.
    PreRun()      - actions needed immediately before the run.
    Run()         - actual acquisition loop (for DAQ) and writing data (for recorder).
    PostRun()     - actions needed immediately after the run.
    Deconfigure() - free system resources.

3) Also optionally one can implement the following three functions as well:
    Initialize()
    Deinitialize()
    Idle()
*/

#ifndef RANDOMWALKDAQ_H
    #define RANDOMWALKDAQ_H 1

#include <fstream>

#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


class RandomWalkDAQ : public plrsStateMachine {

public:

    RandomWalkDAQ( plrsController* ctrl);

    ~RandomWalkDAQ();

protected:

    void Configure();

    void Deconfigure();

    void PreRun();
        //!< This method is called once before entering the Run-loop.

    void Run();
        //!< This function is called continuously as long as the state is RUN.

    void PostRun();
        //!< This function is called once after the entire run is finished.
private:

    int next_addr;
        //!< ID of the next module to which data buffer will be sent.

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
