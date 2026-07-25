//
// Created by kayde on 7/16/2026.
//

#include "Graph.h"
#include <queue>
#include <unordered_set>

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

bool Graph::isConnected(int source, int dest) {
    // bfs ->
    queue<int> q;
    unordered_set<int> visited;

    q.push(source);
    visited.insert(source);

    while (!q.empty()) {
        // add and visit neighbors of the next node if they haven't been visited yet
        int currNode = q.front();
        q.pop();

        set<EdgeTo*> neighbors = getEdges(currNode);
        for (EdgeTo* neighbor : neighbors) {
            if (neighbor->open) {
                if (visited.count(neighbor->id) > 0) {
                    continue;
                }

                if (neighbor->id == dest) {
                    return true;
                }
                q.push(neighbor->id);
                visited.insert(neighbor->id);
            }
        }
    }

    return false;
}

// map<int, int> Graph::getShortestEdges(string studentID) {
//     priority_queue<int> pq;
//     map<int, pair<int, int>> info;
//     // previously visited
//     // initialize all the dist to infinity (except source)
//     // initialize all the prev to -1
//     // get the first thing in pq and relax the neighbors, updating the prev and dist as needed
//     // do that while pq not empty
//     // return a map of the class name and dist.
// }
