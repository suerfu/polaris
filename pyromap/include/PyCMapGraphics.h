#ifndef PYCMAPGRAPHICS_H
    #define PYCMAPGRAPHICS_H 1

#include "plrsModuleGraphics.h"

#include <vector>
#include <chrono>

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph2D.h"
#include "TH2F.h"


/// PyCMapGraphics module uses ROOT to visualize time-series data ( with second resolution).
/// The module expects single integer data to be sent from previous module.
/// Internally it contains two vectors of type int with twice as large capacity as requested.
/// It will continuously push new data to back and plotting is done at different starting locations.
/// Once incoming data fills up all spaces, vector will erase previous members once.

class PyCMapGraphics : public plrsModuleGraphics{

public:

    PyCMapGraphics( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~PyCMapGraphics();
        //!< Destructor. It will remove it's own pointer from Controller.

protected:

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

private:

    TApplication* app;

    TCanvas* canvas;
        // ROOT canvas class

    TH2F* graph2d;

    TGraph2D* graph;
        // ROOT graph object

    unsigned int last_size;

    int colx, coly, colz;

    bool x_is_int, y_is_int, z_is_int;

    Float_t x_t;
    
    Float_t y_t;

    Float_t z_t;
};



extern "C" PyCMapGraphics* create_PyCMapGraphics( plrsController* c){ return new PyCMapGraphics(c);}


extern "C" void destroy_PyCMapGraphics( PyCMapGraphics* p){ delete p;}

#endif
