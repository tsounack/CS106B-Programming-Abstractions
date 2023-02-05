/*
 * File edited by Thomas Sounack
 */
#include <iostream>    // for cout, endl
#include "queue.h"
#include "SimpleTest.h"
using namespace std;


/*
 * This function is a helper function that takes a queue of ints
 * as input and returns the bool true if it is sorted, false if
 * it is not
 */
bool isOrdered(Queue<int> a) {
    int n = a.size();

    if (n != 0) {

        int previous = a.dequeue();

        for (int i = 1; i < n; i++) {
            int elem = a.dequeue();
            if (elem < previous) {
                return false;
            }
            previous = elem;
        }
    }
    return true;
}



/*
 * This function takes two sorted queues as argument and
 * merges them in a sorted manner in a queue. It then
 * returns this queue.
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    Queue<int> result;

    if (!isOrdered(a) || !isOrdered(b)) {
        error("The queues are not ordered");
    }

    // Both queues are non-empty, we add the smallest element
    // to the result queue
    while (a.size() >= 1 && b.size() >= 1) {

        int elem_a = a.peek();
        int elem_b = b.peek();

        if (elem_a < elem_b) {
            result.enqueue(elem_a);
            a.dequeue();
        }
        else {
            result.enqueue(elem_b);
            b.dequeue();
        }

    }

    // One queue is empty, we add the remaining element of
    // the other queue
    while (a.size() >= 1) {
        result.enqueue(a.dequeue());
    }

    while (b.size() >= 1) {
        result.enqueue(b.dequeue());
    }

    return result;
}



/*
 * The function naiveMultiMerge assumes you have a correctly working
 * binaryMerge function, which this function calls to iteratively
 * merge a collection of sequences, starting off with an empty sequence
 * and repeatedly merging in an additional sequence until all are merged.
 * The code below is provided to you is implemented correctly and you
 * should not need to modify it.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

/*
 * This function takes a vector of queues of ints as input, and
 * merges them in a sorted manner using recursivity. It returns
 * a queue of ints.
 */
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {

    // If only 1 queue is left, we return it
    if (all.size() == 1) {
        return all[0];
    }

    int half = all.size() / 2;

    // Splitting the all vector
    Vector<Queue<int>> left  = all.subList(0, half);
    Vector<Queue<int>> right = all.subList(half);

    return binaryMerge(recMultiMerge(left), recMultiMerge(right));
}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}


STUDENT_TEST("Testing binaryMerge") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1};
    Queue<int> expected = {1, 2, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);

    a = {};
    b = {};
    expected = {};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
}

STUDENT_TEST("Testing unsorted queues") {
    Queue<int> a = {8, 4, 5};
    Queue<int> b = {1};
    EXPECT_ERROR(binaryMerge(a,b));

    a = {1, 3, 2};
    b = {1, 1, 2};
    EXPECT_ERROR(binaryMerge(a,b));

    a = {1, 7, 5};
    b = {1, 3, 2};
    EXPECT_ERROR(binaryMerge(a,b));
}

STUDENT_TEST("Time binaryMerge operation") {
    for (int n = 1000000; n <= 10000000; n *= 2) {
        Queue<int> a = createSequence(n);
        Queue<int> b = createSequence(n);
        TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
    }
}

STUDENT_TEST("naiveMultiMerge test") {
    Vector<Queue<int>> all = {
                            };
    Queue<int> expected = {};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

STUDENT_TEST("naiveMultiMerge test") {
    Vector<Queue<int>> all = {{},
                              {},
                              {},
                              {},
                              {-5, -5},
                              {3402}
                             };
    Queue<int> expected = {{-5, -5, 3402}};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

STUDENT_TEST("naiveMultiMerge test") {
    Vector<Queue<int>> all = {{},
                              {},
                              {},
                              {},
                              {},
                              {}
                             };
    Queue<int> expected = {};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

STUDENT_TEST("Time naiveMultiMerge operation - variable n") {
    int k = 15;
    for (int n = 100000; n <= 1500000; n *= 2) {
        Vector<Queue<int>> all = {};
        for (int i = 0; i < k; i++) {
            all.add(createSequence(n/k));
        }
        TIME_OPERATION(n, naiveMultiMerge(all));
    }
}

STUDENT_TEST("Time naiveMultiMerge operation - variable k") {
    int n = 10000;
    for (int k = 50; k <= 400; k *= 2) {
        Vector<Queue<int>> all = {};
        for (int i = 0; i < k; i++) {
            all.add(createSequence(n/k));
        }
        TIME_OPERATION(k, naiveMultiMerge(all));
    }
}

STUDENT_TEST("Test recMultiMerge") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));

    all = {{},
          {},
          {},
          {},
          {-5, -5},
          {3402}
         };
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

STUDENT_TEST("Time recMultiMerge operation - variable n") {
    int k = 15;
    for (int n = 100000; n <= 1500000; n *= 2) {
        Vector<Queue<int>> all = {};
        for (int i = 0; i < k; i++) {
            all.add(createSequence(n/k));
        }
        TIME_OPERATION(n, recMultiMerge(all));
    }
}

STUDENT_TEST("Time recMultiMerge operation - variable k") {
    int n = 1000000;
    for (int k = 100; k <= 10000; k *= 4) {
        Vector<Queue<int>> all = {};
        for (int i = 0; i < k; i++) {
            all.add(createSequence(n/k));
        }
        TIME_OPERATION(k, recMultiMerge(all));
    }
}
