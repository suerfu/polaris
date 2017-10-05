#include <sstream>
#include <ostream>
#include <istream>
#include <fstream>
#include <iostream>

using namespace std;

class Person{

public:
    string name;
    int age;
    void serialize( ostream& os){
        stringstream ss;
        ss<< name << age;
        os.write( ss.str().c_str(), ss.str().length());
        cout << " writing: "<<ss.str() <<endl;
        for( int i=0; i<10; i++)
            os.write( (char*)( &i ), sizeof( int));
    }
};

int main(){
    Person a, b;
    a.name = "LiuYinyu";
    a.age = 28;

    b.name = "JinLu";
    b.age = 29;

    ofstream file;
    file.open( "test-serialize.txt", ios::binary);

    uint32_t header = 0xcc1234cc;
    file.write( reinterpret_cast<char*>( &header ), sizeof( header) );
    file.write( reinterpret_cast<char*>( &header ), sizeof( header) );
    file.write( reinterpret_cast<char*>( &header ), sizeof( header) );

    //a.serialize(file);
    //b.serialize(file);

    return 0;
}
