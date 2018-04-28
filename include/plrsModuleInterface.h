#ifndef PLRSMODULEINTERFACE_H
    #define PLRSMODULEINTERFACE_H 1

#include <string>

#include "plrsController.h"
#include "plrsStateMachine.h"

/// Interface module for polaris.

/// It can either interface to other applications via unix socket or to other hosts via TCP/IP over the net.

class plrsModuleInterface : public plrsStateMachine{

public:

    plrsModuleInterface( plrsController* c);    //!< Constructor.

    virtual ~plrsModuleInterface();  //!< Destructor

    string GetModuleName(){ return "interface";}

protected:

    void Configure();

    void Run();

	void Deconfigure();

    void Deinitialize();

protected:

    ofstream output_file;

private:

    int descriptor;

};



extern "C" plrsModuleInterface* create_plrsModuleInterface( plrsController* c);


extern "C" void destroy_plrsModuleInterface( plrsModuleInterface* p );

#endif
