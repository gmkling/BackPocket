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

    Data_Fifo(int size) : kNBytes(size), byteMask(size-1), readHead(0), writeHead(0)
    {
        int power;

        if (!(size & (size-1))==0) // if size is not a power of 2
        {
          for(power=1; 1<<power < size; power++){}
          //
          size = 1<<power;
        }

        kNBytes = size;
        byteMask = size-1;

        // Gurus hate him!
        qBuf = new char[kNBytes];
        memset(qBuf, '0', kNBytes);
    }

    ~Data_Fifo() { delete[] qBuf; }



    void * openWrite(unsigned int &nBytesReq)
    {
      unsigned int wPtr;
      // get space avail, check split
      unsigned int wAvail = getWriteSpace();
      if ( nBytesReq > wAvail) nBytesReq = wAvail;
      if (nBytesReq<=0) return nullptr;

      wPtr = writeHead & byteMask;

      if ( (wPtr + nBytesReq) > kNBytes ) // if going that far is an overrun, split
      {
        // just use the end portion
        nBytesReq = kNBytes - wPtr;
        return qBuf+wPtr;
      } else // count is valid, return ptr
      {
        return qBuf+wPtr;
      }

    }

    void closeWrite(unsigned int nBytesWritten)
    {
      unsigned int temp = (writeHead + nBytesWritten) & byteMask;
      // update writeHead
      writeHead = temp;
    }

    void * openRead(unsigned int &nBytesReq)
    {
      unsigned int rPtr;
      // get reads avail, check for split
      unsigned int rAvail = getReadSpace();
      if ( nBytesReq > rAvail) nBytesReq = rAvail;
      if ( nBytesReq <=0) return nullptr;

      rPtr = readHead & byteMask; // wrap and read

      if ( (rPtr+nBytesReq) > kNBytes)
      {
        nBytesReq = kNBytes - rPtr;
        return qBuf+rPtr;
      } else // nothing to fix
      {
        return qBuf+rPtr;
      }

    } 

    void closeRead(unsigned int nBytesRead)
    {
      unsigned int temp = ( readHead + nBytesRead ) & byteMask;
      // update read head
      readHead = temp;
    }

  private:

   // returns number of bytes avail to read
    unsigned int getReadSpace(void)
    {
      unsigned int rPtr, wPtr;
      rPtr = readHead;
      wPtr = writeHead;

      if (wPtr>rPtr)
      {
        return wPtr - rPtr;
      }

      return (wPtr-rPtr + kNBytes) & byteMask; 
    }

    // returns number of bytes avail to be written
    unsigned int getWriteSpace(void)
    {
      unsigned int rPtr, wPtr;
      rPtr = readHead;
      wPtr = writeHead;

      if (wPtr>rPtr) 
      {
        return (( rPtr - wPtr + kNBytes ) & byteMask ) - 1;
      } else if ( wPtr < rPtr)
      {
        return rPtr - wPtr - 1;
      } else // w==r , empty
      {
        return kNBytes-1;
      }
    }


   char * qBuf;
   unsigned int kNBytes; // must be power of 2
   unsigned int byteMask;
   volatile unsigned int readHead, writeHead;
   

};



#endif // DATA_FIFO_HPP