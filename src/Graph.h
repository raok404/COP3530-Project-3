#ifndef PROJECT3_GRAPH_H
#define PROJECT3_GRAPH_H

#include <unordered_map>
#include <map>
#include <set>
#include "Student.h"
#include <iostream>

using namespace std;

struct EdgeTo {
    int id;
    int weight;
    bool open;

    EdgeTo(int to, int wt, bool openStatus) : id(to), weight(wt), open(openStatus) {}

    bool operator<(const EdgeTo& other) const {
        return (this->id < other.id);
    }

    void toggle() {
        open = !open;
    }
};

class Graph {
    // graph class, using adjacency list

    unordered_map<int, set<EdgeTo*>> adjList;
    // location id is the key, stores a set of the edges as a pair (to, weight)

public:
    Graph();

    bool addEdge(int from, int to, int weight);

    void printAdjList();

    void toggleEdge(int from, int to);

    set<EdgeTo*> getEdges(int from);

    bool edgeExists(int from, int to);

    string getEdgeStatus(int from, int to);

    bool isConnected(int source, int dest);

};


#endif //PROJECT3_GRAPH_H