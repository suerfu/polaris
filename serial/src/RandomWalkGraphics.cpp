#include "RandomWalkGraphics.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "TSystem.h"

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
}


RandomWalkGraphics::~RandomWalkGraphics(){
//    if( app!=0 )
//        delete app;
}


void RandomWalkGraphics::Configure(){
    if( canvas==0 )
        canvas = new TCanvas("Random Walk Pattern");
    if( graph==0 )
        graph = new TGraph();
}



void RandomWalkGraphics::UnConfigure(){
    if( graph!=0 )
        delete graph;
    if( canvas!=0 )
        delete canvas;
}



void RandomWalkGraphics::CleanUp(){}



void RandomWalkGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}


void RandomWalkGraphics::PreEvent(){
    if( canvas==0 )
        canvas = new TCanvas("Random Walk Pattern");
    if( graph==0 )
        graph = new TGraph();
}



void RandomWalkGraphics::Draw( void* rdo ){
    RandomArray* array = reinterpret_cast<RandomArray*>( rdo );
    graph->DrawGraph( array->GetSize(), array->GetX(), array->GetY(), "AC");
    canvas->Update();
    gSystem->ProcessEvents();
}


void RandomWalkGraphics::PostEvent(){
    canvas->Update();
    gSystem->ProcessEvents();
}

