// Test_Lf_Fifo.cpp
//
// gtest framework for Lf_Fifo.hpp template
//
// Copyright (c) 2016 Garry Kling
//

#include "Test_Lf_Fifo.hpp"
#include <thread>
#include <chrono>

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
    }
    // see that the queue is empty
    EXPECT_EQ(false, testFifo.hasItems());

    // clear it, test
    testFifo.clear();
    EXPECT_EQ(true, testFifo.isClear());
    
    // refill and then execute them all again
    for(int i = 0; i<128; i++)
    {
        testFifo.add(qTestArray1[i]);
    }

    testFifo.execute();
    
    // test that there are no items and the fifo is clear
    EXPECT_EQ(true, testFifo.isClear());
    EXPECT_EQ(false, testFifo.hasItems());

    // see if we can make this break something
    testFifo.execute();
    
    // add more tests for intermittent add/executes
    Q_Test_Item qti = Q_Test_Item(); 
    testFifo.add(qti);
    EXPECT_EQ(true, testFifo.hasItems());
    EXPECT_EQ(false, testFifo.isClear());

    testFifo.execute();
    
     // test that there are no items and the fifo is clear
    EXPECT_EQ(true, testFifo.isClear());
    EXPECT_EQ(false, testFifo.hasItems());

}

// some naive tests of inter-thread communication
TEST(Lf_FifoTest, testLf_Fifo_dual)
{
    // test single-producer, single consumer
    // just going to fire off two threads
    // producer will produce one at a time
    // consumer will consume one at a time
    
    std::atomic<bool> flag;
    flag.store(true);
    int counter = 0;
    std::atomic<int> failCount;
    failCount.store(0);
 
    Q_Test_Item qti = Q_Test_Item();

    auto prodFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            if(!testFifo.add(qti)) failCount++;
        }
    };
    
    auto conFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            testFifo.executeNext();
            counter++;
        }
    };

    std::thread producer(prodFunc);
    std::thread consumer(conFunc);
    
    // let this situation go on for a while
    std::chrono::seconds sec(2);
    std::this_thread::sleep_for(sec);
    
    flag.store(false, std::memory_order_relaxed);
    
    // join the threads so we don't dump core
    producer.join();
    consumer.join();

    std::cout<<"Total items created/consumed (executeNext): ";
    //std::cout<<Q_Test_Item::itemCount<<std::endl;
    std::cout<<counter<<std::endl;

    std::cout<<"Total fails on add() due to overflow: "<<failCount<<std::endl;
}

TEST(Lf_FifoTest, testLf_Fifo_dual_execAll)
{
    // test single-producer, single consumer
    // just going to fire off two threads
    // producer will produce one at a time
    // consumer will consume as many as are available
    
    std::atomic<bool> flag;
    flag.store(true);
    int counter = 0;
    std::atomic<int> failCount;
    failCount.store(0);
 
    Q_Test_Item qti = Q_Test_Item();

    auto prodFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            if(!testFifo.add(qti)) failCount++;
        }
    };
    
    auto conFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            testFifo.execute();
            counter++;
        }
    };

    std::thread producer(prodFunc);
    std::thread consumer(conFunc);
    
    // let this situation go on for a while
    std::chrono::seconds sec(2);
    std::this_thread::sleep_for(sec);
    
    flag.store(false, std::memory_order_relaxed);
    
    // join the threads so we don't dump core
    producer.join();
    consumer.join();

    std::cout<<"Total items created/consumed (executeAll): ";
    //std::cout<<Q_Test_Item::itemCount<<std::endl;
    std::cout<<counter<<std::endl;
    std::cout<<"Total fails on add() due to overflow: "<<failCount<<std::endl;


}

//int main(int argc, char **argv)
//{
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}


// scrapped tests for multi-produder, multiconsumer
/*
 * TEST(Lf_FifoTest, testLf_Fifo_twoOne_exec)

{
    // test dual-producer, single consumer
    // just going to fire off three threads
    // 2 producers will produce one at a time
    // consumer will consume one at a time
    
    std::atomic<bool> flag;
    flag.store(true);
    int counter = 0;
    std::atomic<int> failCount;
    failCount.store(0);
 
    Q_Test_Item qti = Q_Test_Item();

    auto prodFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            if(!testFifo.add(qti)) failCount++;
        }
    };
    
    auto conFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            testFifo.executeNext();
            counter++;
        }
    };

    std::thread producer(prodFunc);
    std::thread producer2(prodFunc);
    std::thread consumer(conFunc);
    
    // let this situation go on for a while
    std::chrono::seconds sec(2);
    std::this_thread::sleep_for(sec);
    
    flag.store(false, std::memory_order_relaxed);
    
    // join the threads so we don't dump core
    producer.join();
    producer2.join();
    consumer.join();

    std::cout<<"Total items created/consumed (2 prod, 1 consumer): ";
    //std::cout<<Q_Test_Item::itemCount<<std::endl;
    std::cout<<counter<<std::endl;
    std::cout<<"Total fails on add() due to overflow: "<<failCount<<std::endl;


}
*/
/*
TEST(Lf_FifoTest, testLf_Fifo_OneTwo_exec)

{
    // test single-producer, dual consumer
    // just going to fire off three threads
    // producer will produce one at a time
    // 2 consumers will consume one at a time
    
    std::atomic<bool> flag;
    flag.store(true);
    std::atomic<int> counter;
    counter.store(0);
    std::atomic<int> failCount;
    failCount.store(0);
    testFifo.clear();

    Q_Test_Item qti = Q_Test_Item();

    auto prodFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            if(!testFifo.add(qti)) failCount++;
        }
    };
    
    auto conFunc = [&](){
        while(flag.load(memory_order_relaxed))
        {
            testFifo.executeNext();
            counter++;
        }
    };

    std::thread producer(prodFunc);
    std::thread consumer(conFunc);
    std::thread consumer2(conFunc);

    // let this situation go on for a while
    std::chrono::seconds sec(2);
    std::this_thread::sleep_for(sec);
    
    flag.store(false, std::memory_order_relaxed);
    
    // join the threads so we don't dump core
    producer.join();
    consumer.join();
    consumer2.join();

    std::cout<<"Total items created/consumed (2 prod, 1 consumer): ";
    //std::cout<<Q_Test_Item::itemCount<<std::endl;
    std::cout<<counter<<std::endl;
    std::cout<<"Total fails on add() due to overflow: "<<failCount<<std::endl;

}

*/
