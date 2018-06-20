#include "PyCMapDAQ.h"

#include "plrsBaseData.h"

#include <sstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <numeric>
#include <algorithm>


/// Constructor. buff_depth will control depth of FIFO buffer.
PyCMapDAQ::PyCMapDAQ( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_depth = 100;

    unit_mm = false;

    ax_step_to_mm = 1;
    az_step_to_mm = 1;
}


/// Destructor. Nothing needs to be done.
PyCMapDAQ::~PyCMapDAQ(){}



void PyCMapDAQ::Configure(){

    Print("Configuring serial port...\n", DETAIL);

    string fname = cparser->GetString("/module/"+GetModuleName()+"/port");
    if( fname=="" ){
        fname = "ttyS0";
        Print( "Cannot find /module/"+GetModuleName()+"/port, using ttyS0\n", ERR);
    }
    else
        Print( "Opening "+fname+" for data acquisition\n", DETAIL);

    if( port.serial_open( fname.c_str() )!=0 ){
        Print( "Error: "+port.get_errmsg()+"\n", ERR);
        SetStatus( ERROR );
        return;
    }

    port.set_cooked();
    port.set_cflag( CLOCAL, true);
    port.set_baud( B9600 );

    Print("Serial port configured.\n", DETAIL);
    
    Print("Configuring ADC parameters.\n",DETAIL);

    for( int i=0; i<buff_depth; ++i ){
        int id = ctrl->GetIDByName( this->GetModuleName() );
        PushToBuffer( id, new vector< plrsBaseData> );
    }

    if( !cparser->Find("/module/"+GetModuleName()+"/range_ax") || !cparser->Find("/module/"+GetModuleName()+"/range_az") ){
        Print( "Error: either axial or azimuthal range of scan not specified.\n", ERR);
        SetStatus( ERROR );
        return;
    }

    vector<int> range_ax = cparser->GetIntArray("/module/"+GetModuleName()+"/range_ax");
    vector<int> range_az = cparser->GetIntArray("/module/"+GetModuleName()+"/range_az");

    float stp_ax = cparser->GetFloat("/module/"+GetModuleName()+"/step_ax", 10);
    float stp_az = cparser->GetFloat("/module/"+GetModuleName()+"/step_az", 5);

    // if unit is specified, then use mm and degree for everything and do conversion.

    // The axial rail has screw pitch of 5 mm, and motor has 200 steps.
    // Axial travel is 0.025 mm per step.

    // The azimuthal motor has 200 steps, giving a 1.8 degree rotation per step.
    // The timing belt has 0.08" pitch, and the timing pulley has 24 teeth per rotation.
    // Azimuthal travel is 0.24384 mm per step.


    if( cparser->GetBool( "/module/"+GetModuleName()+"/unit_mm", false) ){
        ax_step_to_mm = 0.025;
        az_step_to_mm = 0.24384;
        unit_mm = true;
        range_ax[0] /= ax_step_to_mm;
        range_ax[1] /= ax_step_to_mm;
        stp_ax /= ax_step_to_mm;
        range_az[0] /= az_step_to_mm;
        range_az[1] /= az_step_to_mm;
        stp_az /= az_step_to_mm;
    }

    int step_ax = int(stp_ax);
    int step_az = int(stp_az);

    if( range_ax.size()!=2 || range_az.size()!=2 ){
        Print("Error: range in wrong format. Specify range with lower and upper bounds\n", ERR );
        SetStatus( ERROR );
        return;
    }
    if( range_ax[0]>range_ax[1] ){
        int temp = range_ax[0];
        range_ax[0] = range_ax[1];
        range_ax[1] = temp;
    }
    range_ax[1] = ((range_ax[1]-range_ax[0])/step_ax)*step_ax;

    if( range_az[0]>range_az[1] ){
        int temp = range_az[0];
        range_az[0] = range_az[1];
        range_az[1] = temp;
    }
    range_az[1] = ((range_az[1]-range_az[0])/step_az)*step_az;

    scan_ax.clear();
    scan_az.clear();

    for( int az=range_az[0]; az<=range_az[1];){
        for( int ax=range_ax[0]; ax<=range_ax[1]; ax+=step_ax ){
            scan_ax.push_back( ax );
            scan_az.push_back( az );
        }
        az += step_az;/*
        for( int ax=range_ax[1]; ax>=range_ax[0]; ax-=step_ax ){
            scan_ax.push_back( ax );
            scan_az.push_back( az );
        }
        az += step_az;*/
    }
    std::reverse( scan_ax.begin(), scan_ax.end());
    std::reverse( scan_az.begin(), scan_az.end());

    navg = cparser->GetInt("/module/"+GetModuleName()+"/navg", 100);

    if( cparser->Find("/module/"+GetModuleName()+"/offset_ax") )
        offset_ax = cparser->GetInt("/module/"+GetModuleName()+"/offset_ax", 0);
    if( cparser->Find("/module/"+GetModuleName()+"/offset_az") )
        offset_az = cparser->GetInt("/module/"+GetModuleName()+"/offset_az", 0);

    drift_threshold = cparser->GetFloat( "/module/"+GetModuleName()+"/drift_threshold", -1);

    Print("ADC configured.\n",DETAIL);
}



