#ifndef ASCIIRECORDER_H
    #define ASCIIRECORDER_H 1

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

/// 3) default behavior of Run module is to interpret incoming void pointer as array of two floats and writes them separately in two columns.

class AsciiRecorder : public plrsModuleRecorder{

public:

    AsciiRecorder( plrsController* c);    //!< Constructor.

    virtual ~AsciiRecorder();  //!< Destructor

protected:

    virtual void PreRun();

    virtual void WriteToFile( void* );

};

extern "C" AsciiRecorder* create_AsciiRecorder( plrsController* c);

extern "C" void destroy_AsciiRecorder( AsciiRecorder* p );


#endif
