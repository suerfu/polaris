#include "PTConverter.h"

#include "plrsBaseData.h"

#include <unistd.h>


extern "C" PTConverter* create_PTConverter( plrsController* c ){ return new PTConverter(c);}



extern "C" void destroy_PTConverter( PTConverter* p ){ delete p;}



PTConverter::PTConverter( plrsController* c) : plrsStateMachine(c){}



PTConverter::~PTConverter(){;}



void PTConverter::Configure(){

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


void PTConverter::Run(){

    void* rdo = 0;

    Print( "running...\n", DETAIL);

    float nsamp = 0;
    float avg_time = 0;
    float avg_temp = 0;
    float avg_pres = 0;

    float v_pres = 0;

    while( GetState()==RUN ){

        rdo = PullFromBuffer();
        if( rdo==0 ){
            usleep(10000);
            continue;
        }

        vector<plrsBaseData>& data = *(reinterpret_cast< vector<plrsBaseData>*>(rdo));

        avg_time += data[0].GetInt();
        avg_temp += data[1].GetInt();
        avg_pres += data[2].GetInt();
        nsamp++;

        if( nsamp >= adc_navg){
            avg_time /= nsamp*1000.;
            avg_pres /= nsamp;
            avg_temp /= nsamp;

            v_pres = adc_vpref*avg_pres/4095;

            data[0].SetFloat( avg_time );
            data[1].SetFloat( SPIToDegree( avg_temp) );
            data[2].SetFloat( VoltToTorr(v_pres) );

            PushToBuffer( addr_nxt, rdo);

            avg_time = avg_temp = avg_pres = nsamp = 0;
        }
        else
            PushToBuffer( addr_prev, rdo);

        rdo = 0;
    }
    Print( "run finished\n", DETAIL);
}

float PTConverter::SPIToDegree( int x){
    return x*(1000.0/0b11111010000000);
}

float PTConverter::VoltToTorr( float x){
    return (x-1)*50./4*51.7149326;
}

