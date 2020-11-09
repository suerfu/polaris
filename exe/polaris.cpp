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

    cout << "\nUsage: " << "polaris   --cfg foo.cfg [--option [parameter1...]]\n";
    cout << "\nOptions:\n";
    cout << "\t--cfg,\n\t\tSpecifies configuration file.\n";
    cout << "\t-t, --time,\n\t\tSpecifies max run time in seconds.\n"; 
    cout << "\t-v, --verbose, --detail,\n\t\tMessages with verbosity of at least DETAIL will be printed.\n"; 
    cout << "\t-q, --quiet,\n\t\tOnly errors will be printed.\n"; 
    cout << "\t--log foo.log,\n\t\tThe same messages on the standard output is recorded in foo.log.\n";

    if( libname=="" ){
        cout << "\t--help [libxxx.so],\n\t\tRetrieves and prints the commandline usage of polaris module in libxxx.so if specified.\n";
        cout << "\t\tUsers can specify custom commandline arguments as --xxx foo or -x for custom modules.\n"; 
        cout << "\t\t-- is used for multi-characters while - is used for a single character.\n"; 
        cout << "\t\tInside the framework, commandline arguments are accessed through ConfigParser with directory /cmdl/.\n";
        cout << "\t\tThese custom commandline arguments are defined by each module/library to be used.\n"; 
        cout << "\t\tIf defined, these parameters can be retrieved and printed to screen by specifying the library name after --help.\n\n";
    }

    else{
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
