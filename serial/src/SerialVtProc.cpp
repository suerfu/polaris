#include "SerialVtProc.h"

#include "plrsBaseData.h"

#include <unistd.h>


extern "C" SerialVtProc* create_SerialVtProc( plrsController* c ){ return new SerialVtProc(c);}



extern "C" void destroy_SerialVtProc( SerialVtProc* p ){ delete p;}



SerialVtProc::SerialVtProc( plrsController* c) : plrsStateMachine(c){}



SerialVtProc::~SerialVtProc(){;}



void SerialVtProc::Configure(){

    adc_res = cparser->GetInt("/module/"+GetModuleName()+"/adc_res", 12);
    adc_vpref = cparser->GetFloat( "/module/"+GetModuleName()+"/adc_vpref", 3.3);
    adc_navg = cparser->GetFloat( "/module/"+GetModuleName()+"/adc_navg", 30);
    
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
    float avg_temp = 0;
    float avg_pres = 0;

    float v_temp = 0;
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
            avg_time /= nsamp*1000;
            avg_temp /= nsamp;
            avg_pres /= nsamp;

            v_temp = 3*adc_vpref*avg_temp/4095;
            v_pres = adc_vpref*avg_pres/4095;

            //cout << avg_temp << "\t" << avg_pres << endl;
            data[0].SetFloat( avg_time );
            data[1].SetFloat( ( v_temp -1.25)/0.005 );
            data[2].SetFloat( 50/4*(v_pres-1)*51.71 );

            PushToBuffer( addr_nxt, rdo);

            avg_time = avg_temp = avg_pres = nsamp = 0;
        }
        else
            PushToBuffer( addr_prev, rdo);

        rdo = 0;
    }
    Print( "run finished\n", DETAIL);
}


