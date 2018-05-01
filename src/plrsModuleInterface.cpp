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



plrsModuleInterface::plrsModuleInterface( plrsController* c) : plrsStateMachine(c){
    descriptor = -1;
}



plrsModuleInterface::~plrsModuleInterface(){;}



void plrsModuleInterface::Configure(){

    string path = "/module/"+GetModuleName()+"/";
    struct sockaddr_un soc_addr;

    if( cparser->GetString( path+"type" )=="unix" ){

        descriptor = socket( AF_UNIX, SOCK_STREAM, 0);

        memset( &soc_addr, 0, sizeof(soc_addr));
        soc_addr.sun_family = AF_UNIX;

        path = "/module/"+GetModuleName()+"/path";
        strncpy( soc_addr.sun_path, path.c_str(), sizeof( soc_addr.sun_path)-1);
        unlink( soc_addr.sun_path);
    }

    if( descriptor>=0){

        if ( bind( descriptor, ( struct sockaddr*) &soc_addr, sizeof(soc_addr)) < 0 ){
            Print("socket bind error\n", ERR);
            SetStatus( ERROR );
        }

        if( listen( descriptor, 5) < 0 ){
            Print("socket listen error\n", ERR);
            SetStatus( ERROR );
        }

        int flags = fcntl( descriptor, F_GETFL);
        fcntl( descriptor, F_SETFL, flags | O_NONBLOCK);
    }
}


void plrsModuleInterface::Run(){

    void* rdo = 0;

    int s2;

    while( GetState()==RUN && GetStatus()!=ERROR ){
        
        while( rdo==0 ){
            usleep(10000);
            rdo = PullFromBuffer();
        }


        if( ( s2 = accept( descriptor, 0, 0))<0 ){
            sleep(1);
            continue;
        }
        
//        write( s2, buff, 13);
        s2 = -1;

    }
    close( s2 );
}


void plrsModuleInterface::Deconfigure(){ 
    Print( "unconfiguring...\n", DETAIL);
    if( descriptor>=0 )
        close( descriptor);
}


