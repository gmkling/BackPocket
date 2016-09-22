// Edge.hpp
// Edges connect nodes in a function graph for FRP-like systems
// Copyright © 2016 Garry Kling, All Rights Reserved

#ifndef _EDGE_H_
#define _EDGE_H_

#include "Graph_Common.hpp"

class Edge
{

public:

    Node *fromNode;
    Node *toNode;

    float32 *buffer;

    void setFrom(Node* node)
    {
        fromNode = node;
    }

    void setTo(Node* node)
    {
        toNode = node;
    }
};

#endif /* _EDGE_H_ */
