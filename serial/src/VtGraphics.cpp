#include "VtGraphics.h"

#include "TSystem.h"
#include "TMultiGraph.h"
#include "TAxis.h"

TMultiGraph* graphs = new TMultiGraph();

extern "C" VtGraphics* create_VtGraphics( plrsController* c){
    return new VtGraphics(c);
}


extern "C" void destroy_VtGraphics( VtGraphics* p){
    delete p;
}



VtGraphics::VtGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);
    canvas = 0;
    graph = 0;

    x_size = 250;

    x_array.reserve( 2*x_size );
    y_array.reserve( 2*x_size );
}


VtGraphics::~VtGraphics(){}


void VtGraphics::Configure(){

    if( canvas==0 )
        canvas = new TCanvas("V(t)");

    if( graph==0 )
        graph = new TGraph();
}



void VtGraphics::Deconfigure(){
}



void VtGraphics::Deinitialize(){}



void VtGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}



void VtGraphics::PreEvent(){
    if( canvas==0 ){
        canvas = new TCanvas("V(t)");
        canvas->SetTitle("Voltage as function of time");
    }
    if( graph==0 ){
        graph = new TGraph();
        graph->SetTitle( "Voltage as function of time" );
        graph->GetXaxis()->SetTitle( "Vt since start (s)" );
    }
    if( x_array.size()>2*x_size ){
        x_array.erase( x_array.begin(), x_array.begin()+x_array.size()-x_size);
    }
    if( y_array.size()>2*x_size ){
        y_array.erase( y_array.begin(), y_array.begin()+y_array.size()-x_size);
    }
}



void VtGraphics::Process( void* rdo ){
    float* temp = reinterpret_cast<float*>(rdo);
    x_array.push_back( temp[0] );
    y_array.push_back( temp[1] );
}


void VtGraphics::Draw( void* rdo ){
    unsigned int size = x_array.size() < x_size ? x_array.size() : x_size;
    if( size>0 )
        graph->DrawGraph( size, &x_array[x_array.size()-size], &y_array[y_array.size()-size], "APL");
}


void VtGraphics::PostEvent(){
    canvas->Update();
    gSystem->ProcessEvents();
}

