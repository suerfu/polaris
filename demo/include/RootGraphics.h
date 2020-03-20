#ifndef ROOTGRAPHICS_H
    #define ROOTGRAPHICS_H 1

#include "plrsModuleGraphics.h"

#include <vector>
#include <chrono>

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TH1F.h"
#include "TH2F.h"


/// RootGraphics module uses ROOT to visualize time-series data ( with second resolution).
/// The module expects single integer data to be sent from previous module.
/// Internally it contains two vectors of type int with twice as large capacity as requested.
/// It will continuously push new data to back and plotting is done at different starting locations.
/// Once incoming data fills up all spaces, vector will erase previous members once.


/// Information regarding different graphics, including on which window to plot, position on the window, titles.

struct GraphInfo{

    bool operator<( const GraphInfo& b) const { return page<b.page;}

    int page;
    int row;
    int col;

    string title;

    string x_title;
    int x_index;
    string y_title;
    int y_index;
    int z_index;

    vector<float> x_range;
    vector<float> y_range;

    TGraph* gr;
    TGraph2D* gr2d;
    TH1F* hist;
    TH2F* hist2d;

    string option;
};



struct CanvasInfo{
    int nrow;
    int ncol;
    TCanvas* cv;
};


class RootGraphics : public plrsModuleGraphics{

public:

    RootGraphics( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~RootGraphics();
        //!< Destructor. It will remove it's own pointer from Controller.

protected:

    void Initialize();

    void Configure();
        //!< This method will be called by controller to configure the DAQ.

    void Deconfigure();

    void Deinitialize();
        //!< Called when module goes into END state from RUN 

    void PreRun();
        //!< Will configure canvas and graphs. If vector is full, it will also erase previous elements and reallocates.

    void Process( void* p);
        //!< Called every iteration. If there are data that must be processed, use this function.
        //!< Here Process is used to push new data into memory used for plotting.

    void Draw( void* p);
        //!< Draw method will be called at the refresh rate specified by config file and plrsModuleGraphics.
        //!< Do not place any sensitive data in this method as it might not be processed.

    virtual void Clear();
        //!< Clear the graphics

    void Divide();
        //!< Divide Canvas into rows and columns.

private:

    TApplication* app;

    vector<CanvasInfo> canvas;
        // ROOT canvas arrays since there could be multiple plots

    vector<GraphInfo> grinfo;


    unsigned int max_size;

    std::map< int, std::vector<Float_t> > data;
        // 2-D array to store data.
};

extern "C" RootGraphics* create_RootGraphics( plrsController* c){ return new RootGraphics(c);}


extern "C" void destroy_RootGraphics( RootGraphics* p){ delete p;}


#endif
