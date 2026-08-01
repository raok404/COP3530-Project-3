#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.h"
#include "Student.h"

using namespace std;

// 5 graded test cases
TEST_CASE("Graded Test Cases", "[parse][edgeCases]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");

  SECTION("5 INVALID commands") {
    vector<string> invalid;
    invalid.push_back(R"(insert John Smith 10012002 1 3 COP3503 PHY2049 CDA3101)"); // wrong format
    invalid.push_back(R"(insert "Hello 123!" 10012002 1 3 COP3503 PHY2049 CDA3101)"); // invalid name
    invalid.push_back(R"(insert "John Smith" 100 1 0 COP3503 PHY2049 CDA3101)"); // invalid id
    invalid.push_back(R"(insert "John Smith" 10012002 1 3 PHY2049 CDA3101)"); // wrong number of classes
    invalid.push_back(R"(dropClass 12345678 CA3101)"); // invalid course code
    for (auto command : invalid) {
      REQUIRE(!compass.ParseCommand(command));
    }
  }

  SECTION("3 edge cases") {
    vector<string> classes = {"COP3530"};
    compass.insert("John Doe", "00000000", 1, classes);

    vector<string> edgeCases;
    edgeCases.push_back(R"(remove 87654321)"); // removing student that doesn't exist
    edgeCases.push_back(R"(dropClass 00000000 CDA3101)"); // drop class they don't have
    edgeCases.push_back(R"(removeClass ZZZ0000)"); // remove class that doesn't exist
    for (auto command : edgeCases) {
      REQUIRE(!compass.ParseCommand(command));
    }
  }

  SECTION("dropClass, removeClass, remove, replaceClass") {
    vector<string> classes = {"COP3530", "PHY2048", "CDA3101", "COT3100"};
    compass.insert("John Doe", "00000000", 1, classes);
    vector<string> classes2 = {"COP3530", "CDA3101", "MAC2311"};
    compass.insert("Ali May", "00000001", 55, classes2);

    REQUIRE(compass.dropClass("00000000", "CDA3101"));
    REQUIRE(compass.removeClass("CDA3101") == 1);
    REQUIRE(compass.replaceClass("00000000", "PHY2048", "PHY2049"));
    REQUIRE(!compass.dropClass("00000000", "PHY2048")); // no longer has the class
    REQUIRE(!compass.replaceClass("00000001", "MAC2312", "COP3530")); // can't replace class don't have
    REQUIRE(compass.remove("00000000"));
    // I wrote many more test cases for these down in the campus compass helper functions test case
  }

  SECTION("printShortestEdges where they can reach class, turn off edges, then cannot reach that class") {
    vector<string> classes = {"COP3530", "PHY2048", "CDA3101", "COT3100"};
    compass.insert("John Doe", "00000000", 1, classes);

    REQUIRE(compass.allClassesReachable("00000000")); // same as printShortestEdges but returns a bool
    compass.toggleEdgesClosure(15, 6); // close an important edge
    REQUIRE(!compass.allClassesReachable("00000000")); // should not be reachable now
  }
}


// my own test cases below
TEST_CASE("Parsing insert", "[parse][insert]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");

  SECTION("Valid commands") {
    string valid0 = R"(insert "Amanda" 12345678 55 2 MAC2313 COP3530)";
    string valid1 = R"(insert "Josh Smith" 10012002 1 3 COP3502 PHY2048 CDA3101)";
    string valid2 = R"(insert "Jane Doe" 76543219 4 1 CDA3101)";
    string valid3 = R"(insert "Alex B" 40205060 55 2 PHY2048 MAC2311)";
    REQUIRE(compass.ParseCommand(valid0));
    REQUIRE(compass.ParseCommand(valid1));
    REQUIRE(compass.ParseCommand(valid2));
    REQUIRE(compass.ParseCommand(valid3));
  }

  SECTION("INVALID commands") {
    vector<string> invalid;
    invalid.push_back(R"(insert John Smith 10012002 1 3 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Sm123h" 10012002 1 3 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 1001200 1 3 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 3 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 1 10 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 1 0 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 1 3 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" ABCD2002 1 2 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 1 2 COP3503 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 1 3 COP3A03 PHY2049 CDA3101)");
    invalid.push_back(R"(insert "John Smith" 10012002 1 3 CaP3503 PHY2049 CDA3101)");

    for (auto command : invalid) {
      REQUIRE(!compass.ParseCommand(command));
    }

    // also invalid if id is not unique
    REQUIRE(compass.ParseCommand(R"(insert "Amanda" 12345678 55 2 MAC2313 COP3530)"));
    REQUIRE(!compass.ParseCommand(R"(insert "Ama" 12345678 51 2 MAC2313 COP3530)"));
  }
}

