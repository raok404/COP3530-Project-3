#include <iostream>

#include "CampusCompass.h"

using namespace std;

int main() {
    CampusCompass compass;
    compass.ParseCSV("data/edges.csv", "data/classes.csv");

    int no_of_lines;
    string command;
    cin >> no_of_lines;
    cin.ignore(); // ignore newline that first cin left over
    for (int i = 0; i < no_of_lines; i++) {
        getline(cin, command);

        // parse your commands however you see fit
        compass.ParseCommand(command);
    }

}
