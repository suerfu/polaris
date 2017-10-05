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

	int get_errid();
		// return error ID. 0 for success.	
    std::string get_errmsg();
        // get error message if there is any.

    void set_raw();
        // configure port for raw mode
    void set_text();
        // configure port for text application

    void set_input();
        // input configuration for text mode
    void set_output();
        // output configuration for text mode
    void set_control();
        // control configuration for text mode
    void set_local();
        // local processing for test mode

    void set_baud( speed_t);
        // set baud rate
    speed_t get_baud();
        // return baud rate

    termios* get_termios();
        // this will allow modification of setting directly for custom setting.
    void apply_setting();
        // apply settings. made public so that custom setting can be applied.

    int serial_read( char* buffer, int bytes_to_rd);
        // returns number of bytes read
    int serial_write( char* buffer, int bytes_to_wr);
        // returns number of bytes written.

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
