#include "SerialVtDAQ.h"

#include <sstream>
#include <iostream>
#include <cstring>


/// creator function for loading the module.
extern "C" SerialVtDAQ* create_SerialVtDAQ( plrsController* c ){ return new SerialVtDAQ(c);}


/// destructor function for releasing the module.
extern "C" void destroy_SerialVtDAQ( SerialVtDAQ* p ){ delete p;}


/// Constructor. buff_depth will control depth of FIFO buffer.
SerialVtDAQ::SerialVtDAQ( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_depth = 20;
}


/// Destructor. Nothing needs to be done.
SerialVtDAQ::~SerialVtDAQ(){}



void SerialVtDAQ::Configure(){

    Print("Configuring serial port...\n", DETAIL);

    // open serial port
    string fname = cparser->GetString("/module/daq/port");
    if( fname=="" ){
        fname = "ttyS0";
        Print( "Cannot find /module/daq/port, using ttyS0\n", ERR);
    }
    else
        Print( "opening "+fname+" for data acquisition\n", DETAIL);

    if( port.serial_open( fname.c_str() )!=0 ){
        Print( "Error - "+port.get_errmsg()+"\n", ERR);
        SetStatus( ERROR );
        return;
    }

    // port successfully opened.

    port.set_cooked();
    port.set_baud( B9600 );

    Print("SerialVt port configured.\n", DETAIL);

    // fill in circular FIFO buffer with resources.
    for( int i=0; i<buff_depth; ++i ){
        int id = ctrl->GetIDByName( this->GetModuleName() );
        if( sizeof(int)>=sizeof(float) )
            PushToBuffer( id, new int[2] );
        else
            PushToBuffer( id, new float[2] );
            // one for time in milisecond and the other for average voltage value
    }


    string freq = cparser->GetString("/module/daq/sample_interval");
    if( freq=="" )
        freq = "20";
    freq = "/adc/freq " + freq + "\r";
    port.serial_write( &freq[0], freq.size());

    Print("ADC configured.\n",DETAIL);
}



void SerialVtDAQ::Deconfigure(){
    Print( "Closing serial port...\n", DETAIL);
    port.serial_close();
}



void SerialVtDAQ::Deinitialize(){

    Print( "Cleaning up...\n", DETAIL);

    void* rdo = PullFromBuffer();
    while( rdo!=0 ){
        delete reinterpret_cast<int*>(rdo);
        rdo = PullFromBuffer();
        sched_yield();
    }
}


void SerialVtDAQ::PreRun(){
    Print( "DAQ starting\n", DETAIL);

    start_time = std::chrono::high_resolution_clock::now();

    char c[] = "/adc/on\r";
    port.serial_write( c, strlen(c));
}



void SerialVtDAQ::PreEvent(){
    sleep(1);
}



void SerialVtDAQ::Event(){

    void* rdo = 0;
    rdo = PullFromBuffer();
    if( rdo==0 ){
        sleep(1);
        return;
    }
        

    char data_in[64];
    data_in[0] = 64;

    int nbyte = 0;
    while( ( nbyte = port.serial_read( data_in, 64) )<=0 ){
        if(GetState()!=RUN)
            return;
    }
    
    int t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();

    data_in[nbyte] = '\0';

    if( data_in[0]=='#' || data_in[0]=='/'){
        PushToBuffer( ctrl->GetIDByName(this->GetModuleName()), rdo);
        return;
    }
    else{
        reinterpret_cast<int*>(rdo)[0] = t;
        reinterpret_cast<int*>(rdo)[1] = atoi(data_in);
        PushToBuffer( addr_nxt, rdo);
    }
}



void SerialVtDAQ::PostEvent(){}



void SerialVtDAQ::PostRun(){
    Print( "DAQ stopping\n", DETAIL);

    char c[] = "/adc/off\r";
    port.serial_write( c, strlen(c) );
}



