// Test_Lf_Fifo.cpp
//
// gtest framework for Lf_Fifo.hpp template
//
// Copyright (c) 2016 Garry Kling
//

#include "Test_Lf_Fifo.hpp"

TEST(Lf_FifoTest, testLf_Fifo_basic)
{
    Q_Test_Item qTestArray1[128];
    Q_Test_Item qTestArray2[128];

    // single-threaded test of basic queue-ness
    
    // should be constructed empty and not dump core   
    bool clearTest = testFifo.isClear();
    EXPECT_EQ(true, clearTest);
    
    // create a bunch of commands and add them
    for(int i = 0; i<128; i++)
    {
        testFifo.add(qTestArray1[i]);
    }

    // should test positive for items now
    EXPECT_EQ(true, testFifo.hasItems());

    // execute them all once
    for(int j = 0; j<128; j++)
    {
        testFifo.executeNext();
        EXPECT_EQ(1, qTestArray1[j].execCount);
    }

    // clear it, test
    testFifo.clear();
    EXPECT_EQ(false, testFifo.hasItems());
    
    // refill and then execute them all again
    for(int i = 0; i<128; i++)
    {
        testFifo.add(qTestArray1[i]);
    }

    testFifo.execute();
    for(int j = 0; j<128; j++)
    {
        EXPECT_EQ(2, qTestArray1[j].execCount);
    }

    // add more tests for intermittent add/executes

}

TEST(Lf_FifoTest, testLf_Fifo_dual)
{
    // test single-producer, single consumer
    // basic queue-ness
}

TEST(Lf_FifoTest, testLf_Fifo_exec)
{
    // test single-producer, single consumer
    // execution of enqueued elements
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

