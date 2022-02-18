# polaris


polaris is a general-purpose, modular and versatile framework for facilitating small- to mid-scale data acquisition. In the polaris framework:

* User-specific functionalities are implemented as runtime dynamic loaded libraries (DLL) referred to as modules.
* Each module is launched as a separate thread for fast and efficient multi-tasking.
* polaris framework acts as a common platform that coordinates the transition of user modules throughout different phases of DAQ, and the communication between them.

NOTE: if this program is used in your experiment, please cite the following work: *Suerfu, B., 2018. Polaris: a general-purpose, modular data acquisition framework. Journal of Instrumentation, 13(12), p.T12004.*

### Requirement

polaris is based on Linux OS and Linux API. Library dependence is minimal: in addition to standard C++ libraries, it requires pthread and dll at the least.

If inter-process/inter-host communication is needed, the default interface module is linux socket. However user can easily implement an interface module that utilizes other socket libraries.

The graphics of the test module depends on ROOT ( a data analysis framework mainly used in high energy physics), and if one wishes to run the visualization, ROOT libraries should be installed. If python is installed instead, user can also visualiza data by sending it to python program via inter-host communication module.


## Getting Started

### Obtaining the source code

To get a copy of polaris, open terminal and run:

    git clone https://github.com/suerfu/polaris


### Compiling

To compile the project, type in commandline terminal:

    cd polaris
    make

### Installing

To install polaris, under polaris directory type in terminal

    sudo make install

This will copy polaris libraries and executables to /usr/local directory.


### Running polaris

The syntax for running polaris main program is:

    polaris --cfg config-file.cfg --foo bar0 bar1

where config-file.cfg contains information about
1. where to find the libraries containing user modules, and
2. custom parameters passed to the user modules.

Modules should be created by inheriting from plrs StateMachine such that all modules consistently and coherently follow a number of global rules.

Additional arbitrary arguments can be passed along through the commandline interface. In the above case, the additional argument is equivalent to the following line in the config file

    /cmdl/foo : bar0 bar1,

These values (bar0, bar1) is stored as array of strings and transformed depending on the type of the data by different Get methods. For details, please refer to later section on ConfigParser.

## Creating Custom Module

Every custom module must inherit from plrsStateMachine object to make sure everything is coherent and consistent in terms of 
1. state transition and
2. inter-module communication.

### State transition
polaris uses finite-state-machine (FSM) architecture: the state of the system must be one of (NULL, INIT, CONFIG, RUN, END, ERROR). Users specify what the module should be doing at each transition and in each state by implementing certain functions. For example, the function 
```c++
void Configure()
```

