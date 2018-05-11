#ifndef SOCKETUNIX_H
    #define SOCKETUNIX_H 1

#include <sys/socket.h>
#include <signal.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

using std::string;


class socketunix{

public:

    socketunix();

    ~socketunix(){};

    int Initialize( string filename);

    int GetDescriptor(){ return descriptor;}

    int Bind(){ return bind( descriptor, (struct sockaddr*) &soc_addr, sizeof(soc_addr));}

    int Listen( int n){ return listen( descriptor, n);}

    int SetNonBlock( bool b=true);

    int SetNonBlock( int fd, bool b=true);

    int Accept(){
        return accept( descriptor, 0, 0);
    }
        //!< returns new descriptors.

    int Write( int fd, char* buff, int nbyte){
        return write( fd, buff, nbyte);
    }

    int Read( int fd, char* buff, int nbyte){
        return read( fd, buff, nbyte);
    }

    void Close(){
        if( descriptor>=0 )
            close( descriptor);
    }

    operator bool(){
        if( descriptor<0 )
            return false;
        else
            return true;
    }

private:

    int descriptor;

    struct sockaddr_un soc_addr;

};


#endif


