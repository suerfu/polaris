#ifndef RANDOMWALKGRAPHICS_H
    #define RANDOMWALKGRAPHICS_H 1

#include "plrsModuleGraphics.h"
#include "RandomWalkDAQ.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TBox.h"
#include "TLine.h"

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

    void PreEvent();

    void Draw( void* p);
        //!< Main part of data acquisition.

    void PostEvent();

    virtual void Clear();

private:

    TApplication* app;

    TCanvas* canvas;
        // ROOT canvas class

    TGraph* graph;
        // ROOT graph object

    TBox* box;

    TLine* line;
};

extern "C" RandomWalkGraphics* create_RandomWalkGraphics( plrsController* c);

extern "C" void destroy_RandomWalkGraphics( RandomWalkGraphics* p);

#endif
