#include <iostream>

#include "CampusCompass.h"

using namespace std;

int main() {
    // initialize your main project object
    CampusCompass compass;

    // ingest CSV data
    compass.ParseCSV("data/edges.csv", "data/classes.csv");
    // parse into an adjacency list and the classes dictionary

    // vector<string> classes = {"MAC2313", "MAC2311", "COP3502"};
    // compass.printDijkstras(1);
    // compass.insert("harvey", "123", 1,  classes);
    // compass.printShortestEdges("123");
    //
    // compass.toggleEdgesClosure(4, 1);
    // compass.printDijkstras(1);
    // compass.printShortestEdges("123");
    // cout << compass.isConnected(1, 18) << endl;
    // cout << compass.printStudentZone("123") << endl;
    //
    // compass.verifySchedule("123");

    string valid1 = R"(insert "Josh Smith" 10012002 1 3 COP3502 PHY2048 CDA3101)";
    compass.ParseCommand(valid1);

    // // the below is example code for parsing commandline input
    // int no_of_lines;
    // string command;
    // cin >> no_of_lines;
    // cin.ignore(); // ignore newline that first cin left over
    // for (int i = 0; i < no_of_lines; i++) {
    //     getline(cin, command);
    //
    //     // parse your commands however you see fit
    //     compass.ParseCommand(command);
    // }


}
