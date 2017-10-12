#include "SerialGnuplot.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

extern "C" SerialGnuplot* create_SerialGnuplot( plrsController* c){ return new SerialGnuplot(c);}


extern "C" void destroy_SerialGnuplot( SerialGnuplot* p){ delete p;}


SerialGnuplot::SerialGnuplot( plrsController* c) : plrsStateMachine(c){
}


SerialGnuplot::~SerialGnuplot(){
    Print( "Plotter destructor called\n", DEBUG);
    if( gnuplot!=0 )
        delete gnuplot;
}


void SerialGnuplot::Configure(){

    Print( "configuring Gnuplot\n", DETAIL);

    if( cparser->GetBool("/module/plotter/enable", false))
        gnuplot = new Gnuplot("Random Walk");
    else
        gnuplot = 0;

    if( gnuplot!=0 ){
        gnuplot->set_title("Light Intensity at 1000 nm");
        gnuplot->set_grid();
        gnuplot->set_pointsize(1);
        gnuplot->set_style("line");
    }

    Print( "Gnuplot configured\n", DETAIL);
    
}


void SerialGnuplot::UnConfigure(){
    if( gnuplot!=0 ){
        delete gnuplot;
        gnuplot = 0;
    }
    Print( "unconfiguring Gnuplot\n", DETAIL);
}


void SerialGnuplot::CleanUp(){
    Print( "Gnuplotter cleaning up\n", DETAIL);
}


void SerialGnuplot::Clear(){
    if( gnuplot ){
        gnuplot->reset_plot();
//        gnuplot->clear();
//        gnuplot->set_grid();
    }
}

void SerialGnuplot::PreRun(){;}



void SerialGnuplot::PostRun(){;}



void SerialGnuplot::Run(){
    Print( "running...\n", DETAIL);

    void* rdo = 0;

    vector<int> data;

    while( GetState()==RUN && GetStatus()!=ERROR ){

        rdo = PullFromBuffer( RUN );

        if( rdo!=0 ){

            data.push_back( (*(reinterpret_cast<int*> (rdo))));    // least sig bit

            if( data.size()>1000 ){
                Clear();
                gnuplot->plot_x < vector<int> > ( data, "Intensity of light at 1000 nm");
                data.clear();
            }

            PushToBuffer( addr_nxt, rdo);
            rdo = 0;
        }

        else{
            break;
        }

    }
    Print( "Gnuplot finished plotting\n", DETAIL);
}


void SerialGnuplot::DrawWaveform( vector<int> data ){
    gnuplot->set_style("points");
    gnuplot->plot_x < vector<int> > ( data, "Intensity of Light at 1000 nm in Jadwin B38");
}

