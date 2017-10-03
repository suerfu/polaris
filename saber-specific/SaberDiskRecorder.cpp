#include "SaberDiskRecorder.h"
#include "SaberDAQData.h"


extern "C" SaberDiskRecorder* create_SaberDiskRecorder( plrsController* c ){ return new SaberDiskRecorder(c);}


extern "C" void destroy_SaberDiskRecorder( SaberDiskRecorder* p ){ delete p;}


SaberDiskRecorder::SaberDiskRecorder( plrsController* c) : plrsModuleRecorder(c){}


SaberDiskRecorder::~SaberDiskRecorder(){}



void SaberDiskRecorder::UnConfigure(){

    if(!output_file || !output_file.is_open() )
        return;

    void* rdo = PullFromBuffer();
    while( rdo!=0 && GetState()!=ERROR ){
        rdo = PullFromBuffer();
        if( rdo!=0 ){
            SaberDAQData* d = reinterpret_cast<SaberDAQData*>(rdo);
            d->Write( output_file);
            rdo = 0;
        }
    }

    plrsModuleRecorder::UnConfigure();
}



void SaberDiskRecorder::Run(){

    Print( "running...\n", DEBUG);

    void* rdo = 0;

    while( GetState()==RUN && GetStatus()==RUN && output_file){

        // obtain valid event buffer
        rdo = PullFromBuffer();
        while( rdo == 0){
            if( GetState()!=RUN )
                break;
            rdo = PullFromBuffer();
        }

        // if rdo is 0, RUN has finished
        if( rdo==0 )
            break;
        // otherwise there is new event to be written to disk
        else{
            if( output_file ){
                SaberDAQData* d = reinterpret_cast<SaberDAQData*>(rdo);
                d->Write( output_file);
            }
            PushToBuffer( addr_nxt, rdo);
        }
    }

    Print( "run ended.\n", DEBUG);
}


