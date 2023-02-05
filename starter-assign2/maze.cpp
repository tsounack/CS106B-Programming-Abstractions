// File edited by Thomas Sounack

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// This function takes a maze (grid) and a current position (gridlocation) as
// argument and returns the possible directions as a set of gridlocations.
// It verifies if all 4 directions are possible (ie not a wall or out of bounds)
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    // Not the most elegant, but this way we don't have to run multiple if statements to
    // check if a combination is a possible direction (and we have only one loop!).
    Vector<GridLocation> possibleDirections = {{cur.row, cur.col - 1}, {cur.row, cur.col + 1},
                                               {cur.row - 1, cur.col}, {cur.row + 1, cur.col}};

    for (GridLocation direction : possibleDirections) {
            // The inBounds condition makes sure we are not adding a direction that is
            // outside of the maze.
            if (maze.inBounds(direction)) {
                if (maze[direction] == true) {
                    neighbors.add(direction);
            }
        }
    }

    return neighbors;
}


// This function takes a maze (grid) and a path (vector of GridLocation) as
// argument and returns an error if a path is not valid.
// It verifies if the path is not empty, if the starting and ending positions
// are correct, if each move is valid and if the path is not looping.
void validatePath(Grid<bool>& maze, Vector<GridLocation>& path) {
    if (path.isEmpty()) {
        error("Path is empty!");
    }

    int n = path.size();

    // Checking the starting position
    if (path[0] != GridLocation {0, 0}) {
        error("Path does not start at the upper left corner of the maze");
    }

    // Checking the ending position
    if (path[n - 1] != GridLocation {maze.numRows() - 1, maze.numCols() - 1}) {
        error("Path does not end at the lower right corner of the maze");
    }

    // Checking the validity of each move
    for (int i = 1; i < n; i++) {
        // If the move i is not contained in the set of possible moves computed
        // at the move i - 1, return an error
        if (! generateValidMoves(maze, path[i - 1]).contains(path[i])) {
            error("This path is not valid");
        }
    }

    // We can check redundancy simply by creating a set with the vector and
    // checking if the length of the vector is the same as the length of the set.
    Set<GridLocation> pathSet;
    for (GridLocation move : path) {
        pathSet.add(move);
    }

    if (pathSet.size() != path.size()) {
        error("This path contains a loop");
    }
}


// This function takes a grid (maze) as argument and solves it using
// the Breadth-first search (BFS) method. It returns a valid path
// in the form of a vector of gridlocations.
Vector<GridLocation> solveMazeBFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Queue<Vector<GridLocation>> allPaths;
    Set<GridLocation> storedMoves;

    drawMaze(maze);

    path.add(GridLocation {0, 0});
    allPaths.enqueue(path);
    storedMoves.add(GridLocation {0, 0});

    // We keep searching until all paths have been exhausted
    while (! allPaths.isEmpty()) {
        path = allPaths.dequeue();
        int n = path.size();
        highlightPath(path, "blue");

        // If the path reached the end of the maze, we return it.
        if (path[n - 1] == GridLocation {maze.numRows() - 1, maze.numCols() - 1}) {
            return path;
        }

        // Otherwise, we compute new paths
        else {
            Set<GridLocation> neighbors = generateValidMoves(maze, path[n - 1]);

            for (GridLocation elem : neighbors) {
                // The move has to be unvisited to be valid
                if (! storedMoves.contains(elem)) {
                    // We create a new vector, add the move and queue the vector
                    Vector<GridLocation> newPath = path;
                    newPath.add(elem);
                    allPaths.enqueue(newPath);

                    // We store the move to avoid repeating it
                    storedMoves.add(elem);
                }
            }
        }
    }
    return path;
}



// This function takes a grid (maze) as argument and solves it using
// the Depth-first search (DFS) method. It returns a valid path
// in the form of a vector of gridlocations.
Vector<GridLocation> solveMazeDFS(Grid<bool>& maze) {
    Vector<GridLocation> path;
    Stack<Vector<GridLocation>> allPaths;
    Set<GridLocation> storedMoves;

    drawMaze(maze);

    path.add(GridLocation {0, 0});
    allPaths.push(path);
    storedMoves.add(GridLocation {0, 0});

    // We keep searching until all paths have been exhausted
    while (! allPaths.isEmpty()) {
        path = allPaths.pop();
        int n = path.size();
        highlightPath(path, "blue");

        // If the path reached the end of the maze, we return it.
        if (path[n - 1] == GridLocation {maze.numRows() - 1, maze.numCols() - 1}) {
            return path;
        }

        // Otherwise, we compute new paths
        else {
            Set<GridLocation> neighbors = generateValidMoves(maze, path[n - 1]);

            for (GridLocation elem : neighbors) {
                // The move has to be unvisited to be valid
                if (! storedMoves.contains(elem)) {
                    // We create a new vector, add the move and queue the vector
                    Vector<GridLocation> newPath = path;
                    newPath.add(elem);
                    allPaths.push(newPath);

                    // We store the move to avoid repeating it
                    storedMoves.add(elem);
                }
            }
        }
    }
    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Vector<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0, 0}, {0, 2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution, hand-constructed maze") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Vector<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution read from file, medium maze") {
    Grid<bool> maze;
    Vector<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> not_end_at_exit = { {0, 0}, {1, 0}, {2, 0} };
    Vector<GridLocation> not_begin_at_entry = { {1, 1}, {2, 1} };
    Vector<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1}, {2, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
}

PROVIDED_TEST("solveMazeBFS on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMazeDFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeDFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}



STUDENT_TEST("generateValidMoves on location in the center of 3x3 grid with walls at top and right") {
     Grid<bool> maze = {{true, false, true},
                        {true, true, false},
                        {true, true, true}};
     GridLocation center = {1, 1};
     Set<GridLocation> expected = {{1, 0}, {2, 1}};

     EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves on location in the center of 3x3 grid with start at top center and no walls") {
     Grid<bool> maze = {{true, true, true},
                        {true, true, true},
                        {true, true, true}};
     GridLocation center = {0, 1};
     Set<GridLocation> expected = {{0, 0}, {0, 2}, {1, 1}};

     EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves on location in the center of 3x3 grid with start at bottom left and a wall above") {
     Grid<bool> maze = {{true, true, true},
                        {false, true, true},
                        {true, true, true}};
     GridLocation center = {2, 0};
     Set<GridLocation> expected = {{2, 1}};

     EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("validatePath on invalid path should raise error - loop in path") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> loop_in_path = { {0, 0}, {1, 0}, {1, 1},  {1, 0}, {2, 0}, {2, 1} };

    EXPECT_ERROR(validatePath(maze, loop_in_path));
}

STUDENT_TEST("validatePath on invalid path should raise error - empty path") {
    Grid<bool> maze = {{true, false},
                       {true, true},
                       {true, true}};
    Vector<GridLocation> empty_path = { };

    EXPECT_ERROR(validatePath(maze, empty_path));
}

STUDENT_TEST("validatePath on valid path should raise no error") {
    Grid<bool> maze = {{true, true, false},
                       {false, true, true},
                       {true, true, false},
                       {true, true, true}};
    Vector<GridLocation> valid_path = { {0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 0}, {3, 0}, {3, 1}, {3, 2} };

    EXPECT_NO_ERROR(validatePath(maze, valid_path));
}

STUDENT_TEST("solveMazeBFS on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}


STUDENT_TEST("solveMazeBFS on file 33x41") {
    Grid<bool> maze;
    readMazeFile("res/33x41.maze", maze);
    Vector<GridLocation> soln = solveMazeBFS(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}
