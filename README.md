# polaris


polaris is a general-purpose modular and versatile data acquisition framework. User-specific functionalities are implemented as runtime dynamic loaded libraries (DLL) referred to as modules. polaris acts as a common platform that coordinates the function of user modules throughout different stages of DAQ.


## Getting Started


To get a copy of polaris from github:

    git clone https://github.com/suerfu/polaris


### Requirement

polaris is based on Linux operation system and Linux API. Library dependence is minimal: in addition to standard C++ libraries, it required pthread and dll at the least. If inter-process/inter-host communication is needed, the default interface module relies on linux socket. However user can easily implement an interface module that utilizes other socket libraries.

The graphics of the test module depends on ROOT ( a data analysis framework mainly used in high energy physics), and if one wishes to run the visualization, ROOT libraries should be installed. If python is installed instead, user can also visualiza data by sending it to python program via inter-host communication module.


### Compiling

To compile the project, simply change to polaris main directory and type in commandline terminal:

    make

### Installing

To install polaris, switch to polaris directory and type in terminal

    make install

This will copy polaris libraries and executables to /usr/local directory.


## Running polaris

The syntax for running polaris main program is:

    polaris --cfg config-file.cfg --foo bar

where config-file specified contains information about user modules to be loaded. User modules are libraries loadable at runtime, and modules should be created following certain rules. Additional arguments can be passed along to user modules. In the above case, the additional argument is available via ConfigParser as

    /cmdl/foo : bar

For details, please refer to ConfigParser.


You can get an idea of how the modules function by running the test program.

### Running tests

To run the tests, first change directory to serial and type

    make

to compile the Serial libraries needed in the tests.

To execute the test application, type

    polaris --cfg cfg/random.cfg --output foo.txt

which reads random numbers from /dev/random and increment/decrement previous data depending on the eveness/oddness of the random number, therefore simulating 'random walk'.

The program can be stopped at any time by typing: 

    quit ( or simply q)

in the terminal.

## Making your own DAQ program

For details on how to write your own polaris module, please refer to the test application and comments in the source files.

## Authors

* **Suerfu** - *Initial work* - https://github.com/suerfu

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

