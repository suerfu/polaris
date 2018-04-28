#ifndef PLRSMODULEGRAPHICS_H
    #define PLRSMODULEGRAPHICS_H 1

#include "plrsStateMachine.h"


class plrsModuleGraphics : public plrsStateMachine {

public:

    plrsModuleGraphics( plrsController* c);

    ~plrsModuleGraphics();

protected:

    std::string GetModuleName(){ return "graphics";}

    void Configure();

    void Deconfigure();

    void Deinitialize();


    virtual void Clear();
        //!< Will clear the graphics for plotting the next.

    virtual void PreEvent();

    virtual void Event();
        //!< Calling this function will plot the data structure pointed to by the pointer in the argument.

    virtual void PostEvent();

    virtual void CommandHandler();

    virtual void Process( void* p){}
    
    virtual void Draw( void* p){}

private:

    bool pause;

    uint32_t refresh_rate;

    void* rdo;

    uint32_t now;

    uint32_t last_update;
};


#endif
