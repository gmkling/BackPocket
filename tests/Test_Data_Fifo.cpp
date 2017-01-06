// Test_Data_Fifo.cpp
//
// gtest framework for Data_Fifo.hpp class
//
// Copyright (c) 2016 Garry Kling
//

#include "Test_Data_Fifo.hpp"
#include <thread>
#include <cstring>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <random>


TEST(Data_FifoTest, testData_Fifo_nicely)
{
    // test single-producer, single consumer
    // just going to fire off two threads
    // producer will read from inQuote
    //  in chunks that are the lesser of whats avail to write
    //  and remains to be read
    // consumer will peal them off the same way
    // when they are done, outQuote should match inQuote

    int qSize = 8;
    Data_Fifo theFifo(qSize);
    unsigned int quoteSize = std::strlen(inQuote);
    memset(outQuote, 0, quoteSize);
    const char * inPtr = &inQuote[0];
    char * outPtr = &outQuote[0];
    std::cout<<"Quote Size: "<<quoteSize<<std::endl;
    auto writer = [&](){
        
        int nBytesToWrite = 0;
        unsigned int totalBytesWritten = 0;
        
        while( totalBytesWritten < quoteSize)
        {
            unsigned int bytesAvailToWrite = 1;
            char * writePtr = nullptr;
            
            writePtr = (char *)theFifo.startWrite(bytesAvailToWrite);
            if (bytesAvailToWrite <= 0) { continue; }
            nBytesToWrite = std::min(bytesAvailToWrite, quoteSize-totalBytesWritten);
            memcpy(writePtr, inPtr+totalBytesWritten, nBytesToWrite);
            for (int i = 0; i < nBytesToWrite; ++i)
            {
                *(writePtr+i) = *(inPtr+(totalBytesWritten+i));
            }

            theFifo.finishWrite(nBytesToWrite);
            totalBytesWritten += nBytesToWrite;
            
        }
    };
    
    auto reader = [&](){
           
        int nBytesToRead = 0;
        unsigned int TotalBytesRead = 0; 

        while( TotalBytesRead < quoteSize)
        {
            unsigned int bytesAvailToRead = 1;
            char * readPtr = nullptr;
            unsigned int offset = 0;
            
            readPtr = (char *)theFifo.startRead(bytesAvailToRead);
            if (bytesAvailToRead <= 0) { continue; }
            nBytesToRead = std::min(bytesAvailToRead, (quoteSize-TotalBytesRead));

            for (int i = 0; i < nBytesToRead; ++i)
            {
                 offset = TotalBytesRead+i;
                 *(outQuote+offset) = *(readPtr+i); 
            }

            theFifo.finishRead(nBytesToRead);
            TotalBytesRead += nBytesToRead;
        }
    };

    std::thread producer(writer);
    std::thread consumer(reader);
    
    try{
        // join the threads
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

}

TEST(Data_FifoTest, testData_Fifo_roughly)
{
    // test single-producer, single consumer
    // just going to fire off two threads
    // producer will read from inQuote
    //  in chunks that are the lesser of: a) whats avail to write
    //  b)remains to be read c) a random number x where 0<x<qSize
    // consumer will peal them off the same way
    // when they are done, outQuote should match inQuote

    int qSize = 16;
    Data_Fifo theFifo(qSize);
    int quoteSize = std::strlen(inQuote);
    memset(outQuote, 0, quoteSize);
    const char * inPtr = &inQuote[0];
    char * outPtr = &outQuote[0];

    // random numbers
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned int> dist(1, qSize-1);

   
        auto writer = [&](){
        
        int nBytesToWrite = 0;
        unsigned int totalBytesWritten = 0;
        
        while( totalBytesWritten < quoteSize)
        {
            unsigned int bytesAvailToWrite = 16;
            char * writePtr = nullptr;
            unsigned int randBytesToWrite = 5; //dist(mt);
            bytesAvailToWrite = std::min(bytesAvailToWrite, randBytesToWrite);

            writePtr = (char *)theFifo.startWrite(bytesAvailToWrite);
            if (bytesAvailToWrite <= 0) { continue; }
            nBytesToWrite = std::min(bytesAvailToWrite, quoteSize-totalBytesWritten);
            memcpy(writePtr, inPtr+totalBytesWritten, nBytesToWrite);
            for (int i = 0; i < nBytesToWrite; ++i)
            {
                *(writePtr+i) = *(inPtr+(totalBytesWritten+i));
            }

            theFifo.finishWrite(nBytesToWrite);
            totalBytesWritten += nBytesToWrite;
            
        }
    };


    
    auto reader = [&](){
           
        int nBytesToRead = 0;
        unsigned int TotalBytesRead = 0; 

        while( TotalBytesRead < quoteSize)
        {
            unsigned int bytesAvailToRead = 16;
            char * readPtr = nullptr;
            unsigned int offset = 0;
            unsigned int randBytesToRead = dist(mt);
            bytesAvailToRead = std::min(bytesAvailToRead, randBytesToRead);

            
            readPtr = (char *)theFifo.startRead(bytesAvailToRead);
            if (bytesAvailToRead <= 0) { continue; }
            nBytesToRead = std::min(bytesAvailToRead, (quoteSize-TotalBytesRead));

            for (int i = 0; i < nBytesToRead; ++i)
            {
                 offset = TotalBytesRead+i;
                 *(outQuote+offset) = *(readPtr+i); 
            }

            theFifo.finishRead(nBytesToRead);
            TotalBytesRead += nBytesToRead;
        }
    };

    std::thread producer(writer);
    std::thread consumer(reader);

    try{
        // join the threads
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
}

// int main(int argc, char **argv)
// {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }



