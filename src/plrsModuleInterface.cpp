#include <sys/socket.h>
#include <signal.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include "plrsModuleInterface.h"

#include <sstream>


/// creator function for loading the module.
extern "C" plrsModuleInterface* create_plrsModuleInterface( plrsController* c ){ return new plrsModuleInterface(c);}



/// destructor function for releasing the module.
extern "C" void destroy_plrsModuleInterface( plrsModuleInterface* p ){ delete p;}



plrsModuleInterface::plrsModuleInterface( plrsController* c) : plrsStateMachine(c){}


void plrsModuleInterface::Configure(){

    descriptor = socket( AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un soc_addr;
    memset( &soc_addr, 0, sizeof(soc_addr));
    soc_addr.sun_family = AF_UNIX;
    strncpy( soc_addr.sun_path, "/tmp/1234567_test", sizeof( soc_addr.sun_path)-1);

    unlink( soc_addr.sun_path);
    if ( bind( descriptor, ( struct sockaddr*) &soc_addr, sizeof(soc_addr)) < 0 )
        Print("socket bind error\n", ERR);

    if( listen( descriptor, 5) < 0 )
        Print("socket listen error\n", ERR);

    int flags = fcntl( descriptor, F_GETFL);
    fcntl( descriptor, F_SETFL, flags | O_NONBLOCK);
//    signal( SIGPIPE, SIG_IGN);
}


void plrsModuleInterface::Run(){

    int s2;

    while( GetState()==RUN ){

        if( ( s2 = accept( descriptor, 0, 0))<0 ){
            cout << "s2 is " << s2 << endl;
            sleep(1);
            continue;
        }
        
        char buff[13] = "Hello World!";
        write( s2, buff, 13);
        close( s2 );
        s2 = -1;
    }
}


void plrsModuleInterface::Deconfigure(){

    Print( "unconfiguring...\n", DETAIL);

}



void plrsModuleInterface::Deinitialize(){

    Print( "cleaning up...\n", DETAIL);

    if( descriptor>=0 )
        close( descriptor);
}



plrsModuleInterface::~plrsModuleInterface(){;}
