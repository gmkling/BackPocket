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


// A try with a slightly simpler interface

// class Data_Fifo
// {

// public:

//     Data_Fifo(int size) : kNBytes(size), byteMask(size-1), wPass(0), rPass(0), readHead(0), writeHead(0)
//     {
//         int power;

//         // check if kNBytes is a power of two and juke it if not
//         if (!(size & (size-1))==0) // if size is not a power of 2
//         {
//           for(power=1; 1<<power < size; power++){}
//           //
//           size = 1<<power;
//         }

//         kNBytes = size;
//         byteMask = size-1;
//         qBuf = new char[kNBytes];
//         memset(qBuf, '0', kNBytes);
//     }

//     ~Data_Fifo() { delete[] qBuf; }

//     void * startWrite(unsigned int &nBytesReq)
//     {
//       unsigned int readLoc, writeLoc, writeSpace;

//       readLoc = readHead;
//       writeLoc = writeHead;

//       if( writeLoc > readLoc)
//       {
//         writeSpace = kNBytes-writeLoc-1;
//       } else if (readLoc>writeLoc)
//       {
//         writeSpace = readLoc-writeLoc;
//       } else if (readLoc==writeLoc)
//       {
//         if(wPass<=rPass) 
//         { 
//             writeSpace = kNBytes-writeLoc-1;
//         } else if (wPass>rPass) // wait for read
//         {
//             writeSpace = 0;
//         }
//       }

//       nBytesReq = std::min(writeSpace, nBytesReq);
    
//       if (nBytesReq==0) { return nullptr; }

//       return (qBuf+writeHead);
    
//     }

//     void finishWrite(unsigned int nBytesWritten)
//     {
//       unsigned int writeLoc = writeHead;
      
//       writeLoc = writeLoc + nBytesWritten;

//       if (writeLoc>=byteMask)
//       {
//         writeLoc = writeLoc & byteMask;
//         wPass++;
//       }

//       writeHead = writeLoc;
//       return;
//     }

//     void * startRead(unsigned int &nBytesReq)
//     {
//       unsigned readLoc, writeLoc, readSpace;

//       readLoc = readHead;
//       writeLoc = writeHead;
//       readSpace = 0;

//       if (readLoc>writeLoc)
//       {
//         readSpace = kNBytes-readLoc;
//       } else if ( writeLoc > readLoc)
//       {
//         readSpace = writeLoc-readLoc;
//       } else if ( writeLoc==readLoc)
//       {
//         if(rPass>=wPass)
//         {
//           readSpace = 0;
//         } else if (rPass<wPass) { readSpace = kNBytes-readLoc; }
//       }

//       nBytesReq = std::min(readSpace, nBytesReq);
//       if (nBytesReq==0) { return nullptr; }
//       return (qBuf+readHead);

//     }

//     void finishRead(unsigned int nBytesRead)
//     {
//       unsigned int readLoc = readHead;
//       unsigned int writeLoc = writeHead;

//       readLoc = readLoc + nBytesRead;
      
//       if( readLoc >= byteMask )
//       {
//         readLoc = readLoc & byteMask;
//         rPass++;
//       }
      
//       readHead = readLoc;
//       return;
//     }

// private:

//    char * qBuf;
//    unsigned int kNBytes; // must be power of 2
//    int byteMask, wPass, rPass;
//    volatile unsigned int readHead, writeHead;
   
// };


// class Data_Fifo
// {

// public:

//     Data_Fifo(int size) : kNBytes(size), readHead(0), writeHead(1), nBytesWritable(0), nBytesReadable(0)
//     {
//         // check if kNBytes is a power of two and juke it if not?
//         kNBytes = size;
//         qBuf = new char[kNBytes];
//         memset(qBuf, 0, kNBytes);
//     }

//     ~Data_Fifo() { delete[] qBuf; }

//     void * startWrite(unsigned int &nBytesAvail)
//     {
//         unsigned int index = writeHead;
//         unsigned int rMark = readHead;
//         nBytesWritable = 0;

//         if (index==readHead) // case of empty buffer
//         { 
//           nBytesAvail=0; 
//           return nullptr; 
//         }
        
//         while(index != rMark) // nWritten space
//         {
//             nBytesWritable++;
//             index = nextIndex(index);
//             if (index==rMark) { break; } // edge case where read head is moved before next test
//         }

//         nBytesAvail = std::min(nBytesAvail, nBytesWritable);        
//         return (qBuf+writeHead);
//     }

//     void finishWrite(unsigned int nBytesWritten)
//     {
//         int i = 0;
//         unsigned int index = writeHead;
        
//         while(i<nBytesWritten) // update writehead
//         {
//           index = nextIndex(index);
//           i++;
//         }
//         nBytesWritable = 0;
//         writeHead = index;
//     }

//     void * startRead(unsigned int &nBytesReq)
//     {

//       unsigned int index = nextIndex(readHead);
//       unsigned int wMark = writeHead; // cache the loc
//       nBytesReadable = 0;

//       if (index==writeHead) // case of empty buffer
//       { 
//         nBytesReq=0; 
//         return nullptr; 
//       }

//       while(index != wMark)
//       {
//         nBytesReadable++;
//         index = nextIndex(index);
//         if (index==wMark) { break; } // edge case where write head is moved before next test
//       }

//        nBytesReq = std::min(nBytesReadable, nBytesReq);
//        return qBuf+nextIndex(readHead);
//     }

//     void finishRead(unsigned int nBytesRead)
//     {

//         int i = 0;
//         unsigned int index = readHead;

//         while(i<nBytesRead) 
//         {
//           index = nextIndex(index);
//           i++;
//         }

//         readHead = index;  
//     }

// private:

//    int kNBytes; // must be power of 2
//    unsigned int readHead, writeHead, nBytesWritable, nBytesReadable;
//    char * qBuf;

//    unsigned int nextIndex(unsigned int index)
//    {
//        return (index+1) & (kNBytes - 1);
//    }
   
//    unsigned int prevIndex(unsigned int index)
//    {
//        return (index-1) & (kNBytes - 1);
//    }

// };