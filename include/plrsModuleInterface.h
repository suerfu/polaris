#ifndef PLRSMODULEINTERFACE_H
    #define PLRSMODULEINTERFACE_H 1

#include <string>
#include <vector>
#include <list>

#include "plrsController.h"
#include "plrsStateMachine.h"

#include "socketunix.h"

/// Interface module for polaris.

/// It can either interface to other applications via unix socket or to other hosts via TCP/IP over the net.

using std::string;
using std::vector;
using std::list;

class plrsModuleInterface : public plrsStateMachine{

public:

    plrsModuleInterface( plrsController* c);    //!< Constructor.

    virtual ~plrsModuleInterface();  //!< Destructor

protected:

    void Configure();

    void Run();

    void PostRun();

	void Deconfigure();

protected:

    ofstream output_file;

private:

    socketunix socket;

    list<int> list_client;

};


/// creator function for loading the module.
extern "C" plrsModuleInterface* create_plrsModuleInterface( plrsController* c );



/// destructor function for releasing the module.
extern "C" void destroy_plrsModuleInterface( plrsModuleInterface* p );


#endif
