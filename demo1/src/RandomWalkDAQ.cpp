#include "RandomWalkDAQ.h"

#include "plrsBaseData.h"

#include <sstream>
#include <unistd.h>


RandomWalkDAQ::RandomWalkDAQ( plrsController* c) : plrsStateMachine(c){}


RandomWalkDAQ::~RandomWalkDAQ(){}


void RandomWalkDAQ::Configure(){
    
    // *************************************************************************
    // This function is invoked When transitioning to CONFIG state.
    // It should configure how the data is passed along and open the file for read.
    // *************************************************************************

    // Above all, decide to which module pointer (to acquired data)  should be sent to.
    // The name of next module is acquired from configuration parser, and ID is looked up.
    // If next module is not specified, or it is invalid, then loop-back is set up.

    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }


    // Next find out how frequent to read.
    sample_intv = GetConfigParser()->GetFloat("/module/"+GetModuleName()+"/sample_interval", 1000);


    // Next get the size of buffer
    buff_depth = GetConfigParser()->GetFloat("/module/"+GetModuleName()+"/buff_depth", 1000);


    // Open the file to get random numbers. 
    // If the file cannot be opened, then set status to ERROR to inform controller.
    // Otherwise, initialize memory buffer with new and place them in the buffer queue of this module.
    file.open("/dev/urandom");
    
    if(!file){
        Print( "Error: failed to open /dev/random\n", ERR);
        SetStatus(ERROR);
        return;
            // Signal error and program will terminate.
    }
    else{
        int id = ctrl->GetIDByName( this->GetModuleName() );
            // this is the ID for its own fifo buffer queue.

        for( int i=0; i<buff_depth; ++i )
            PushToBuffer( id, new int);
    }

}


/// In this method, allocated memory is freed.
/// This is done by continuously pulling from its own FIFO memory buffer and deleting the memories.
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


/// This method is called continuously and repeatedly in the run loop.
/// In this method, first an attempt is made to get available memory from its FIFO buffer.
/// If memory is available (pointer is non-zero), then read a random number from urandom and write it to the pointer.
/// Next, the pointer is pushed to the FIFO buffer of the next module and sleep for some time to get the appropriate DAQ frequency.
void RandomWalkDAQ::Run(){

    void* p = PullFromBuffer();
        // Try to obtain a pointer to memory.

    if( p!=0 ){

        // The memory is valid, read an integer from urandom and give the pointer to the next module.
        file.read( (char*)(p), sizeof(int));

        PushToBuffer( next_addr, p );
        usleep( sample_intv );
    }
}


void RandomWalkDAQ::PostRun(){}
