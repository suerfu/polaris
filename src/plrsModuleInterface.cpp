#include "plrsModuleInterface.h"
#include "plrsBaseData.h"

#include <sstream>
#include <cstring>

#include <signal.h>


extern "C" plrsModuleInterface* create_plrsModuleInterface( plrsController* c ){ return new plrsModuleInterface(c);}


extern "C" void destroy_plrsModuleInterface( plrsModuleInterface* p ){ delete p;}



plrsModuleInterface::plrsModuleInterface( plrsController* c) : plrsStateMachine(c){}



plrsModuleInterface::~plrsModuleInterface(){;}



void plrsModuleInterface::Configure(){

    string path = "/module/"+GetModuleName()+"/";
    hostname = cparser->GetString( path+"hostname");

    if( cparser->GetString( path+"domain" )=="unix" ){
        string filename = cparser->GetString( path+"path");
        socket.InitSocUnix( filename );
    }
    else if( cparser->GetString( path+"domain" )=="inet" ){

        int port = cparser->GetInt( path+"port", -1);
        if( port<2000 || port>65535)
            Print("Invalid Port number.\n", ERR);

        string hostname = cparser->GetString( path+"hostname");

        socket.InitSocInet( port, hostname );
    }

    if( socket ){
        if( hostname=="" ){
            if( socket.Bind() < 0 ){
                Print("socket bind error\n", ERR);
                SetStatus( ERROR );
            }
            if( socket.Listen() < 0 ){
                Print("socket listen error\n", ERR);
                SetStatus( ERROR );
            }
        }
        else{
            if( socket.Connect()>=0 ){
                Print( "Connect to remote server\n", INFO);
                list_connections.push_back(socket.GetDescriptor() );
            }
        }
        socket.SetNonBlock();
    }
    signal( SIGPIPE, SIG_IGN);
}


void plrsModuleInterface::Run(){

    void* rdo = 0;
    int s2 = -1;

    while( GetState()==RUN && GetStatus()!=ERROR ){
        
        rdo = PullFromBuffer();

        if( rdo==0 ){
            usleep(10000);
            continue;
        }

        if( hostname=="" ){ // server mode
            if( (s2 = socket.Accept())>=0 ){
                socket.SetNonBlock( s2 );
                list_connections.push_back( s2 );
                s2 = -1;
            }
        }
        
        vector<plrsBaseData>* temp = reinterpret_cast< vector<plrsBaseData>*>(rdo);
        stringstream ss;
        for( unsigned int i=0; i<temp->size(); i++){
            ss << (*temp)[i] << ' ';
        }

        for( list<int>::iterator itr=list_connections.begin(); itr!=list_connections.end(); ++itr ){

            int nr = read( *itr, 0, 0);
            if( nr<0 ){
                Print( "closing socket due to: "+string( strerror(errno))+"\n", ERR);
                close( *itr);
                *itr = -1;
                continue;
            }

            int nbytes = write( *itr, ss.str().c_str(), strlen(ss.str().c_str()) );
            if( nbytes<0 ){
                Print( "write error: "+string( strerror(errno))+"\n", ERR);
                close( *itr);
                *itr = -1;
                continue;
            }

            char command[256];
            nbytes = read( *itr, command, 256 );
            if( nbytes<0 ){
                if( errno==EAGAIN )
                    continue;
                else{
                    Print( "read error: "+string( strerror(errno))+"\n", ERR);
                    close( *itr);
                    *itr = -1;
                    continue;
                }
            }
            command[nbytes]='\0';
            Print( string(command), ERR);
            SendUserCommand( string(command) );
        }
        list_connections.remove( -1 );

        PushToBuffer( addr_nxt, rdo );
        rdo = 0;
    }
}


void plrsModuleInterface::PostRun(){
    for( list<int>::iterator itr=list_connections.begin(); itr!=list_connections.end(); ++itr ){
        close( *itr );
    }
}


void plrsModuleInterface::Deconfigure(){ 
    Print( "unconfiguring...\n", DETAIL);
    socket.Close();
}

