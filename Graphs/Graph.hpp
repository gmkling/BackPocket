// Graph.hpp
// Graph types for an FRP-like system in C++
// Copyright © 2016 Garry Kling, All Rights Reserved

#ifndef _GRAPH_H_
#define _GRAPH_H_

class Graph
{
public:
    int numNodes;
    Node *nodes;

    int numEdges;
    Edge *edges;



    void run(Graph* graphIn)
    {
        for (int i=0; i<graphIn->numNodes; i++)
        {
            Node *node = graphIn->nodes[i];
            (node->yield)(node);
        }
    }
};

#endif /* _GRAPH_H_ */
