/**
 * File: warmup.cpp
 * ----------------
 * @author Julie Zelenski, Fall 2020
 * This code is provided as part of an exercise to practice using a
 * debugger. This program has buggy use of recursion.
 */

#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "simpio.h"    // for getLine
#include "hanoigui.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


/*
 * This is a correct implementation of a function that uses recursion
 * to explore all possible subsets of elements from v and returns the
 * count of how many of those subsets sum to zero.
 */
int countZeroSumSubsets(Vector<int> &v, int index, int sumSoFar) {
    if (index == v.size()) {
        if (sumSoFar == 0) {
            return 1;   // base case success: sums to zero, count = 1
        } else {
            return 0;   // base case failure: not zero, count = 0
        }
    } else {
        return countZeroSumSubsets(v, index + 1, sumSoFar + v[index])
             + countZeroSumSubsets(v, index + 1, sumSoFar);
    }
}

/*
 * This function is only one character different than the
 * correct version above, but even a small edit can cause big havoc
 * when amplified by recursion.
 *
 * WARNING: The given code is buggy. See the exercise writeup for
 * more information on how to test and diagnose.
 */
int buggyCount(Vector<int> &v, int index, int sumSoFar) {
    if (index == v.size()) {
        if (sumSoFar == 0) {
            return 1;   // base case success: sums to zero, count = 1
        } else {
            return 0;   // base case failure: not zero, count = 0
        }
    } else {
        return buggyCount(v, index + 1, sumSoFar + v[index])
             + buggyCount(v, index + 1, sumSoFar);
    }
}

/* This function moves a tower of the specified height from the start
 * peg to the end peg using temporary peg as waystation.
 *
 * This function uses the functions of the HanoiGui to visually animate
 * the movement.
 *
 * This function also counts the total number of moves made by incrementing
 * the totalMoves parameter (passed by reference) for each single disk move.
 * An alternative way to count would be to return the count from the
 * recursive calls and sum those counts together.
 */
void moveTower(int towerHeight, string startPeg, string endPeg, string tempPeg, int& totalMoves) {
    if (towerHeight == 1) {
        // base case: tower of single disk is easy to move
        HanoiGui::moveOneDisk(startPeg, endPeg);
        totalMoves++;
    } else {
        // recursive case:
        // move smaller tower aside onto tmp, this uncovers the largest disk that was at bottom
        // move the largest disk from src to dst
        // move the smaller tower from tmp to dst
        moveTower(towerHeight - 1, startPeg, tempPeg, endPeg, totalMoves);
        HanoiGui::moveOneDisk(startPeg, endPeg);
        totalMoves++;
        moveTower(towerHeight - 1, tempPeg, endPeg, startPeg, totalMoves);
    }
}

/*
 * Thus function initializes the HanoiGui and calls the above
 * moveTower function to move the tower on left peg to the right
 * peg, using the middle as the temporary
 */
void hanoiAnimation() {
    int myHeight = 4;
    HanoiGui::initialize(myHeight);
    pause(1000);

    // do the recursive search and count total moves by reference
    int totalMoves = 0;
    moveTower(myHeight, "left", "right", "middle", totalMoves);
    cout << "Total moves made = " << totalMoves << endl;
}

/* * * * * * Test Cases * * * * * */

//PROVIDED_TEST("Animate Towers of Hanoi") {
//    hanoiAnimation();
//}

PROVIDED_TEST("Test correct countZeroSumSubsets against buggyCount") {
    Vector<int> nums = {};
    EXPECT_EQUAL(countZeroSumSubsets(nums, 0, 0), buggyCount(nums, 0, 0));

    nums = {1, 2, 3, -4, -3, 2, 5, 1, 9, -2};
    EXPECT_EQUAL(countZeroSumSubsets(nums, 0, 0), buggyCount(nums, 0, 0));
}

STUDENT_TEST("Test correct countZeroSumSubsets against buggyCount") {
    Vector<int> nums = {1};
    EXPECT_EQUAL(countZeroSumSubsets(nums, 0, 0), buggyCount(nums, 0, 0));
}