is invoked when the state transitions from INIT to CONFIG. And likewise the functions
```c++
void PreRun()
void Run()
void PostRun()
````
are invoded in order when the state goes into the RUN state and then back to CONFIG state when the run is finished.

The FSM architecture ensures that multiple different modules have well-defined behaviors at each stages of the program execution. For example, by implementing COnfigure() method, a run is never started without first configuring the device.

#### State vs Status

polaris distinguishes *State* from *Status*: although both have exact set of values (*NULL, INIT, CONFIG, RUN, END, ERROR*), *State* is a global property that indicates **which state polaris want you to go to** while *Status* is a local property that indicates **which state user module is currently in**. *State* is obtained by each module by 
```c++
DAQSTATE GetState()
```
However, since *State* is a global property, it cannot be set by user modules. On the other hand, user modules report its *Status* to polaris by using
```c++
void SetStatus( DAQSTATE state)
```
If user module sets a wrong (inconsistent) status, reports error status, or didn't manage go to the target status in time, the global state goes to *ERROR* and the program tries to terminate safely.

Therefore, when implementing certain functions, it is necessary to report that local state transition is successful, or check that global state is still consistent with local status. For example, at the end of Configure() function, one should call SetStatus(CONFIG) to report that configuration was successful:
```c++
void Configure(){
    // configure your device
    SetStatus( CONFIG );
}
```
Similarly, in the Run() method, one should make sure that global *State* is still *RUN* between successive events:
```c++
void Run(){
    while( GetState()==RUN ){
        // Acquire your data and process events
    }
}
```

#### Inter-module Communication

Frequently during data acquisition, different modules need to communicate to each other or pass data along. One example is once data is acquired by DAQ module, data should be passed on to recorder objects to write to disk, then optionally data can be passed to graphics object for visualization. For fast DAQ, copying large amount of data is not efficient, therefore one should be using pointers to data buffer instead.

When each user module is created, polaris framework allocates a FIFO circular buffer for every module which is used to store pointers to data. The FIFO buffer is not fixed inside: when new pointers are pushed in when the buffer is full, then it simply grows in size without losing current pointers.

Buffers are accessed by each module using
```c++
void* PullFromBuffer()
```
Above function pulls a general pointer from its own FIFO buffer. If the buffer is emply, it returns a pointer of value 0. Since the framework does not know what data type user is using, the pointer is of type void and user has to cast it to his/her own data type.

Once done with using the resources pointer by the pointer, the pointer should be passed on to the next module in line. This is done by
```c++
void PushToBuffer( int ID, void* ptr)
```
This will give the pointer to the module with the specified ID. The ID of each module can be looked up by using
```c++
void GetIDByName( string module_name)
```
To do this, the name of the module one is trying to pass data to should be specified. Usually this is done in the configuration file. Two special IDs are already provided, and they are access ed by
```c++
int GetNextID()
int GetPrevID()
```
These two IDs corresponds to the modules specified in config file with the key word next_module and prev_module, respectively. In this case, a config file will look like the following:
```
module {
    module1 {
        next_module : xxx,
        prev_module : yyy
    }
    module2 {
        next_module : xxx,
        prev_module : yyy
    }
}
```

#### Writing Configuration Files

Polaris obtains many essential informations via configuration files users specify. In genetal, the configuration file should follow the following structure:

```
par0 : val0_0 val0_1 ... val0_n,
    # These are global parameters (key value pairs).
    # The values are registered as arrays so multiple values can be specified.

module {
    # keyword module indicates that the followings are modules to be loaded from dynamic libraries.
    
    mod0 {
        lib : /path/to/library/libxxx.so,
            # this is the shared library from where module is loaded.
        fcn : Function,
            # the name of the class object to be loaded from the library.
        enable : true,
            # if set to false, it won't be loaded.
        next_module : mod1,
            # the name of the next module in the data flow pipeline.
            # one can also use next instead of next_module as the key.
        prev_module : mod2,
            # the name of the previous module in the data flow pipeline.
            # one can also use prev instead of prev_module as the key.
            # Note that above two modules are completely voluntary and user is not obliged to use them.
            # Users are welcome to specify and configure data flow in other ways as long as the module name is passed to user functions.
        par1 : val1
            # User custom parameters.
            # This parameter will be accessed as GetXXX("/module/mod0/par1", default_value).
            # Depending on the type of data, XXX can be Int, Float, String or Bool.
            # Default value is returtned when trying to access a parameter that is not specified.
    }
}
```

Config file should also follow a number of rules:
  * Comments start with #
  * Key names (parameter name) and values cannot contain space.
  * Directories can be nested with { } (directory module is a special directoty).
  * Between parameter names and values there should be either a semicolon(:) or equal sign(=), which is separated from adjacent characters by space.
  * Each lines end with a comma(,), unless the line is the last line in a directory.
  * Multiple values can be passed by using space between different values.

### Example

To run the tests, first change directory to serial and type

    make

to compile the Serial libraries needed in the tests.

To execute the test application, type

    polaris --cfg cfg/random.cfg --output foo.txt

which reads random numbers from /dev/random and increment/decrement previous data depending on the eveness/oddness of the random number, therefore simulating 'random walk'.

The program can be stopped at any time by typing: 

    quit ( or simply q)

in the terminal.

## Authors

* **Suerfu** - *Initial work* - https://github.com/suerfu

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

