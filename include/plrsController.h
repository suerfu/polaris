#ifndef PLRSCONTROLLER_H
    #define PLRSCONTROLLER_H 1

#include <vector>
#include <string>
#include <set>
#include <pthread.h>
#include <chrono>

#include "ConfigParser.h"
#include "plrsStateMachine.h"


void* LaunchStateMachine( void* p);
    //!< Helper function that launches state machines.
    //!< The argument void* pointer is a pointer to plrsStateMachine objects.

class plrsController {

friend class plrsStateMachine;
		//!< Every DAQ object will derive from  plrsStateMachine
        //!< Each module needs to access the inter-module and module-controller interface.

public:

    plrsController( ConfigParser* parser);
        //!< Constructor. It will initialize mutex variables.

    ~plrsController();
        //!< Destructor. Will clear the state machine vector and release libraries.

	void StateLoop();
        //!< Main function. Make each installed modules transition between different states.

    unsigned int GetTimeStamp();
        //!< Thread safe way to call time(0)  and returns time since Linux epoch.

    unsigned int GetMSTimeStamp(){
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time_ms).count();
    }

    unsigned int GetWDTimer(){ return wdtimer;}
        //!< Returns current value of watchdog timer.

    unsigned int GetWDRemainingTime(){ return wdtimer-GetTimeStamp();}

    void UpdateWDTimer( unsigned int n = 2){
        if ( wdtimer - GetTimeStamp() < n )
            wdtimer += n;
    }
        // Increment watchdog timer by two seconds to allow for more time for state transition.

    ConfigParser* GetConfigParser();
        //!< Returns parameter map.

    //===================== Modules =================

    void LoadModules();
        //!< Load modules specified in the config file with key /module/mod_name

    void InsModule( plrsStateMachine* p, const std::string& modname);
        //!< Dynamically launches a new module as a thread.
        //!< Arguments are the library that contains the module, and the name of the function.

    void InsModule( const std::string& libname, const std::string& fname, const std::string& modname);
        //!< Dynamically launches a new module as a thread.
        //!< Arguments are the library that contains the module, and the name of the function.

    void RmModule( unsigned int i );
        //!< Remove a module using its ID
    
    void RmModule( string modname);
        //!< Remove a module using module name.

    map< string, int> GetModuleTable();
        //!< Return registered module names and ID

    int GetIDByName( const string& module_name);    //!< Returns the index for the given module name. -1 if no module found.

    string GetNameByID( const int a);   //!< Given the id of the module, return it's name

    template <class T>
    void Print( T, VERBOSITY);

private:

    ConfigParser* cparser;  //!< Configuration parser to store user parameters.

    vector< Module > module;    //!< Polaris modules.

    void* LibraryOpen( const string& libname);
        //!< keeps track of open libraries. If library is already open, return handle to it.

    map< string, int> module_table;

    unsigned int wdtimer;
        //!< Watchdog timer. When this timer is hit, timeout error is issued.

    void InitializeWDTimer( unsigned int n = 3){ wdtimer = GetTimeStamp() + n; }

    // ============ DAQ state-related ====================

    DAQSTATE state; //!< The global DAQ state.

    bool ChangeState( DAQSTATE foo, unsigned int wait_time=6);
        //!< Changes the state of all registered state machines.
        //!< Controller will try max_try times at most, with wait_time interval in microseconds.

    DAQSTATE GetState();
        //!< Returns current state;

    void SetState( DAQSTATE st);
        //!< Set global state. Cannot be used to reset error state.

    DAQSTATE GetNextState();
        //!< Sets the next state.

    std::map< int, std::string > pause_req;

    bool CheckPauseFlag();

    bool CheckErrorFlag();   //!< Return whether an error condition exists, either in global state or in any of registered modules.

    bool CheckStateConsistency();   //!< Return whether an error condition exists, either in global state or in any of registered modules.

    bool CheckStopFlag();   //!< Return whether DAQ should stop. Will be probed during event loop.

    unsigned int start_time;    //!< Start time of DAQ
    unsigned int max_run_time;  //!< Maximum running time of DAQ

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_ms;

    bool stop_flag;         //!< Set to true if user types quit or q.

    void PrintState( VERBOSITY v = ERR ); //!< print the states of all current modules

    // ============ inter-module communication ====================


	int PushToBuffer( unsigned int i, void* data);  //!< Pushes raw pointer into buffer denoted by index i.
                                                	//!< If invalid data or index, return -1.

    void* PullFromBuffer( unsigned int i);  //!< Pulls one raw pointer from buffer denoted by index i.
                                        	//!< 0 is returned if empty, out of range, or invalid caller.

    void PushCommand( int i, plrsCommand c);  //!< Pushes command into stack.

    plrsCommand PullCommand( int i); //!< Gets one command from command stack.

    void CommandHandler();          //!< Called in the event loop to handle commands from IO and other modules.

    // ============= thread-safe ================


    pthread_mutex_t mux_state;  //!< Protect controller's own state.
    pthread_mutex_t mux_fsm;    //!< Protect state machine access and control.
    pthread_mutex_t mux_buffer; //!< Protect data buffer.
    pthread_mutex_t mux_cmd;    //!< Protect command stack.
    pthread_mutex_t mux_time;   //!< Guard time-stamp
    pthread_mutex_t mux_flag;   //!< mutex to protect stop flag

};

template <class T>
void plrsController::Print( T t, VERBOSITY v){
    cparser->Print( t, v);
}

#endif
