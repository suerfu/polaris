#ifndef SERIALGNUPLOT_H
    #define SERIALGNUPLOT_H 1

#include "plrsStateMachine.h"
#include "SerialDAQ.h"

#include "gnuplot_cpp.h"

class SerialGnuplot : public plrsStateMachine{

public:

    SerialGnuplot( plrsController* h );
        //!< Constructor. Upon creation, it will register in Controller and obtain an ID.
        //!< The default state will be INIT.
    
    ~SerialGnuplot();
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

    virtual void DrawWaveform( vector<int> data );

    virtual void Clear();

    Gnuplot* gnuplot;
};

extern "C" SerialGnuplot* create_SerialGnuplot( plrsController* c);

extern "C" void destroy_SerialGnuplot( SerialGnuplot* p);

#endif
