/*
 * File edited by Thomas Sounack
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "grid.h"
#include "gridgraphics.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


/*
 * This function is a helper function. It takes a grid of ints (g) and
 * a GridLocationRange (range) as input, and returns the sum of the
 * integers as an integer in the associated subgrid.
 */
int subgridSum(Grid<int>& g, GridLocationRange range) {
    int sumPop = 0;
    for (GridLocation loc : range) {
        sumPop += g[loc];
    }
    return sumPop;
}


/*
 * This function is a helper function. Using a inputted Row, it defines
 * two subgrids within a grid, and passes them by reference. It returns
 * nothing.
 */
void divideHorizontal(GridLocationRange range, int topEndRow, GridLocationRange& top, GridLocationRange& bottom) {
    GridLocation start = range.startLocation();
    GridLocation end   = range.endLocation();

    GridLocation new_end   = GridLocation(topEndRow, end.col);
    GridLocation new_start = GridLocation(topEndRow + 1, start.col);

    // Defining the top and bottom ranges
    top    = GridLocationRange(start, new_end);
    bottom = GridLocationRange(new_start, end);
}


/*
 * This function is a helper function. Using a inputted Column, it defines
 * two subgrids within a grid, and passes them by reference. It returns
 * nothing.
 */
void divideVertical(GridLocationRange range, int leftEndCol, GridLocationRange& left, GridLocationRange& right) {
    GridLocation start = range.startLocation();
    GridLocation end   = range.endLocation();

    GridLocation new_end   = GridLocation(end.row, leftEndCol);
    GridLocation new_start = GridLocation(start.row, leftEndCol + 1);

    left  = GridLocationRange(start, new_end);
    right = GridLocationRange(new_start, end);
}


/*
 * This function takes a Grid of ints (populationGrid), a GridLocationRange (range) and an int
 * (districtSize) as input. It uses recursion to determine if there is a way to split the grid
 * in districts of the same size. The function returns a bool: true if it is possible, false if
 * it is not.
 */
bool canRedistrictHelper(Grid<int>& populationGrid, GridLocationRange range, int districtSize) {
    int sumPop = subgridSum(populationGrid, range);

    // Base case: sum of the population in range equals the target
    // disctrict size - success
    if (sumPop == districtSize) {
        return true;
    }

    // Base case: Sum of the population in current range is not a
    // multiple of the target district size - failure
    if (sumPop % districtSize != 0) {
        return false;
    }


    // Iterates through cols and rows to choose a H or V splitline

    for (int i = range.startRow(); i < range.endRow(); i++) {
        // Divide current range into two subranges and recurse on
        // the two subranges
        GridLocationRange top;
        GridLocationRange bottom;
        divideHorizontal(range, i, top, bottom);

        drawChosenRanges(populationGrid, top, bottom);

        if (canRedistrictHelper(populationGrid, top, districtSize) &&
            canRedistrictHelper(populationGrid, bottom, districtSize)) {
            return true;
        }

        undoChosenRanges(populationGrid, top, bottom);
    }

    for (int j = range.startCol(); j < range.endCol(); j++) {
        GridLocationRange left;
        GridLocationRange right;
        divideVertical(range, j, left, right);

        drawChosenRanges(populationGrid, left, right);

        if (canRedistrictHelper(populationGrid, left, districtSize) &&
            canRedistrictHelper(populationGrid, right, districtSize)) {
            return true;
        }

        undoChosenRanges(populationGrid, left, right);
    }

    return false;
}


/*
 * This function is the wrapper function for the previous function. It takes
 * a Grid of ints (populationGrid) and an int (nDistricts) as input, and
 * returns a boolean determining if the grid can be split in nDistricts of
 * the same size.
 */
bool canRedistrict(Grid<int>& populationGrid, int nDistricts) {
    GridLocationRange range = GridLocationRange(0, 0, populationGrid.numRows() - 1,
                                                populationGrid.numCols() - 1);
    int districtSize = subgridSum(populationGrid, range) / nDistricts;

    return canRedistrictHelper(populationGrid, range, districtSize);
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("subgridSum sums values in grid range") {
    Grid<int> grid = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 10, 10} };
    GridLocationRange all = grid.locations();
    GridLocationRange firstColumn = GridLocationRange(0, 0, 3, 0);
    GridLocationRange bottomCorner = GridLocationRange(3, 2, 3, 2);

    EXPECT_EQUAL(subgridSum(grid, firstColumn), 22);
    EXPECT_EQUAL(subgridSum(grid, bottomCorner), 10);
    EXPECT_EQUAL(subgridSum(grid, all), 75);
}

PROVIDED_TEST("divide grid range horizontally, test using subgridSum") {
    Grid<int> grid = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 10, 10} };
    GridLocationRange all = grid.locations();
    GridLocationRange subrange1, subrange2;

    divideHorizontal(all, 0, subrange1, subrange2); // split first row from rest
    EXPECT_EQUAL(subgridSum(grid, subrange1), 6);
    EXPECT_EQUAL(subgridSum(grid, subrange2), 69);
}

PROVIDED_TEST("nevada example from writeup can be divided into 4 districts") {
    Grid<int> nevada = { {20, 31, 20},
                         { 5, 19, 30},
                         {25, 25, 25} };

    initForPopulationGrid(nevada, true); // you can remove if not animating test case
    EXPECT(canRedistrict(nevada, 4));
}

PROVIDED_TEST("wyoming example from writeup cannot be divided into 2 districts") {
    Grid<int> wyoming = { {10, 15},
                          { 5, 10} };

    initForPopulationGrid(wyoming, true); // you can remove if not animating test case
    EXPECT(!canRedistrict(wyoming, 2));
}

PROVIDED_TEST("medium-size animated example from writeup can be divided into 12 districts") {
    Grid<int> medium = { {10, 1,  2, 5, 1, 4},
                          {1, 1,  8, 2, 1, 7},
                          {9, 8, 10, 3, 3, 4},
                          {5, 2,  2, 8, 7, 3},
                          {2, 1,  1, 7, 1, 1} };
    initForPopulationGrid(medium, false); // you can remove if not animating test case
    EXPECT(canRedistrict(medium, 12));
}

PROVIDED_TEST("large example can be divided into 25 districts") {
    Grid<int> large = { { 5,  2, 2,  1,  6, 4, 8, 2},
                        {10, 10, 2,  1,  3, 4, 2, 3},
                        { 1,  1, 4,  5,  1, 9, 1, 4},
                        { 9,  5, 5, 10, 10, 3, 3, 4},
                        { 3,  1, 1,  1,  2, 8, 6, 2},
                        { 2,  1, 1,  7,  3, 5, 1, 1},
                        { 1,  1, 2,  2,  7, 5, 1, 9},
                        { 4,  1, 2, 10,  1, 9, 5, 5} };
    initForPopulationGrid(large, false); // you can remove if not animating test case
    EXPECT(canRedistrict(large, 25));
}
