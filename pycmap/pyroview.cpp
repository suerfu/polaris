#include <iostream>
#include <fstream>

#include "TGraph2D.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TColor.h"

using namespace std;

void CreatePalette();

int main( int argc, char* argv[]){

    if( argc<2 ){
        cout << "usage: " << argv[0] << " input.txt"<<endl;
        return -1;
    }

    ifstream input( argv[1]);
    if( !input.good() ){
        cout << "error opening " << argv[1] << endl;
        return -1;
    }

    TApplication app( "_app", 0, 0);
    TGraph2D graph = TGraph2D();
    CreatePalette();

    string dummy;
    input>>ws;
    while( input.good() && input.peek()=='#' ){
        getline( input, dummy, '\n');
        input>>ws;
    }

    const int Ncol = 6;
    float data[Ncol];
    int ctr = 0;

    while( input.good() ){
        for( int i=0; i<Ncol; i++){
            input >> data[i];
        }
        graph.SetPoint( ctr, data[1], data[2], data[3]);
        ctr++;
    }

    graph.Draw("colz");

    app.Run();


    return 0;
}

void CreatePalette(){

    // create custom plasma color table

	const Int_t NColor = 255;
    const Int_t Npt = 7;//9;
	Int_t MyPalette[NColor];
	Double_t Red[] = { //0,
                       (1.0*0x1f)/0xff,
                       (1.0*0x55)/0xff,
                       (1.0*0x88)/0xff,
                       (1.0*0xa8)/0xff,
                       (1.0*0xe3)/0xff,
                       (1.0*0xf9)/0xff,
                       (1.0*0xf8)/0xff//,
                       //(1.0*0xfc)/0xff,
                       };
	Double_t Green[] = { //0,
                       (1.0*0x0c)/0xff,
                       (1.0*0x0f)/0xff,
                       (1.0*0x22)/0xff,
                       (1.0*0x36)/0xff,
                       (1.0*0x59)/0xff,
                       (1.0*0x95)/0xff,
                       (1.0*0xc9)/0xff//,
                       //(1.0*0xff)/0xff,
                       };
	Double_t Blue[] = { //(1.0*0x04)/0xff,
                       (1.0*0x48)/0xff,
                       (1.0*0x6d)/0xff,
                       (1.0*0x6a)/0xff,
                       (1.0*0x55)/0xff,
                       (1.0*0x33)/0xff,
                       (1.0*0x0a)/0xff,
                       (1.0*0x32)/0xff//,
                       //(1.0*0xa4)/0xff,
                       };
	Double_t Length[Npt];
    for( int i=0; i<Npt; i++)
        Length[i] = 1.0*i/(Npt-1);
	Int_t FI = TColor::CreateGradientColorTable(Npt, Length, Red, Green, Blue, NColor);
	for (int i=0;i<NColor;i++)
		MyPalette[i] = FI+i;
//    gStyle->SetPalette( kDarkBodyRadiator);
    gStyle->SetPalette( NColor, MyPalette);
}
