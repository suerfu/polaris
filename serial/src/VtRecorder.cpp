#include "VtRecorder.h"
#include "SerialVtDAQ.h"

#include <unistd.h>


extern "C" VtRecorder* create_VtRecorder( plrsController* c ){ return new VtRecorder(c);}


extern "C" void destroy_VtRecorder( VtRecorder* p ){ delete p;}


VtRecorder::VtRecorder( plrsController* c) : plrsModuleRecorder(c){}

VtRecorder::~VtRecorder(){;}


void VtRecorder::PreRun(){

    if( output_file ){
        cparser->Print( output_file, "# " );
        time_t rawtime;
        struct tm* loctime;
        time( &rawtime );
        loctime = localtime( &rawtime);
        output_file << "# " << asctime( loctime) << endl;
        output_file << "# Time since start | Voltage (Volt) " << endl;
    }

}


void VtRecorder::Run(){

    void* rdo=0;

    Print( "running...\n", DETAIL);

    while( GetState()==RUN ){

        rdo = PullFromBuffer();

        if( rdo==0 ){
            sleep(1);
            continue;
        }

        if( output_file ){
            float* temp = reinterpret_cast<float*>(rdo);
            output_file <<  temp[0] << '\t' << temp[1] << endl;
        }

        PushToBuffer( addr_nxt, rdo);
        rdo = 0;
    }
    Print( "run finished\n", DETAIL);
}


