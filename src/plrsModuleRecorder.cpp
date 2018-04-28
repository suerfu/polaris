#include "plrsModuleRecorder.h"

#include <sstream>


plrsModuleRecorder::plrsModuleRecorder( plrsController* c) : plrsStateMachine(c){}

void plrsModuleRecorder::Configure(){
    string filename = cparser->GetString( "/cmdl/output" );
    if( filename == "" )
        filename = cparser->GetString( "/cmdl/file" );
    if( filename == "" )
        filename = cparser->GetString( "/module/recorder/file" );

    if( filename != "" ){

        output_file.open( filename, ios_base::out );
        if( !output_file.is_open() || !output_file.good() ){
            Print( "Error: cannot open "+filename+" for writing\n", ERR);
            SetStatus(ERROR);
            return;
        }
        else{
            Print( "opened file "+filename+" for writing output\n", INFO);
        }
    }

    else{
        Print( "Warning - no output file specified\n", INFO);
    }
}



void plrsModuleRecorder::Deconfigure(){
    if( output_file.is_open())
        output_file.close();
    Print( "unconfiguring...\n", DETAIL);
}



void plrsModuleRecorder::Deinitialize(){
    if( output_file.is_open())
        output_file.close();
    Print( "cleaning up...\n", DETAIL);
}



plrsModuleRecorder::~plrsModuleRecorder(){;}
