#ifndef RANDOMWALKRECORDER_H
    #define RANDOMWALKRECORDER_H 1

#include <string>

#include "plrsStateMachine.h"


class RandomWalkRecorder : public plrsStateMachine{

public:

    RandomWalkRecorder( plrsController* c);    //!< Constructor.

    virtual ~RandomWalkRecorder();  //!< Destructor

protected:

    void Configure();

	void Deconfigure();

    void Run();

protected:

    ofstream output_file;

private:

    int next_addr;
        //!< ID of the next module to which data pointer is passed to.

    int current_value;
        //!< Used to keep track of current value.
        //!< It is incremented or decremented ramdomly.

};



extern "C" RandomWalkRecorder* create_RandomWalkRecorder( plrsController* c );


extern "C" void destroy_RandomWalkRecorder( RandomWalkRecorder* p );

#endif
