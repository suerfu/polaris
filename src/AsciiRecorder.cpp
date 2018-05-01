#include "plrsBaseData.h"
#include "AsciiRecorder.h"

#include <ctime>
#include <vector>

extern "C" AsciiRecorder* create_AsciiRecorder( plrsController* c ){ return new AsciiRecorder(c);}


extern "C" void destroy_AsciiRecorder( AsciiRecorder* p ){ delete p;}


AsciiRecorder::AsciiRecorder( plrsController* c) : plrsModuleRecorder(c){}


AsciiRecorder::~AsciiRecorder(){;}


void AsciiRecorder::PreRun(){

    if( output_file ){
        if( cparser->GetString("/cmdl/no-header")=="" && cparser->GetString("/module/"+GetModuleName()+"/no-header")=="" ){
            cparser->Print( output_file, "# " );

            time_t rawtime;
            struct tm* loctime;
            time( &rawtime );
            loctime = localtime( &rawtime);

            output_file << "# " << asctime( loctime) << endl;
        }
    }
}


void AsciiRecorder::WriteToFile( void* rdo){
    if( rdo!=0 ){
        vector<plrsBaseData>* data = reinterpret_cast< vector<plrsBaseData>* >(rdo);
        for( unsigned int i=0; i<data->size(); i++){
            output_file << (*data)[i] << '\t';
        }
        output_file << endl;
    }
}
