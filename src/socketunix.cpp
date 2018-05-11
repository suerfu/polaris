
#include "socketunix.h"

socketunix::socketunix() : descriptor(-1) {
    memset( &soc_addr, 0, sizeof(soc_addr));
    soc_addr.sun_family = AF_UNIX;
}


int socketunix::Initialize( string filename){

    if( filename=="" ){
        descriptor = -1;
        return descriptor;
    }

    descriptor = socket( AF_UNIX, SOCK_STREAM, 0);
    strncpy( soc_addr.sun_path, filename.c_str(), sizeof( soc_addr.sun_path)-1);
    unlink( soc_addr.sun_path);

    return descriptor;
}

int socketunix::SetNonBlock( bool b ){
    if( descriptor<0 )
        return -1;
    int flags = fcntl( descriptor, F_GETFL);
    if( b )
        fcntl( descriptor, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl( descriptor, F_SETFL, flags & ~O_NONBLOCK);
    return 1;
}

int socketunix::SetNonBlock( int fd, bool b ){
    if( fd<0 )
        return -1;
    int flags = fcntl( fd, F_GETFL);
    if( b )
        fcntl( fd, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl( fd, F_SETFL, flags & ~O_NONBLOCK);
    return 1;
}



