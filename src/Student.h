#ifndef PROJECT3_STUDENT_H
#define PROJECT3_STUDENT_H
#include <string>
#include <unordered_set>

using namespace std;
class Student {
    string name;
    unordered_set<string> classes;
    int residenceID;

public:
    Student(string n, int r) : name(n), residenceID(r) {}
    Student() {};

    bool hasClass(string classcode);
    int getNumClasses();

    void addClass(string classcode);
    bool dropClass(string classcode);
    bool replaceClass(string course1, string course2);
};


#endif //PROJECT3_STUDENT_H