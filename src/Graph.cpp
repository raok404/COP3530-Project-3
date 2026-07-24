//
// Created by kayde on 7/16/2026.
//

#include "Graph.h"

Graph::Graph() {
}

bool Graph::addEdge(int from, int to, int weight) {
    // adds an open edge
    EdgeTo* newEdge = new EdgeTo(to, weight, true);
    adjList[from].insert(newEdge);

    return true;
}

void Graph::printAdjList() {
    for (auto it = adjList.begin(); it != adjList.end(); it++) {
        int from = it->first;
        set<EdgeTo*> edges = it->second;

        cout << from << " goes to these locations:" << endl;

        for (auto edge : edges) {
            cout << "\t" << edge->id << ", weight" << edge->weight << ", open:" << edge->open << endl;
        }
    }
}

void Graph::toggleEdge(int from, int to) {
    set<EdgeTo*> edges = getEdges(from);
    for (EdgeTo* edge : edges) {
        if (edge->id == to) {
            edge->toggle();
        }
    }
}

set<EdgeTo*> Graph::getEdges(int from) {
    return adjList[from];
}

bool Graph::edgeExists(int from, int to) {
    for (EdgeTo* edge : getEdges(from)) {
        if (edge->id == to) {
            return true;
        }
    }

    return false;
}

string Graph::getEdgeStatus(int from, int to) {
    for (EdgeTo* edge : getEdges(from)) {
        if (edge->id == to) {
            if (edge->open) {
                return "open";
            }
            return "closed";
        }
    }

    return "DNE";
}
