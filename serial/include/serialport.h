#ifndef SERIALPORT_H
#define SERIALPORT_H 1

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>

#include <string>

class serialport{

public:
    serialport();

	serialport( const char*, int open_flag = O_RDWR | O_NOCTTY);

    ~serialport();

    int serial_open( const char*, int open_flag = (O_RDWR | O_NOCTTY | O_NDELAY) );
        // possible flags are O_RDONLY, O_WRONLY, O_RDWR, O_NDELAY
        // return 0 on success.

    int serial_close();
        // return 0 on success.

    void set_raw();
        // configure port for raw mode

    void set_iflag( tcflag_t flag, bool);
        // input configuration for text mode
    void set_oflag( tcflag_t flag, bool);
        // output configuration for text mode
    void set_cflag( tcflag_t flag, bool);
        // control configuration for text mode
    void set_lflag( tcflag_t flag, bool);
        // local processing for test mode
    void set_spchar( unsigned int , char c);
        // set special characters
        // VMIN, VTIME, etc.


    void set_baud( speed_t);
        // set baud rate

    speed_t get_baud();
        // return baud rate

    void apply();
        // apply settings. made public so that custom setting can be applied.

    int serial_read( char* buffer, int bytes_to_rd);
        // returns number of bytes read

    int serial_write( char* buffer, int bytes_to_wr);
        // returns number of bytes written.

	int get_errid();
		// return error ID. 0 for success.	

    std::string get_errmsg();
        // get error message if there is any.


private:

    int handle;
        // file descriptor.
    int id_error;
        // error id from previous operation.

    termios setting;
        // settings to be applied to the open terminal.
    termios prev_setting;
        // store previous setting upon finish.

    serialport( const serialport& );
    serialport& operator=( const serialport&);
        // disable copy constructor and assignment operator
};

#endif


/*
termios_p->c_iflag &= 
			~(IGNBRK | BRKINT | PARMRK | ISTRIP
            | INLCR | IGNCR | ICRNL | IXON);

	// ignore break signal ( such as ctrl + c )
	// if BRKINT is not set, will send SIGINT
	// missing here is IGNPAR - ignore paraty error
	// PARMRK - mark paraty errors
	// ISTRIP - strip 8th bit
	// INLCR - translate NL to CR on input
	// IGNCR - ignore CR
	// IXON - flow control

           termios_p->c_oflag &= ~OPOST;
	// no output special character processing


           termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	// ECHO - disable echo character back
	// ICANON - character is available line by line for line editing

           termios_p->c_cflag &= ~(CSIZE | PARENB);
           termios_p->c_cflag |= CS8;
	// CSIZE will turn character size off
	// CS8 - sets character size


*/
