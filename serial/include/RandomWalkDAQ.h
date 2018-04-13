#ifndef RANDOMWALKDAQ_H
    #define RANDOMWALKDAQ_H 1

#include <fstream>

#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"


class RandomWalkDAQ : public plrsModuleDAQ {

public:
    RandomWalkDAQ( plrsController* ctrl);

    ~RandomWalkDAQ();

protected:
    void Configure();

    void UnConfigure();

    void CleanUp();

    void StartDAQ();

    void StopDAQ();

    void PreEvent();

    void Event();

    void PostEvent();

private:

    ifstream file;

    int event_size;

    int buff_size;

    int sample_intv;

    int current_value;
};


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c);

extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p );

#endif
