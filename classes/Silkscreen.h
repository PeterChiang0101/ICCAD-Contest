// Silkscreen.h
// declearation of class Silkscreen

#include "Graph.h"
#include "Segment.h"

#ifndef SILKSCREEN_H
#define SILKSCREEN_H

class Silkscreen
{
public:
    vector<Graph> Silkscreen_Assembly(const Graph, const Graph, const vector<Graph>);
    //                                      assembly    silkscreen      copper

private:
    // in main.cpp : Final_Silkscreen()
    Graph Untuned_Silkscreen(const Graph, const vector<Graph>);
    vector<Graph> Delete_Short_Silkscreen(Graph);
    vector<Graph> fit_boarder_condition(vector<Graph>, Graph, Graph, vector<Graph>);
};

#endif // SILKSCREEN_H