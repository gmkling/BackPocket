#ifndef T_FIFO_HPP
#define T_FIFO_HPP
// Single reader, single writer data queue. Lock-free and atomic-free
// Copyright © 2016 Garry Kling, All Rights Reserved

#include <cstring>

class T_Fifo
{

public:

    T_Fifo(int size) : kNBytes(8), readHead(0), writeHead(1),  nBytesWritable(0), nBytesReadable(0)
    {
        kNBytes = size;
        qBuf = new char[kNBytes];
        memset(qBuf, 0, kNBytes);
    }

    ~T_Fifo() { delete[] qBuf; }

    // startWrite () -> int nBytesAvail
    unsigned int startWrite()
    {
        unsigned int index = writeHead;
        nBytesWritable = 0;

        while(index != readHead)
        {
            nBytesWritable++;
            index = nextIndex(index);
            if (index==readHead) { break; }
        }
        return nBytesWritable;
    }

    // writeBytes (void* bytes, nBytes) -> int nBytesWritten
    unsigned int writeBytes(void * bytes, unsigned int nBytes)
    {
        if (nBytesWritable==0) { return 0; }
        unsigned int nBytesToWrite = std::min(nBytes, nBytesWritable);
        unsigned int index = writeHead;

        for (int i=0; i<nBytesToWrite; i++)
        {
           *(qBuf+index) = *(((char*)bytes)+i);  
           index = nextIndex(index);
        }
        writeHead = index;
        return nBytesToWrite;
    }

    // startRead (void* ptr) -> int nBytesAvail
    unsigned int startRead()
    {
       unsigned int index = nextIndex(readHead);
       nBytesReadable = 0;

       if (index==writeHead){ return 0; }
       while(index != writeHead)
       {
           nBytesReadable++;
           index = nextIndex(index);
           if (index==writeHead) { break; }
       }
       return nBytesReadable;
    }

    unsigned int readBytes(void * ptr, unsigned int nBytes)
    {
        if (nBytesReadable==0) { return 0; }
        unsigned int nBytesToRead = std::min(nBytes, nBytesReadable);
        unsigned int index = nextIndex(readHead);

        for (int i=0; i<nBytesToRead; i++)
        {
           *(((char*)ptr)+i) = *(qBuf+index);
           index = nextIndex(index);
        }
        readHead = prevIndex(index);
        unsigned int nBytesReadThisTime = nBytesToRead;
        nBytesReadable = 0;

        return nBytesReadThisTime;
    }

private:

   int kNBytes; // must be power of 2
   unsigned int readHead, writeHead, nBytesWritable, nBytesReadable;
   char * qBuf;
   // ringbuffer utils:
   unsigned int nextIndex(unsigned int index)
   {
       return (index+1) & (kNBytes - 1);
   }
   unsigned int prevIndex(unsigned int index)
   {
       return (index-1) & (kNBytes - 1) ;
   }
};
#endif // T_FIFO_HPP

