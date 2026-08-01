#include "CampusCompass.h"
#include "Graph.h"
#include "Student.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <regex>

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
    bool is_valid = true; // replace with your actual validity checking

    istringstream stream(command);
    string first;
    stream >> first;

    if (first == "insert") {
        return parseInsert(stream);
    }
    else if (first == "remove") {
        return parseRemove(stream);
    }
    else if (first == "dropClass") {
        return parseDropClass(stream);
    }
    else if (first == "replaceClass") {
        return parseReplaceClass(stream);
    }
    else if (first == "toggleEdgesClosure") {

    }
    else if (first == "checkEdgeStatus") {
        //
    }
    else if (first == "isConnected") {
        //
    }
    else if (first == "printShortestEdges") {
        //
    }
    else if (first == "printStudentZone") {
        //
    }
    else if (first == "verifySchedule") {
        //
    }
    else {
        cout << "unsuccessful" << endl; // whatever an invalid command should do
        return false;
    }

    return is_valid;
}

bool CampusCompass::parseInsert(istringstream& stream) {
    string command = stream.str();
    regex pattern("insert \"([A-Za-z ]*)\" ([0-9]{8}) ([0-9]+) ([1-6])( [A-Z]{3}[0-9]{4})+");
    smatch matches;
    if (regex_search(command, matches, pattern)) {
        string name = matches[1];
        string studentID = matches[2];
        int resID = stoi(matches[3]);
        int numClasses = stoi(matches[4]);

        string temp;
        getline(stream, temp, '"');
        getline(stream, temp, '"'); // extract name;
        stream >> temp; // extract studentid
        stream >> temp; // extract resID
        stream >> temp; // extract N

        vector<string> classes;

        for (int i = 0; i < numClasses; i++) {
            // now extract class names
            string courseCode;
            if ((stream >> ws).eof()) {
              return false;
            }
            stream >> courseCode;
            classes.push_back(courseCode);
        }

        if (!(stream >> ws).eof()) {
            // more classes than expected
            return false;
        }
        cout << " aobut to insert " << endl;
        return insert(name, studentID, resID, classes);
    }
    else {
        cout << " invalid match " << endl;
        return false;
    }
}

bool CampusCompass::parseRemove(istringstream &stream) {
    string command = stream.str();
    regex pattern("remove ([0-9]{8})");
    smatch matches;
    if (regex_search(command, matches, pattern)) {
        string studentID = matches[1];
        if (remove(studentID)) {
            cout << "successful" << endl;
            return true;
        }
    }
    cout << "unsuccessful" << endl;
    return false;
}

bool CampusCompass::parseDropClass(istringstream &stream) {
    string command = stream.str();
    regex pattern("dropClass ([0-9]{8}) ([A-Z]{3}[0-9]{4})");
    smatch matches;
    if (regex_search(command, matches, pattern)) {
        string studentID = matches[1];
        string courseCode = matches[2];
        if (dropClass(studentID, courseCode)) {
            cout << "successful" << endl;
            return true;
        }
    }
    cout << "unsuccessful" << endl;
    return false;
}

bool CampusCompass::parseReplaceClass(istringstream &stream) {
    string command = stream.str();
    regex pattern("replaceClass ([0-9]{8}) ([A-Z]{3}[0-9]{4}) ([A-Z]{3}[0-9]{4})");
    smatch matches;
    if (regex_search(command, matches, pattern)) {
        string studentID = matches[1];
        string courseCode1 = matches[2];
        string courseCode2 = matches[3];
        if (replaceClass(studentID, courseCode1, courseCode2)) {
            cout << "successful" << endl;
            return true;
        }
    }
    cout << "unsuccessful" << endl;
    return false;
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
    unordered_map<int, int> results = dijkstras(students[studentID].getResID())[0];

    set<string> classes;
    for (auto course : students[studentID].getClasses()) {
        classes.insert(course); // sorts them
    }

    for (auto course : classes) {
        cout << course << ": " << (results[classLocations[course]] == INT_MAX ? -1 : results[classLocations[course]]) << endl;
    }
}

int CampusCompass::printStudentZone(string studentID) {
    unordered_map<int, int> predecessors = dijkstras(students[studentID].getResID())[1];

    unordered_set<string> studentClassCodes = students[studentID].getClasses();
    vector<int> classLocationIDs;
    for (string classCode : studentClassCodes) {
        classLocationIDs.push_back(classLocations[classCode]);
    }

    unordered_set<int> nodesForSubgraph = getNodesFromDijkstras(predecessors, classLocationIDs);
    // // debugging printing
    // for (auto i : nodesForSubgraph) {
    //     cout << i << " ";
    // }
    // cout << endl;

    Graph subgraph = getSubGraph(nodesForSubgraph);
    return mst(subgraph, students[studentID].getResID());
}

int timeToInt(string time) {
    int minutes = 0;
    minutes += (time.back() - '0');
    time.pop_back();

    minutes += ((time.back() - '0') * 10);
    time.pop_back();

    time.pop_back(); // ":"

    minutes += ((time.back() - '0') * 60);
    time.pop_back();

    minutes += ((time.back() - '0') * 600);

    return minutes;
}


