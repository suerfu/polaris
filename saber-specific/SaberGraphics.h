#ifndef SABERGRAPHICS_H
    #define SABERGRAPHICS_H 1

#include "plrsStateMachine.h"
#include "SaberDAQData.h"

#include "gnuplot_cpp.h"


class SaberGraphics : plrsStateMachine {

public:

    SaberGraphics( plrsController* c);

    ~SaberGraphics();

protected:

    std::string GetModuleName(){ return "graphics";}

    void Configure();

    void UnConfigure();

    void CleanUp();

    void PreRun();
        //!< Function called at the beginning of run.

    void Run();
        //!< Main part of data acquisition.

    void PostRun();
        //!< Function called after the end of run.

    virtual void Clear();

    Gnuplot* gnuplot;

    void CommandHandler();

private:

    int board;
    bool set_board;

    int channel;
    bool set_channel;

    bool pause;

};

extern "C" SaberGraphics* create_SaberGraphics( plrsController* c);

extern "C" void destroy_SaberGraphics( SaberGraphics* p);

#endif
