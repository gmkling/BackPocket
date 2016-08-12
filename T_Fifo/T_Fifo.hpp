// T_Fifo.hpp
// A transactional messaging queue
// (c)2016 Garry Kling
// A single-producer, single-consumer data queue

#include <cstring>

// should indexes and sizes be aliased with using?
// using fifoIndexType = uint_16 

class T_Fifo 
{

public: 
// constructor
T_Fifo() : readHead(0), writeHead(1),  nBytesAvail(0), nBytesReadable(0) 
{
    // allocate arrays - could this be in the default args?
    *qBuf = new char[kNBytes];
    *slotState = new char[kNBytes];

    // memset state and byte vals to 0
    memset(qBuf, 0, kNBytes); 
    memset(slotState, 0, kNBytes);
}

~T_Fifo()
{
    delete[] qBuf;
    delete[] slotState;
}

// startWrite () -> int nBytesAvail
unsigned int startWrite()
{
    unsigned int index = writeHead;
    unsigned short flag = *(slotState+index);
    nBytesAvail = 0;

    // flag==0 means we can write
    while(flag==0 && index != readHead)
    {
        // slot is open, reserve it
        *(slotState+index)++; 
        ++nBytesAvail;
        index = nextIndex(index);
        flag = *(slotState+index);    
    }

    return nBytesAvail;
}

// writeBytes (void* bytes, nBytes) -> int nBytesWritten
unsigned int writeBytes(void * bytes, unsigned int nBytes) 
{
    if nBytesAvail==0 { return 0; }
    // should we recalc nBytesAvail or assume user recently called startWrite?
    // Would that make startWrite redundant?
    unsigned int nBytesToWrite = (nBytes < nBytesAvail) ? nBytes : nBytesAvail;
    unsigned int index = writeHead;
    
    // perhaps we can be more clever and use memcpy without breaking the ringbuffer
    for (int i=0; i++; i<=nBytesToWrite)
    {
       *(qBuf+index) = (char*)(bytes+i);
       *(slotState+index)--; // unreserve
       index = nextIndex(index);
    }

    return nBytesToWrite;
}

// startRead (void* ptr) -> int nBytesAvail
unsigned int startRead()
{
   unsigned int index = readHead;
   unsigned short flag = *(slotState+index);
   nBytesReadable = 0;

   // return if there is nothing to read 
   if (index==writeHead-1){ return 0; }

   while(flag==0 && index != writeHead)
   {
       *(slotState+index)++;
       ++nBytesReadable;
       index = nextIndex(index);
       flag = *(slotState+index);
   }
   
   return nBytesReadable;
}

unsigned int readBytes(void * ptr, unsigned int nBytes)
{
 
    if nBytesReadable==0 { return 0; }
    // should we recalc nBytesAvail or assume user recently called startWrite?
    // Would that make startWrite redundant?
    unsigned int nBytesToRead = (nBytes < nBytesReadable) ? nBytes : nBytesReadable;
    unsigned int index = readHead;
    
    // perhaps we can be more clever and use memcpy without breaking the ringbuffer
    for (int i=0; i++; i<=nBytesReadable)
    {
       (char*)(bytes+i) = *(qBuf+index);
       *(slotState+index)--; // unreserve
       index = nextIndex(index);
    }

    return nBytesReadable;}

private:
    
   unsigned int nextIndex(unsigned int index)
   {
       return (index+1) & (kNBytes - 1);
   }

   const int kNBytes = 8; // must be power of 2, capacity will be size-1
   unsigned int readHead, writeHead, nBytesAvail, nBytesReadable;
   unsigned short * slotState;
   char * qBuf;
}
