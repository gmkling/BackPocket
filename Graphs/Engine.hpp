// Engine.hpp
// An engine to run functional graphs in an FRP-like system in C++
// Copyright © 2016 Garry Kling, All Rights Reserved

#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "Graph.hpp"
#include <thread>


class Engine
{
public:
    Engine(){};

    // starts necessary OS services for contained graph
    void initEngine()
    {

    }

    bool addGraph(Graph *inGraph)
    {

    }

    bool startEngine(void)
    {

    }

private:
    std::vector<Graph*> graphs;


};

#endif /* _ENGINE_H_ */