void PyCMapDAQ::Deconfigure(){
    Print( "Closing serial port...\n", DETAIL);
    port.serial_close();
}



// In PreRun, Zero current position, move by the offset and zero again.
void PyCMapDAQ::PreRun(){
    Print( "DAQ starting\n", DETAIL);

    if( cparser->GetString("/cmdl/zero") + cparser->GetString("/module/"+GetModuleName()+"/zero") !="" ){
        Print( "zero-ing coordinate.\n", DETAIL);
        ZeroAx();
        ZeroAz();
    }

    MotorOn( true );
    LaserOn( true );

    start_time = ctrl->GetMSTimeStamp();
}



void PyCMapDAQ::Run(){
    while( GetState()==RUN){
        if( scan_az.size()==0 )
            break;
        PreEvent();
        Event();
        PostEvent();

        sched_yield();
    }
}


void PyCMapDAQ::PreEvent(){
    MoveTo( scan_ax.back()+offset_ax, scan_az.back()+offset_az);
}



void PyCMapDAQ::Event(){

    void* rdo = 0;
    rdo = PullFromBuffer();
    if( rdo==0 ){
        Print( "Waiting for buffer\n", DETAIL);
        usleep(100000);
        return;
    }
    
    vector<int> adc;
    while( GetState()==RUN ){
        adc.clear();
        for( int i=0; i<navg; i++){
            adc.push_back( ReadADC() );
            //usleep(10000);
            if( GetState()!=RUN)
                break;
        }
        if( QualityControl( adc, drift_threshold ) )
            break;
    }
    if( GetState()!=RUN )
        return;

    float avg = GetAvg( adc );
    float std_dev = sqrt( GetVar( adc ) );
    float std_error = std_dev/sqrt( adc.size() );


    vector<plrsBaseData>* data = reinterpret_cast< vector<plrsBaseData>*>(rdo);
    data->clear();
    data->push_back( plrsBaseData( int(ctrl->GetMSTimeStamp()-start_time)) );
    data->push_back( plrsBaseData( scan_ax.back() * ax_step_to_mm ));
    data->push_back( plrsBaseData( scan_az.back() * az_step_to_mm ));
    data->push_back( plrsBaseData( avg ));
    data->push_back( plrsBaseData( std_error ));
    data->push_back( plrsBaseData( std_dev ));

    PushToBuffer( addr_nxt, rdo);

    scan_ax.pop_back();
    scan_az.pop_back();
}



void PyCMapDAQ::PostEvent(){
    if( scan_ax.size()==0 ){
        PushCommand( 0, "quit");
        usleep(10000);
    }
}



void PyCMapDAQ::PostRun(){
    Print( "DAQ finished\n", DETAIL);

    LaserOn( false );
    MotorOn( false );
}



void PyCMapDAQ::ZeroAx(){
    Print("Resetting axial coordinate\n", DETAIL);
    char c = 'z';
    while( GetAx()!=0 ){
        port.serial_write( &c, 1);
        if( GetState()!=RUN )
            break;
    }
}


void PyCMapDAQ::ZeroAz(){
    Print("Resetting azimuthal coordinate\n", DETAIL);
    char c = 'Z';
    while( GetAz()!=0 ){
        port.serial_write( &c, 1);
        if( GetState()!=RUN )
            break;
    }
}


// Turn on/off laser.
// Repeat 3 times with time interval to guarantee laser turns on.
void PyCMapDAQ::LaserOn( bool on ){
    char c = on ? 'l' : 'k';
    for( int i=0; i<3; i++){
        port.serial_write( &c, 1 );
        usleep(10000);
    }
    if( on )
        Print("Laser on\n", DETAIL);
    else
        Print("Laser off\n", DETAIL);
}


// Engage/Disengage motor.
// Repeat 3 times with time interval to guarantee laser turns on.
void PyCMapDAQ::MotorOn( bool on ){
    char c = on ? 'e' : 'E';
    for( int i=0; i<3; i++){
        port.serial_write( &c, 1 );
        usleep(10000);
    }
    if( on )
        Print("Motor engaged\n", DETAIL);
    else
        Print("Motor dis-engaged\n", DETAIL);
}




// Get azimuthal coordinate and compute destination.
// Next send command to device and re-get coordinate
// Repeat above until moved to the final position.
void PyCMapDAQ::Rotate( bool cw ){
    char c = cw ? 'c' : 'C';
    port.serial_write( &c, 1);
}



