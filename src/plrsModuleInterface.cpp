#include "plrsModuleInterface.h"
#include "plrsBaseData.h"

#include <sstream>
#include <cstring>

#include <signal.h>


extern "C" plrsModuleInterface* create_plrsModuleInterface( plrsController* c ){ return new plrsModuleInterface(c);}


extern "C" void destroy_plrsModuleInterface( plrsModuleInterface* p ){ delete p;}



plrsModuleInterface::plrsModuleInterface( plrsController* c) : plrsStateMachine(c){
    payload = 1000;
}



plrsModuleInterface::~plrsModuleInterface(){;}



void plrsModuleInterface::Configure(){

    string path = "/module/"+GetModuleName()+"/";

    // Find out where to send data.
    // 
    string next_module = GetConfigParser()->GetString( path+"next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }

    // Next get payload.
    // This is the number of data points to send at a time.
    payload = GetConfigParser()->GetInt( path+"payload", payload);


    hostname = GetConfigParser()->GetString( path+"hostname", "");

    // If Unix socket, then get the filename.
    // If internet socket, get IP address and initialize.
    if( GetConfigParser()->GetString( path+"domain" )=="unix" ){
        string filename = GetConfigParser()->GetString( path+"path");
        socket.InitSocUnix( filename );
    }
    else if( GetConfigParser()->GetString( path+"domain" )=="inet" ){

        int port = GetConfigParser()->GetInt( path+"port", -1);
        if( port<2000 || port>65535)
            Print( "Invalid Port number.\n", ERR);

        string hostname = GetConfigParser()->GetString( path+"hostname");
        socket.InitSocInet( port, hostname );
    }
    Print( "Socket initialized.\n", DETAIL);

    // If hostname is not specified, then it is run as a server. Otherwise as a client.
    if( socket ){
        if( hostname=="" ){
            if( socket.Bind() < 0 ){
                Print( "Socket bind error\n", ERR);
                SetStatus( ERROR );
            }
            if( socket.Listen() < 0 ){
                Print( "Socket listen error\n", ERR);
                SetStatus( ERROR );
            }
        }
        else{
            if( socket.Connect()>=0 ){
                Print( "Connect to remote server\n", INFO);
                list_connections.push_back(socket.GetDescriptor() );
            }
            else{
                Print( "Socket failed to connect\n", ERR);
                SetStatus( ERROR );            
            }
        }
        socket.SetNonBlock();
    }
    signal( SIGPIPE, SIG_IGN);
}


void plrsModuleInterface::Run(){

    void* rdo = PullFromBuffer();
    if( rdo==0 ){
        return;
    }

    int s2 = -1;

    if( hostname=="" ){ // server mode
        if( (s2 = socket.Accept())>=0 ){
            socket.SetNonBlock( s2 );
            list_connections.push_back( s2 );
            s2 = -1;
            Print( "Connection accepted.\n", ERR);
        }
    }
    
    int* temp = reinterpret_cast< int*>(rdo);
    data.push_back( *temp );

    if( data.size()>= payload ){
        stringstream ss;
        for( unsigned int i=0; i<data.size(); i++){
            ss << data[i] << ' ';
        }

        for( list<int>::iterator itr=list_connections.begin(); itr!=list_connections.end(); ++itr ){
            // write the data stream to remote.
            int nbytes = write( *itr, ss.str().c_str(), strlen( ss.str().c_str() ) );
            if( nbytes<0 ){
                Print( "Write error: " + string( strerror(errno)) + "\n", ERR);
                close( *itr);
                *itr = -1;
                continue;
            }
        }

        list_connections.remove( -1 );            // -1 corresponds to broken connection.
        data.clear();
    }

    PushToBuffer( next_addr, rdo );

}


void plrsModuleInterface::PostRun(){
    for( list<int>::iterator itr=list_connections.begin(); itr!=list_connections.end(); ++itr ){
        close( *itr );
    }
}


void plrsModuleInterface::Deconfigure(){ 
    Print( "Deconfiguring...\n", DETAIL);
    socket.Close();
}

