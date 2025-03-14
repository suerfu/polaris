#include "plrsModuleRecorder.h"

#include <sstream>

#include <unistd.h>


extern "C" plrsModuleRecorder* create_plrsModuleRecorder( plrsController* c ){ return new plrsModuleRecorder(c);}



extern "C" void destroy_plrsModuleRecorder( plrsModuleRecorder* p ){ delete p;}



plrsModuleRecorder::plrsModuleRecorder( plrsController* c) : plrsStateMachine(c){
    wait_time_us = 1000;
    del_time = 100;
}



plrsModuleRecorder::~plrsModuleRecorder(){;}



void plrsModuleRecorder::Configure(){

    string filename = cparser->GetString( "/cmdl/output" );

    if( filename == "" )
        filename = cparser->GetString( "/cmdl/file" );

    if( filename == "" )
        filename = cparser->GetString( "/module/"+GetModuleName()+"/output" );

    if( filename == "" )
        filename = cparser->GetString( "/module/"+GetModuleName()+"/file" );

    if( filename != "" ){

        output_file.open( filename, ios_base::out );
        if( !output_file.is_open() || !output_file.good() ){
            Print( "Error: cannot open "+filename+" for writing\n", ERR);
            SetStatus(ERROR);
            return;
        }
        else{
            Print( "Opened file "+filename+" for writing output\n", INFO);
        }
    }

    else{
        Print( "Warning: no output file specified\n", INFO);
    }
}



void plrsModuleRecorder::Deconfigure(){
    Print( "Deconfiguring...\n", DETAIL);
    if( output_file.is_open())
        output_file.close();
}



void plrsModuleRecorder::Run(){

    void* rdo = PullFromBuffer();

    while( GetState()==RUN ){

        if( rdo==0 ){
            if( wait_time_us<0xffffff )
                wait_time_us += del_time;
            usleep( wait_time_us );
        }

        else{
            if( wait_time_us>del_time )
                wait_time_us -= del_time;

            if( output_file )
                WriteToFile( rdo );

            PushToBuffer( addr_nxt, rdo);
        }
    }

}



void plrsModuleRecorder::WriteToFile( void* rdo ){}



