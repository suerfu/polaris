#include "SerialVtProc.h"

#include <unistd.h>


extern "C" SerialVtProc* create_SerialVtProc( plrsController* c ){ return new SerialVtProc(c);}



extern "C" void destroy_SerialVtProc( SerialVtProc* p ){ delete p;}



SerialVtProc::SerialVtProc( plrsController* c) : plrsStateMachine(c){}



SerialVtProc::~SerialVtProc(){;}



void SerialVtProc::Configure(){

    adc_res = cparser->GetInt("/module/"+GetModuleName()+"/adc_res", 12);
    adc_vpref = cparser->GetFloat( "/module/"+GetModuleName()+"/adc_vpref", 3.3);
    adc_navg = cparser->GetFloat( "/module/"+GetModuleName()+"/adc_navg", 10);
    
    string  nxt = cparser->GetString( "/module/"+GetModuleName()+"/prev_module");
    if( nxt=="" ){
        Print( GetModuleName()+" needs to know where to recycle data.\n", ERR);
        SetStatus(ERROR);
    }
    else
        addr_prev = ctrl->GetIDByName( nxt );
}


void SerialVtProc::Run(){

    void* rdo = 0;

    Print( "running...\n", DETAIL);

    float nsamp = 0;
    float avg_time = 0;
    float avg_adc = 0;

    while( GetState()==RUN ){

        rdo = PullFromBuffer();
        if( rdo==0 ){
            sleep(1);
            continue;
        }

        avg_time += reinterpret_cast<int*>(rdo)[0];
        avg_adc += reinterpret_cast<int*>(rdo)[1];
        nsamp++;

        if( nsamp == adc_navg){
            reinterpret_cast<float*>(rdo)[0] = (1.0*avg_time)/nsamp/1000;
            reinterpret_cast<float*>(rdo)[1] = (adc_vpref*avg_adc)/nsamp/(1<<adc_res);

            PushToBuffer( addr_nxt, rdo);

            avg_time = avg_adc = nsamp = 0;
        }
        else
            PushToBuffer( addr_prev, rdo);

        rdo = 0;
    }
    Print( "run finished\n", DETAIL);
}


