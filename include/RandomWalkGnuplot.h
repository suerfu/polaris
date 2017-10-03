#ifndef RANDOMWALKGNUPLOT_H
    #define RANDOMWALKGNUPLOT_H 1

#include "plrsStateMachine.h"
#include "RandomWalkDAQ.h"

#include "gnuplot_cpp.h"

class RandomWalkGnuplot : public plrsStateMachine{

public:

    RandomWalkGnuplot( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~RandomWalkGnuplot();
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

    virtual void Clear();

    Gnuplot* gnuplot;
};

extern "C" RandomWalkGnuplot* create_RandomWalkGnuplot( plrsController* c);

extern "C" void destroy_RandomWalkGnuplot( RandomWalkGnuplot* p);

#endif
