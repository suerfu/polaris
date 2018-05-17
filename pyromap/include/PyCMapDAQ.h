#ifndef PYCMAPDAQ_H
    #define PYCDAQMAP_H 1

#include "plrsModuleDAQ.h"
#include "plrsController.h"

#include "serialport.h"

#include <unistd.h>

class PyCMapDAQ : public plrsModuleDAQ{

public:

    PyCMapDAQ( plrsController* c);

    ~PyCMapDAQ();


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

    int GetResponse( char );

    int GetAz();

    int GetAx();

    void MoveTo( int ax, int az);

    int ReadADC();

private:

    float GetAvg( vector<int> input );
    
    float GetVar( vector<int> input );

    bool QualityControl( vector<int> input, float thresh = -1);

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


extern "C" PyCMapDAQ* create_PyCMapDAQ( plrsController* c ){ return new PyCMapDAQ(c);}


extern "C" void destroy_PyCMapDAQ( PyCMapDAQ* p ){ delete p;}

#endif
