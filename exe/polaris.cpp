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

    if( map.GetString("/cmdl/quiet")=="" && map.GetString("/cmdl/q")=="" )
        PrintLogo();
    if( map.GetString("/cmdl/cfg")=="" ){
        PrintUsage(argc, argv);
        return -1;
    }

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
    cout<< setw(5) << "usage: " << argv[0] << " --cfg config-file [--option] [parameter(s)]" <<endl;
    cout<< setw(width1) << " opt: " <<" ";
    cout<< setw(width2-width1-1) << " " << " --cfg,   config-file   specifies configuration file."<<endl;
    cout<< setw(width2) << " " << " --file,  filename      output data to a file."<<endl;
    cout<< setw(width2) << " " << " --time,  time_in_sec   specifies total run time.\n"<<endl; 
}
