#ifndef SABERDISKRECORDER_H
    #define SABERDISKRECORDER_H 1

#include <string>

#include "plrsController.h"
#include "plrsModuleRecorder.h"


class SaberDiskRecorder : public plrsModuleRecorder{

public:
    SaberDiskRecorder( plrsController* c);

    ~SaberDiskRecorder();

protected:

    void Configure(){ plrsModuleRecorder::Configure(); }

	void UnConfigure();

    void PreRun();

    virtual void Run();

    void PostRun();

    void CleanUp(){ plrsModuleRecorder::CleanUp(); }

};


extern "C" SaberDiskRecorder* create_SaberDiskRecorder( plrsController* c);

extern "C" void destroy_SaberDiskRecorder( SaberDiskRecorder* p );


#endif
