#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H 1

#include <string>
#include <vector>
#include <map>

#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <iomanip>

#include <ctime>
#include <sstream>

#include <pthread.h>

//#include "pLog.h"

//! Verbosity denotes the importance / detailness of each message.
//! Both registered output channel and output messages have a verbosity level.
//! Messages will be output by channels with verbosity level higher than that of the message
enum VERBOSITY{ ERR = 1, INFO = 2, DETAIL = 3, DEBUG = 4};


using namespace std;

//! ConfigParser loads parameters from a config file as strings and store in a directory-like structure using std::map.
//! The config file format is directory { key value ... subdir { key value ... } }

class ConfigParser {

public:

    // Constructor
    ConfigParser();
        //!< Default constructor.

    ConfigParser( const char* p);
        //!< Load parameters from file.

    ConfigParser( int argc, char* argv[]);
        //!< Load parameters from commandline arguments.


    // Destructor
    ~ConfigParser();


    // Copy constructor and assignment operator
    ConfigParser( const ConfigParser& rhs);

    ConfigParser& operator=( const ConfigParser& rhs);

    void Serialize( ostream& os);
        //!< Write the content of config parser into byte stream

    void Deserialize( istream& is);
        //!< Read file to get the content of config parser

    void Initialize();
        //!< Initialize function to avoid repeating code in constructor.


    // Operations on entire structure
    void LoadCmdl( int argc, char* argv[]);
        //!< All commandline arguments will be added as parameters

    int LoadFile( const string& s);
        //!< load parameters in the config file.

    void Clear();
        //!< Clear all parameters.

    string GetQuotedString( istream& );
        //!< Get " delimited string from file


    // Operation on individual parameters.
    int AddParameter( const string& s, string t);
        //!< Add parameter. First argument should be in the form dir/parameter.

    int RemoveParameter( const string& s);
        //!< Removes entire parameter from map.

    int RemoveParameter( const string& s, const string& rm);
        //!< Searches the value and remove only matched value.

    bool Find( const string& s);


    // access methods for arrays.
    // string array
    vector<string> GetStrArray( const string& name );
        //!< Returns vector of parameters as strings. Empty vector if not found.

    // int
    vector<int> GetIntArray( const string& name );
        //!< Returns vector of parameters as integers. Empty vector if not found.

    // float
    vector<float> GetFloatArray( const string& name ); 

    // bool
    vector<bool> GetBoolArray( const string& name ); 


    // returns 0th member in the vector.
    string GetString( const string& name);
        //!< Returns the first element of the string vector. Emprty string if not found.

    string GetString( const string& name, string def);
        //!< Returns the first element of the string vector. With default value and warning message.

    int GetInt( const string& name, bool* found);
        //!< Returns the first element of the vector. Set second argument to false if not found.

    int GetInt( const string& name, int def);
        //!< Returns the first element of the vector. Set second argument to false if not found.

    float GetFloat( const string& name, bool* found); 

    float GetFloat( const string& name, float def); 

    bool GetBool( const string& name, bool* found); 

    bool GetBool( const string& name, bool def); 

    map< string, vector<string> > GetListOfParameters( const string& s);

    ConfigParser GetSubParameters( const string& s);

    int size();
        //!< return size of the underlying map

    bool empty();

    map< string, map<string, vector<string> > >::iterator begin();
    map< string, map<string, vector<string> > >::iterator end();

    map< string, map<string, vector<string> > >::reverse_iterator rbegin();
    map< string, map<string, vector<string> > >::reverse_iterator rend();

    map< string, map<string, vector<string> > >::const_iterator cbegin();
    map< string, map<string, vector<string> > >::const_iterator cend();

    map< string, map<string, vector<string> > >::const_reverse_iterator crbegin();
    map< string, map<string, vector<string> > >::const_reverse_iterator crend();

    void Print( ostream& os, string prefix = "");

    void Print();


    template < class T >
    void Print( T t, VERBOSITY v);

    void SetVerbosity( VERBOSITY v) { verb = v;}

    void AddLog( char* s );

private:

    vector<string> directory;
        //!< Variable to hold working directory of parameters.

    pthread_mutex_t mutex_map;
        //!< mutex to guard data structure for multithreading.

    map< string, map<string, vector<string> > > parameters;
        //!< data structure to hold the parameters.
        //!< Key of the first map is the directory name, and key of second is the parameter name.
        //!< Parameter type supports vector.

    vector< string > file_loaded;
        //!< Prevent loading same file multiple times via the @include feature.

    void RmComment(  ifstream& input, char marker = '#' );
        //!< Removes white space, lines starting with comment marker and any following white lines.

    void RmLine(  ifstream& input );
        //!< Remove one line (up to \n) and any succeeding white space characters.

    string ExtractDirectory( const string& s) const;
        //!< Given a string, it will strip the previous /../ and returns the directory

    string ExtractParameter( const string& s) const;
        //!< Given a string, it returns the parameter with no preceding directory.

    string GetCurrentDir(vector<string> s);
        //!< It will sum up all strings in the vector to form a directory

    bool str_to_bool( string s);
    
    pthread_mutex_t mutex_cout;
        //!< mutex to protect IO output

    VERBOSITY verb;

    ofstream log;

    time_t last_print;
};


template < class T >
void ConfigParser::Print( T t, VERBOSITY v){
    if( verb>=v ){
        pthread_mutex_lock( &mutex_cout);
            time_t ct = time(0);

            if( last_print == ct){
                std::cout << "          " << t<< std::setfill(' ') << std::setw(0);
                if( log )
                    log << "          " << t<< std::setfill(' ') << std::setw(0);
            }
            else{
                last_print = ct;
                tm* tm_info = localtime(&ct);
                std::cout << std::setfill('0') << std::setw(2) << tm_info->tm_hour <<":"
                          << std::setfill('0') << std::setw(2) << tm_info->tm_min << ":"
                          << std::setfill('0') << std::setw(2) << tm_info->tm_sec << "  " << t
                          << std::setfill(' ') << std::setw(0);
                if( log )
                    log << std::setfill('0') << std::setw(2) << tm_info->tm_hour <<":"
                        << std::setfill('0') << std::setw(2) << tm_info->tm_min << ":"
                        << std::setfill('0') << std::setw(2) << tm_info->tm_sec << "  " << t
                        << std::setfill(' ') << std::setw(0);
            }
        pthread_mutex_unlock( &mutex_cout);
    }
}




/// This function should be launched as a thread.
/// It will obtain user input as a string, and push it to the queue of strings pointed to the pointer in the argument. 
void* GetInput( void* a);



/// Calling this function will launch GetInput thread.
/// This function has a static queue of strings that holds the input strings.
/// Calling this function will either return the next string in queue, or "" in case of no input.
string getstr();


#endif
