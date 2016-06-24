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
        static unsigned int itemCount;
        
        Q_Test_Item() : itemID(itemCount++), execCount(0)
        {
        }
        int itemID;
        int execCount;
        // optional function type called from the execute() method?
        void execute()
        {
            execCount++;
        }
    };
    
    unsigned int Q_Test_Item::itemCount = 0;

    class Lf_FifoTest : public ::testing::Test 
    {
        Q_Test_Item qTestArray1[128];
        Q_Test_Item qTestArray2[128];

        // per-case func
        static void SetUpTestCase()
        {}

        static void TearDownTestCase()
        {}
        
        // hate the v-word
        virtual void SetUp()
        {}

        virtual void TearDown()
        {}

   
    };

        // globally avail Q    
    Lf_Fifo<Q_Test_Item, 1024> testFifo;


}

#endif /* _TEST_LF_FIFO_H_ */
