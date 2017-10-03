#include "serialport.h"
#include <iostream>
using namespace std;

int main( int argc, char* argv[]){
    if( argc<2) return 0;

    serialport port;
    port.set_raw();
    port.set_baud( B9600);
    int a = port.serial_open( argv[1], O_RDWR | O_NOCTTY);
    if( a < 0 ){
        cout << port.get_errmsg() << endl;
        return -1;
    }

    char c = 0xe;
    port.serial_write( &c, 1);
    for( int j=0; j<100; j++){
        int b = port.serial_read( &c, 1);
        if( b>=0 ){
            cout<<"Got "<<(int)c<<endl;
        }
        else{
            cout << "Did not read anything"<<endl;
        }
    }
    c = 0xff;
    port.serial_write( &c, 1);
    for( int j=0; j<10; j++){
        int b = port.serial_read( &c, 1);
        if( b>=0 ){
            cout<<"Got "<<(int)c<<endl;
        }
        else{
            cout << "Did not read anything"<<endl;
        }
    }
    port.serial_close();
    return 0;
    
}
