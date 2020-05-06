
#include "plrsSocket.h"
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>

plrsSocket::plrsSocket() : descriptor(-1) {
    sock_addr = 0;
    memset( &soc_addr_un, 0, sizeof(soc_addr_un));
    memset( &soc_addr_in, 0, sizeof(soc_addr_in));
}


int plrsSocket::InitSocUnix( string filename){

    memset( &soc_addr_un, 0, sizeof(soc_addr_un));

    if( filename=="" ){
        descriptor = -1;
        return descriptor;
    }

    descriptor = socket( AF_UNIX, SOCK_STREAM, 0);
    if( descriptor<0 )
        return -1;

    soc_addr_un.sun_family = AF_UNIX;
    sock_addr = (struct sockaddr*) &soc_addr_un;

    strncpy( soc_addr_un.sun_path, filename.c_str(), sizeof( soc_addr_un.sun_path)-1);
    unlink( soc_addr_un.sun_path);

    return descriptor;
}


int plrsSocket::InitSocInet( int port, string hostname){

    descriptor = socket( AF_INET, SOCK_STREAM, 0);

    if( descriptor<0 ){
        sock_addr = 0;
        return -1;
    }

    memset( &soc_addr_in, 0, sizeof(soc_addr_in));

    soc_addr_in.sin_family = AF_INET;
    soc_addr_in.sin_port = htons( port);

    if( hostname=="" ){
        soc_addr_in.sin_addr.s_addr = INADDR_ANY;
            // host mode
    }
    else{
        struct hostent* server = gethostbyname( hostname.c_str() );
        if( server==0 ){
            std::cerr << "Error getting host by name" << std::endl;
        }

        struct in_addr **addr_list = (struct in_addr**) server->h_addr_list;
        char ip[16];
        strcpy( ip, inet_ntoa( *addr_list[0]) );

        std::cout << "Initializing socket to host " << server->h_name <<", address " << ip << std::endl;
        if( inet_pton( AF_INET, ip, &soc_addr_in.sin_addr) <=0 ){
            std::cerr << "invalid address" << std::endl;
            return -1;
        }
    }

    sock_addr = (struct sockaddr*) &soc_addr_in;

    return descriptor;
}


int plrsSocket::Bind(){
    int i = bind( descriptor, sock_addr, sizeof(soc_addr_in));
    if( i<0 ){
        std::cerr << "Error binding: " << strerror(errno) << std::endl;
        return -1;
    }
    return i;
}


int plrsSocket::Connect(){
    int i = connect( descriptor, (struct sockaddr*) &soc_addr_in, sizeof(soc_addr_in) );
    if( i<0 ){
        std::cerr << "Error connecting: " << strerror(errno) << std::endl;
        return -1;
    }
    return i;
}


int plrsSocket::SetNonBlock( bool b ){
    if( descriptor<0 )
        return -1;
    int flags = fcntl( descriptor, F_GETFL);
    if( b )
        fcntl( descriptor, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl( descriptor, F_SETFL, flags & ~O_NONBLOCK);
    return 1;
}

int plrsSocket::SetNonBlock( int fd, bool b ){
    if( fd<0 )
        return -1;
    int flags = fcntl( fd, F_GETFL);
    if( b )
        fcntl( fd, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl( fd, F_SETFL, flags & ~O_NONBLOCK);
    return 1;
}



