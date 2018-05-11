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

    void Deconfigure();

    void PreRun();

    void Run();

private:

    ifstream file;

    unsigned int start_time;

    int sample_intv;

    int buff_depth;

    int ncol;

    vector<int> current_value;
};


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c ){ return new RandomWalkDAQ(c);}


extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p ){ delete p;}


#endif
