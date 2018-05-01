#include "PyCMapDAQ.h"

#include "plrsBaseData.h"

#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>




/// Constructor. buff_depth will control depth of FIFO buffer.
PyCMapDAQ::PyCMapDAQ( plrsController* ctrl) : plrsModuleDAQ( ctrl){
    buff_depth = 100;
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
    if( range_az[0]>range_az[1] ){
        int temp = range_az[0];
        range_az[0] = range_az[1];
        range_az[1] = temp;
    }

    scan_ax.clear();
    scan_az.clear();
    for( int az=range_az[0]; az<=range_az[1];){
        for( int ax=range_ax[0]; ax<=range_ax[1]; ax++ ){
            scan_ax.push_back( ax );
            scan_az.push_back( az );
        }
        az++;
        for( int ax=range_ax[1]; ax>=range_ax[0]; ax-- ){
            scan_ax.push_back( ax );
            scan_az.push_back( az );
        }
        az++;
    }

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

    LaserOn( true );
    MotorOn( true );

    start_time = ctrl->GetTimeStamp();
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
            usleep(100000);
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
    data->push_back( plrsBaseData( int(ctrl->GetTimeStamp()-start_time)) );
    data->push_back( plrsBaseData( scan_ax.back() ));
    data->push_back( plrsBaseData( scan_az.back() ));
    data->push_back( plrsBaseData( avg ));
    data->push_back( plrsBaseData( std_error ));
    data->push_back( plrsBaseData( std_dev ));

    PushToBuffer( addr_nxt, rdo);

    scan_ax.pop_back();
    scan_az.pop_back();
}



void PyCMapDAQ::PostEvent(){
    if( scan_ax.size()==0 )
        PushCommand( 0, "quit");
    sleep(1);
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
        if( GetStatus()!=RUN )
            break;
    }
}


void PyCMapDAQ::ZeroAz(){
    Print("Resetting azimuthal coordinate\n", DETAIL);
    char c = 'Z';
    while( GetAz()!=0 ){
        port.serial_write( &c, 1);
        if( GetStatus()!=RUN )
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



// Get axial coordinate in integer.
// Procedure:
// send command p(lower case) to the hardware, wait for 20 ms.
// If no response, repeat above.
int PyCMapDAQ::GetAx(){
//    ctrl->UpdateWDTimer();
    char c = 'p';
    char buff[10];
    int nbytes = -1;
    while( nbytes<=0 ){
        port.serial_write( &c, 1);
        usleep( 40000 );
        nbytes = port.serial_read( buff, 10);
        if( GetStatus()!=RUN )
            break;
    }
    buff[nbytes] = '\0';
    return atoi( buff );
}



// Get azimuthal coordinate in integer.
// Procedure:
// send command P(upper case) to the hardware, wait for 20 ms.
// If no response, repeat above.
int PyCMapDAQ::GetAz(){
//    ctrl->UpdateWDTimer();
    char c = 'P';
    char buff[10];
    int nbytes = -1;
    while( nbytes<=0 ){
        port.serial_write( &c, 1);
        usleep( 40000 );
        nbytes = port.serial_read( buff, 10);
        if( GetStatus()!=RUN )
            break;
    }
    buff[nbytes] = '\0';
    return atoi( buff );
}




void PyCMapDAQ::MoveTo( int ax, int az){
    stringstream ss;
    ss << "Moving to coordinate (" << ax << ", " << az <<")\n";
    Print(ss.str(), DETAIL);

    int pos_ax = GetAx();
    int pos_az = GetAz();

    while( ax!=pos_ax || az!=pos_az){
        if( ax!=pos_ax )
            Move( ax>pos_ax );
        if( az!=pos_az )
            Rotate( az>pos_az );
        usleep(50000);
        pos_ax = GetAx();
        pos_az = GetAz();
        if( GetStatus()!=RUN )
            break;
    }

    ss.str( std::string() );
    ss << "Moved to coordinate (" << ax << ", " << az <<")\n";
    Print(ss.str(), DETAIL);
}



int PyCMapDAQ::ReadADC(){
    char c = 'r';
    char buff[10];
    int nbytes = -1;
    while( nbytes<=0 ){
        port.serial_write( &c, 1);
        usleep( 20000 );
        nbytes = port.serial_read( buff, 10);
        if( GetStatus()!=RUN )
            break;
    }
    buff[nbytes] = '\0';
    return atoi( buff );
}



float PyCMapDAQ::GetAvg( vector<int> input ){
    return std::accumulate( input.begin(), input.end(), 0.0)/input.size();
}



float PyCMapDAQ::GetVar( vector<int> input){
    if( input.size()<2)
        return 0.;
    float avg = GetAvg( input );
    for( unsigned int i=0; i<input.size(); i++){
        input[i] = (input[i] - avg) * (input[i] - avg);
    }
    return std::accumulate( input.begin(), input.end(), 0.0)/(input.size()-1);
}


bool PyCMapDAQ::QualityControl( vector<int> input, float thresh ){
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
