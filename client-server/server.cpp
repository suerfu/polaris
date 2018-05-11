#include <sys/socket.h>
//#include <sys/types.h>
#include <signal.h>
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
	strncpy( soc_addr.sun_path, "/tmp/1234567_test", sizeof(soc_addr.sun_path)-1);

	unlink( soc_addr.sun_path);
	bind( fd, (struct sockaddr*) &soc_addr, sizeof(soc_addr));

	listen ( fd, 1);

	char buff[1024];
	for(int i=0; i<1024;i++)
		buff[i] = 'a';
	signal(SIGPIPE, SIG_IGN);

	while(1){
		int s2 = accept( fd, 0, 0);
		if(s2==-1)
			continue;
		else{
			for(int i=0; i<1024; i++){
				int s = write( s2, buff, 1024);
				/*if(s>=0){
					sleep(2);
					if(i>=5){
						cout<<"Server writing 0"<<endl;
						s = write( s2, buff, 0);
						sleep(10);
						cout<<"Closing the socket"<<endl;
						close(s2);
						return 0;
					}
				}*/
			}
			close( s2);
		}
	}
	return 0;
}
