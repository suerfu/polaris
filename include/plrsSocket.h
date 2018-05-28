#ifndef PLRSSOCKET_H
    #define PLRSSOCKET_H 1

#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>

using std::string;


class plrsSocket{

public:

    plrsSocket();

    ~plrsSocket(){};

    int InitSocUnix( string filename );
        // initialize a unix-domain socket by filename.

    int InitSocInet( int portno, string hostname="" );
        // initialize an internet-domain host.
        // If hostname is specified, will create a client
        // If no server name, will create a server.

    int GetDescriptor(){ return descriptor;}

    int Bind();

    int Connect();

    int Listen( int n=5){ return listen( descriptor, n);}

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

    struct sockaddr* sock_addr;

    struct sockaddr_un soc_addr_un;
        // UNIX domain socket.

    struct sockaddr_in soc_addr_in;
        // Internet domain socket.

    int portno;
        // Port number for Internet domain socket.

};


#endif


