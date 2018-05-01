#ifndef PLRSMODULEDAQ_H
    #define PLRSMODULEDAQ_H 1

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <map>

#include "ConfigParser.h"
#include "circular_buffer.h"

#include "plrsController.h"
#include "plrsStateMachine.h"

using namespace std;

class plrsModuleDAQ : public plrsStateMachine {

public:

    plrsModuleDAQ( plrsController* ctrl);
        //!< It initializes all hardwares according to the parameters passed in by the parameter handler obtained from controller.

    virtual ~plrsModuleDAQ();
        //!< Destructor.

protected:

    virtual void Initialize();  //!< Initialize parameters and establish connections, etc.

    virtual void Configure();   //!< Do necessary hardware setup.

    virtual void Deinitialize();     //!< Called in the END phase to deallocate resources.

    virtual void Deconfigure(); //!< Unset hardware setup.

    virtual void PreRun();  //!< Starting DAQ hardware.

    virtual void Run();   //!< Acquire one event.

    virtual void PostRun(); //!< Stop DAQ hardware.

};

extern "C" plrsModuleDAQ* create_plrsModuleDAQ( plrsController* c);

extern "C" void destroy_plrsModuleDAQ( plrsModuleDAQ* p );


#endif
