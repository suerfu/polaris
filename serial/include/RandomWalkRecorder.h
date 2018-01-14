#ifndef RANDOMWALKRECORDER_H
    #define RANDOMWALKRECORDER_H 1

#include <string>

#include "plrsController.h"
#include "plrsModuleRecorder.h"

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

/// 3) default behavior of Run module is keep asking buffer for data and call Write method to write them on disk.

class RandomWalkRecorder : public plrsModuleRecorder{

public:

    RandomWalkRecorder( plrsController* c);    //!< Constructor.

    virtual ~RandomWalkRecorder();  //!< Destructor

protected:

    virtual void Run();

};

extern "C" RandomWalkRecorder* create_RandomWalkRecorder( plrsController* c);

extern "C" void destroy_RandomWalkRecorder( RandomWalkRecorder* p );


#endif
