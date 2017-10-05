#include <iostream>
#include <pthread.h>
#include <string>
#include <cstdlib>

using namespace std;

class IO{

public:
    IO(){ pthread_mutex_init( &mux, 0); name='\0';}
    ~IO(){}
    void SetName( char c, int i){ name = c; id = i;}
    char GetName(){ return name;}
    void output( string s);
private:
    pthread_mutex_t mux;
    char name;
    int id;
};

//pthread_mutex_t IO::mux = PTHREAD_MUTEX_INITIALIZER;

void IO::output( string s){
    int r = rand()%10;
    pthread_mutex_lock( &mux);
        cout << "fst output called from "<<id<<endl;
        usleep(r);
        cout << "sec output called from "<<id<<endl;
    pthread_mutex_unlock( &mux);
}

void* launch(void* p){
    IO* ptr = reinterpret_cast<IO*>(p);
    for( int i=0; i<10; i++){
        ptr->output("Hello World from "+ptr->GetName());
        sleep(1);
    }
}

int main(){
    pthread_t thread[10];
    IO io[10];
    for( int i=0; i<10; ++i){
        cout << "Launching "<< i << endl;
        io[i].SetName( 'a', i);
        pthread_create( &thread[i], 0, launch, &io[i]);
    }
    for( int i=0; i<10; ++i){
        pthread_join( thread[i], 0);
    }
}
