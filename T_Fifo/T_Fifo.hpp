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


    // memset state and byte vals to 0
    memset(qBuf, 0, kNBytes);
    for (int i=0; i<kNBytes; i++)
    {
        slotState[i] = 0;
    }
}

~T_Fifo()
{
//    delete[] qBuf;
//    delete[] slotState;
}

// startWrite () -> int nBytesAvail
unsigned int startWrite()
{
    unsigned int index = writeHead;
    unsigned short flag = *(slotState+index);
    nBytesAvail = 0;

    // flag==0 means we can write
    while(flag==0 && (index != readHead))
    {
        // slot is open, reserve it
        *(slotState+index)=1; 
        nBytesAvail++;
        index = nextIndex(index);
        if (index==readHead) { break; }
        flag = *(slotState+index);    
    }
  //  std::cout<<"Write requested, "<<nBytesAvail<<" bytes available."<<std::endl;   
    return nBytesAvail;
}

// writeBytes (void* bytes, nBytes) -> int nBytesWritten
unsigned int writeBytes(void * bytes, unsigned int nBytes) 
{
    if (nBytesAvail==0) { return 0; }
    unsigned int nBytesToWrite = std::min(nBytes, nBytesAvail);
    unsigned int index = writeHead;
    
    // perhaps we can be more clever and use memcpy without breaking the ringbuffer
    for (int i=0; i<nBytesToWrite; i++)
    {
       *(qBuf+index) = *(((char*)bytes)+i);
       *(slotState+index) = 0; // unreserve
       index = nextIndex(index);
    }
    writeHead = index;
    // if we reserved more, unset the flag
    if (nBytesToWrite<nBytesAvail)
    {
        int nToClear = nBytesAvail-nBytesToWrite;
        unsigned int retAddr = writeHead;
        
        for (int j = 0; j<nToClear; j++)
        {
           *(slotState+writeHead)=0;
           writeHead = nextIndex(writeHead);
        }
        writeHead = retAddr;
    }
    return nBytesToWrite;
}

// startRead (void* ptr) -> int nBytesAvail
unsigned int startRead()
{
   unsigned int index = nextIndex(readHead);
   unsigned short flag = *(slotState+index);
   nBytesReadable = 0;

   // return if there is nothing to read 
   if (index==writeHead){ return 0; }
   while(flag==0 && (index != writeHead))
   {
       *(slotState+index)=1;
       nBytesReadable++;
       index = nextIndex(index);
       if (index==writeHead) { break; }
       flag = *(slotState+index);
   }
//   std::cout<<"Read requested, "<<nBytesReadable<<" bytes readable."<<std::endl;   
   return nBytesReadable;
}

unsigned int readBytes(void * ptr, unsigned int nBytes)
{
 
    if (nBytesReadable==0) { return 0; }
    unsigned int nBytesToRead = std::min(nBytes, nBytesReadable);
    unsigned int index = nextIndex(readHead);
    
    // perhaps we can be more clever and use memcpy without breaking the ringbuffer
    for (int i=0; i<nBytesToRead; i++)
    {
       *(((char*)ptr)+i) = *(qBuf+index);
       *(slotState+index)=0; // unreserve
       index = nextIndex(index);
    }

    readHead = index;
    // if we reserved more, unset the flag
    if (nBytesToRead<nBytesReadable)
    {
        int nToClear = nBytesReadable-nBytesToRead;
        unsigned int retAddr = readHead;

        for (int j = 0; j<nToClear; j++)
        {
           *(slotState+readHead)=0;
           readHead = nextIndex(readHead);
        }
        readHead = retAddr;
    }
    readHead = prevIndex(readHead);
    unsigned int nBytesReadThisTime = nBytesToRead;
    nBytesReadable = 0;
    
    return nBytesReadThisTime;
}

private:
    
   unsigned int nextIndex(unsigned int index)
   {
       return (index+1) & (kNBytes - 1);
   }
   unsigned int prevIndex(unsigned int index)
   {
       return (index-1) & (kNBytes - 1) ;
   }
   static const int kNBytes = 8; // must be power of 2
   unsigned int readHead, writeHead, nBytesAvail, nBytesReadable;
//   unsigned short * slotState;
//   char * qBuf;

//   unsigned short * slotState = new unsigned short[kNBytes];
//   char * qBuf = new char[kNBytes];

   unsigned int slotState[kNBytes];
   char qBuf[kNBytes];

};
