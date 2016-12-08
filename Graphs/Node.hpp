// Node.hpp
// Node types for a FRP-like system in C++
// Copyright © 2016 Garry Kling, All Rights Reserved

#ifndef _NODE_H_
#define _NODE_H_

#include "Graph_Common.hpp"

// The type of the Node's function
// for now they are just free function ptrs that take a pointer to "this"

typedef void (*YieldFunction)(Node *inputNode);

class Node
{

public:

    Graph *parent;

    // the thing that does stuff -
    // could we write method to trigger this N times if we want N samples out?
    YieldFunction yield;

    // edges in and out
    Edge *inputs, *outputs;
    uint16_t numInputs, numOutputs;

};


#endif /* _NODE_H_ */
