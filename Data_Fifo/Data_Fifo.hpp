#ifndef DATA_FIFO_HPP
#define DATA_FIFO_HPP
// Single reader, single writer data queue. Lock-free and atomic-free
// Copyright © 2016 Garry Kling, All Rights Reserved

#include <cstring>

// Data_Fifo clients ask for a certain amount of space to read or write
// and promise to respect the returned value



// A try with a slightly simpler interface

class Data_Fifo
{

public:

    Data_Fifo(int size) : kNBytes(size), readHead(0), writeHead(1)
    {
        int power;

        // check if kNBytes is a power of two and juke it if not
        if (!(size & (size-1))==0) // if size is not a power of 2
        {
          for(power=1; 1<<power < size; power++){}
          //
          size = 1<<power;
        }

        kNBytes = size;
        byteMask = size-1;
        qBuf = new char[kNBytes];
        memset(qBuf, '0', kNBytes);
    }

    ~Data_Fifo() { delete[] qBuf; }

    void * startWrite(unsigned int &nBytesReq)
    {
      unsigned int readLoc, writeLoc, writeSpace;

      readLoc = readHead;
      writeLoc = writeHead;

      // check wrap
      if ( (writeLoc==byteMask) && (readLoc>0) ) // to wrap
      {
        writeHead = writeLoc = 0;
        //return;
      } else if ( (writeLoc==byteMask) && (readLoc==0)) // or not to wrap
      {
        writeHead = writeLoc = byteMask;
        //return;
      }

      if( writeLoc > readLoc)
      {
        writeSpace = kNBytes-writeLoc-1;
      } else if (readLoc>writeLoc)
      {
        writeSpace = readLoc-writeLoc-1;
      }

      nBytesReq = std::min(writeSpace, nBytesReq);

      return (qBuf+writeHead);
    
    }

    void finishWrite(unsigned int nBytesWritten)
    {
      unsigned int writeLoc = writeHead;
      unsigned int readLoc = readHead;

      writeLoc = (writeLoc + nBytesWritten) & byteMask;

    
      writeHead = writeLoc;
      return;
    }

    void * startRead(unsigned int &nBytesReq)
    {
      unsigned readLoc, writeLoc, readSpace;

      readLoc = readHead;
      writeLoc = writeHead;
      readSpace = 0;

      // wrapping
      if( (readLoc==byteMask) && (writeLoc>0)) 
      {
        readHead = readLoc = 0;
        //return;
      } else if ( (readLoc == byteMask) && (writeLoc==0))
      {
        readHead = readLoc = byteMask;
        //return;
      }

      
      if (readLoc>writeLoc)
      {
        readSpace = kNBytes-readLoc-1;
      } else if ( writeLoc > readLoc)
      {
        readSpace = writeLoc-readLoc-1;
      } else if ( writeLoc==readLoc)
      {
        readSpace=0;
      }

      nBytesReq = std::min(readSpace, nBytesReq);

      return (qBuf+readHead);

    }

    void finishRead(unsigned int nBytesRead)
    {
      unsigned int readLoc = readHead;
      unsigned int writeLoc = writeHead;

      readLoc = readLoc + nBytesRead;
    
      
      readHead = readLoc;
      return;
    }

private:

   char * qBuf;
   unsigned int kNBytes; // must be power of 2
   int byteMask;
   volatile unsigned int readHead, writeHead;
   
};
#endif // DATA_FIFO_HPP


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