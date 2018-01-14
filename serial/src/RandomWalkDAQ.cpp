#include "RandomWalkDAQ.h"

#include <sstream>
#include <unistd.h>

RandomArray::RandomArray( int s) : _size(s){
    array = new int[_size];
    x = new int[_size];
    for( int i=0; i<_size; i++)
        x[i] = i;
    n = 0;
}

RandomArray::~RandomArray(){
    delete [] array;
    _size = 0;
}

void RandomArray::Read( istream& file ){
    file.read( reinterpret_cast<char*>( array), sizeof(int)*_size);
    array[0] = 0;
    for( int i=1; i<_size; ++i)
        //array[i] = i;
        array[i] = array[i]>=0 ? ++array[i-1] : --array[i-1];
}

void RandomArray::Write( ostream& file){
    for( int i=0; i<_size; ++i)
        file << array[i] << endl;
}


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c ){ return new RandomWalkDAQ(c);}


extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p ){ delete p;}


RandomWalkDAQ::RandomWalkDAQ( plrsController* c) : plrsModuleDAQ(c){
    event_size = 1000;
    buff_size = 100;
    sample_intv = 220000;
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
            PushToBuffer( id, reinterpret_cast<void*>(new RandomArray( event_size )) );
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
        data = reinterpret_cast<RandomArray*>( p );
        data->Read( file );
        PushToBuffer( addr_nxt, reinterpret_cast<void*>(data) );
        usleep( sample_intv);
    }
}

void RandomWalkDAQ::PreEvent(){}

void RandomWalkDAQ::PostEvent(){}

void RandomWalkDAQ::StartDAQ(){;}

void RandomWalkDAQ::StopDAQ(){;}

void RandomWalkDAQ::CleanUp(){

    void* p = PullFromBuffer();

    while( p!=0 ){
        delete reinterpret_cast<RandomArray*>(p);
        p = PullFromBuffer();
    }
}
