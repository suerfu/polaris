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

    string GetModuleName(){ return "daq";}

protected:

    virtual void Initialize();  //!< Initialize parameters and establish connections, etc.

    virtual void CleanUp();     //!< Called in the END phase to deallocate resources.

    virtual void Configure();   //!< Do necessary hardware setup.

    virtual void UnConfigure(); //!< Unset hardware setup.

    void PreRun();  //!< Called at the beginning of RUN phase. It calls StartDAQ.

    virtual void StartDAQ();
        //!< Start data acquisition.
        //!< This function is called by base class at the beginning of a run.

    void PostRun(); //!< Called at the end of RUN phase. It calls StopDAQ

    virtual void StopDAQ();
        //!< Temporarily or permanently stop the DAQ for current run.
        //!< This function is called by base class after each run.

};

extern "C" plrsModuleDAQ* create_plrsModuleDAQ( plrsController* c);

extern "C" void destroy_plrsModuleDAQ( plrsModuleDAQ* p );


#endif
