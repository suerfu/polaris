#include "plrsBaseData.h"
#include "RawRecorder.h"

extern "C" RawRecorder* create_RawRecorder( plrsController* c ){ return new RawRecorder(c);}


extern "C" void destroy_RawRecorder( RawRecorder* p ){ delete p;}


RawRecorder::RawRecorder( plrsController* c) : plrsModuleRecorder(c){}


RawRecorder::~RawRecorder(){;}


void RawRecorder::WriteToFile( void* rdo){
    if( rdo!=0 ){
        int* bytes_to_write = reinterpret_cast< int* >(rdo);
        output_file.write( reinterpret_cast<char*>(rdo) + sizeof(int), *bytes_to_write);
    }
}
