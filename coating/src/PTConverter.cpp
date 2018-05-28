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
            avg_time /= nsamp*1000;
            avg_pres /= nsamp;
            avg_temp /= nsamp;

            v_pres = 3*adc_vpref*avg_pres/4095;

            //cout << avg_temp << "\t" << avg_pres << endl;
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
    
    if( x< 2.842 ){
        float a = -0.02585;
        float b = 0.03767;
        float c = 0.04563;
        float d = 0.1151;
        float e = -0.04158;
        float f = 0.008738;
        return a+b*x+c*x*x+d*x*x*x+e*x*x*x*x+f*x*x*x*x*x;
    }

    else if( x < 4.945 ){
        float a = 0.1031;
        float b = -0.3986;
        float c = -0.02322;
        float d = 0.07438;
        float e = 0.07229;
        float f = -0.006866;

        return (a+c*x+e*x*x)/(1+b*x+d*x*x+f*x*x*x);
    }

    else if( x<5.659 ){
        float a = 100.624;
        float b = -0.37679;
        float c = -20.5623;
        float d = 0.0348656;

        return (a+c*x)/(1+b*x+d*x*x);
    }

    else
        return 0;
}

