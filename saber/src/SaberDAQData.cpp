#include "SaberDAQData.h"


SaberDAQData::SaberDAQData(){;}


SaberDAQData::SaberDAQData( std::vector<CAENV1720Parameter> a){

    std::vector<CAENV1720Parameter>::iterator itr;
    for(  itr = a.begin(); itr!=a.end(); ++itr){
        SaberBoardRawData bd;
        bd.AllocateBuffer( 4*itr->GetEvtSizeInWord() );
        board_data.push_back( bd );
    }
}


SaberDAQData::SaberDAQData( const SaberDAQData& rhs){
    board_data = rhs.board_data;
}


SaberDAQData& SaberDAQData::operator=(const SaberDAQData& rhs){
    board_data = rhs.board_data;
    return *this;
}


SaberDAQData::~SaberDAQData(){}


void SaberDAQData::AddBoardData( const SaberBoardRawData& b){
    board_data.push_back(b);
}


void SaberDAQData::Write( ostream& os){
    if( board_data.size()!=0 )
        for( unsigned int i=0; i<board_data.size(); i++){
            os.write( reinterpret_cast<char*>( board_data[i].GetBufferAddr()), board_data[i].bytes());
        }
}


SaberBoardRawData& SaberDAQData::operator[]( unsigned int n){
        return board_data[n];
}


SaberBoardRawData SaberDAQData::GetBoardData( int n){
    return board_data[n];
}


SaberBoardRawData* SaberDAQData::GetBoardDataPtr( int n){
    if( n<int(board_data.size()) )
        return &board_data[n];
    else
        return 0;
}



