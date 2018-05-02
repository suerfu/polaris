#ifndef PYCWAVEDAQ_H
    #define PYCWAVEDAQ_H 1

#include "plrsModuleDAQ.h"
#include "plrsController.h"

#include "serialport.h"

#include <unistd.h>

class PyCWaveDAQ : public plrsModuleDAQ{

public:

    PyCWaveDAQ( plrsController* c);

    ~PyCWaveDAQ();


    void Configure();

    void Deconfigure();

    void PreRun();

    void Run();

    void PostRun();

    void PreEvent();

    void Event();

    void PostEvent();



    void ZeroAx();

    void ZeroAz();

    void LaserOn( bool t = true);

    void MotorOn( bool t = true);

    void Rotate( bool cw);

    void Move( bool fw);

    int GetAz();

    int GetAx();

    void MoveTo( int ax, int az);

    int ReadADC();

private:

    uint32_t start_time;

    serialport port;

    vector<int> scan_ax;

    vector<int> scan_az;

    int buff_depth;

    int navg;

    float drift_threshold;

    int offset_ax;

    int offset_az;
};


extern "C" PyCWaveDAQ* create_PyCWaveDAQ( plrsController* c ){ return new PyCWaveDAQ(c);}


extern "C" void destroy_PyCWaveDAQ( PyCWaveDAQ* p ){ delete p;}

#endif
