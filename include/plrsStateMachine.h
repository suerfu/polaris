#ifndef PLRSSTATEMACHINE_H
    #define PLRSSTATEMACHINE_H 1

#include <string>
#include <pthread.h>
#include <sched.h>

#include "circular_buffer.h"


//! States in the polaris state machine. For both state and status.
enum DAQSTATE { NUL, INIT, CONFIG, RUN, RUN_PAUSE, END, ERROR };
    //!< DAQ states.
    //!< NUL is the initial default state.
    //!< INIT and CONFIG are the states after initialization (memory allocation, etc.) and configuration (preparing daq parameters).
    //!< RUN state is only accessible from CONFIG state.
    //!< END indicates end of program and modules will be deleted.
    //!< When any module signals an ERROR status, the DAQ Controller signals an ERROR state and program will enter END state.


string GetStateName( DAQSTATE state );


class plrsCommand{

public:
    plrsCommand(){
        from = -1;
        cmd = "";
    };

    plrsCommand( int i, string s){
        from = i;
        cmd = s;
    }

    plrsCommand( const plrsCommand& rhs ){
        from = rhs.from;
        cmd = rhs.cmd;
    }

    ~plrsCommand(){}

    plrsCommand& operator =( const plrsCommand& rhs){
        from = rhs.from;
        cmd = rhs.cmd;
        return *this;
    }

//private:
    int from;

    string cmd;
};


class plrsController;   // forward declaration for Module


class plrsStateMachine; // forward declaration for Module


//! Resources allocated to each module.
typedef struct {

    string libname;
        //!< name of library to load the module.

    string fcnname;
        //!< name of the function which was used to get the module object.

    void* handle;
        //!< handle to libraries.
    
    plrsStateMachine* (*helper_creator) (plrsController*);
        //!< extern helper functions
        //
    void (*helper_destroyer) (plrsStateMachine*);
        //!< extern helper functions
    
    plrsStateMachine* fsm;
        //!< Modules under control. IDs are the indexes of modules.
        //
    circular_buffer<void*> buffer;
        //!< Circular buffer allocated to each module. ID-0 reserved.
        //
	circular_buffer< plrsCommand > buffer_cmd;
        //!< Vector of circular buffers for command
        //

    pthread_t* thread;
        //!< each module will be inserted and launched as a new thread.
        //
} Module;


// Module has been declared. Now safe to include controller.
#include "plrsController.h"

// State Loop Flow
//--NUL
//  |-- UpdateStatus
//--INIT
//  |-- Configure
//--CONFIG
//  |-- Deconfigure
//  |
//  |-- PreRun
//  |-- Run
//  |-- PostRun
//--END
//  |-- Deinitialize

class plrsStateMachine{

friend class plrsController;
friend void* LaunchStateMachine( void* );

public:

    plrsStateMachine( plrsController* rhs );  //!< Constructor. Register in Controller and obtain an ID.    
    virtual ~plrsStateMachine();    //!< Destructor. Remove it's own pointer from Controller.

    DAQSTATE GetState();
        //!< Return the state of this FSM.
        //!< State is a global property that every module should be in.

	DAQSTATE GetStatus();
        //!< Return the local status of this FSM.
        //!< Status is a local property.
        //!< Every module should take actions to be in status consistent with state.

    std::string GetModuleName(){
        return module_name;
    }
        //!< Return module name. Used by ctrl to identify different modules.

    int GetID(){
        return ID;
    }
        //!< Returns the module ID of the current module.

//    int GetNextID(){
//        return addr_nxt;
//    }
        //!< Returns module ID of the module specified in /module/foo/next_module or /module/foo/next

//    int GetPrevID(){
//        return addr_prv;
//    }
        //!< Returns module ID of the module specified in /module/foo/prev_module or /module/foo/prev

    //std::string GetVersion(){ return plrsController::GetVersion(); }
        //!< Returns version number for tracking changes.

private:

	int ID; //!< Obtained from controller upon registration. Used to identify different modules.
    void SetID( int i){ ID = i;}    //!< Called by controller to set FSM id.

