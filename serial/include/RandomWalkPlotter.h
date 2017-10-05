#ifndef RANDOMWALKPLOTTER_H
    #define RANDOMWALKPLOTTER_H 1

#include "plrsStateMachine.h"
#include "RandomWalkDAQ.h"

class RandomWalkPlotter : public plrsStateMachine{

public:

    RandomWalkPlotter( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~RandomWalkPlotter();
        //!< Destructor. It will remove it's own pointer from Controller.

protected:

    std::string GetModuleName(){ return "plotter";}
        //!< Return module name. Used by ctrl to identify different modules.

    void Configure();
        //!< This method will be called by controller to configure the DAQ.

    void UnConfigure();

    void CleanUp();
        //!< Called when module goes into END state from RUN 

    void PreRun();
        //!< Function called at the beginning of run.

    void Run();
        //!< Main part of data acquisition.

    void PostRun();
        //!< Function called after the end of run.

    virtual void DrawWaveform( RandomArray* data );

    virtual void DrawFrame();

    virtual void Clear();

    virtual void DrawTicksX( float min_x, float max_x, float spacing_x);

    virtual void DrawTicksY( float min_x, float max_x, float spacing_x);

    virtual void DrawTitle( char* p);

    virtual void TickSpacing ( float min, float max, float *tick_spacing, int num_ticks=5);

    virtual void FindMinMax( RandomArray* data, int* min, int* max);

    plPlotterParams* params;
        //!< Parameters in creating plotter object.

    plPlotter* plotter;
        //!< Plotter object.

    static const float pad_l;
//    static constexpr float pad_l = -10.;
    static constexpr float pad_h = 110.;
		// graphics window is defined over -10 to 110.
    
    static constexpr float frame_l = 0.;
    static constexpr float frame_h = 100.;
		// actual frame for drawing is defined between 0 and 100.
};

extern "C" RandomWalkPlotter* create_RandomWalkPlotter( plrsController* c);

extern "C" void destroy_RandomWalkPlotter( RandomWalkPlotter* p);

#endif
