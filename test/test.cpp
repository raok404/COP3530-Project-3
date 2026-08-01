#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.h"
#include "Student.h"

using namespace std;

// the syntax for defining a test is below. It is important for the name to be
// unique, but you can group multiple tests with [tags]. A test can have
// [multiple][tags] using that syntax.

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
  // ADD TESTS FOR WHEN YOU TOGGLE SOME EDGES
}

TEST_CASE("Compass printShortestEdges", "[compass][algorithm]") {
  CampusCompass compass;
  compass.ParseCSV("data/edges.csv", "data/classes.csv");
  // add tests
  REQUIRE(false);
}


TEST_CASE("Parser", "[parse]") {
  // test cases for in/valid UFID
  // test cases for in/valid names
  // test cases for in/valid class codes
  // test cases for in/valid # of course codes
  // test cases for misspelled commands
  // test cases for successful overall commands
  REQUIRE(true); // also fix me!
}

// TEST_CASE("Test 2", "[tag]") {
//   // you can also use "sections" to share setup code between tests, for example:
//   int one = 1;
//
//   SECTION("num is 2") {
//     int num = one + 1;
//     REQUIRE(num == 2);
//   };
//
//   SECTION("num is 3") {
//     int num = one + 2;
//     REQUIRE(num == 3);
//   };
//
//   // each section runs the setup code independently to ensure that they don't
//   // affect each other
// }

// Refer to Canvas for a list of required tests. 
// We encourage you to write more than required to ensure proper functionality, but only the ones on Canvas will be graded.

// See the following for an example of how to easily test your output.
// Note that while this works, I recommend also creating plenty of unit tests for particular functions within your code.
// This pattern should only be used for final, end-to-end testing.

// // This uses C++ "raw strings" and assumes your CampusCompass outputs a string with
// //   the same thing you print.
// TEST_CASE("Example CampusCompass Output Test", "[flag]") {
//   // the following is a "raw string" - you can write the exact input (without
//   //   any indentation!) and it should work as expected
//   // this is based on the input and output of the first public test case
//   string input = R"(6
// insert "Student A" 10000001 1 1 COP3502
// insert "Student B" 10000002 1 1 COP3502
// insert "Student C" 10000003 1 2 COP3502 MAC2311
// dropClass 10000001 COP3502
// remove 10000001
// removeClass COP3502
// )";
//
//   string expectedOutput = R"(successful
// successful
// successful
// successful
// unsuccessful
// 2
// )";
//
//   string actualOutput;
//
//   // somehow pass your input into your CampusCompass and parse it to call the
//   // correct functions, for example:
//   /*
//   CampusCompass c;
//   c.parseInput(input)
//   // this would be some function that sends the output from your class into a string for use in testing
//   actualOutput = c.getStringRepresentation()
//   */
//
//   REQUIRE(actualOutput == expectedOutput);
// }
