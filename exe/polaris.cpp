#include <iostream>
#include <iomanip>
#include <cstring>
#include <dlfcn.h>

#include "ConfigParser.h"
#include "plrsController.h"

#include "plrsBaseData.h"

using namespace std;

void PrintUsage( int argc, char* argv[], string libname = "" );

void PrintLogo();

int main( int argc, char* argv[] ){

    ConfigParser map;
    map.LoadCmdl( argc, argv);

    if( map.GetString("/cmdl/version")!="" ){
        cout << plrsController::GetVersion() << endl;
        return 0;
    }

    if( map.GetString("/cmdl/cfg")=="" ){
        
        string libname = map.GetString("/cmdl/help");
        
        if( libname=="" || libname=="true" )
            PrintUsage( argc, argv);
        else
            PrintUsage( argc, argv, libname);

        return -1;
    }

    if( map.GetString("/cmdl/detail")!="" || map.GetString("/cmdl/debug")!="" ){
        map.Print();
    }

    plrsController ctrl( &map );

    ctrl.StateLoop();

    return 0;
}

void PrintUsage( int argc, char* argv[], string libname ){

    cout << "usage: " << "polaris --cfg foo.cfg [--option [parameter1...]]\n";
    
    cout << "\nRuns polaris DAQ program by loading the modules specified in the configuration file\n";
    
    cout << "\noptions:\n";
    
    cout << "    -h, --help\t\tshow this help message and exit. See below for optional parameter\n";
    
    cout << "    --help [libxxx.so]\tprints the commandline usage of polaris module in libxxx.so if defined\n";
    //cout << "\t\tUsers can specify custom commandline arguments as --xxx foo or -x for custom modules.\n"; 
    //cout << "\t\t-- is used for multi-characters while - is used for a single character.\n"; 
    //cout << "\t\tInside the framework, commandline arguments are accessed through ConfigParser with directory /cmdl/.\n";
    //cout << "\t\tThese custom commandline arguments are defined by each module/library to be used.\n"; 
    //cout << "\t\tIf defined, these parameters can be retrieved and printed to screen by specifying the library name after --help.\n\n";

    cout << "    --cfg [foo.cfg]\tspecifies configuration file\n";
    cout << "    --version\t\tdisplays current polaris version\n";
    cout << "    -t, --time [second]\tspecifies maximum run time in seconds\n";
    cout << "    --disable-input,--no-input\n    \t\t\tno user interaction, need to press ctrl+D at the end of program\n";
    cout << "    \t\t\twhen enabled, type /module-name/command to send command to the specific module\n";
    
    cout << "    --log [foo.log]\tsame messages on the standard output is recorded in foo.log\n";
    cout << "    -q, --quiet\t\tonly errors will be printed\n";
    cout << "    -v, --verbose, --detail\n    \t\t\tprint messages with verbosity greater than DETAIL\n";
    cout << "    --debug\t\tprint all messages for debugging purposes\n";

    if( libname!="" ){

        cout << endl;

        void* handle;
        void (*PU)( void ); // function to print usage.

        handle = dlopen( libname.c_str(), RTLD_NOW);
        if( !handle ){
            cout << "Error: Unable to open "+libname+": "+dlerror() << "\n" << endl;
            return;
        }

        PU = reinterpret_cast< void (*)(void) >( dlsym( handle, "PrintUsage") );
        if( !PU ){
            cout << "Error: Unable to retrieve PrintUsage() from "+libname+": "+dlerror() << "\n" << endl;
        }
        else{
            PU();
        }
        dlclose( handle);
    }
}
