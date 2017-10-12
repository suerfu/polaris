#include "SaberGraphics.h"

extern "C" SaberGraphics* create_SaberGraphics( plrsController* c ){ return new SaberGraphics(c);}
    //!< create method to return the DAQ object.

extern "C" void destroy_SaberGraphics( SaberGraphics* p ){ delete p;}
    //!< destroy method.


SaberGraphics::SaberGraphics( plrsController* c ) : plrsStateMachine( c ){
    channel = 0;    set_channel = false;
    board = 0;      set_board = false;
    pause = false;
    refresh_rate = 1;   // refresh interval in second
}



SaberGraphics::~SaberGraphics(){
    if( gnuplot!=0 )
        delete gnuplot;
    Print( "SaberGraphics deleted\n", DETAIL);
}



void SaberGraphics::Configure(){
    if( cparser->GetBool("/module/graphics/enable", false)){
        gnuplot = new Gnuplot("Waveform");
        refresh_rate = cparser->GetInt( "/module/graphics/refresh_rate", refresh_rate);
    }
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

    uint32_t now, last_update;
    last_update = 0;

    void* rdo = 0;
    SaberDAQData* data;

    while( GetState()==RUN && GetStatus()!=ERROR ){

        rdo = PullFromBuffer( RUN );
        
        if( rdo!=0){

            data = reinterpret_cast<SaberDAQData*>(rdo);

            if( !data->IsHeader() ){

                if( !pause ){
                    now = ctrl->GetTimeStamp();
                    if( now-last_update > refresh_rate ){
                        Clear();
                        gnuplot->plot_x < SaberBoardRawData > ( (*data)[0], "PMT Waveform");
                        last_update = now;
                    }
                }
            }

            PushToBuffer( addr_nxt, rdo);
            rdo = 0;
        }
        else{
            break;
        }

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

    if( s=="ch" ){
        set_channel = true;
        return;
    }
}
