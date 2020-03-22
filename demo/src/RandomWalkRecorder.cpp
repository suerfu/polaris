#include "RandomWalkRecorder.h"

#include <sstream>

#include <unistd.h>


extern "C" RandomWalkRecorder* create_RandomWalkRecorder( plrsController* c ){ return new RandomWalkRecorder(c);}


extern "C" void destroy_RandomWalkRecorder( RandomWalkRecorder* p ){ delete p;}


RandomWalkRecorder::RandomWalkRecorder( plrsController* c) : plrsStateMachine(c){}


RandomWalkRecorder::~RandomWalkRecorder(){;}


void RandomWalkRecorder::Configure(){

    // obtain output filename from commandline with argument either --output or --file
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
            Print( "opened file "+filename+" for writing output\n", INFO);
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

    int curr = 0;

    int next = GetNextID();
    if( next<0 )
        next = GetID();

    while( GetState()==RUN ){

        if( rdo==0 )
            rdo = PullFromBuffer();

        if( rdo!=0 ){
            if( output_file ){
                int* int_ptr = reinterpret_cast<int*>(rdo);
                if( *int_ptr%2==0 )
                    curr++;
                else
                    curr--;

                output_file << curr << endl;
            }

            PushToBuffer( next, rdo);
            rdo = 0;
                // set it to 0 such that it continues in the loop if no resource is available.
        }
    }
}


