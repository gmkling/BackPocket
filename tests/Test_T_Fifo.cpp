// Test_T_Fifo.cpp
//
// gtest framework for T_Fifo.hpp class
//
// Copyright (c) 2016 Garry Kling
//

#include "Test_T_Fifo.hpp"
#include <thread>
#include <cstring>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>


TEST(T_FifoTest, testT_Fifo_nicely)
{
    // test single-producer, single consumer
    // just going to fire off two threads
    // producer will read from inQuote one by one with sleep 
    // consumer will peal them off the same way
    // when they are done, outQuote should match inQuote
    
    T_Fifo theFifo;
    int quoteSize = std::strlen(inQuote);
    memset(outQuote, 0, quoteSize);
    const char * inPtr = &inQuote[0];
    char * outPtr = &outQuote[0];

    auto prodFunc = [&](){
        
        int nBytesToRead = 0;
        int totalBytesWritten = 0;
        char readCache[8];
        memset(readCache, 0, 8);
        
        while( totalBytesWritten < quoteSize)
        {
            int bytesWritten = 0;
            int bytesAvailToWrite = 0;
            
            bytesAvailToWrite = theFifo.startWrite();
            
            if (bytesAvailToWrite <= 0) { continue; }

            // minimum of what can be written and what is left
            nBytesToRead = std::min(bytesAvailToWrite, quoteSize-totalBytesWritten); //(floor((rand()/RAND_MAX)*bytesAvail)) + 1;
            // read from inQuote
            memcpy(readCache, inPtr+totalBytesWritten, nBytesToRead);
            // push into T_Fifo
            bytesWritten = theFifo.writeBytes((void*)readCache, nBytesToRead);
            totalBytesWritten += bytesWritten;
            //std::cout<<"nBytesToRead: "<<nBytesToRead<<std::endl;
            //std::cout<<"nBytes IN: "<<nBytesRead<<std::endl;
        }

        // say something
        //std::cout<<"Finished writing quote to queue: "<<nBytesRead<<" of "<<quoteSize<<" bytes read."<<std::endl;
    };
    
    auto conFunc = [&](){
           
        int nBytesToRead = 0;
        int nBytesRead = 0; 
        char consumeCache[8];
        memset(consumeCache, 32, 8);

        while( nBytesRead < quoteSize)
        {
            unsigned int bytesRead = 0;
            int bytesAvailToRead = 0;
            
            bytesAvailToRead = theFifo.startRead();
            
            // if nothing is avail, go around the horn until n>0
            // I believe we could spin here if byteAvail never rises and nBytesRead never does either
            if (bytesAvailToRead == 0) { continue; }

            // do the minimum of the available vs. what is left
            nBytesToRead = std::min(bytesAvailToRead, (quoteSize-nBytesRead));

            // pull from T_Fifo
            bytesRead = theFifo.readBytes((void*)consumeCache, nBytesToRead);
            // write data to outQuote
            memcpy(outPtr+nBytesRead, &consumeCache, bytesRead);
        
            nBytesRead += bytesRead;

          // std::cout<<"nBytes OUT: "<<nBytesRead<<std::endl;
        }

        //std::cout<<"Finished reading quote from queue: "<<nBytesRead<<" of "<<quoteSize<<" bytes read."<<std::endl;
    };

    std::thread producer(prodFunc);
    std::thread consumer(conFunc);
    
    try{
        // join the threads so we don't dump core
        producer.join();
        consumer.join();
    } catch(const std::system_error& e) {
        std::cout << "Caught system_error with code " << e.code()
                  << " meaning " << e.what() << '\n';
    }

    // test that the output matches input
    // then print them and say something about the test

    std::cout<<"Input quote: "<<inQuote<<std::endl;
    std::cout<<"Output quote: "<<outQuote<<std::endl;

    int result = strncmp(inQuote, outQuote, std::strlen(inQuote));
    EXPECT_EQ(0, result);
    std::cout<<"Test complete."<<std::endl;
    delete[] outQuote;
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



