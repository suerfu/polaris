#include "plrsBaseData.h"

#include "RootGraphics.h"

#include <algorithm>
#include <set>

#include "TSystem.h"
#include "TMultiGraph.h"



RootGraphics::RootGraphics( plrsController* c) : plrsStateMachine( c ){
    app = new TApplication( "_app", 0, 0);
        // Needed to use ROOT graphics library.
    max_size = 1000;
        // Default value. Will be updated if necessary.
    canvas = 0;
    graph = 0;
}


RootGraphics::~RootGraphics(){}


void RootGraphics::Initialize(){}



void RootGraphics::Configure(){

    // As usual, find address of next module.

    string next_module = GetConfigParser()->GetString("/module/"+GetModuleName()+"/next", "");
        // if not found, returns default value of ""
    if( next_module!="" ){
        next_addr = ctrl->GetIDByName( next_module );   // nonnegative if valid
    }
    if( next_module=="" || next_addr<0 ){
        next_addr = ctrl->GetIDByName( this->GetModuleName() );
    }


    max_size = GetConfigParser()->GetInt("/module/"+GetModuleName()+"/max_size", max_size);

    string title = GetConfigParser()->GetString("/module/"+GetModuleName()+"/title", "Random Walk");

    canvas = new TCanvas( title.c_str(), title.c_str() );

    graph = new TGraph( max_size );
}



void RootGraphics::Deconfigure(){
    if( canvas )
        delete canvas;
    if( graph )
        delete graph;
}



void RootGraphics::Deinitialize(){}



void RootGraphics::PreRun(){}



void RootGraphics::Run(){
    
    // Try to get available resource.

    void* rdo = PullFromBuffer();
    if( rdo == 0 ){
        gSystem->ProcessEvents();
            // this allows one to use mouse and GUI even if no waveform is updated.
        return;
    }

    int* intptr = reinterpret_cast< int* >(rdo);
    data.push_back( *intptr );

    if( data.size()>= max_size ){
        Draw();
        gSystem->ProcessEvents();
        data.clear();
    }

    PushToBuffer( next_addr, rdo);
}


void RootGraphics::PostRun(){}

void RootGraphics::Draw(){

    if( canvas )
        canvas->Clear();

    for( unsigned int i=0; i<max_size; i++){
        graph->SetPoint( i, i, data[i]);
    }
    graph->Draw("AL");

    canvas->Modified();
    canvas->Update();

        // This function call is needed to refresh in real time.
}

