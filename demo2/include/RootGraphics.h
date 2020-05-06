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


/// RootGraphics module uses ROOT to visualize time-series data.
/// The module expects single integer data to be sent from previous module.
/// Internally it contains a vector of type int with twice as large capacity as requested.
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


class RootGraphics : public plrsStateMachine /*ModuleGraphics*/{

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
        //!< Will configure canvas and graphs.

    void Run();

    void PostRun();

    void Draw();
        //!< This method actually clears the canvas, sets the points on TGraph and draws.

private:

    int next_addr;

    TApplication* app;

    TCanvas* canvas;
    
    TGraph* graph;

    unsigned int max_size;
        // number of data points to accumulate before plotting.

    std::vector<int> data;
        // 2-D array to store data.
};


extern "C" RootGraphics* create_RootGraphics( plrsController* c){ return new RootGraphics(c);}


extern "C" void destroy_RootGraphics( RootGraphics* p){ delete p;}


#endif
