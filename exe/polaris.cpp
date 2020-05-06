#include <iostream>
#include <iomanip>
#include <cstring>

#include "ConfigParser.h"
#include "plrsController.h"

#include "plrsBaseData.h"

using namespace std;

void PrintUsage( int argc, char* argv[]);

void PrintLogo();

int main( int argc, char* argv[] ){

    ConfigParser map;
    map.LoadCmdl( argc, argv);

    //if( map.GetString("/cmdl/quiet")=="" && map.GetString("/cmdl/q")=="" )
    //    PrintLogo();
    if( map.GetString("/cmdl/cfg")=="" ){
        PrintUsage(argc, argv);
        return -1;
    }

    if( map.GetString("/cmdl/detail")!="" || map.GetString("/cmdl/debug")!="" )
        map.Print();

    plrsController ctrl( &map );

    ctrl.StateLoop();

    return 0;
}

void PrintLogo(){
    string star = "*\n";
    int offset = 20;
    cout << "\n";
    cout << setw(offset) << " " << star;
    cout << setw(offset) << " "<< setw(5) << star;
    cout << endl;
    cout << setw(offset) << " "<< setw(7) << star;
    cout << endl;
    cout << setw(offset) << " "<< setw(9) << star;
    cout << endl;
    cout << setw(offset) << " "<< setw(7) << star;
    cout << setw(offset) << " "<< setw(16) << star;
    cout << setw(offset) << " "<< setw(12) << star;
    cout << "\n";
}

void PrintUsage( int argc, char* argv[] ){
    int width1 = strlen("usage: ");
    int width2 = strlen("usage: ") + strlen( argv[0] );
    cout<< setw(5) << "usage: " << argv[0] << " --cfg foo.cfg [--option] [parameter(s)]" <<endl;
    cout<< setw(width1) << " opt: " <<" ";
    cout<< setw(width2-width1-1) << " " << " --cfg,  specifies configuration file.\n"<<endl;
    cout<< setw(width2) << " " << " --time,  specifies max run time in seconds.\n"<<endl; 
    cout<< setw(width2) << " " << " --verbose, -v, --detail,  messages with verbosity of at least DETAIL will be printed.\n"<<endl; 
    cout<< setw(width2) << " " << " --quiet, -q,  only errors will be printed.\n"<<endl; 
    cout<< setw(width2) << " " << " --log foo.log,  the same messages on the standard output is recorded in the specified file.\n"<<endl; 
    cout<< setw(width2) << " " << "Above are system commandline arguments."<<endl; 
    cout<< setw(width2) << " " << "Users can specify custom commandline arguments as --xxx foo or -x for custom modules."<<endl; 
    cout<< setw(width2) << " " << "-- is used for multi-characters while - is used for a single character."<<endl; 
    cout<< setw(width2) << " " << "Inside the framework, commandline arguments are accessed through ConfigParser with directory /cmdl/xxx."<<endl; 
}
