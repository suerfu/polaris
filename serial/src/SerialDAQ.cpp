#include "SerialDAQ.h"

#include <sstream>
#include <iostream>
#include <cstring>


/// creator function for loading the module.
extern "C" SerialDAQ* create_SerialDAQ( plrsController* c ){ return new SerialDAQ(c);}


/// destructor function for releasing the module.
extern "C" void destroy_SerialDAQ( SerialDAQ* p ){ delete p;}


/// Constructor. buff_size will control depth of FIFO buffer.
SerialDAQ::SerialDAQ( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_size = 1000;
}


/// Destructor. Nothing needs to be done.
SerialDAQ::~SerialDAQ(){}



void SerialDAQ::Configure(){

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

    Print("Serial port configured.\n", DETAIL);

    // fill in circular FIFO buffer with resources.
    for( int i=0; i<buff_size; ++i ){
        int id = ctrl->GetIDByName( this->GetModuleName() );
        PushToBuffer( id, new int );
    }


    string freq = cparser->GetString("/module/daq/interval");
    if( freq=="" )
        freq = "1000";
    freq = "/adc/freq " + freq + "\r";
    port.serial_write( &freq[0], freq.size());

    Print("ADC configured.\n",DETAIL);
}



void SerialDAQ::UnConfigure(){
    Print( "Closing serial port...\n", DETAIL);
    port.serial_close();
}



void SerialDAQ::CleanUp(){

    Print( "Cleaning up...\n", DETAIL);

    void* rdo = PullFromBuffer();
    while( rdo!=0 ){
        delete reinterpret_cast<int*>(rdo);
        rdo = PullFromBuffer();
        sched_yield();
    }
}



void SerialDAQ::Event(){

    void* rdo = 0;
    rdo = PullFromBuffer( RUN );
    if( rdo==0 )
        return;

    char data_in[64];
    data_in[0] = 64;

    int nbyte = 0;
    while( ( nbyte = port.serial_read( data_in, 64) )<=0 ){
        if(GetState()!=RUN)
            return;
    }

    data_in[nbyte] = '\0';

    if( data_in[0]=='#' || data_in[0]=='/'){
        PushToBuffer( ctrl->GetIDByName(this->GetModuleName()), rdo);
        return;
    }
    else{
        *(reinterpret_cast<int*>(rdo)) = atoi(data_in);
        PushToBuffer( addr_nxt, rdo);
    }
}


void SerialDAQ::PreRun(){
    Print( "DAQ starting\n", DETAIL);

    char c[] = "/adc/on\r";
    port.serial_write( c, strlen(c));
}


void SerialDAQ::PostRun(){
    Print( "DAQ stopping\n", DETAIL);

    char c[] = "/adc/off\r";
    port.serial_write( c, strlen(c) );
}


void SerialDAQ::PreEvent(){}


void SerialDAQ::PostEvent(){}