struct course {
    int classLocation;
    string classCode;
    int startTime;
    int endTime;

    course(int clsLoc, string clsC, int sT, int eT) : classLocation(clsLoc), classCode(clsC), startTime(sT), endTime(eT) {}

    bool operator<(const course& other) const {
        return startTime < other.startTime;
    }
};

void CampusCompass::verifySchedule(string studentID) {
    // do dijkstras to find distance b/w class1 + class2
    // and class2 + class3

    if (students[studentID].getNumClasses() == 1) {
        cout << "unsuccessful" << endl;
    }

    vector<course> classes; // stores { locationID, classStartInt, classEndInt }
    for (auto classCode : students[studentID].getClasses()) {
        classes.push_back(course(classLocations[classCode],classCode, timeToInt(classTimes[classCode].first), timeToInt(classTimes[classCode].second)));
    }
    sort(classes.begin(), classes.end()); // sorts them by class start time

    // now use dijkstras
    for (size_t i = 0; i < classes.size()-1; i++) {
        course& currClass = classes[i];
        course& nextClass = classes[i+1];

        cout << currClass.classCode << " - " << nextClass.classCode << ": ";

        unordered_map<int, int> distances = dijkstras(currClass.classLocation)[0];
        if (distances[nextClass.classLocation] <= nextClass.startTime - currClass.endTime) {
            cout << "successful" << endl;
        }
        else {
            cout << "unsuccessful" << endl;
        }
    }
}

struct ComparePair {
    bool operator()(const pair<int, int>& pair1, const pair<int, int>& pair2) {
        return pair1.second > pair2.second;
    }
};

vector<unordered_map<int, int>> CampusCompass::dijkstras(int source) {
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
        set<Edge*> neighbors = graph.getEdges(id);
        for (Edge* edge : neighbors) {
            if (!edge->open) {
                continue;
            }
            if (currDist + edge->weight < distance[edge->to]) {
                distance[edge->to] = currDist + edge->weight;
                predecessor[edge->to] = id;

                pq.emplace(make_pair(edge->to, distance[edge->to]));
            }
        }

        // move to processed set
        unprocessed.erase(source);
        processed.insert(source);
    }

    return {distance, predecessor};
}

unordered_set<int> CampusCompass::getNodesFromDijkstras(unordered_map<int, int> &predecessors, vector<int> classNodes) {
    unordered_set<int> nodesInPaths;
    for (int classLocationID : classNodes) {
        nodesInPaths.insert(classLocationID);

        int prev = predecessors[classLocationID];
        while (prev != -1) {
            nodesInPaths.insert(prev);
            prev = predecessors[prev];
        }
    }

    return nodesInPaths;
}

Graph CampusCompass::getSubGraph(unordered_set<int> &locations) {
    // iterate through the adj list and add all edges in those adj lists
    Graph subgraph;

    for (int location : locations) {
        for (Edge* edge : graph.getEdges(location)) {
            if (edge->open && locations.count(edge->to) != 0) {
                subgraph.addEdge(location, edge->to, edge->weight);
            }
        }
    }

    return subgraph;
}

struct CompareEdgePtr {
    // functor to correctly order edges in the priority queue
    bool operator()(const Edge* edge1, const Edge* edge2) const {
        return edge1->weight > edge2->weight;
    }
};

int CampusCompass::mst(Graph &subgraph, int resID) {
    // have a connected and unconnected set
    // at first, connected will be empty and unconnected will not have all the graph nodes
    int sum = 0;

    unordered_set<int> connected;
    unordered_set<int> unconnected = subgraph.getAllNodeInts();

    priority_queue<Edge*, vector<Edge*>, CompareEdgePtr> pq;

    int nodeToProcess = resID;

    while (unconnected.size()>0) {
        unconnected.erase(nodeToProcess);
        connected.insert(nodeToProcess);

        // add all outbound edges from all connected nodes to a priority queue
        for (Edge* edge : subgraph.getEdges(nodeToProcess)) {
            if (unconnected.count(edge->to) != 0) {
                // the outbound edge connects to a node in the unconnected part of the graph
                pq.push(edge);
            }
        }
        if (pq.size() == 0) {
            break;
        }
        // after that, pop nodes that are connecting already connected nodes to clear them out
        bool isBetweenConnected = connected.count(pq.top()->from) != 0 && connected.count(pq.top()->to) != 0;
        while (isBetweenConnected) {
            pq.pop();

            isBetweenConnected = connected.count(pq.top()->from) != 0 && connected.count(pq.top()->to) != 0;
        }
        // now next lowest edge should be from connected to unconnected
        sum += pq.top()->weight;
        nodeToProcess = pq.top()->to;
        pq.pop();
    }

    return sum;
}

void CampusCompass::printDijkstras(int source) {
    vector<unordered_map<int, int>> result = dijkstras(source);
    unordered_map<int, int> distances = result[0];
    unordered_map<int, int> predecessors = result[1];

    for (auto dist : distances) {
        cout << "Node #" << dist.first << ": distance " << (dist.second == INT_MAX ? -1 : dist.second);
        cout << " prev: " << predecessors[dist.first] << endl;
    }
}
