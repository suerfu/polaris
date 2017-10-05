#ifndef SABERBOARDRAWDATA_H
    #define SABERBOARDRAWDATA_H 1

#include <vector>
#include <cstdint>

class SaberBoardRawData {

public:

    // constructors and destructor

    SaberBoardRawData();

    ~SaberBoardRawData();

    SaberBoardRawData( const SaberBoardRawData& rhs);

    SaberBoardRawData& operator = ( const SaberBoardRawData& rhs);


    // related to raw memory

    int size();
    
    int size() const;

    int bytes();

    void AllocateBuffer( unsigned int n);

    uint32_t* GetBufferAddr();


    // access details of the event raw data

    bool Valid();

    int operator[]( int n);

    int operator[]( int n) const;

    int GetEventSize();

    unsigned int GetChannelMask();

    int GetBoardID();

    int GetEventID();

    uint32_t GetTimeTag();

private:

    std::vector<uint32_t> buffer;

};

#endif
