#ifndef PYCWAVEGRAPHICS_H
    #define PYCWAVEGRAPHICS_H 1

#include "plrsModuleGraphics.h"

#include <vector>
#include <chrono>

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"


/// PyCWaveGraphics module uses ROOT to visualize time-series data ( with second resolution).

class PyCWaveGraphics : public plrsModuleGraphics{

public:

    PyCWaveGraphics( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~PyCWaveGraphics();
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

    //TMultiGraph* graphs = new TMultiGraph();
        // holder object for graphs

    TGraph* graph;
        // ROOT graph object

    unsigned int max_size;

    std::vector<Float_t> x_array;
    
    std::vector<Float_t> y_array;

    bool x_is_int, y_is_int;

    int colx, coly;
};


extern "C" PyCWaveGraphics* create_PyCWaveGraphics( plrsController* c){ return new PyCWaveGraphics(c);}


extern "C" void destroy_PyCWaveGraphics( PyCWaveGraphics* p){ delete p;}


#endif
