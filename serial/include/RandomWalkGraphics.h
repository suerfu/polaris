#ifndef RANDOMWALKGRAPHICS_H
    #define RANDOMWALKGRAPHICS_H 1

#include "plrsModuleGraphics.h"
#include "RandomWalkDAQ.h"

#include <vector>
#include <chrono>

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TBox.h"
#include "TLine.h"


/// RandomWalkGraphics module uses ROOT to visualize time-series data ( with second resolution).
/// The module expects single integer data to be sent from previous module.
/// Internally it contains two vectors of type int with twice as large capacity as requested.
/// It will continuously push new data to back and plotting is done at different starting locations.
/// Once incoming data fills up all spaces, vector will erase previous members once.

class RandomWalkGraphics : public plrsModuleGraphics{

public:

    RandomWalkGraphics( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~RandomWalkGraphics();
        //!< Destructor. It will remove it's own pointer from Controller.

protected:

    void Configure();
        //!< This method will be called by controller to configure the DAQ.

    void UnConfigure();

    void CleanUp();
        //!< Called when module goes into END state from RUN 

    void PreRun();
        //!< Obtain a time stamp that marks the beginning of a run.

    void PreEvent();
        //!< Will configure canvas and graphs. If vector is full, it will also erase previous elements and reallocates.

    void Process( void* p);
        //!< Called every iteration. If there are data that must be processed, use this function.
        //!< Here Process is used to push new data into memory used for plotting.

    void Draw( void* p);
        //!< Draw method will be called at the refresh rate specified by config file and plrsModuleGraphics.
        //!< Do not place any sensitive data in this method as it might not be processed.

    void PostEvent();

    virtual void Clear();
        //!< Clear the graphics

private:

    TApplication* app;

    TCanvas* canvas;
        // ROOT canvas class

    TGraph* graph;
        // ROOT graph object

    unsigned int x_size;

    // unsigned int start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    std::vector<Float_t> x_array;
    
    std::vector<Float_t> y_array;
};

extern "C" RandomWalkGraphics* create_RandomWalkGraphics( plrsController* c);

extern "C" void destroy_RandomWalkGraphics( RandomWalkGraphics* p);

#endif
