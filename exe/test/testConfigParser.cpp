#include <iostream>
#include "ConfigParser.h"

using namespace std;

int main( int argc, char* argv[] ){

    if( argc<2){
        cerr << "usage: "<<argv[0]<< " file.cfg" <<endl;
        return 0;
    }

    ConfigParser conf;
    conf.LoadFile( argv[1]);
    conf.Print( cout );
    cout<<endl;

    if( argc==2){
        ofstream file( "test-configparser.txt", ios::binary);
        conf.Serialize( file);
        file.close();

        ifstream file2( "test-configparser.txt", ios::binary);
        ConfigParser conf2;
        conf2.Deserialize( file2 );
        conf2.Print( cerr );
        cout<<endl;
    }
    

/*
    bool found = false;
    cout << "Retrieving integer parameters: /dir1/param_int : ";
    int a = conf.GetInt("/dir1/param_int", &found);
    if( found ){
        cout << a << endl;
    }
    else{
        cout << "not found "<<endl;
    }

    cout << "Retrieving float parameters: /dir1/dir2/param_float : ";
    float b = conf.GetFloat("/dir1/dir2/param_float", &found);
    if( found ){
        cout << b << endl;
    }
    else{
        cout << "not found "<<endl;
    }

    cout << "Retrieving bool parameters: /dir1/param_bool : ";
    bool c = conf.GetBool("/dir1/param_bool", &found);
    if( found ){
        cout << c << endl;
    }
    else{
        cout << "not found "<<endl;
    }

    map<string, vector<string> > mp = conf.GetListOfParameters("/dir");
    for( map<string, vector<string> >::iterator itr = mp.begin(); itr!=mp.end(); itr++){
        cout<<itr->first<<"\t"<<(itr->second)[0]<<endl;
    }
*/
    return 0;
}
