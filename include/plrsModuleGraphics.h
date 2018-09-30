#ifndef PLRSMODULEGRAPHICS_H
    #define PLRSMODULEGRAPHICS_H 1

#include "plrsStateMachine.h"


class plrsModuleGraphics : public plrsStateMachine {

public:

    plrsModuleGraphics( plrsController* c);

    ~plrsModuleGraphics();

protected:

    void Configure();

    virtual void Clear();
        //!< Will clear the graphics for plotting the next.

    void Run();
        //!< Calling this function will plot the data structure pointed to by the pointer in the argument.

    //void CommandHandler();

    virtual void Draw( void* p){}

    virtual void Process( void* p){}

private:

    uint32_t refresh_rate;

    uint32_t now;

    uint32_t last_update;
};


#endif
