#include "plrsBaseData.h"

#include "VtGraphics.h"

#include "TSystem.h"
#include "TMultiGraph.h"
#include "TPad.h"


TMultiGraph* graphs = new TMultiGraph();


extern "C" VtGraphics* create_VtGraphics( plrsController* c){ return new VtGraphics(c);}


extern "C" void destroy_VtGraphics( VtGraphics* p){ delete p;}


VtGraphics::VtGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);
    canvas = 0;
    canvas2 = 0;
    graph1 = 0;
    graph2 = 0;

    x_size = 250;

    x_array.reserve( 2*x_size );
    y_array.reserve( 2*x_size );
    z_array.reserve( 2*x_size );
}


VtGraphics::~VtGraphics(){}


void VtGraphics::Configure(){
}



void VtGraphics::Deconfigure(){}



void VtGraphics::Deinitialize(){}



void VtGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}



void VtGraphics::PreRun(){
    canvas = new TCanvas("T(t)");
    canvas2 = new TCanvas("P(t)");
    graph1 = new TGraph();
    graph2 = new TGraph();
}



void VtGraphics::Process( void* rdo ){

    if( x_array.size()>2*x_size ){
        x_array.erase( x_array.begin(), x_array.begin()+x_array.size()-x_size);
        y_array.erase( y_array.begin(), y_array.begin()+y_array.size()-x_size);
        z_array.erase( z_array.begin(), z_array.begin()+z_array.size()-x_size);
    }

    vector<plrsBaseData>* temp = reinterpret_cast< vector<plrsBaseData>* >(rdo);

    if( temp->size()>1 ){
        x_array.push_back( (*temp)[0].GetFloat() );
        y_array.push_back( (*temp)[1].GetFloat() );
        z_array.push_back( (*temp)[2].GetFloat() );
    }
}



void VtGraphics::Draw( void* rdo ){
    unsigned int size = x_array.size() < x_size ? x_array.size() : x_size;
    if( size>0 ){
        for( unsigned int i=0; i<size; i++){
            int id = x_array.size()-size;
            graph1->SetPoint( i, x_array[ id+i ], y_array[ id+i ]);
            graph2->SetPoint( i, x_array[ id+i ], z_array[ id+i ]);
        }
        canvas->cd();
            graph1->Draw("APL");
        canvas2->cd();
            graph2->Draw("APL");

//        canvas->Modified();
        canvas->Update();
        canvas2->Update();
        gSystem->ProcessEvents();
    }
}



