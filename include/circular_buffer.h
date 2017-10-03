#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H 1

#include <pthread.h> 
#include <iostream>
#include <vector>
using namespace std;

template <typename T>
class circular_buffer{

public:
    circular_buffer();
        //!< Default constructor. Creates an empty container.

    ~circular_buffer();
        //!< Destructor.

    circular_buffer<T>& operator= ( const circular_buffer<T>& rhs);

    void clear();
        //!< Clears all elements and make the buffer an empty container.

    //===============================================================

    T front();
        //!< Used to access the first element in the FIFO circular buffer (pointed to by head).

    T back();
        //!< Used to access the last element (most recently inserted element) in the buffer (element pointed to by tail).

    //===============================================================

    void push_back(const T& item);
        //!< Inserts item to the end of the buffer. Returns 0 on success and -1 on failure.

    void pop_front();
        //!< Reads from the front of the FIFO. Result is written in item directly. Returns 0 on success, and -1 on failure.

    //===============================================================

    int size();
        //!< Returns the number of items currently in the buffer.

    bool empty();
        //!< True if empty.

    void print();

private:

    std::vector<T> buffer;

    int head;
        //!< Used to access the beginning of the circular buffer.
    int tail;
        //!< Used to access the end of the circular buffer.

    int _size;
        //!< Refers to the number of accessible elements in the buffer, not the size of the underlying vector.

    bool _empty, next_empty;
        // _empty: head==tail==0
        // next_empty: head==tail, if one does one more pop_front, it will be empty.

    pthread_mutex_t mutex_buff;

};


template < typename T>
circular_buffer<T>::circular_buffer( ) {
    head = 0;
    tail = 0;
    _size = buffer.size();
    _empty = true;   next_empty = false;
    pthread_mutex_init( &mutex_buff, 0);
};

template <typename T>
circular_buffer<T>& circular_buffer<T>::operator= ( const circular_buffer<T>& rhs){
    head = rhs.head;
    tail = rhs.tail;
    _size = buffer.size();
    _empty = rhs._empty;
    next_empty = rhs.next_empty;
    buffer = rhs.buffer;
    return *this;
}


template < typename T>
circular_buffer<T>::~circular_buffer(){
    pthread_mutex_destroy( &mutex_buff);
}

template < typename T>
void circular_buffer<T>::clear(){
    pthread_mutex_lock( &mutex_buff);
        buffer.clear();
        head = tail = _size = 0;
        _empty = true;   next_empty = false;
    pthread_mutex_unlock( &mutex_buff);
}

template < typename T>
T circular_buffer<T>::back(){
    T temp;
    pthread_mutex_lock( &mutex_buff);
        temp = buffer[tail];
    pthread_mutex_unlock( &mutex_buff);
    return temp;
}

template < typename T>
T circular_buffer<T>::front(){
    T temp;
    pthread_mutex_lock( &mutex_buff);
        temp = buffer[head];
    pthread_mutex_unlock( &mutex_buff);
    return temp;
}

template < typename T>
void circular_buffer<T>::push_back( const T& item){
    pthread_mutex_lock( &mutex_buff);
        if( _empty){
            // if there is no element in the vector, insert one and have indexes point to the beginning.
            // otherwise, just make both index point to the beginning.
            // status will be next_empty.
            if( buffer.size()==0 )
                buffer.push_back( item);
            else
                buffer[0] = item;
            _size = 1;
            head = tail = 0;
            _empty = false;      next_empty = true;
        }
        else{
            // if it is not _empty to begin with, judge by if the next-to-insert position overlaps with back/tail or not.
            // if overlaps, it is full, and _size needs to be increased.
            // otherwise, just manipulating indexes is enough.
            // since it is not _empty, there is at least one element, and putting another will make _empty and next_empty both false.
            int next = (tail+1)%buffer.size();
                // should use vector size in the modulus.
                // at this point, _size is 1, then next is also 1 and will overlap with head.
            if( next!=head){
                buffer[next] = item;
                tail = next;
            }
            else{
                // re-order the vector
                if( head!=0 ){
                    buffer.push_back(buffer[0]);
                    for( int i=0; i<tail; i++){
                        buffer[i] = buffer[i+1];
                    }
                    buffer[tail] = item;
                }
                else{
                    buffer.push_back(item);
                    ++tail;
                }
            }
            ++_size;
            _empty = next_empty = false;
        }
//    cout<<"> push_back "; print();
    pthread_mutex_unlock( &mutex_buff);
}

template < typename T>
void circular_buffer<T>::pop_front( ){
    // If the buffer is empty, do nothing. If not empty, then depending on if it contains only one element or not (next_empty).
    // If next empty, then simply make the container empty since both pointers are pointing to the same place.
    // If not next empty, increment head towards tail by 1, and decrease _size by 1.
    pthread_mutex_lock( &mutex_buff);
        if(!_empty){
            if( !next_empty ){
                head++;
                head %= buffer.size();
                    // here should use the size of the vector.
                if( head==tail)
                    next_empty = true;
                        // if after the increment, pointing to the same location, next pop will result in an empty vector.
            }
            else{
                _empty = true;
                next_empty = false;
            }
            --_size;
        }
//    cout<<"> pop_front "; print();
    pthread_mutex_unlock( &mutex_buff);
}

template < typename T>
int circular_buffer<T>::size(){
    int r = 0;
    pthread_mutex_lock( &mutex_buff);
        r = _size;
    pthread_mutex_unlock( &mutex_buff);
    return r;
}


template < typename T>
bool circular_buffer<T>::empty(){
    bool r = false;
    pthread_mutex_lock( &mutex_buff);
        r = _empty;
    pthread_mutex_unlock( &mutex_buff);
    return r;
}


template <typename T>
void circular_buffer<T>::print(){
    pthread_mutex_lock( &mutex_buff);
        cout<<endl;
        //typename vector<T>::iterator it;
        cout<<" head is "<<head<<" size is "<<_size<<endl;
        for( int i = head; i<head+_size; ++i){
            cout<<buffer[i]<<" | ";
        }
        cout<<endl;
        for( int i = 0; i<buffer.size(); ++i){
            cout<<buffer[i];
            if( i==head && head==tail) cout<<" HT| ";
            else if( i==head) cout<<" H|";
            else if( i==tail) cout<<" T| ";
            else cout<<"| ";
        }
        cout<<endl;
    pthread_mutex_unlock( &mutex_buff);
}

#endif
