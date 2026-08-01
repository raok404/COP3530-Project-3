#ifndef PROJECT3_GRAPH_H
#define PROJECT3_GRAPH_H

#include <unordered_map>
#include <set>
#include "Student.h"
#include <iostream>

using namespace std;

struct Edge {
    int from;
    int to;
    int weight;
    bool open;

    Edge(int f, int to, int wt, bool openStatus) : from(f), to(to), weight(wt), open(openStatus) {}

    bool operator<(const Edge& other) const {
        return (this->weight > other.weight);
    }

    void toggle() {
        open = !open;
    }
};

class Graph {
    // graph class, using adjacency list

    unordered_map<int, set<Edge*>> adjList;
    // location id is the key, stores a set of the edges as a pair (to, weight)
    unordered_set<int> nodes;

public:
    Graph();

    bool addEdge(int from, int to, int weight);

    void printAdjList();

    void toggleEdge(int from, int to);

    set<Edge*> getEdges(int from);

    bool edgeExists(int from, int to);

    string getEdgeStatus(int from, int to);

    bool isConnected(int source, int dest);

    unordered_set<int> getAllNodeInts();

    ~Graph(); // to delete the edge pointers
};


#endif //PROJECT3_GRAPH_H