// T_Fifo.hpp
// A transactional messaging queue
// (c)2016 Garry Kling
// A single-producer, single-consumer messaging queue

#include <malloc>

// should indexes and sizes be aliased with using?
// using fifoIndexType = uint_16 

class T_Fifo 
{

public: 
// constructor
T_Fifo() : readPos(0), writePos(0) 
{
    // reserve bytes for array (malloc) with compile-time default (2**N)
    *buf = malloc(kNBytes*sizeOf(char));
    // memset to 0
    memset(*buf, 0, kNBytes); 
    // set read 0 and write to start of buf
}

// startWrite (void* ptr) -> int nBytesAvail
unsigned int startWrite(void* ptr)
{
    // set ptr to buf + writeHead
    *ptr = &buf + writeHead
    // return capacity 
    return writePos - readPos;
}


// closeWrite (int nBytesWritten)
void closeWrite(int nBytesWritten)
{
   // move ptr ahead by nBytesWritten
   writeHead += nBytesWritten;
   // capacity -= nBytesWritten
}

// startRead (void* ptr) -> int nBytesAvail
unsigned int startRead(void * ptr)
{
   // if read==write, set ptr to null, return 0
   if (readHead==writeHead)
   {
      *ptr = 0;
      return 0;
   }
   // set ptr to readHead
   *ptr = &buf + readHead
   return sizeof(writeHead-readHead)
}

// closeRead (int nBytesRead)
void closeRead(unsigned int nBytesRead)
{
   // move readHead forward nBytesRead
   readHead += nBytesRead;
   // memset read range to 0?
   // capacity += nBytesRead 
}

private:

   const int kNBytes = 1024;
   unsigned int readPos, writePos;
   void * buf;
}
