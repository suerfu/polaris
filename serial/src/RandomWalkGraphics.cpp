#include "RandomWalkGraphics.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "TSystem.h"
#include "TMultiGraph.h"
#include "TAxis.h"

TMultiGraph* graphs = new TMultiGraph();

extern "C" RandomWalkGraphics* create_RandomWalkGraphics( plrsController* c){
    return new RandomWalkGraphics(c);
}


extern "C" void destroy_RandomWalkGraphics( RandomWalkGraphics* p){
    delete p;
}



RandomWalkGraphics::RandomWalkGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);
    canvas = 0;
    graph = 0;

    x_size = 250;

    x_array.reserve( 2*x_size );
    y_array.reserve( 2*x_size );
}


RandomWalkGraphics::~RandomWalkGraphics(){}


void RandomWalkGraphics::Configure(){

    if( canvas==0 )
        canvas = new TCanvas("Random Walk Pattern");

    if( graph==0 )
        graph = new TGraph();
}



void RandomWalkGraphics::UnConfigure(){
}



void RandomWalkGraphics::CleanUp(){}



void RandomWalkGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}



void RandomWalkGraphics::PreRun(){
    start_time = std::chrono::high_resolution_clock::now();//ctrl->GetTimeStamp();
}



void RandomWalkGraphics::PreEvent(){
    if( canvas==0 ){
        canvas = new TCanvas("Random Walk Pattern");
        canvas->SetTitle("Random Walk Demo");
    }
    if( graph==0 ){
        graph = new TGraph();
        graph->SetTitle( "Random Walk Demo" );
        graph->GetXaxis()->SetTitle( "Time since start (s)" );
    }
    if( x_array.size()>2*x_size ){
        x_array.erase( x_array.begin(), x_array.begin()+x_array.size()-x_size);
    }
    if( y_array.size()>2*x_size ){
        y_array.erase( y_array.begin(), y_array.begin()+y_array.size()-x_size);
    }
}



void RandomWalkGraphics::Process( void* rdo ){

    int t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
    x_array.push_back( t/1000.  );
    y_array.push_back( *(reinterpret_cast<int*>( rdo ))  );
}


void RandomWalkGraphics::Draw( void* rdo ){

    unsigned int size = x_array.size() < x_size ? x_array.size() : x_size;
    if( size>0 )
        graph->DrawGraph( size, &x_array[x_array.size()-size], &y_array[y_array.size()-size], "APL");
}


void RandomWalkGraphics::PostEvent(){
    canvas->Update();
    gSystem->ProcessEvents();
}

