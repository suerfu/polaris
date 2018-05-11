#include <sys/socket.h>
//#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
using namespace std;

int main(){

	int fd;
	fd = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un soc_addr;
	soc_addr.sun_family = AF_UNIX;
	strncpy( soc_addr.sun_path, "/tmp/12345", sizeof(soc_addr.sun_path)-1);

	char buffer[256];

    int ctr = 0;

	if( connect( fd, (struct sockaddr*) &soc_addr, sizeof(soc_addr))==-1 ){
		cerr<<"Connection failed."<<endl;
		return 0;
	}
	else{
		cout<<"Connection to server established!"<<endl;
		int n = -1;
		while(1){
			n = read( fd, buffer, 256);
			if(n>0){
                buffer[n] = '\0';
				cout<<"Received: "<<buffer<<endl;
                ctr++;
            }
			else if(n==0){
				cout<<"End-of-file received"<<endl;
				break;
			}
			else{
				cout<<"Error received"<<endl;
			}
            if( ctr==10 ){
                char cm[] = "/ctrl/print";
                write( fd, cm, strlen(cm));
            }
            else if( ctr==100){            
                char cr[] = "/ctrl/quit";
                write( fd, cr, strlen(cr));
            }
		}
	}

	return 0;
}
