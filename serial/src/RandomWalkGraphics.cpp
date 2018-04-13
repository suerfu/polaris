#include "RandomWalkGraphics.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "TSystem.h"
#include "TMultiGraph.h"

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
//    if( canvas!=0 )
//        delete canvas;
//    if( graph!=0 )
//        delete graph;
//    if( line!=0 )
//        delete line;
//    if( box!=0 )
//        delete box;
}



void RandomWalkGraphics::CleanUp(){}



void RandomWalkGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}


void RandomWalkGraphics::PreRun(){
    start_time = ctrl->GetTimeStamp();
}

void RandomWalkGraphics::PreEvent(){
    if( canvas==0 )
        canvas = new TCanvas("Random Walk Pattern");
    if( graph==0 )
        graph = new TGraph();
    if( x_array.size()>2*x_size ){
        x_array.erase( x_array.begin(), x_array.begin()+x_array.size()-x_size);
    }
    if( y_array.size()>2*x_size ){
        y_array.erase( y_array.begin(), y_array.begin()+y_array.size()-x_size);
    }
}



void RandomWalkGraphics::Draw( void* rdo ){

    if( rdo!=0 ){
        x_array.push_back( ctrl->GetTimeStamp() - start_time  );
        y_array.push_back( *(reinterpret_cast<int*>( rdo ))  );

        unsigned int size = x_array.size() < x_size ? x_array.size() : x_size;
        cout << "x-size is "<< x_array.size() << endl;
    
        if( size>0 )
            graph->DrawGraph( size, &x_array[x_array.size()-size], &y_array[y_array.size()-size], "AC");
    }
}


void RandomWalkGraphics::PostEvent(){
    canvas->Update();
    gSystem->ProcessEvents();
}