TEST_CASE("Parsing remove", "[parse][remove]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  vector<string> classes = {"MAC2311"};

  compass.insert("Amanda", "12345678", 55, classes);

  REQUIRE(compass.ParseCommand(R"(remove 12345678)"));
  REQUIRE(!compass.ParseCommand(R"(remove 12345678)"));
  REQUIRE(!compass.ParseCommand(R"(remove 1234)"));
  REQUIRE(!compass.ParseCommand(R"(remove 1234abcd)"));
}

TEST_CASE("Parsing dropClass", "[parse][dropClass]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  vector<string> classes = {"MAC2311", "PHY2048", "CDA3101"};

  compass.insert("Amanda", "12345678", 55, classes);

  REQUIRE(compass.ParseCommand(R"(dropClass 12345678 CDA3101)"));
  REQUIRE(!compass.ParseCommand(R"(dropClass 12345678 CDA3101)"));
  REQUIRE(!compass.ParseCommand(R"(dropClass 12345679 CDA3101)"));
  REQUIRE(!compass.ParseCommand(R"(dropClass 1234)"));
  REQUIRE(!compass.ParseCommand(R"(dropClass 12345678 CD3101)"));
  REQUIRE(!compass.ParseCommand(R"(dropClass 12345678 CDa3101)"));
  REQUIRE(!compass.ParseCommand(R"(dropClass 12345678 CDA3p01)"));
}

TEST_CASE("Parsing replaceClass", "[parse][replaceClass]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  vector<string> classes = {"MAC2311", "PHY2048", "CDA3101"};

  compass.insert("Amanda", "12345678", 55, classes);

  REQUIRE(compass.ParseCommand(R"(replaceClass 12345678 CDA3101 ENC1101)"));
  REQUIRE(!compass.studentHasClass("12345678", "CDA3101"));
  REQUIRE(!compass.ParseCommand(R"(replaceClass 12345678 CDA3101 PHY2049)"));
  REQUIRE(!compass.ParseCommand(R"(replaceClass 12345678 PHY2048 MAC2311)"));
  REQUIRE(!compass.ParseCommand(R"(replaceClass 12345678 CDA3101)"));
  REQUIRE(!compass.ParseCommand(R"(replaceClass 1245678 CDA3101 ENC1101)"));
  REQUIRE(!compass.ParseCommand(R"(replaceClass 12345678 CDA3101 EN1101)"));
}

TEST_CASE("Parsing removeClass", "[parse][removeClass]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  vector<string> classes = {"MAC2311", "PHY2048", "CDA3101"};

  compass.insert("Amanda", "12345678", 55, classes);
  REQUIRE(compass.ParseCommand(R"(removeClass PHY2048)"));
  REQUIRE(!compass.ParseCommand(R"(removeClass PHY048)"));
  REQUIRE(!compass.ParseCommand(R"(removeClass 11111111 PHY2048)"));
  REQUIRE(!compass.ParseCommand(R"(removeClass PHY2048 COP3530)"));
  REQUIRE(!compass.ParseCommand(R"(removeClass ABC0000)"));
}

