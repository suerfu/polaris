#include "plrsBaseData.h"

#include "RootGraphics.h"

#include <algorithm>
#include <set>

#include "TSystem.h"
#include "TMultiGraph.h"



RootGraphics::RootGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);

    max_size = 500;
}


RootGraphics::~RootGraphics(){}


void RootGraphics::Initialize(){

    max_size = cparser->GetInt("/module/"+GetModuleName()+"/max_size", max_size);

    // in this phase fill GraphInfo and CanvasInfo vectors.

    // First start with 1D plots
    map<string, vector<string> > maps = cparser->GetListOfParameters("/module/"+GetModuleName()+"/plot");
    map<string, vector<string> >::iterator itr;

    for( itr=maps.begin(); itr!=maps.end(); itr++){

        // Find the position of each graph.
        vector<int> dim = cparser->GetIntArray(itr->first+"pos");
        if( dim.size()==3 ){

            GraphInfo gi;

            gi.page = dim[0];
            gi.row = dim[1];
            gi.col = dim[2];

            if( gi.page>=0 && gi.row>=0 && gi.col>=0 ){

                // Get title of each window.
                size_t end = itr->first.find_last_of('/');
                size_t beg = itr->first.find_last_of('/', end-1);
                gi.title = itr->first.substr( beg+1, end-beg-1);
                if( cparser->GetString(itr->first+"title")!="")
                    gi.title = cparser->GetString(itr->first+"title");
                gi.x_title = cparser->GetString(itr->first+"x-title");
                gi.y_title = cparser->GetString(itr->first+"y-title");
                gi.x_index = cparser->GetInt( itr->first+"x-index", 0);
                gi.y_index = cparser->GetInt( itr->first+"y-index", -1);
                gi.z_index = cparser->GetInt( itr->first+"z-index", -1);

                gi.x_range = cparser->GetFloatArray( itr->first+"x-range");
                gi.y_range = cparser->GetFloatArray( itr->first+"y-range");

                gi.option = cparser->GetString( itr->first+"option");

                gi.gr = 0;
                gi.gr2d = 0;
                gi.hist = 0;
                gi.hist2d = 0;

                if( gi.x_index>=0 && gi.y_index>=0 && gi.z_index>=0 ){    // all xyz column specified, either 2D histogram with specified content or time-series plot
                    if( gi.x_range.size()==3 && gi.y_range.size()==3 ){
                        gi.hist2d = new TH2F(gi.title.c_str(), gi.title.c_str(), gi.x_range[0], gi.x_range[1], gi.x_range[2], gi.y_range[0], gi.y_range[1], gi.y_range[2]);
                        if( gi.option=="" )
                            gi.option = "colz";
                    }
                    else{
                        gi.gr2d = new TGraph2D();
                        if( gi.option=="" )
                            gi.option = "pcol";
                    }
                    grinfo.push_back( gi );
                }
                else if( gi.x_index>=0 && gi.y_index>=0 ){  // specified X and Y, can be line plot, or 2D histogram that increments by 1.
                    if( gi.x_range.size()==3 && gi.y_range.size()==3 ){
                        gi.hist2d = new TH2F(gi.title.c_str(), gi.title.c_str(), gi.x_range[0], gi.x_range[1], gi.x_range[2], gi.y_range[0], gi.y_range[1], gi.y_range[2]);
                    }
                    else{
                        gi.gr = new TGraph();
                        if( gi.option=="" )
                            gi.option = "APL";
                    }
                    grinfo.push_back( gi );
                }
                else if( gi.x_index>0 ){
                    if( gi.x_range.size()==3 ){
                        gi.hist = new TH1F( gi.title.c_str(), gi.title.c_str(), gi.x_range[0], gi.x_range[1], gi.x_range[2]);
                    }
                    else
                        Print("warning : range for histogram not specified.\n", ERR);
                    grinfo.push_back( gi );
                }

            }
        }
    }
    std::sort( grinfo.begin(), grinfo.end());
}



void RootGraphics::Configure(){

    // find out how many independent canvas are needed.
    int prev_page = -1;
    for( unsigned int i = 0; i<grinfo.size(); i++ ){
        if( grinfo[i].page!=prev_page ){

            prev_page = grinfo[i].page;
            grinfo[i].page = canvas.size();

            stringstream ctitle;
            ctitle << "window-"<<canvas.size();

            CanvasInfo ci;
            ci.cv = new TCanvas( ctitle.str().c_str(), ctitle.str().c_str() );
            ci.nrow = grinfo[i].row+1;
            ci.ncol = grinfo[i].col+1;

            canvas.push_back( ci );
        }
        else{
            grinfo[i].page = canvas.size()-1;
            if( grinfo[i].row >= canvas.back().nrow )
                canvas.back().nrow = grinfo[i].row+1;
            if( grinfo[i].col >= canvas.back().ncol )
                canvas.back().ncol = grinfo[i].col+1;
        }
    }
}