// Get axial coordinate and compute destination.
// Next send command to device and re-get coordinate
// Repeat above until moved to the final position.
void PyCMapDAQ::Move( bool fw ){
    char c = fw ? 'f' : 'F';
    port.serial_write( &c, 1);
}



int PyCMapDAQ::GetResponse( char c ){

    char buff[32];

    int nbytes = 1;
    while( nbytes>0 ){
        usleep( 10000 );
        nbytes = port.serial_read( buff, 32);
        if( GetState()!=RUN )
            break;
    }

    nbytes = -1;
    while( nbytes<=0 ){

        port.serial_write( &c, 1);

        usleep( 10000 );
        nbytes = port.serial_read( buff, 32);

        if( nbytes>0 )
            break;

        usleep( 50000 );
        nbytes = port.serial_read( buff, 32);

        if( GetState()!=RUN )
            break;
    }
    if( nbytes>0 )
        buff[nbytes] = '\0';
    return atoi( buff );
}



// Get axial coordinate in integer.
// Procedure:
// send command p(lower case) to the hardware, wait for 20 ms.
// If no response, repeat above.
int PyCMapDAQ::GetAx(){
    return GetResponse('p');
}



// Get azimuthal coordinate in integer.
// Procedure:
// send command P(upper case) to the hardware, wait for 20 ms.
// If no response, repeat above.
int PyCMapDAQ::GetAz(){
    return GetResponse('P');

}




void PyCMapDAQ::MoveTo( int ax, int az){
    int pos_ax = GetAx();
    int pos_az = GetAz();

    stringstream ss;
    ss << "moving to coordinate (" << ax << ", " << az <<") from ("<<pos_ax<<", "<<pos_az<<")\n";
    Print(ss.str(), DETAIL);

    while( ax!=pos_ax || az!=pos_az){
        if( ax!=pos_ax )
            Move( ax>pos_ax );
        if( az!=pos_az )
            Rotate( az>pos_az );
        usleep(1000);
        pos_ax = GetAx();
        pos_az = GetAz();

//        ss.str( std::string() );
//        ss << "moved to coordinate (" << pos_ax << ", " << pos_az <<")\n";
//        Print(ss.str(), DETAIL);

        if( GetState()!=RUN ){
            Print("run interrupted...\n", DETAIL);
            return;
        }
    }

    ss.str( std::string() );
    ss << "moved to coordinate (" << ax << ", " << az <<")\n";
    Print(ss.str(), DETAIL);
}



int PyCMapDAQ::ReadADC(){
    char c = 'r';
    char buff[10];
    int nbytes = -1;
    while( nbytes<=0 ){
        port.serial_write( &c, 1);
        usleep( 10000 );
        nbytes = port.serial_read( buff, 10);
        if( GetState()!=RUN )
            break;
    }
    buff[nbytes] = '\0';
    return atoi( buff );
}



float PyCMapDAQ::GetAvg( vector<int> input ){
    return accumulate( input.begin(), input.end(), 0.0)/input.size();
}



float PyCMapDAQ::GetVar( vector<int> input){
    if( input.size()<2)
        return 0.;
    float avg = GetAvg( input );
    for( unsigned int i=0; i<input.size(); i++){
        input[i] = (input[i] - avg) * (input[i] - avg);
    }
    return accumulate( input.begin(), input.end(), 0.0)/(input.size()-1);
}


bool PyCMapDAQ::QualityControl( vector<int> input, float thresh ){
    return true;
    float sum_x(0), sum_x2(0), sum_y(0), sum_y2(0), sum_xy(0);
    float k(0), b(0);

    for( unsigned int i=0; i<input.size(); i++){
        sum_x += i;
        sum_y += input[i];
        sum_x2 += i*i;
        sum_y2 += input[i]*input[i];
        sum_xy += i*input[i];
    }

    k = (input.size()*sum_xy-sum_x*sum_y)/(input.size()*sum_x2 - sum_x*sum_x);
    b = (sum_x2*sum_y-sum_x*sum_xy)/(input.size()*sum_x2-sum_x*sum_x);

    if( thresh > 0 && fabs(k)<thresh )
        return true;

    float s = 0;

    for( unsigned int i=0; i<input.size(); i++){
        s += (input[i]-k*i-b) *  (input[i]-k*i-b);
    }
    s /= input.size()-2;
    s = sqrt(s);

    float err_k = s*sqrt( input.size()/(input.size()*sum_x2-sum_x*sum_x));

    stringstream ss;
    ss << "slope is " << k <<", error is " << err_k <<"\n";
    Print( ss.str(), DETAIL);

    if( 0>k-err_k && 0<k+err_k )
        return true;
    else{
        Print("drift detected. Repeating measurement.\n", DETAIL);
        return false;
    }
}