TEST_CASE("Parsing toggleEdgesClosure", "[parse][toggle]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  vector<string> classes = {"MAC2313", "MAC2311", "COP3502"};
  compass.insert("harvey", "123", 1,  classes);

  REQUIRE(!compass.ParseCommand(R"(toggleEdgesClosure a 15 6)"));
  REQUIRE(!compass.ParseCommand(R"(toggleEdgesClosure 3 1 2 3 4)"));
  REQUIRE(!compass.ParseCommand(R"(toggleEdgesClosure 1 1 2 3 4)"));
  REQUIRE(compass.ParseCommand(R"(toggleEdgesClosure 2 1 2 3 4)"));
  REQUIRE(compass.ParseCommand(R"(toggleEdgesClosure 2 1 2 3 4)"));
  REQUIRE(compass.ParseCommand(R"(toggleEdgesClosure 1 15 6)"));
  REQUIRE(!compass.isConnected(1, 18));
  REQUIRE(compass.ParseCommand(R"(toggleEdgesClosure 1 15 6)"));
  REQUIRE(compass.isConnected(1, 18));
}

TEST_CASE("Parsing functions", "[parse]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  vector<string> classes = {"MAC2311", "PHY2048", "CDA3101"};
  compass.insert("harvey", "12345678", 1,  classes);

  SECTION("check edge status") {
    REQUIRE(compass.ParseCommand(R"(checkEdgeStatus 1 18)"));
    REQUIRE(!compass.ParseCommand(R"(checkEdgeStatus 18)"));
  }

  SECTION("check is connected") {
    REQUIRE(compass.ParseCommand(R"(isConnected 1 18)"));
    REQUIRE(!compass.ParseCommand(R"(isConnected 18)"));
    compass.toggleEdgesClosure(6,15);
    REQUIRE(!compass.ParseCommand(R"(isConnected 1 18)"));
  }

  SECTION("print shortest edges") {
    REQUIRE(!compass.ParseCommand(R"(printShortestEdges)"));
    REQUIRE(!compass.ParseCommand(R"(printShortestEdges 123)"));
    REQUIRE(!compass.ParseCommand(R"(printShortestEdges 123aaaaa)"));
    REQUIRE(compass.ParseCommand(R"(printShortestEdges 12345678)"));
  }

  SECTION("print student zone") {
    REQUIRE(!compass.ParseCommand(R"(printStudentZone)"));
    REQUIRE(!compass.ParseCommand(R"(printStudentZone 123)"));
    REQUIRE(!compass.ParseCommand(R"(printStudentZone 123aaaaa)"));
    REQUIRE(compass.ParseCommand(R"(printStudentZone 12345678)"));
  }

  SECTION("verify schedule") {
    REQUIRE(!compass.ParseCommand(R"(verifySchedule)"));
    REQUIRE(!compass.ParseCommand(R"(verifySchedule 123)"));
    REQUIRE(!compass.ParseCommand(R"(verifySchedule 123aaaaa)"));
    REQUIRE(compass.ParseCommand(R"(verifySchedule 12345678)"));
  }
}

TEST_CASE("Student helper functions", "[student][helper]") {
  Student testStudent("Allie Gator", 12);
  testStudent.addClass("COP3502");

  REQUIRE(testStudent.hasClass("COP3502"));
  REQUIRE(!testStudent.hasClass("ENC1101"));

  testStudent.addClass("ENC1101");
  REQUIRE(testStudent.hasClass("ENC1101"));

  REQUIRE(!testStudent.dropClass("doesn't exist"));
  REQUIRE(testStudent.dropClass("ENC1101"));
  REQUIRE(!testStudent.hasClass("ENC1101"));
  REQUIRE(testStudent.getNumClasses()==1);

  testStudent.addClass("PHY2048");

  REQUIRE(testStudent.getNumClasses()==2);
  REQUIRE(!testStudent.replaceClass("ENC1102", "MAC2312"));
  REQUIRE(!testStudent.replaceClass("COP3502", "PHY2048"));
  REQUIRE(testStudent.replaceClass("PHY2048", "ENC1102"));

  REQUIRE(testStudent.hasClass("ENC1102"));
  REQUIRE(testStudent.getNumClasses() == 2);
}

