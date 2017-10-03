#include "SaberGraphics.h"

extern "C" SaberGraphics* create_SaberGraphics( plrsController* c ){ return new SaberGraphics(c);}
    //!< create method to return the DAQ object.

extern "C" void destroy_SaberGraphics( SaberGraphics* p ){ delete p;}
    //!< destroy method.


SaberGraphics::SaberGraphics( plrsController* c ) : plrsStateMachine( c ){
    channel = 0;    set_channel = false;
    board = 0;      set_board = false;
    pause = false;
}



SaberGraphics::~SaberGraphics(){
    if( gnuplot!=0 )
        delete gnuplot;
    Print( "SaberGraphics deleted\n", DETAIL);
}



void SaberGraphics::Configure(){
    if( cparser->GetBool("/module/graphics/enable", false))
        gnuplot = new Gnuplot("Waveform");
    else
        gnuplot = 0;

    if( gnuplot!=0 ){
        gnuplot->set_title("PMT Waveform");
        gnuplot->set_grid();
        gnuplot->set_pointsize(1);
        gnuplot->set_style("line");
    }

    Print( "SaberGraphics configured\n", DETAIL);
}


void SaberGraphics::UnConfigure(){
    if( gnuplot!=0 ){
        delete gnuplot;
        gnuplot = 0;
    }
    Print( "Gnuplotter unconfigured\n", INFO);
}



void SaberGraphics::CleanUp(){
    Print( "Gnuplotter cleaning up\n", INFO);
}


void SaberGraphics::Clear(){
    if( gnuplot ){
        gnuplot->reset_plot();
    }
}

void SaberGraphics::PreRun(){;}


void SaberGraphics::PostRun(){;}


void SaberGraphics::Run(){
    Print( "SaberGraphics running\n", INFO);


    void* rdo = 0;
    SaberDAQData* data;

    while( GetState()==RUN && GetStatus()!=ERROR ){

        rdo = PullFromBuffer();
        while( rdo==0 ){
            if( GetState()!=RUN )
                break;
            rdo = PullFromBuffer();
        }
        
        if( rdo!=0){
            data = reinterpret_cast<SaberDAQData*>(rdo);
        }
        else
            break;

        if( !pause ){
            Clear();
            gnuplot->plot_x < SaberBoardRawData > ( (*data)[0], "PMT Waveform");
        }

        PushToBuffer( addr_nxt, rdo);
        rdo = 0;

        CommandHandler();

        sched_yield();
    }
}



void SaberGraphics::CommandHandler(){

    string s = PullCommand();

    if( set_board )
        board = stoi(s);
    
    if( set_channel)
        channel = stoi(s);

    set_board = false;
    set_channel = false;

    if( s=="pause" ){
        pause = true;
        return;
    }
    else if( s=="go" ){
        pause = false;
        return;
    }

    if( s=="bd" ){
        set_board = true;
        return;
    }

}
