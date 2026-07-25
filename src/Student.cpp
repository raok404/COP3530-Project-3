//
// Created by kayde on 7/16/2026.
//

#include "Student.h"

bool Student::hasClass(string classcode) {
    return classes.count(classcode) > 0;
}

int Student::getNumClasses() {
    return classes.size();
}

string Student::getName() {
    return name;
}

unordered_set<string> &Student::getClasses() {
    return classes;
}

int Student::getResID() {
    return residenceID;
}

void Student::addClass(string classcode) {
    classes.insert(classcode);
}

bool Student::dropClass(string classcode) {
    if (!hasClass(classcode)) {
        return false;
    }

    classes.erase(classcode);
    return true;
}

bool Student::replaceClass(string course1, string course2) {
    if (!hasClass(course1) || hasClass(course2)) {
        return false;
    }

    dropClass(course1);
    addClass(course2);

    return true;
}
