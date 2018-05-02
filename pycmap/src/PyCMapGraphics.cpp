#include "plrsBaseData.h"

#include "PyCMapGraphics.h"

#include "TSystem.h"
#include "TStyle.h"
#include "TAxis.h"



PyCMapGraphics::PyCMapGraphics( plrsController* c) : plrsModuleGraphics(c){
    app = new TApplication( "_app", 0, 0);
    canvas = 0;
    graph2d = 0;

}



PyCMapGraphics::~PyCMapGraphics(){}


void PyCMapGraphics::Configure(){

    colx = cparser->GetInt("/module/"+GetModuleName()+"/colx", 0);
        // index of x-column
    coly = cparser->GetInt("/module/"+GetModuleName()+"/coly", colx+1);
        // index of y-column
    colz = cparser->GetInt("/module/"+GetModuleName()+"/colz", coly+1);
        // index of y-column


    string type = cparser->GetString("/module/"+GetModuleName()+"/typex");
    if( type=="int" || type=="Int" || type=="INT"){
        x_is_int = true;
    }
    else
        x_is_int = false;

    type = cparser->GetString("/module/"+GetModuleName()+"/typey");
    if( type=="int" || type=="Int" || type=="INT"){
        y_is_int = true;
    }
    else
        y_is_int = false;

    type = cparser->GetString("/module/"+GetModuleName()+"/typez");
    if( type=="int" || type=="Int" || type=="INT"){
        z_is_int = true;
    }
    else
        z_is_int = false;

    if( canvas==0 )
        canvas = new TCanvas();

    vector<int> ax = cparser->GetIntArray("/module/daq/range_ax");
    vector<int> az = cparser->GetIntArray("/module/daq/range_az");
    int nbins_ax = ax[1]-ax[0]+10 < 250 ? ax[1]-ax[0]+10 : 250;
    int nbins_az = az[1]-az[0]+10 < 250 ? az[1]-az[0]+10 : 250;
    graph2d = new TH2F("h", "h", nbins_ax, ax[0]-5, ax[1]+5, nbins_az, az[0]-5, az[1]+5);

    gStyle->SetOptStat(0);

    // create custom plasma color table

	const Int_t NColor = 100;
    const Int_t Npt = 9;
	Int_t MyPalette[NColor];
	Double_t Red[] = { 0,
                       (1.0*0x1f)/0xff,
                       (1.0*0x55)/0xff,
                       (1.0*0x88)/0xff,
                       (1.0*0xa8)/0xff,
                       (1.0*0xe3)/0xff,
                       (1.0*0xf9)/0xff,
                       (1.0*0xf8)/0xff,
                       (1.0*0xfc)/0xff,};
	Double_t Green[] = { 0,
                       (1.0*0x0c)/0xff,
                       (1.0*0x0f)/0xff,
                       (1.0*0x22)/0xff,
                       (1.0*0x36)/0xff,
                       (1.0*0x59)/0xff,
                       (1.0*0x95)/0xff,
                       (1.0*0xc9)/0xff,
                       (1.0*0xff)/0xff,};
	Double_t Blue[] = { (1.0*0x04)/0xff,
                       (1.0*0x48)/0xff,
                       (1.0*0x6d)/0xff,
                       (1.0*0x6a)/0xff,
                       (1.0*0x55)/0xff,
                       (1.0*0x33)/0xff,
                       (1.0*0x0a)/0xff,
                       (1.0*0x32)/0xff,
                       (1.0*0xa4)/0xff,};
	Double_t Length[Npt];
    for( int i=0; i<Npt; i++)
        Length[i] = 1.0*i/(Npt-1);
	Int_t FI = TColor::CreateGradientColorTable(Npt, Length, Red, Green, Blue, NColor);
	for (int i=0;i<NColor;i++)
		MyPalette[i] = FI+i;
//    gStyle->SetPalette( kDarkBodyRadiator);
    gStyle->SetPalette( NColor, MyPalette);
}



void PyCMapGraphics::Deconfigure(){
    if( canvas!=0 )
        delete canvas;
    if( graph2d!=0 )
        delete graph2d;
}



void PyCMapGraphics::Deinitialize(){}



void PyCMapGraphics::Clear(){
    if( canvas )
        canvas->Clear();
}



void PyCMapGraphics::PreRun(){
    if( canvas==0 )
        canvas = new TCanvas();
}



void PyCMapGraphics::Process( void* rdo ){

    vector<plrsBaseData>* temp = reinterpret_cast< vector<plrsBaseData>* >(rdo);

    if( temp->size()>1 ){
        x_t = x_is_int ? (*temp)[colx].GetInt() : (*temp)[colx].GetFloat();
        y_t = y_is_int ? (*temp)[coly].GetInt() : (*temp)[coly].GetFloat();
        z_t = z_is_int ? (*temp)[colz].GetInt() : (*temp)[colz].GetFloat();
        graph2d->SetBinContent( graph2d->FindBin( x_t, y_t), z_t);
    }
    gSystem->ProcessEvents();
}



void PyCMapGraphics::Draw( void* rdo ){
    graph2d->Draw( "colz" );
    canvas->Update();
    gSystem->ProcessEvents();
}



