// Test_Lf_Fifo.hpp
//
// Gtest header for Lf_Fifo template
// Copyright (c) 2016 Garry Kling
//

#ifndef _TEST_LF_FIFO_H_
#define _TEST_LF_FIFO_H_


#include "gtest/gtest.h"
#include "Lf_Fifo.hpp"

namespace {

    // messsage class
    struct Q_Test_Item
    {
        int itemID;
        int execCount;
        // optional function type called from the execute() method?
    
        void execute()
        {
            // do something cool that will show this is working
        }
    }

    Lf_Fifo<Q_Test_Item, 1024> testFifo;
}

#endif /* _TEST_LF_FIFO_H_ */
