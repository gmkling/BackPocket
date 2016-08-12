// Test_Lf_Fifo.cpp
//
// gtest framework for Lf_Fifo.hpp template
//
// Copyright (c) 2016 Garry Kling
//

#include "Test_Lf_Fifo.hpp"
#include <thread>
#include <cstring>

TEST(T_FifoTest, testT_Fifo_nicely)
{
    // test single-producer, single consumer
    // just going to fire off two threads
    // producer will read from inQuote one by one with sleep 
    // consumer will peal them off the same way
    // when they are done, outQuote should match inQuote
    
    T_Fifo theFifo();

    auto prodFunc = [&](){
        
        int nBytesToRead = 0;
        int nBytesRead = 0;
        int i = 0;
        char * inPtr = inQuote[0];
        char readCache[8];
        memset(readCache, "");

        while(/* nRead < sizeOf(inQuote) */)
        {
            int bytesWritten = 0;
            int bytesAvail = 0;
            
            bytesAvail = theFifo.startWrite();
            
            if (bytesAvail < 1) { continue; }

            // generate random < 8
            nBytesToRead = floor(rand()*byteAvail);
            // read from inQuote
            memcpy(readCache, inPtr+i);
            // push into T_Fifo
            bytesWritten = theFifo.writeBytes((void*)readCache, nBytesToRead);
            i += bytesWritten;
        }

        // say something
    };
    
    auto conFunc = [&](){
        while(/* nRead < sizeOf(inQuote)*/)
        {
            // generate random < 8
            // strcpy into outQuote
        }

        // say something
    };

    std::thread producer(prodFunc);
    std::thread consumer(conFunc);
    
    // join the threads so we don't dump core
    producer.join();
    consumer.join();

    // test that the output matches input
    // then print them and say something about the test
    std::cout<<" "<<std::endl;
    std::cout<<" "<<std::endl;
    EXPECT_EQ(inQuote, outQuote);

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