void RootGraphics::Deconfigure(){}



void RootGraphics::Deinitialize(){}



void RootGraphics::Clear(){
    vector< CanvasInfo >::iterator itr;
    for( itr=canvas.begin(); itr!=canvas.end(); itr++){
        itr->cv->Clear();
    }
}



void RootGraphics::Divide(){
    vector< CanvasInfo >::iterator itr;
    for( itr=canvas.begin(); itr!=canvas.end(); itr++){
        itr->cv->Divide( itr->ncol, itr->nrow);
    }
}


void RootGraphics::PreRun(){}



void RootGraphics::Process( void* rdo ){

    vector<plrsBaseData>* temp = reinterpret_cast< vector<plrsBaseData>* >(rdo);

    set<int> extract;

    vector<GraphInfo>::iterator gritr;
    for( gritr = grinfo.begin(); gritr!=grinfo.end(); gritr++){
        extract.insert( gritr->x_index);
        if( gritr->y_index>=0 )
            extract.insert( gritr->y_index);
        if( gritr->z_index>=0 )
            extract.insert( gritr->z_index);
    }

    for( set<int>::iterator itr=extract.begin(); itr!=extract.end(); itr++){
        data[*itr].push_back( (*temp)[*itr].GetFloat() );
        if( data[*itr].size() > 2*max_size )
            data[*itr].erase( data[*itr].begin(), data[*itr].begin() + data[*itr].size()-max_size);
    }

    for( gritr = grinfo.begin(); gritr!=grinfo.end(); gritr++){
        if( gritr->hist!=0 )
            gritr->hist->Fill( (*temp)[gritr->x_index].GetFloat());
        if( gritr->hist2d!=0 ){
            if( gritr->z_index<0 )
                gritr->hist2d->Fill( (*temp)[gritr->x_index].GetFloat(), (*temp)[gritr->y_index].GetFloat());
            else{
                gritr->hist2d->SetBinContent( gritr->hist2d->FindBin( (*temp)[gritr->x_index].GetFloat(), (*temp)[gritr->y_index].GetFloat()) , (*temp)[gritr->z_index].GetFloat());
            }
        }
    }
}



void RootGraphics::Draw( void* rdo ){
    Clear();
    Divide();

    vector<GraphInfo>::iterator gritr;
    for( gritr = grinfo.begin(); gritr!=grinfo.end(); gritr++){

        int addr = 1 + gritr->row * canvas[gritr->page].ncol + gritr->col;
        canvas[gritr->page].cv->cd( addr );

        unsigned int size = std::min( static_cast< unsigned int>(data[gritr->x_index].size()), max_size);
        int offset = data[gritr->x_index].size()-size;

        if( gritr->gr!=0 && size>0 ){
            for( unsigned int i=0; i<size; i++){
                gritr->gr->SetPoint( i, data[gritr->x_index][offset+i], data[gritr->y_index][offset+i]);
            }
            gritr->gr->Draw( gritr->option.c_str() );
            gritr->gr->SetTitle( gritr->title.c_str() );
            gritr->gr->GetXaxis()->SetTitle( gritr->x_title.c_str() );
            gritr->gr->GetYaxis()->SetTitle( gritr->y_title.c_str() );
        }
        else if( gritr->gr2d!=0 && size>5 ){
            gritr->gr2d->Clear();
            for( unsigned int i=0; i<size; i++){
                gritr->gr2d->SetPoint( i, data[gritr->x_index][offset+i], data[gritr->y_index][offset+i], data[gritr->z_index][offset+i]);
            }
            gritr->gr2d->Draw( gritr->option.c_str() );
            gritr->gr2d->SetTitle( gritr->title.c_str() );
            gritr->gr2d->GetXaxis()->SetTitle( gritr->x_title.c_str() );
            gritr->gr2d->GetYaxis()->SetTitle( gritr->y_title.c_str() );
        }
        else if( gritr->hist!=0 ){
            gritr->hist->Draw( gritr->option.c_str() );
            gritr->hist->SetTitle( gritr->title.c_str() );
            gritr->hist->SetXTitle( gritr->x_title.c_str() );
            gritr->hist->SetYTitle( gritr->y_title.c_str() );
        }
        else if( gritr->hist2d!=0 ){
            gritr->hist2d->Draw( gritr->option.c_str() );
            gritr->hist2d->SetTitle( gritr->title.c_str() );
            gritr->hist2d->SetXTitle( gritr->x_title.c_str() );
            gritr->hist2d->SetYTitle( gritr->y_title.c_str() );
        }

        canvas[gritr->page].cv->Modified();
    }

    for( vector<CanvasInfo>::iterator cvitr = canvas.begin(); cvitr!=canvas.end(); ++cvitr){
        cvitr->cv->Update();
    }
    gSystem->ProcessEvents();
}