    string module_name;
        //!< Module name. Configurable via Config Parser.

    
    void SetModuleName( std::string s){
        module_name = s;
    }
        //!< Set module name based on the directory in the configuration file.


    DAQSTATE state;     //!< Denotes state issued by controller.
	DAQSTATE status;    //!< Current local status of the state machine.

	void SetState(DAQSTATE s);  //!< Set the state of FSM. Should be accessed only by controller to change target state.

	DAQSTATE WaitForState( DAQSTATE s, int max_try=0xfffff );
		//!< Will wait for designated global state to be issued. If not issued in time, signal ERROR.

	void EventLoop();
        //!< Called right after thread/module is created.
        //!< This function handles transition between different states.

    pthread_mutex_t mutex_fasm; //!< Protect access to state.

protected:

    template <class T>
    void Print( T t, VERBOSITY v ); //!< Call Controller's Print method to output/display message.

	void SetStatus( DAQSTATE s );   //!< Set the status of this FSM. Derived modules should be able to report status.

	int PushToBuffer( unsigned int i, void*);  //!< Pushes one event into the buffer identified by the ID.

    void* PullFromBuffer();     //!< Pulls one event from the buffer. Can only pull from it's own buffer. This function returns immediately.

    void* PullFromBuffer( DAQSTATE st); //!< Pulls one event from it's own buffer. Will keep trying as long as state is same as specified.

    void PushCommand( unsigned int i, string);   //!< Used to send command to controller

    void PushCommand( unsigned int i, plrsCommand);   //!< Used to send command to controller

    plrsCommand PullCommand();   //!< Check/retrieve command from controller.

    void SendUserCommand( string ); //!< command is specified as string in the format /module-name/command. This function will send the command to specified module. If module is not specified, command is sent to ctrl.

    virtual void CommandHandler(){;}  //!< Called to process command stack.

    void SendPauseReq();

    void ClearPauseReq();

    // ===================================================================================

	plrsController* ctrl;   //!< Pointer to DAQ controller object.

    ConfigParser* cparser;

    ConfigParser* GetConfigParser(){
        return cparser;
    }
        //!< Used to access configuration file inside the program.
        //!< This approach is better than using protected variable.

    map< string, int > module_table;

    void GetModuleTable();  //!< Get information on other modules.


    // ===================================================================================
    // virtual methods to be implemented in derived classes
    // ===================================================================================

    int addr_nxt;
        //!< Next address to push data to.

    int addr_prv;
        //!< Previous address to push data to. Needed in case data has to be rejected.


    virtual void Initialize(){}
        //!< Called at the beginning to change state to INIT.

    //virtual void ConfigDataFlow();
        //!< Called to establish data flow between state machines.
        //!< This function is called after module table is initialized and before Configure is called.

    virtual void Configure() = 0;
        //!< Called by controller to configure the DAQ system.

    virtual void Deconfigure(){}
        //!< Used to transition from Config back to Init.

    virtual void Deinitialize(){}
        //!< Called at the beginning of END phase.

    void RunLoop();
        //!< 

    virtual void PreRun(){}
        //!< Function called before the beginning of run.

    virtual void Run(){
        PreEvent();
        Event();
        PostEvent();
    }
        //!< Main part of data acquisition.

    virtual void PostRun(){}
        //!< Function called after the end of run.

    virtual void Idle();

    virtual void Pause(){
        Print( GetModuleName()+" pausing...\n", INFO);
    }

    virtual void Resume(){
        Print( GetModuleName()+" resuming...\n", INFO);
    }

    virtual void PreEvent(){}
        //!< Function called at the beginning of each event.

    virtual void Event(){}
        //!< Function called to process each event.

    virtual void PostEvent(){}
        //!< Function called at the end of each event.

};

template <class T>
void plrsStateMachine::Print( T t, VERBOSITY v ){
    cparser->Print( "    "+this->GetModuleName()+" : "+t, v);
}

#endif
