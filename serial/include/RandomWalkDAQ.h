#ifndef RANDOMWALKDAQ_H
    #define RANDOMWALKDAQ_H 1

#include <fstream>

#include "ConfigParser.h"
#include "plrsModuleDAQ.h"
#include "plrsController.h"

class RandomWalkDAQ;

/// class to hold array of random numbers read from /dev/rand
class RandomArray{

public:

    RandomArray( int size);

    ~RandomArray();

    int GetSize(){ return _size;}

    int size() const { return _size;}

    int* GetX() { return x;}

    int* GetY() { return array;}

    void Read( istream& file);
        //!< Read random numbers from file.
        //

    void Write( ostream& file);
        //!< Write the numbers to file.
        //

    int operator[]( int i){ return array[i];}

    int operator[] ( int i) const { return array[i];}

private:

    int* x;

    int* array;

    int _size;

    int n;
        // previous value
};

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

    RandomArray* data;

    int event_size;

    int buff_size;

    int sample_intv;
};


extern "C" RandomWalkDAQ* create_RandomWalkDAQ( plrsController* c);

extern "C" void destroy_RandomWalkDAQ( RandomWalkDAQ* p );

#endif
