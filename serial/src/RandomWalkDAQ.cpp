#include "RandomWalkDAQ.h"

#include "plrsBaseData.h"

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
    
    Print( "opening /dev/urandom\n", DEBUG);
    file.open("/dev/urandom");
    
    if(!file){
        Print( "Error: failed to open /dev/random\n", ERR);
        SetStatus(ERROR);
    }
    else{
        int id = ctrl->GetIDByName( this->GetModuleName() );

        for( int i=0; i<buff_depth; ++i )
            PushToBuffer( id, reinterpret_cast<void*>(new vector<plrsBaseData>));
    }
}


void RandomWalkDAQ::Deconfigure(){

    void* p = PullFromBuffer();

    while( p!=0 ){
        delete reinterpret_cast<vector<plrsBaseData>* >(p);
        p = PullFromBuffer();
    }

    Print( "closing input file /dev/random\n", DEBUG);
    file.close();
}


void RandomWalkDAQ::PreRun(){ start_time = ctrl->GetTimeStamp();}


void RandomWalkDAQ::Run(){

    void* p = PullFromBuffer();

    while( GetState()==RUN ){
    
        p = PullFromBuffer();

        if( p!=0 ){
            char c;
            vector<plrsBaseData>* data = reinterpret_cast< vector<plrsBaseData>* >( p );

            file.read( &c, sizeof(char));
            if( c%2==0 )
                current_value++;
            else
                current_value--;

            int time = (ctrl->GetTimeStamp() - start_time);
            int value = current_value;

            data->clear();
            data->push_back( plrsBaseData( time ));
            data->push_back( plrsBaseData( value ));

            PushToBuffer( addr_nxt, p );

            p = 0;
        }

        sched_yield();
        CommandHandler();
        usleep( sample_intv );
    }
}


