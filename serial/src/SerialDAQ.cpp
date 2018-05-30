#include "SerialDAQ.h"

#include "plrsBaseData.h"

#include <sstream>
#include <iostream>
#include <cstring>


/// creator function for loading the module.
extern "C" SerialDAQ* create_SerialDAQ( plrsController* c ){ return new SerialDAQ(c);}



/// destructor function for releasing the module.
extern "C" void destroy_SerialDAQ( SerialDAQ* p ){ delete p;}



/// Constructor. buff_depth will control depth of FIFO buffer.
SerialDAQ::SerialDAQ( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_depth = 20;
}


/// Destructor. Nothing needs to be done.
SerialDAQ::~SerialDAQ(){}



void SerialDAQ::Configure(){

    Print("Configuring serial port...\n", DETAIL);

    // open serial port
    string fname = cparser->GetString("/module/"+GetModuleName()+"/port");
    if( fname=="" ){
        fname = "ttyS0";
        Print( "Cannot find /module/"+GetModuleName()+"/port, using ttyS0\n", ERR);
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
    port.set_cflag( CLOCAL, true);
    port.set_baud( B9600 );

    Print("SerialVt port configured.\n", DETAIL);

    // fill in circular FIFO buffer with resources.
    for( int i=0; i<buff_depth; ++i ){
        int id = ctrl->GetIDByName( this->GetModuleName() );
        PushToBuffer( id, new vector<plrsBaseData> );
    }

    samp_interval = cparser->GetInt("/module/"+GetModuleName()+"/sample_interval_ms", 10);

    spi = cparser->GetBool("/module/"+GetModuleName()+"/spi", false);

    adc_pchannels = cparser->GetIntArray("/module/"+GetModuleName()+"/adc_pchannels");
    if( adc_pchannels.size()==0 )
        adc_pchannels.push_back(0);
    adc_nchannels = cparser->GetIntArray("/module/"+GetModuleName()+"/adc_nchannels");
    if( adc_nchannels.size()==0 )
        adc_nchannels.push_back(0xf);


    Print("ADC configured.\n",DETAIL);
}



void SerialDAQ::Deconfigure(){
    Print( "Closing serial port...\n", DETAIL);
    port.serial_close();
}



void SerialDAQ::PreRun(){
    Print( "DAQ starting\n", DETAIL);
    start_time = ctrl->GetMSTimeStamp();
}



void SerialDAQ::Run(){
    while( GetState()==RUN){
        PreEvent();
        Event();
        PostEvent();

        sched_yield();
    }
}



void SerialDAQ::PreEvent(){
    usleep( samp_interval*1000 );
}



void SerialDAQ::Event(){

    void* rdo = 0;
    rdo = PullFromBuffer();
    if( rdo==0 ){
        usleep(100000);
        return;
    }

    vector<int> adc;
    adc.push_back( int(ctrl->GetMSTimeStamp()-start_time));

    if( spi )
        adc.push_back( ReadSPI() );

    for( unsigned int i=0; i<adc_pchannels.size(); i++ ){
        adc.push_back( ReadADC( adc_pchannels[i], adc_nchannels[i]));
    }

    vector<plrsBaseData>* data = reinterpret_cast< vector<plrsBaseData>*>(rdo);
    data->clear();
    for( unsigned int i=0; i<adc.size(); i++)
        data->push_back( plrsBaseData( int( adc[i]) ) );

    PushToBuffer( addr_nxt, rdo);
}



void SerialDAQ::PostEvent(){}



void SerialDAQ::PostRun(){
    Print( "DAQ stopping\n", DETAIL);
}



int SerialDAQ::ReadADC( int pos, int neg){
    char p = pos+'0';  // 0x30 is 0 in ascii
    char q = neg<0 ? 'N' : neg+'a';  // 0x61 is a in ascii
//    port.serial_write( &p, 1);
//    port.serial_write( &q, 1);

    char r = 'r';
    port.serial_write( &r, 1);

    char buffer[12];
    int nbytes = -1;
    while( nbytes<0 ){
        nbytes = port.serial_read( buffer, 12);
        if( GetState()!=RUN )
            break;
    }

    if( nbytes>=0 )
    buffer[nbytes] = '\0';

    return atoi( buffer );
}


int SerialDAQ::ReadSPI( ){
    char p = 's';  // 0x30 is 0 in ascii
    port.serial_write( &p, 1);

    char buffer[12];
    int nbytes = -1;
    while( nbytes<0 ){
        nbytes = port.serial_read( buffer, 12);
        if( GetState()!=RUN )
            break;
    }

    if( nbytes>=0 )
    buffer[nbytes] = '\0';

    return atoi( buffer );
}
