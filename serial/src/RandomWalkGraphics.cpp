#include "RandomWalkGraphics.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "TSystem.h"
#include "TBox.h"
#include "TLine.h"
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
    line = 0;
    box = 0;
}


RandomWalkGraphics::~RandomWalkGraphics(){}


void RandomWalkGraphics::Configure(){
    if( canvas==0 )
        canvas = new TCanvas("Random Walk Pattern");
    if( graph==0 )
        graph = new TGraph();
    if( line==0 )
        line = new TLine();
    if( box==0 )
        box = new TBox();
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
    line->SetLineColor( kBlue );
    line->DrawLine(0, 0, array->GetSize(), 0 );

    box->SetFillStyle( 0 );
    box->SetLineColor( kRed );
    box->DrawBox( 0, 0, 100, (array->GetY())[99]);

    canvas->Update();
    gSystem->ProcessEvents();
}


void RandomWalkGraphics::PostEvent(){
    canvas->Update();
    gSystem->ProcessEvents();
}

