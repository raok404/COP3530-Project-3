#include "CampusCompass.h"
#include "Graph.h"
#include "Student.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

CampusCompass::CampusCompass() {
    graph = Graph();
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    // return boolean based on whether parsing was successful or not
    bool edgeSuccess = ParseEdges(edges_filepath);
    bool classSuccess = ParseClasses(classes_filepath);
    return edgeSuccess && classSuccess;
}

bool CampusCompass::ParseEdges(const string &edges_filepath) {

    ifstream file(edges_filepath);

    if (!file.is_open()) {
        return false;
    }

    string line;
    getline(file, line); // consume the header line

    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        int id1 = stoi(row[0]);
        int id2 = stoi(row[1]);
        int wt = stoi(row[4]);
        graph.addEdge(id1, id2, wt);
        graph.addEdge(id2, id1, wt);

        nodes.insert(id1);
        nodes.insert(id2);
    }

    return true;
}

bool CampusCompass::ParseClasses(const string &classes_filepath) {
    ifstream file(classes_filepath);

    if (!file.is_open()) {
        return false;
    }

    string line;
    getline(file, line); // consume the header line

    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        string classCode = row[0];
        int locationID = stoi(row[1]);
        string start = row[2];
        string end = row[3];

        classLocations[classCode] = locationID;
        classTimes[classCode] = make_pair(start, end);
    }

    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    // do whatever regex you need to parse validity
    // hint: return a boolean for validation when testing. For example:
    bool is_valid = true; // replace with your actual validity checking

    return is_valid;
}


// test all the stuff below - goal for tmmr ;.;
bool CampusCompass::studentExists(string studentID) {
    return students.find(studentID) != students.end();
}

bool CampusCompass::studentHasClass(string studentID, string courseCode) {
    if (studentExists(studentID)) {
        return students[studentID].hasClass(courseCode);
    }
    return false;
}

void CampusCompass::dropStudentIfNoClass(string studentID) {
    if (students[studentID].getNumClasses()==0) {
        students.erase(studentID);
    }
}

void CampusCompass::printClasses() {
    for (auto c : classLocations) {
        cout << c.first << ": " << c.second << endl;
    }

    for (auto c : classTimes) {
        cout << c.first << ": " << c.second.first << ", " << c.second.second << endl;
    }
}

bool CampusCompass::insert(string name, string studentID, int resID, vector<string> &classes) {
    if (studentExists(studentID)) {
        return false;
    }
    students[studentID] = Student(name, resID);
    for (string code : classes) {
        students[studentID].addClass(code);
    }

    return true;
}

bool CampusCompass::remove(string studentID) {
    if (!studentExists(studentID)) {
        return false;
    }
    students.erase(studentID);
    return true;
}

bool CampusCompass::dropClass(string studentID, string course) {
    if (!studentExists(studentID)) {
        return false;
    }

    bool result = students[studentID].dropClass(course);
    dropStudentIfNoClass(studentID);
    return result;
}

bool CampusCompass::replaceClass(string studentID, string course1, string course2) {
    // should I check if both classes are real classes?
    if (!studentExists(studentID)) {
        return false;
    }
    if (classLocations.count(course2) == 0) {
        return false;
    }
    return students[studentID].replaceClass(course1, course2);
}

int CampusCompass::removeClass(string classcode) {
    if (classLocations.count(classcode) == 0) {
        return 0;
    }
    int removalCount = 0;
    for (auto studentInfo : students) {
        Student currStudent = studentInfo.second;
        if (currStudent.hasClass(classcode)) {
            removalCount++;
            currStudent.dropClass(classcode);

        dropStudentIfNoClass(studentInfo.first);
        }
    }

    return removalCount;
}

void CampusCompass::toggleEdgesClosure(int location1, int location2) {
    graph.toggleEdge(location1, location2);
    graph.toggleEdge(location2, location1);
}

string CampusCompass::getEdgeStatus(int location1, int location2) {
    return graph.getEdgeStatus(location1, location2);
}

bool CampusCompass::isConnected(int location1, int location2) {
    return graph.isConnected(location1, location2);
}

void CampusCompass::printShortestEdges(string studentID) {
    cout << "Time for Shortest Edges: " << students[studentID].getName() << endl;
    // need to look up the classes from the map?????????
    unordered_map<int, int> results = dijkstras(students[studentID].getResID());

    set<string> classes;
    for (auto course : students[studentID].getClasses()) {
        classes.insert(course); // sorts them
    }

    for (auto course : classes) {
        cout << course << ": " << (results[classLocations[course]] == INT_MAX ? -1 : results[classLocations[course]]) << endl;
    }
}

struct ComparePair {
    bool operator()(const pair<int, int>& pair1, const pair<int, int>& pair2) {
        return pair1.second > pair2.second;
    }
};

unordered_map<int, int> CampusCompass::dijkstras(int source) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, ComparePair> pq;
    // put in a pair<locationID, distance>

    // need to make 2 maps for the size of all the locations
    unordered_map<int, int> distance;
    for (int node : nodes) {
        distance[node] = INT_MAX;
    }
    distance[source] = 0;
    unordered_map<int, int> predecessor;
    for (int node : nodes) {
        predecessor[node] = -1;
    }

    set<int> unprocessed = set<int>(nodes);
    set<int> processed;

    pq.emplace(make_pair(source, distance[source]));

    while (!pq.empty()) {
        int id = pq.top().first;
        int currDist = pq.top().second;
        pq.pop();
        if (processed.count(id) > 0) {
            continue;
        }

        // relax the neighbors of current node
        set<EdgeTo*> neighbors = graph.getEdges(id);
        for (EdgeTo* edge : neighbors) {
            if (!edge->open) {
                continue;
            }
            if (currDist + edge->weight < distance[edge->id]) {
                distance[edge->id] = currDist + edge->weight;
                predecessor[edge->id] = id;

                pq.emplace(make_pair(edge->id, distance[edge->id]));
            }
        }

        // move to processed set
        unprocessed.erase(source);
        processed.insert(source);
    }

    return distance;
}

void CampusCompass::printDijkstras(int source) {
    unordered_map<int, int> results = dijkstras(source);
    for (auto res : results) {
        cout << "Node #" << res.first << ": distance " << (res.second == INT_MAX ? -1 : res.second) << endl;
    }
}