/**
 * File edited by Thomas Sounack
 */

#include <iostream>    // for cout, endl
#include "error.h"
#include "random.h"
#include "SimpleTest.h"
using namespace std;


/* The function factorial is a recursive implementation to calculate
 * factorials. This function works correctly for positive values
 * of n <= 12 (for larger values of n, the computed factorial will
 * overflow the range of the int data type).
 *
 * As currently written, this function does not correctly handle
 * negative inputs.
 */
int factorial(int n) {
    if (n < 0) {
        error("The input of factorial should be a positive or null integer.");
    }

    if (n == 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

/* The function iterativePower calculates base raised to the
 * power exp. The implementation use a loop and repeated multiplication.
 */
double iterativePower(double base, int exp) {
    double result = 1;
    for (int i = 0; i < exp; i++)
       result *= base;
    return result;
}

/* The function recPower calculates base raised to the power exp.
 * The implementation use a recursive decomposition of the
 * exponentiation
 */
double recPower(double base, int exp) {
    if (exp == 0) {
        return 1;
    } else {
        double half = recPower(base, exp/2);
        if (exp % 2 == 0) {
            return half * half;
        } else {
            return half * half * base;
        }
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Confirm factorial(3) is 6 ") {
    EXPECT_EQUAL(factorial(3), 6);
}

PROVIDED_TEST("Test recursive power against iterative power") {
     EXPECT_EQUAL(recPower(7, 3), iterativePower(7, 3));
}

PROVIDED_TEST("Test recursive power against iterative power, generated inputs") {
    for (int base = -8; base < 35; base++) {
        for (int exp = 0; exp < 10; exp++) {
            EXPECT_EQUAL(recPower(base, exp), iterativePower(base, exp));
        }
    }
}


void manyPowerCalls(double base, int exp) {
    // change the value of this constant to adjust the number of
    // iterations until you observe time required hitting a reasonable range on
    // your system
    const int NUM_ITERATIONS = 15000000;

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        recPower(base, exp);
    }
}

PROVIDED_TEST("Time trial recPower, double base, keep exp constant") {
    for (int size = 4; size <= 128; size *= 2) {
        TIME_OPERATION(size, manyPowerCalls(size, 5));
    }
}

PROVIDED_TEST("Time trial recPower, keep base constant, double exp") {
    for (int size = 4; size <= 128; size *= 2) {
        TIME_OPERATION(size, manyPowerCalls(5, size));
    }
}


STUDENT_TEST("Test factorial of negative numbers:") {
    EXPECT_ERROR(factorial(-3));
}
