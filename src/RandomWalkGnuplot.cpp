#include "RandomWalkGnuplot.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>

extern "C" RandomWalkGnuplot* create_RandomWalkGnuplot( plrsController* c){ return new RandomWalkGnuplot(c);}


extern "C" void destroy_RandomWalkGnuplot( RandomWalkGnuplot* p){ delete p;}


RandomWalkGnuplot::RandomWalkGnuplot( plrsController* c) : plrsStateMachine(c){
    gnuplot = new Gnuplot("Random Walk");
}


RandomWalkGnuplot::~RandomWalkGnuplot(){
    Print( "Plotter destructor called\n", DEBUG);
    delete gnuplot;
}


void RandomWalkGnuplot::Configure(){
    gnuplot->set_title("Random Walk");
    gnuplot->set_grid();
    gnuplot->set_pointsize(5);
//    (*gnuplot) << "set style data points pt 5";
//    gnuplot->set_style("");
    
}


void RandomWalkGnuplot::UnConfigure(){
}


void RandomWalkGnuplot::CleanUp(){}


void RandomWalkGnuplot::Clear(){
    if( gnuplot ){
        gnuplot->reset_plot();
//        gnuplot->clear();
//        gnuplot->set_grid();
    }
}

void RandomWalkGnuplot::PreRun(){;}


void RandomWalkGnuplot::PostRun(){;}


void RandomWalkGnuplot::Run(){

    void* rdo = 0;
    bool pause = false;

    while( GetState()==RUN && GetStatus()!=ERROR ){
        usleep( 1000000 );
        while( rdo==0 ){
            if( GetState()!=RUN )
                break;
            rdo = PullFromBuffer();
        }
        if(rdo!=0){
            Clear();
            void* rdo2 = 0;
            while( rdo2==0 ){
                if( GetState()!=RUN )
                break;
                rdo2 = PullFromBuffer();
            }
            if( rdo2!=0 ){
                if( !pause ){
                    gnuplot->plot_xy <RandomArray, RandomArray> ( *(reinterpret_cast<RandomArray*>(rdo)), *(reinterpret_cast<RandomArray*>(rdo2)), "Random Walk Steps");
                }
                PushToBuffer( addr_nxt, rdo2);
            }
            PushToBuffer( addr_nxt, rdo);
            rdo = 0;
        }

        string s = PullCommand();
        if( s=="pause"  )
            pause = true;
        else if( s=="go")
            pause = false;

        sched_yield();

    }
}


//        g1.plot_equation("sin(x)*cos(2*x)","sine product");
//        g1.set_pointsize(0.8).set_style("points");
//        g1.set_style("impulses");
//        g1.set_style("steps");
//        g1.showonscreen(); // window output
//        g1.reset_all();
//        std::vector<double> x, y, y2, dy, z;
//       g1.plot_xyz(x,y,z,"user-defined points 3d");
//        g1.plot_xy_err(x,y,dy,"user-defined points 2d with errorbars");
/*
        g1.set_grid().set_samples(600).set_xrange(0,300);
        g1.plot_equation("sin(x)+sin(x*1.1)");
        g1.set_xautoscale().replot();

        Gnuplot g2;
        g2.plot_x(y2,"points");
        g2.set_smooth().plot_x(y2,"cspline");
        g2.set_smooth("bezier").plot_x(y2,"bezier");
        g2.unset_smooth();
*/
