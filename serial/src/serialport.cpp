
#include "serialport.h"


serialport::serialport(){
	handle = -1;
	id_error = 0;
}



serialport::~serialport(){
	if( handle>=0 )
		serial_close();
}



void serialport::set_baud( speed_t rate = B9600){
    if( handle>=0){
        cfsetispeed( &setting, rate);
        apply();
    }
}



speed_t serialport::get_baud(){
    return cfgetispeed( &setting);
}



void serialport::apply(){
    if( handle>0 )
        tcsetattr( handle, TCSANOW, &setting);
}



void serialport::set_raw( ){
    cfmakeraw( &setting);
    set_iflag( IGNPAR, true);

    set_spchar( VTIME, 1);
    set_spchar( VMIN, 1);

    apply();
}



void serialport::set_iflag( tcflag_t flag, bool on){
    if( on )
        setting.c_iflag |= flag;
    else
        setting.c_iflag &= ~flag;
    apply();
}



void serialport::set_oflag( tcflag_t flag, bool on){
    if( on )
        setting.c_oflag |= flag;
    else
        setting.c_oflag &= ~flag;
    apply();
}



void serialport::set_cflag( tcflag_t flag, bool on){
    if( on )
        setting.c_cflag |= flag;
    else
        setting.c_cflag &= ~flag;
    apply();
}



void serialport::set_lflag( tcflag_t flag, bool on){
    if( on )
        setting.c_lflag |= flag;
    else
        setting.c_lflag &= ~flag;
    apply();
}



void serialport::set_spchar( unsigned int special_char, char c){
    setting.c_cc[ special_char ] = c;
    apply();
}



int serialport::serial_read( char* buffer, int bytes_to_rd){
    int rd = read( handle, buffer, bytes_to_rd);
	id_error = rd<0 ? errno : 0;
    return rd;
}



int serialport::serial_write( char* buffer, int bytes_to_wr){
    int wr = write( handle, buffer, bytes_to_wr);
	id_error = wr<0 ? errno : 0;
    return wr;
}



//serialport::serialport( const serialport& ){}



//serialport& serialport::operator=( const serialport&){}



int serialport::serial_open( const char* file_name, int open_flag){
	if(handle>=0)
		serial_close();     // close previously opened port.

    handle = open( file_name, open_flag);

    if( handle>=0 ){
        tcflush( handle, TCIOFLUSH);
        tcgetattr( handle, &prev_setting);  // store previous settings.
		id_error = 0;
        return 0;
    }
    else{
        id_error = errno;   // store status of open.
        return errno;
    }
}



int serialport::serial_close(){

    tcflush( handle, TCIOFLUSH);
    tcsetattr( handle, TCSANOW, &prev_setting);

    int status = close( handle);

    if( status==0){
		handle = -1;
		id_error = 0;
        return status;
	}
    else{
        id_error = errno;
        return errno;
    }
}



int serialport::get_errid(){
	return id_error;
}

std::string serialport::get_errmsg(){
    switch (id_error){
        case ENOENT: return "device file does not exist";
        case ENXIO : return "physical device not present";
        case EACCES: return "access permission denied";
        case EMFILE: return "too many files open";
        case EBUSY : return "device in use by other programs";
        default: return "";
    }
}


