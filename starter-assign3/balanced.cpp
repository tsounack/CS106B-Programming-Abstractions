/*
 * File edited by Thomas Sounack
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "SimpleTest.h"

using namespace std;

/*This function takes a string as argument, and returns a
 * string containing all its operators with a recursive
 * process.
 */
string operatorsFrom(string str) {
    // base case
    if (str.size() == 0) {
        return "";
    }

    // Defining the operators
    string operators = "(){}[]";
    char first = str[0];

    // If the first char is an operator, we return it and
    // continue the recursion
    if (stringContains(operators, first)) {
        return first + operatorsFrom(str.substr(1));
    }
    // Otherwise we just continue the recursion
    else {
        return operatorsFrom(str.substr(1));
    }
}

/*
 * This function takes a string as argument (operators only) and
 * returns a boolean: true if the operators are matched, false if
 * they are not all matched.
 */
bool operatorsAreMatched(string ops) {

        if (ops.size() == 0) {
        return true;
    }

    string open   = "([{";
    string closed = ")]}";

    // Using this for loop does not replace the recursivity!
    for (int i = 0; i < ops.size(); i++) {

        // op is the i-th character of ops
        char op = ops[i];

        // If op is a closed operator, we store which operator it is
        int indClosed = stringIndexOf(closed, op);

        if (indClosed != -1) {

            // We check if the operator before is the corresponding
            // open operator
            if (ops[i - 1] == open[indClosed]) {
                string new_ops = ops.erase(i - 1, 2);
                return operatorsAreMatched(new_ops);
            }
            // if the corresponding open operator is not found
            return false;
        }
    }
    // if no closed operator is found
    return false;
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

STUDENT_TEST("isBalanced tests") {
    EXPECT(isBalanced("(()[{}]){}"));
    EXPECT(!isBalanced("}{})()[]"));
    EXPECT(isBalanced(""));
}
