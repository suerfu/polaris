#ifndef PLRSMODULERECORDER_H
    #define PLRSMODULERECORDER_H 1

#include <string>

#include "plrsController.h"
#include "plrsStateMachine.h"

/// This module writes data to file on disk.
/// Default behavior ( if not overrided with polymorphism )

/// 1) output file name is configured in the following order:
/// --- specified with commandline option --file
/// --- specified in config file with /module/recorder/file

/// 2) after data is processed data is sent to the next module determined in the following order
/// --- specified with /module/record/next_module
/// --- search if there is interface module
/// --- search if there is graphics module
/// --- send data back to controller.

/// 3) default behavior of Run module is keep asking buffer for data and call write method to write them on disk.

class plrsModuleRecorder : public plrsStateMachine{

public:

    plrsModuleRecorder( plrsController* c);    //!< Constructor.

    virtual ~plrsModuleRecorder();  //!< Destructor

protected:

    void Configure();

	void Deconfigure();

    void Run();

    virtual void WriteToFile( void* );

protected:

    ofstream output_file;

    int wait_time_us;

    int del_time;

};



extern "C" plrsModuleRecorder* create_plrsModuleRecorder( plrsController* c );



extern "C" void destroy_plrsModuleRecorder( plrsModuleRecorder* p );

#endif
