#ifndef PLRSMODULEGRAPHICS_H
    #define PLRSMODULEGRAPHICS_H 1

#include "plrsStateMachine.h"


class plrsModuleGraphics : plrsStateMachine {

public:

    plrsModuleGraphics( plrsController* c);

    ~plrsModuleGraphics();

protected:

    std::string GetModuleName(){ return "graphics";}

    void Configure();

    void UnConfigure();

    void CleanUp();


    virtual void Clear();
        //!< Will clear the graphics for plotting the next.

    virtual void PreEvent();

    virtual void Event();
        //!< Calling this function will plot the data structure pointed to by the pointer in the argument.

    virtual void PostEvent();

    virtual void CommandHandler();

    virtual void Draw( void* p) = 0;

private:

    bool pause;

    uint32_t refresh_rate;

    void* rdo;

    uint32_t now;

    uint32_t last_update;
};


#endif
