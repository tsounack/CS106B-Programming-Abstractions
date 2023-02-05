/*
 * This file was written by Thomas Sounack for the CS106B class
 *
 * About me: this class is making me realize I want to be a software engineer!
 *
 * I'm also impressed by how fast my laptop is processing this - or maybe I'm
 * just used to python? :)
 */

#include "console.h"
#include <iostream>
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* The divisorSum function takes one argument `n` and calculates the
 * sum of proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division using the modulus operator %
 *
 * Note: the C++ long type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}


/* The isPerfect function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* The findPerfects function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}



/* The smarterSum function takes one argument 'n' and calculates the
 * sum of proper divisors of 'n' excludint itself. To find divisors
 * a loop iterates over all numbers from 1 to sqrt(n) (included when
 * sqrt(n) is an integer value), testing for a zero remainder from
 * the division using the modulus operator %. When a divisor is found,
 * the associated divisor (n/divisor) is also added to the sum (except
 * when the divisor is 1 or sqrt(n)).
 */
long smarterSum(long n) {
    long total = 0;
    // We loop until the divisor value reaches the integer part of
    // the square root of n
    for (long divisor = 1; divisor <= floor(sqrt(n)); divisor++) {
        // We want to test n != 1, otherwise we would count n in
        // its own divisorSum for n = 1
        if (n % divisor == 0 && n != 1) {
            total += divisor;
            // We add the associated divisor as long as divisor is
            // not 1 or sqrt(n) - which would lead to counting n in
            // its own divisorSum or counting sqrt(n) two times in
            // the sum
            if (divisor != 1 && divisor != sqrt(n)) {
                total += n/divisor;
            }
        }
    }
    return total;
}

/* The isPerfectSmarter function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself (using the smarterSum
 * function).
 */
bool isPerfectSmarter(long n) {
    return (n != 0) && (n == smarterSum(n));
}

/* The findPerfectsSmarter function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfectsSmarter(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* The findNthPerfectEuclid function takes one argument 'n' and searches
 * for an n-th prime number using Euclid's approach. This algorithm checks
 * each Mersenne number to see if it is prime, and if so calculates its
 * corresponding perfect number.
 * When the n-th perfect number is found, the function returns it.
 */
long findNthPerfectEuclid(long n) {
    long       k = 1;
    long perfect = 0;
    long      nb = 0;
    // loop until we find the n-th perfect number
    while (perfect != n) {
        // compute the associated mersenne number
        long m = pow(2, k) - 1;
        // if m is prime, we increment the perfect counter and compute the
        // associated perfect number
        if (smarterSum(m) == 1) {
            perfect++;
            nb = pow(2, k - 1) * (pow(2, k) - 1);
        }
        k++;
    }
    return nb;
}


/* * * * * * Test Cases * * * * * */

/* Note: Do not add or remove any of the PROVIDED_TEST tests.
 * You should add your own STUDENT_TEST tests below the
 * provided tests.
 */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

PROVIDED_TEST("Time trial of findPerfects on input size 1000") {
    TIME_OPERATION(1000, findPerfects(1000));
}



STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {

    int largest = 8, trials = 4;

    for (int size = largest / pow (2, trials - 1); size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfects(size));
    }
}

STUDENT_TEST("Test reaction of isPerfect to some negative numbers"){
    EXPECT(!isPerfect(-3));
    EXPECT(!isPerfect(-10));
    EXPECT(!isPerfect(-30));
    EXPECT(!isPerfect(-1));
}

STUDENT_TEST("Confirm smarterSum of small inputs"){
    EXPECT_EQUAL(smarterSum(12), divisorSum(12));
    EXPECT_EQUAL(smarterSum(25), divisorSum(25));
    EXPECT_EQUAL(smarterSum(-1), divisorSum(-1));
    EXPECT_EQUAL(smarterSum(-25), divisorSum(-25));
    EXPECT_EQUAL(smarterSum(0), divisorSum(0));
    EXPECT_EQUAL(smarterSum(1), divisorSum(1));
}

STUDENT_TEST("Multiple time trials of findPerfectsSmarter on increasing input sizes") {

    int largest = 8, trials = 4;

    for (int size = largest / pow (2, trials - 1); size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfectsSmarter(size));
    }
}

STUDENT_TEST("Confirm findNthPerfectEuclid of small inputs") {
    EXPECT_EQUAL(findNthPerfectEuclid(1), 6);
    EXPECT_EQUAL(findNthPerfectEuclid(2), 28);
    EXPECT_EQUAL(findNthPerfectEuclid(3), 496);
    EXPECT_EQUAL(findNthPerfectEuclid(4), 8128);
    EXPECT_EQUAL(findNthPerfectEuclid(5), 33550336);
}
