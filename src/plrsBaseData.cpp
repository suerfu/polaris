#include <iostream>
#include <string>

#include "plrsBaseData.h"


plrsBaseData::plrsBaseData( const plrsBaseData& rhs) : type( rhs.type ){
    switch( rhs.type ){
        case DT_INT:
            data.i = rhs.data.i;
        case DT_FLT:
            data.f = rhs.data.f;
        case DT_STR:
            str = rhs.str;
    }
}



plrsBaseData& plrsBaseData::operator=( const plrsBaseData& rhs){
    type = rhs.type;
    switch( rhs.type ){
        case DT_INT:
            data.i = rhs.data.i;
        case DT_FLT:
            data.f = rhs.data.f;
        case DT_STR:
            str = rhs.str;
    }
    return (*this);
}


std::ostream& operator<<( std::ostream& os, plrsBaseData var){
    switch( var.Type()){
        case DT_INT:
            os << var.data.i;
            break;
        case DT_FLT:
            os << var.data.f;
            break;
        case DT_STR:
            os << var.str;
            break;
        default:
            break;
    }
    return os;
}
