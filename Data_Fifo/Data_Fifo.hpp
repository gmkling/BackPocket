#ifndef DATA_FIFO_HPP
#define DATA_FIFO_HPP
// Single reader, single writer data queue. Lock-free and atomic-free
// Copyright © 2016 Garry Kling, All Rights Reserved

#include <cstring>

// Data_Fifo clients ask for a certain amount of space to read or write
// and promise to respect the returned value

class Data_Fifo
{

public:

    Data_Fifo(int size) : kNBytes(8), readHead(0), writeHead(1), nBytesWritable(0), nBytesReadable(0)
    {
        // check if kNBytes is a power of two and juke it if not
        kNBytes = size;
        qBuf = new char[kNBytes];
        memset(qBuf, 0, kNBytes);
    }

    ~Data_Fifo() { delete[] qBuf; }

    // startWrite () -> returns void * to data, nBytesAvail is corrected 
    void * startWrite(unsigned int &nBytesAvail)
    {
        unsigned int index = writeHead;
        nBytesWritable = 0;
        //unsigned int nBytesToWrite = std::min(nBytesAvail, nBytesWritable);

        while(index != readHead)
        {
            nBytesWritable++;
            index = nextIndex(index);
            if (index==readHead) { break; }
        }

        nBytesAvail = nBytesWritable;
        //writeHead = index;
        
        return (qBuf+writeHead);
    }

    // finishWrite (uint nBytes)
    void finishWrite(unsigned int nBytesWritten)
    {
        int i = 0;
        unsigned int index = writeHead;
        
        // update writehead
        while(i<nBytesWritten)
        {
          index = nextIndex(index);
          i++;
        }
        nBytesWritable = 0;
        writeHead = index;
    }

    // startRead (void* ptr) -> int nBytesAvail
    void * startRead(unsigned int &nBytesReq)
    {

      unsigned int index = nextIndex(readHead);
      nBytesReadable = 0;

      if (index==writeHead) // case of empty buffer
      { 
        nBytesReq=0; 
        return nullptr; 
      }

      while(index != writeHead)
      {
        nBytesReadable++;
        index = nextIndex(index);
        if (index==writeHead) { break; }
      }

       nBytesReq = std::min(nBytesReadable, nBytesReq);
       //readHead = prevIndex(index);

       return qBuf+nextIndex(readHead);
    }

    void finishRead(unsigned int nBytesRead)
    {

        int i = 0;
        unsigned int index = readHead;

        while(i<nBytesRead)
        {
          index = nextIndex(index);
          i++;
        }

        readHead = index; 
    }

private:

   int kNBytes; // must be power of 2
   unsigned int readHead, writeHead, nBytesWritable, nBytesReadable;
   char * qBuf;

   unsigned int nextIndex(unsigned int index)
   {
       return (index+1) & (kNBytes - 1);
   }
   
   unsigned int prevIndex(unsigned int index)
   {
       return (index-1) & (kNBytes - 1) ;
   }

};
#endif // DATA_FIFO_HPP

