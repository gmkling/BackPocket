// Lf_Fifo.h
//
// A lock-free queue for a generic array of pointers to commands
// Single-Producer, Single-Consumer
// Copyright © 2016 Garry Kling
//

#ifndef _Lf_Fifo_
#define _Lf_Fifo_

#include <atomic>


using std::memory_order_relaxed;
using std::memory_order_release;

// NodeT must have an execute() method
// NodeN should always be a power of 2!
// should only be used single producer, single consumer

template <class NodeT, int NodeN>
class Lf_Fifo
{
public:
    Lf_Fifo() : readPos(0), writePos(0) {}
   
    // status
    bool isClear()
    {   /* if read == write, then noth useful exists */
        return readPos.load(memory_order_relaxed) == writePos.load(memory_order_relaxed);
    }
    
    bool hasItems()
    {   /* if read != write, then something is there */
        return readPos.load(memory_order_relaxed) != writePos.load(memory_order_relaxed);
    }
    void clear()
    {   /* reset read and write to 0, which implies empty and will overwrite contents */
        readPos.store(0);
        writePos.store(0);
    }
    
    // add, doNext, doAll
    
    bool add(NodeT& item)
    {
        unsigned int next = nextIndex(writePos.load(memory_order_relaxed));
        
        // check if we are at max capacity
        if ( next == readPos.load(memory_order_relaxed)) { return false; }
        
        itemQ[next] = item;
        writePos.store(next, memory_order_release);
        return true;
    }
    
    // executeNext() performs one command
    void executeNext()
    {
        // this is a redundant conditional, so we do not loop over executeNext() from execute()
        if (!hasItems()) { return; }
        
        // do one
        unsigned int next = nextIndex(readPos.load(memory_order_relaxed));
        itemQ[next].execute();
        readPos.store(next, memory_order_release);
    }
    
    // execute() clears all items.
    void execute()
    {
        while (hasItems())
        {
            unsigned int next = nextIndex(readPos.load(memory_order_relaxed));
            itemQ[next].execute();
            readPos.store(next, memory_order_release);
        }
    }
    
private:
    unsigned int nextIndex(unsigned int index)
    {
        // avoid the modulus and AND instead ( NodeN is a pow(2, x) )
        return (index+1) & (NodeN - 1);
    }
    std::atomic<unsigned int> readPos, writePos;
    NodeT itemQ[NodeN];
};

#endif /* _Lf_Fifo_ */
