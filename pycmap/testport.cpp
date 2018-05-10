#include "serialport.h"

#include <iostream>
#include <chrono>

#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;

int main( int argc, char* argv[]){

    if( argc<2 ){
        cout << "usage: " << argv[0] << " /dev/ttySx" << endl;
        return -1;
    }

    serialport port;
    if( port.serial_open( argv[1] ) != 0 ){
        cout << "error: " << port.get_errmsg() << endl;
    }

    port.set_cooked();
    port.set_cflag( CLOCAL, true);
    port.set_cflag( CRTSCTS, false);
    port.set_iflag( IXON, false);
    port.set_iflag( IXANY, false);
    port.set_iflag( IUTF8, false);
    port.set_lflag( ICANON, true );
    port.set_baud( B9600 );

    char rbuffer[64];
    char input;
    int nbytes = -1;
    const int max_try = 50000;
    int ctr;
    int ms;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> send_time_ms;
    
    while( cin>>input ){
        port.serial_write( &input, 1);
        send_time_ms = std::chrono::high_resolution_clock::now();
        ctr = 10000;

        while( nbytes<=0 ){
            if( ctr>max_try ){
                cout << "time out error: did not get response" << endl;
                break;
            }
            nbytes = port.serial_read( rbuffer, 64);
            //usleep(100000);
            ctr++;
        }
        if(nbytes<=0)
            continue;

        ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - send_time_ms).count();

        cout << rbuffer << "\n( " << nbytes << " bytes read with " << ms << " ms time delay. )" << endl;
        nbytes = -1;
    }

    return 0;
}
