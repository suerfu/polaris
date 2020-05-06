#ifndef PLRSMODULEINTERFACE_H
    #define PLRSMODULEINTERFACE_H 1

#include <string>
#include <vector>
#include <list>

#include "plrsController.h"
#include "plrsStateMachine.h"

#include "plrsSocket.h"

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

private:

    string hostname;

    plrsSocket socket;
        //!< Convenient data structure to hold connection information.

    list<int> list_connections;
        //!< contains socket descriptors for established connection.

    vector<int> data;

    int next_addr;

    unsigned int payload;
};


/// creator function for loading the module.
extern "C" plrsModuleInterface* create_plrsModuleInterface( plrsController* c );



/// destructor function for releasing the module.
extern "C" void destroy_plrsModuleInterface( plrsModuleInterface* p );


#endif
