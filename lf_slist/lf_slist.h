// Lf_slist.h
// An experimental lock-free singley-linked list using atomics
// Copyright 2016 Garry Kling
// Started 6/19/2016

#include <atomic>

template <nodeT>
class lf_slist {

public:
    


private:
    struct slist_node 
    {
        slist_node(nodeT val) : value(val), next(nullptr) 
        {}
        nodeT* value;
    
        slist_node* next;
    };
    
    slist_node* first;
    atomic<slist_node*> divider, last; 
} 


