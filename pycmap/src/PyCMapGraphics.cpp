#include "plrsBaseData.h"

#include "PyCMapGraphics.h"

#include "TSystem.h"
#include "TMultiGraph.h"
#include "TAxis.h"


TMultiGraph* graphs = new TMultiGraph();


extern "C" PyCMapGraphics* create_PyCMapGraphics( plrsController* c){ return new PyCMapGraphics(c);}


extern "C" void destroy_PyCMapGraphics( PyCMapGraphics* p){ delete p;}


PyCMapGraphics::PyCMapGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);
    canvas = 0;
    graph = 0;

    x_size = 250;

    x_array.reserve( 2*x_size );
    y_array.reserve( 2*x_size );
}


PyCMapGraphics::~PyCMapGraphics(){}


void PyCMapGraphics::Configure(){

    if( canvas==0 )
        canvas = new TCanvas("V(t)");

    if( graph==0 )
        graph = new TGraph();
}



void PyCMapGraphics::Deconfigure(){}



void PyCMapGraphics::Deinitialize(){}



void PyCMapGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}



void PyCMapGraphics::PreRun(){
    if( canvas==0 ){
        canvas = new TCanvas("V(t)");
        canvas->SetTitle("Voltage as function of time");
    }
    if( graph==0 ){
        graph = new TGraph();
        graph->SetTitle( "Voltage as function of time" );
        graph->GetXaxis()->SetTitle( "Vt since start (s)" );
    }
}



void PyCMapGraphics::Process( void* rdo ){

    if( x_array.size()>2*x_size ){
        x_array.erase( x_array.begin(), x_array.begin()+x_array.size()-x_size);
    }
    if( y_array.size()>2*x_size ){
        y_array.erase( y_array.begin(), y_array.begin()+y_array.size()-x_size);
    }

    vector<plrsBaseData>* temp = reinterpret_cast< vector<plrsBaseData>* >(rdo);

    if( temp->size()>1 ){
        x_array.push_back( (*temp)[0].GetInt() );
        y_array.push_back( (*temp)[3].GetInt() );
    }
}



void PyCMapGraphics::Draw( void* rdo ){
    unsigned int size = x_array.size() < x_size ? x_array.size() : x_size;
    if( size>0 ){
        graph->DrawGraph( size, &x_array[x_array.size()-size], &y_array[y_array.size()-size], "APL");

        canvas->Update();
        gSystem->ProcessEvents();
    }
}



