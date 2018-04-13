#include "RandomWalkDAQ.h"

#include <sstream>
#include <unistd.h>


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c ){ return new RandomWalkDAQ(c);}


extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p ){ delete p;}


RandomWalkDAQ::RandomWalkDAQ( plrsController* c) : plrsModuleDAQ(c){
    buff_size = 100;
    sample_intv = 1000000;
    current_value = 0;
}


RandomWalkDAQ::~RandomWalkDAQ(){}


void RandomWalkDAQ::Configure(){
    // open file for accessing random number
    Print( "Opening /dev/urandom\n", DEBUG);
    file.open("/dev/urandom");
    
    // if failed to open file, exit; otherwise allocate memory.
    if(!file){
        Print( "Error: failed to open /dev/random\n", ERR);
        SetStatus(ERROR);
    }
    else{
        int id = ctrl->GetIDByName( this->GetModuleName() );
        stringstream ss;
        ss << this->GetModuleName()+" has ID " << id << "\n";
        Print( ss.str(), DEBUG);
        for( int i=0; i<buff_size; ++i )
            PushToBuffer( id, reinterpret_cast<void*>(new int ) );
    }
}   

void RandomWalkDAQ::UnConfigure(){
    Print( "Closing input file /dev/random\n", DEBUG);
    file.close();
    CleanUp();
}

void RandomWalkDAQ::Event(){

    void* p = PullFromBuffer( RUN );

    if( p!=0 ){
        char* c = reinterpret_cast<char*>( p );
        int* a = reinterpret_cast<int*>( p );

        file.read( c, sizeof(char));
        if( *c%2==0 )
            current_value++;
        else
            current_value--;

        *a = current_value;
        PushToBuffer( addr_nxt, p );
    }
    usleep( sample_intv );
}

void RandomWalkDAQ::PreEvent(){}

void RandomWalkDAQ::PostEvent(){}

void RandomWalkDAQ::StartDAQ(){;}

void RandomWalkDAQ::StopDAQ(){;}

void RandomWalkDAQ::CleanUp(){

    void* p = PullFromBuffer();

    while( p!=0 ){
        delete reinterpret_cast<int*>(p);
        p = PullFromBuffer();
    }
}
