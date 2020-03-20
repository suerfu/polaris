#include "RandomWalkDAQ.h"

#include "plrsBaseData.h"

#include <sstream>
#include <unistd.h>


RandomWalkDAQ::RandomWalkDAQ( plrsController* c) : plrsStateMachine(c){}


RandomWalkDAQ::~RandomWalkDAQ(){}


void RandomWalkDAQ::Configure(){
    
    /*
    This function is invoked When transitioning to CONFIG state.
    It should open the file for read.
    */

    file.open("/dev/urandom");
    
    if(!file){
        Print( "Error: failed to open /dev/random\n", ERR);
        SetStatus(ERROR);
            // Signal error and program will terminate.
    }
    else{
        int id = ctrl->GetIDByName( this->GetModuleName() );
            // this is the ID for its own fifo buffer.

        for( int i=0; i<buff_depth; ++i )
            PushToBuffer( id, new int);
    }
}


void RandomWalkDAQ::Deconfigure(){

    void* p = PullFromBuffer();
        // pointer obtained from buffer is of type void*
        // needs to be reinterpreted/casted later

    while( p!=0 ){
        delete reinterpret_cast<int*>(p);
        p = PullFromBuffer();
    }
        // Clear allocated memory.

    Print( "closing input file /dev/random\n", DEBUG);
    file.close();
}


void RandomWalkDAQ::PreRun(){
    start_time = ctrl->GetMSTimeStamp();
}


void RandomWalkDAQ::Run(){

    void* p = PullFromBuffer();
        // Try to obtain a pointer to memory.

    int next = GetNextID();
        // ID of the module to pass data to.
    if( next<0 )
        next = GetID();
            // If next module is not specified, then set up for loop back.

    if( p!=0 ){
        // There is something valid in buffer

        int c;
        file.read( (char*)(&c), sizeof(int));
            // read a random number
        
        int* recast = reinterpret_cast<int*>(p);
            // the pointer must be casted before use

        *recast = c;
        PushToBuffer( next, p );
        usleep( sample_intv );
            // sleep for specified interval
    }
}


