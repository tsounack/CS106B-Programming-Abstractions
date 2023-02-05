#include "labyrinth.h"
#include "demo/MazeGenerator.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* Change kYourName constant to your full name.
 *
 * WARNING: Once you've set set this constant and started exploring your labyrinth,
 * do NOT edit the value of kYourName. Changing kYourName will change which
 * maze you get back, which might invalidate all your hard work!
 */
const string kYourName = "Thomas Sounack";

/* Change this constant to the path out of your labyrinth. */
const string kPathForYourName = "WSESSWNSENNENESNWSSSEN";

/**
 * This function is a helper function for isPathToFreedom. It takes a pointer to a
 * MazeCell and two sets of strings as arguments - objects is passed by reference
 * to update it in the function. If the current cell contains an object it is added
 * to the set of objects found. The function returns true if all objects are found,
 * and false otherwise.
 */
bool verifyObjects(MazeCell* current, Set<string> needed, Set<string> &objects) {
    // check if current cell contains a new object
    if (needed.contains(current->contents)) {
        objects.add(current->contents);
    }

    // if all items are gathered, return true
    if (objects.size() == needed.size()) {
        return true;
    }
    return false;
}


/**
 * Given a location in a maze, returns whether the given sequence of
 * steps will let you escape the maze.
 *
 * To escape the maze, you need to collect all items in the `needed` set.
 * You can only take steps in the four cardinal directions,
 * and you can't move in directions that don't exist in the maze.
 */
bool isPathToFreedom(MazeCell* start, string moves, Set<string> needed) {
    Set<string> objects;
    string possibleMoves = "NSEW";
    MazeCell* current = start;
    MazeCell* next;

    for (char move : moves) {
        // if the cell contains an object, add it to the set of objects
        // explored. Return true if all objects are found
        if (verifyObjects(current, needed, objects)) {
            return true;
        }

        // if next step is an invalid char, report error
        if (! stringContains(possibleMoves, charToString(move))) {
            error("The moves contain an invalid character");
            return false;
        }

        // if next step is nullptr, return false
        // (the lines were short so I compacted the cases)
        switch (move) {
        case 'N': next = current->north; break;
        case 'S': next = current->south; break;
        case 'E': next = current->east;  break;
        case 'W': next = current->west;  break;
        }

        if (next == nullptr) {
            return false;
        }

        current = next;
    }

    // This function has to be called again to take the last cell in account
    if (verifyObjects(current, needed, objects)) {
        return true;
    }

    return false;
}


/* * * * * * Test Cases Below This Point * * * * * */

PROVIDED_TEST("isPathToFreedom: paths on example labyrinth from writeup") {
    Set<string> allThree = {"Spellbook", "Potion", "Wand"};
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});
    MazeCell *smileyStart = maze[2][2];

    /* These paths are in the writeup. Both succeed. */
    EXPECT(isPathToFreedom(smileyStart, "ESNWWNNEWSSESWWN", allThree));
    EXPECT(isPathToFreedom(smileyStart, "SWWNSEENWNNEWSSEES", allThree));

    /* This path successfully collects just spellbook */
    EXPECT(isPathToFreedom(smileyStart, "SWWN", {"Spellbook"}));

    /* This path fails, doesn't collect all three items */
    EXPECT(!isPathToFreedom(smileyStart, "WNNE", allThree));

    /* This path fails, cannot walk through nullptr link */
    EXPECT(!isPathToFreedom(smileyStart, "WNWS", allThree));
}

PROVIDED_TEST("isPathToFreedom: collects needed item from start") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "E", {"Potion"}));
    EXPECT(isPathToFreedom(maze[0][1], "W", {"Potion", "Spellbook"}));
}

PROVIDED_TEST("isPathToFreedom: errors if attempt step invalid direction char") {
    auto maze = toMaze({"P-S-W"});

    EXPECT_ERROR(isPathToFreedom(maze[0][0], "EWQ", {"Wand"})); // Q is invalid step
    EXPECT(!isPathToFreedom(maze[0][0], "EWWZ", {"Wand"})); // fails at nullptr link
    EXPECT_NO_ERROR(isPathToFreedom(maze[0][0], "EEG", {"Wand"})); // success, rest of path ignored
}

PROVIDED_TEST("Escape from your personal labyrinth!") {
    Set<string> allThree = {"Spellbook", "Potion", "Wand"};
    /* A maze for you to escape from. This maze will be personalized
     * based on the constant kYourName.
     */
    EXPECT(kYourName != "TODO");
    MazeCell* startLocation = mazeFor(kYourName);

    /* Set a breakpoint here. As a reminder, the labyrinth you
     * get will be personalized to you, so don't start exploring the labyrinth
     * unless you've edited the constant kYourName to include your name(s)!
     * Otherwise, you'll be escaping the wrong labyrinth.
     */
    EXPECT(kPathForYourName != "TODO");
    EXPECT(isPathToFreedom(startLocation, kPathForYourName, allThree));
}

