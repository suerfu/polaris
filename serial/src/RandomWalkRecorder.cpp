#include "RandomWalkRecorder.h"
#include "RandomWalkDAQ.h"

#include <unistd.h>


extern "C" RandomWalkRecorder* create_RandomWalkRecorder( plrsController* c ){ return new RandomWalkRecorder(c);}


extern "C" void destroy_RandomWalkRecorder( RandomWalkRecorder* p ){ delete p;}


RandomWalkRecorder::RandomWalkRecorder( plrsController* c) : plrsModuleRecorder(c){}

RandomWalkRecorder::~RandomWalkRecorder(){;}



void RandomWalkRecorder::Run(){

    void* rdo=0;

    Print( "running...\n", DETAIL);

    while( GetState()==RUN ){

        rdo = PullFromBuffer( RUN );

        if( rdo==0 )
            break;

        if( output_file )
            output_file << *(reinterpret_cast<int*>(rdo)) << endl;

        PushToBuffer( addr_nxt, rdo);
        rdo = 0;
    }
    Print( "run finished\n", DETAIL);
}


