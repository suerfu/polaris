
#include "RandomWalkRecorder.h"

#include <sstream>
#include <unistd.h>
#include <cstring>

extern "C" RandomWalkRecorder* create_RandomWalkRecorder( plrsController* c ){ return new RandomWalkRecorder(c);}


extern "C" void destroy_RandomWalkRecorder( RandomWalkRecorder* p ){ delete p;}


RandomWalkRecorder::RandomWalkRecorder( plrsController* c) : plrsStateMachine( c ){
    current_value = 0;
}


RandomWalkRecorder::~RandomWalkRecorder(){;}


void RandomWalkRecorder::Configure(){

    // Above all, decide to which module pointer (to acquired data)  should be sent to.
    // The name of next module is acquired from configuration parser, and ID is looked up.
    // If next module is not specified, or it is invalid, then loop-back is set up.
    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");

    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );
            // ID of next module is non-negative if the module is registered.
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }


    // Obtain output filename from commandline with argument either --output or --file, with output having higher priority.
    string filename = cparser->GetString( "/cmdl/output" );
    if( filename == "" )
        filename = cparser->GetString( "/cmdl/file" );

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
}



void RandomWalkRecorder::Deconfigure(){
    Print( "Deconfiguring...\n", DETAIL);
    if( output_file.is_open())
        output_file.close();
}



void RandomWalkRecorder::Run(){

    void* rdo = PullFromBuffer();

    if( rdo!=0 ){

        if( output_file ){
            output_file << current_value << endl;
        }

        int* int_ptr = reinterpret_cast<int*>(rdo);
        ( *int_ptr%2==0 ) ?  current_value++ :  current_value--;
            
        memcpy( rdo, &current_value, sizeof(int) );
                // BE CAREFUL! void* pointer must be copied at memory level, not cast and assigned.

        PushToBuffer( next_addr, rdo);
    }
}


