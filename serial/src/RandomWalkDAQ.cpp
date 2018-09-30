#include "RandomWalkDAQ.h"

#include "plrsBaseData.h"

#include <sstream>
#include <unistd.h>


RandomWalkDAQ::RandomWalkDAQ( plrsController* c) : plrsModuleDAQ(c){
    buff_depth = 100;
    sample_intv = 100000;
    ncol = 2;
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

    ncol = cparser->GetInt("/module/"+GetModuleName()+"/N", 10);
    for( int i=0; i<ncol; i++)
        current_value.push_back( 0 );
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


void RandomWalkDAQ::PreRun(){ start_time = ctrl->GetMSTimeStamp();}


void RandomWalkDAQ::Run(){

    void* p = PullFromBuffer();

    if( p!=0 ){
        int c;
        vector<plrsBaseData>* data = reinterpret_cast< vector<plrsBaseData>* >( p );

        file.read( (char*)(&c), sizeof(int));
        int time = (ctrl->GetMSTimeStamp() - start_time);
        data->clear();
        data->push_back( plrsBaseData( time ));

        for( int i=0; i<ncol; i++){
            if( ((c>>i)&0x1)==0 )
                current_value[i]++;
            else 
                current_value[i]--;

            int value = current_value[i];
            data->push_back( plrsBaseData( value ));
        }

        PushToBuffer( addr_nxt, p );
        usleep( sample_intv );
    }
}