TEST_CASE("Compass helper functions involving student/classes", "[compass][helper]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  REQUIRE(!compass.studentExists("0"));
  vector<string> classesTest = {"COP3503"};
  vector<string> multipleClasses = {"COT3100", "PHY2048"};
  REQUIRE(compass.insert("Albert", "0", 5, classesTest));
  REQUIRE(compass.studentExists("0"));

  SECTION("Has class") {
    REQUIRE(compass.studentHasClass("0", "COP3503"));
    REQUIRE(!compass.studentHasClass("0", "ENC1101"));
    REQUIRE(!compass.studentHasClass("1", "COP3503"));
  }

  SECTION("Drop class") {
    REQUIRE(!compass.dropClass("0", "ABC1234"));
    REQUIRE(!compass.dropClass("50", "ABX1234"));
    REQUIRE(compass.dropClass("0", "COP3503"));
    REQUIRE(!compass.studentExists("0"));

    REQUIRE(compass.insert("Albert's twin", "1", 5, multipleClasses));
    REQUIRE(compass.dropClass("1", "COT3100"));
    REQUIRE(compass.studentExists("1"));
  }

  SECTION("Remove class") {
    REQUIRE(compass.removeClass("COP3530") == 0);
    compass.insert("ALBERT", "12345678", 55, multipleClasses);
    REQUIRE(compass.removeClass("COP3503") == 1);
    REQUIRE(compass.removeClass("ABC123") == 0);
    vector<string> test = {"EEL2701", "COT3100"};
    REQUIRE(compass.insert("Ali", "12345675", 12, test));
    REQUIRE(compass.removeClass("COT3100") == 2);
    REQUIRE(compass.removeClass("COT3100") == 0);
    compass.insert("Lila", "123", 12, test);
    REQUIRE(compass.removeClass("COT3100")==1);
  }

  SECTION("Insert") {
    REQUIRE(!compass.insert("Albert's twin", "0", 5, classesTest));
    REQUIRE(compass.insert("Albert's twin", "1", 5, multipleClasses));
  }

  SECTION("Remove") {
    REQUIRE(!compass.remove("123456"));
    REQUIRE(compass.remove("0"));
    REQUIRE(!compass.remove("0"));
    REQUIRE(!compass.studentExists("0"));
  }

  SECTION("Replace class") {
    REQUIRE(!compass.replaceClass("123", "ENC1101", "ENC1102"));
    REQUIRE(!compass.replaceClass("0", "ABC1001", "ENC1101"));
    REQUIRE(!compass.replaceClass("0", "COP3503", "ENC1111"));
    REQUIRE(compass.replaceClass("0", "COP3503", "COP3502"));
  }
}

TEST_CASE("Compass helper functions involving edges", "[compass][helper]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");

  SECTION("Check edge status + toggle") {
    REQUIRE(compass.getEdgeStatus(1, 20) == "DNE");
    REQUIRE(compass.getEdgeStatus(1, 50) == "open");
    REQUIRE(compass.getEdgeStatus(50, 1) == "open");

    compass.toggleEdgesClosure(1,50);
    REQUIRE(compass.getEdgeStatus(1,50) == "closed");
    REQUIRE(compass.getEdgeStatus(50,1) == "closed");

    compass.toggleEdgesClosure(50, 1);
    REQUIRE(compass.getEdgeStatus(1,50)=="open");
  }
}

TEST_CASE("Compass isConnected", "[compass][algorithm]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");

  REQUIRE(compass.isConnected(9, 53));
  REQUIRE(!compass.isConnected(40, 9));
  REQUIRE(!compass.isConnected(29, 52));
  REQUIRE(compass.isConnected(10,14));
  REQUIRE(compass.isConnected(10, 43));
  REQUIRE(!compass.isConnected(45, 48));
}