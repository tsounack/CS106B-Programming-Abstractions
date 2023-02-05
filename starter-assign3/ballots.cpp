/*
 * File edited by Thomas Sounack
 */
#include "recursion.h"
#include "SimpleTest.h"
using namespace std;

/* This function takes two integers (number of ballots for a and b) and
 * returns an integer - the count of all orderings possible using recursion
 */
int countAllOrderings(int a, int b) {
    // base case - return 1
    if (a == 0 || b == 0) {
        return 1;
    }

    // otherwise add the two countAllOrderings of sub-orderings together
    return countAllOrderings(a - 1, b) + countAllOrderings(a, b - 1);
}


// This function takes three integers (number of ballots for a and b,
// number of leads for a) and returns an integer - it is the helper
// function for countGoodOrderings
int helperCountGoodOrderings(int a, int b, int a_leads) {
    // if a_leads is less than 1, the path returns 0
    if (a_leads < 1) {
        return 0;
    }

    if (a == 0) {
        if (a_leads < b) {
            return 0;
        }
        else {
            return 1;
        }
    }

    // base case - return 1
    if (b == 0) {
        return 1;
    }

    // otherwise add the two countAllOrderings of sub-orderings together
    return helperCountGoodOrderings(a - 1, b, a_leads + 1) +
           helperCountGoodOrderings(a, b - 1, a_leads - 1);
}

/* This function takes two integers (number of ballots for a and b) and returns
 * an integer - the count of all good possible orderings using recursion
 * and the helper function defined above
 */
int countGoodOrderings(int a, int b) {
    // the first ballot count has to be a
    int a_leads = 1;
    return helperCountGoodOrderings(a - 1, b, a_leads);
}



/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("countAllOrderings, two A one B") {
   EXPECT_EQUAL(countAllOrderings(2, 1), 3);
}

PROVIDED_TEST("countGoodOrderings, two A one B") {
   EXPECT_EQUAL(countGoodOrderings(2, 1), 1);
}


STUDENT_TEST("countAllOrderings tests") {
   EXPECT_EQUAL(countAllOrderings(3, 1), 4);
   EXPECT_EQUAL(countAllOrderings(2, 2), 6);
}

STUDENT_TEST("countGoodOrderings tests") {
   EXPECT_EQUAL(countGoodOrderings(2, 4), 0);
   EXPECT_EQUAL(countGoodOrderings(3, 2), 2);
}

STUDENT_TEST("Testing using Bertrand's Theorem") {
    const int NUM_MAX = 15;

    for (double a = 0; a <= NUM_MAX; a++) {
        for (double b = 0; b < a; b++) {
            EXPECT_EQUAL(countGoodOrderings(a, b) / countAllOrderings(a, b), (a - b) / (a + b));
        }
    }
}
