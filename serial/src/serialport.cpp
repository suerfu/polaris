#include "serialport.h"

serialport::serialport(){
	handle = -1;
	id_error = 0;
}


serialport::~serialport(){
	if( handle>=0 )
		serial_close();
}

int serialport::serial_open( const char* file_name, int open_flag){
	if(handle>=0)
		serial_close();
			// close previously opened port.
    handle = open( file_name, open_flag);
    if( handle>=0 ){
        tcgetattr( handle, &prev_setting);
            // store previous settings.
		id_error = 0;
        return 0;
    }
    else{
        id_error = errno;
            // store status of open.
        return -1;
    }
}

int serialport::serial_close(){
    tcsetattr( handle, TCSANOW, &prev_setting);
    tcflush( handle, TCOFLUSH);
    int status = close( handle);
    if( status==0){
		handle = -1;
		id_error = 0;
        return status;
	}
    else{
        id_error = errno;
        return -1;
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

void serialport::apply_setting(){
    if( handle>0 ){
        tcsetattr( handle, TCSANOW, &setting);
        tcflush( handle, TCOFLUSH);
    }
}

void serialport::set_baud( speed_t rate = B9600){
    if( handle>0)
        cfsetispeed( &setting, rate);
}

speed_t serialport::get_baud(){
    return cfgetispeed( &setting);
}

termios* serialport::get_termios(){
    return &setting;
}

void serialport::set_raw( ){
    cfmakeraw( &setting);
    setting.c_cc[VTIME] = 2;
    setting.c_cc[VMIN] = 1;
    apply_setting();
}

void serialport::set_text(){
    set_input();
    set_output();
    set_control();
    set_local();
    apply_setting();
}

void serialport::set_input(){
    setting.c_iflag &= ~IGNBRK;
    setting.c_iflag |= ( BRKINT | IGNPAR);
        // don't ignore break and ignore parity
        // send SIGINT upon break condition
    setting.c_iflag &= ~(INPCK | ISTRIP | PARMRK);
        // no parity check on input, no mark parity error
    setting.c_iflag &= ~(IGNCR | INLCR | ICRNL);
        // don't ignore CR, no special character mapping
    setting.c_iflag &= ~( IXON | IXOFF | IXANY);
        // no flow control
    apply_setting();
}

void serialport::set_output(){
    setting.c_oflag |= (OPOST | ONLRET);
        // output post-processing
        // NL will perform CR function as well
    setting.c_oflag &= ~ (ONLCR | OCRNL | OFILL );
        // don't map lower case to upper case and vise versa
    setting.c_oflag |= ONOCR;
        // don't output cr on first column.
    apply_setting();
}

void serialport::set_control( ){
    setting.c_cflag &= ~PARENB;
    setting.c_cflag &= ~CSIZE;    // first clear all character size
    setting.c_cflag |= CS8 | CLOCAL;
    setting.c_cflag &= ~CSTOPB;   // one stop bit
    apply_setting();
}

void serialport::set_local( ){
    setting.c_lflag |= (ICANON | ECHO | ECHOE);
        // non-canonical, input immediately available
    apply_setting();
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
