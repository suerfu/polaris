#include "plrsBaseData.h"

#include "PyCWaveGraphics.h"

#include "TSystem.h"
#include "TMultiGraph.h"
#include "TAxis.h"



PyCWaveGraphics::PyCWaveGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);
    canvas = 0;
    graph = 0;

}



PyCWaveGraphics::~PyCWaveGraphics(){}


void PyCWaveGraphics::Configure(){

    max_size = cparser->GetInt("/module/"+GetModuleName()+"/max_size", 250);
        // maximum size of the plot. Once exceeds this value, older data will be truncated.

    colx = cparser->GetInt("/module/"+GetModuleName()+"/colx", 0);
        // index of x-column
    coly = cparser->GetInt("/module/"+GetModuleName()+"/coly", colx+1);
        // index of y-column

    string type = cparser->GetString("/module/"+GetModuleName()+"/typex");
    if( type=="int" || type=="Int" || type=="INT"){
        x_is_int = true;
    }
    else
        x_is_int = false;

    type = cparser->GetString("/module/"+GetModuleName()+"/typey");
    if( type=="int" || type=="Int" || type=="INT"){
        y_is_int = true;
    }
    else
        y_is_int = false;

    x_array.reserve( 2*max_size );
    y_array.reserve( 2*max_size );

    if( canvas==0 )
        canvas = new TCanvas("V(t)");

    if( graph==0 )
        graph = new TGraph();
}



void PyCWaveGraphics::Deconfigure(){}



void PyCWaveGraphics::Deinitialize(){}



void PyCWaveGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}



void PyCWaveGraphics::PreRun(){
}



void PyCWaveGraphics::Process( void* rdo ){

    if( x_array.size()>2*max_size ){
        x_array.erase( x_array.begin(), x_array.begin()+x_array.size()-max_size);
        y_array.erase( y_array.begin(), y_array.begin()+y_array.size()-max_size);
    }

    vector<plrsBaseData>* temp = reinterpret_cast< vector<plrsBaseData>* >(rdo);

    if( temp->size()>1 ){
        x_array.push_back( x_is_int ? (*temp)[colx].GetInt() : (*temp)[colx].GetFloat() );
        y_array.push_back( y_is_int ? (*temp)[coly].GetInt() : (*temp)[coly].GetFloat() );
    }
}



void PyCWaveGraphics::Draw( void* rdo ){

    unsigned int size = x_array.size() < max_size ? x_array.size() : max_size;

    if( size>0 ){
        graph->DrawGraph( size, &x_array[x_array.size()-size], &y_array[y_array.size()-size], "APL");
        canvas->Update();
        gSystem->ProcessEvents();
    }
}



