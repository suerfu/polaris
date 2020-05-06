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

    ~plrsModuleDAQ();
        //!< Destructor.

protected:

    void Initialize();  //!< Initialize parameters and establish connections, etc.

    void Configure();   //!< Do necessary hardware setup.

    void Deinitialize();     //!< Called in the END phase to deallocate resources.

    void Deconfigure(); //!< Unset hardware setup.
    
    void PreRun();  //!< Prepare for run. Called only once.

    void Run(); //!< Actual event loop. Called continuously.

    void PostRun(); //!< Called after run is finished. Called only once.

};

extern "C" plrsModuleDAQ* create_plrsModuleDAQ( plrsController* c);

extern "C" void destroy_plrsModuleDAQ( plrsModuleDAQ* p );


#endif
