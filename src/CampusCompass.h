#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Student.h"
#include "Graph.h"

using namespace std;

class CampusCompass {
private:
    Graph graph;
    set<int> nodes; // stores every single node in the graph

    unordered_map<string, Student> students;
    // student id is the key

    unordered_map<string, int> classLocations;
    // course code -> locationID

    unordered_map<string, pair<string, string>> classTimes;
    // course code -> pair(start, end)

    unordered_map<int, int> dijkstras(int source);

    // maybe need to keep track of how many diff nodes........
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseEdges(const string &edges_filepath);
    bool ParseClasses(const string &classes_filepath);
    bool ParseCommand(const string &command);

    // helper functions
    bool studentExists(string studentID);
    bool studentHasClass(string studentID, string courseCode);
    void dropStudentIfNoClass(string studentID);

    bool insert(string name, string studentID, int resID, vector<string> &classes);
    bool remove(string studentID);
    bool dropClass(string studentID, string course);
    bool replaceClass(string studentID, string course1, string course2);
    int removeClass(string classcode);
    void toggleEdgesClosure(int location1, int location2);
    string getEdgeStatus(int location1, int location2);

    bool isConnected(int location1, int location2);
    void printShortestEdges(string studentID);

    // for debugging
    void printClasses();
    void printDijkstras(int source);
};
