#ifndef PLRSBASEDATA_H
    #define PLRSBASEDATA_H 1

#include <ostream>
#include <string>

enum PLRS_DT{ DT_INT, DT_FLT, DT_STR};

class plrsBaseData{

    friend std::ostream& operator<<( std::ostream& os, plrsBaseData);

public:

// ---------------------------------- Constructors ----------------------------

    plrsBaseData( int a=0 ) : type( DT_INT){ data.i = a; }

    plrsBaseData( float a ) : type( DT_FLT){ data.f = a; }

    plrsBaseData( std::string a ) : type( DT_STR){ str = a; }

    ~plrsBaseData(){}


// ---------------------------------- Copy constructor and assignment operator ------------

    plrsBaseData( const plrsBaseData& rhs);

    plrsBaseData& operator=( const plrsBaseData& rhs);


// ---------------------------------- Assignment operator to data types -------------------

    int& operator=( int a){ type = DT_INT; data.i = a; return data.i;}

    float& operator=( float a){ type = DT_FLT; data.f = a; return data.f;}

    std::string& operator=( std::string a){ type = DT_STR; str = a; return str;}


    PLRS_DT Type(){ return type;}


// ---------------------------------- Member access operators -----------------------------
    int GetInt(){ return data.i;}

    void SetInt( int a ){ data.i = a; type = DT_INT;}


    float GetFloat(){ return data.f;}

    void SetFloat( float a ){ data.f = a; type = DT_FLT;}


    std::string GetStr(){ return str;}

    void SetStr( std::string a ){ str = a; type = DT_STR;}


private:

    PLRS_DT type;

    union {
        int i;
        float f;
    } data;
    
    std::string str;
};

#endif
