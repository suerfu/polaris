#include "RandomWalkDAQ.h"

#include <sstream>
#include <unistd.h>


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c ){ return new RandomWalkDAQ(c);}


extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p ){ delete p;}


RandomWalkDAQ::RandomWalkDAQ( plrsController* c) : plrsModuleDAQ(c){
    buff_depth = 100;
    sample_intv = 1000000;
    current_value = 0;
}


RandomWalkDAQ::~RandomWalkDAQ(){}


void RandomWalkDAQ::Configure(){
    
    Print( "Opening /dev/urandom\n", DEBUG);
    file.open("/dev/urandom");
    
    if(!file){
        Print( "Error: failed to open /dev/random\n", ERR);
        SetStatus(ERROR);
    }
    else{
        int id = ctrl->GetIDByName( this->GetModuleName() );

        stringstream ss;
        ss << this->GetModuleName()+" has ID " << id << "\n";
        Print( ss.str(), DEBUG);

        for( int i=0; i<buff_depth; ++i )
            PushToBuffer( id, reinterpret_cast<void*>(new int[4] ) );
    }
}   

void RandomWalkDAQ::Deconfigure(){
    Print( "Closing input file /dev/random\n", DEBUG);
    file.close();
    Deinitialize();
}

void RandomWalkDAQ::Event(){

    void* p = PullFromBuffer( RUN );

    if( p!=0 ){
        char* c = reinterpret_cast<char*>( p );
        int* a = reinterpret_cast<int*>( p );
        float* f = reinterpret_cast<float*>( reinterpret_cast<int*>(p)+sizeof(int));

        file.read( c, sizeof(char));
        if( *c%2==0 )
            current_value++;
        else
            current_value--;

        a[0] = 1000*(ctrl->GetTimeStamp() - start_time);
        f[0] = current_value;

        PushToBuffer( addr_nxt, p );
    }
    usleep( sample_intv );
}

void RandomWalkDAQ::PreEvent(){}

void RandomWalkDAQ::PostEvent(){}

void RandomWalkDAQ::PreRun(){
    start_time = ctrl->GetTimeStamp();
;}

void RandomWalkDAQ::StopDAQ(){;}

void RandomWalkDAQ::Deinitialize(){

    void* p = PullFromBuffer();

    while( p!=0 ){
        delete [] reinterpret_cast<int*>(p);
        p = PullFromBuffer();
    }
}
