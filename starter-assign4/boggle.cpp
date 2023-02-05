/*
 * File edited by Thomas Sounack
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "gridlocation.h"
#include "grid.h"
#include "set.h"
#include "lexicon.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/*
 * This function takes a string (str) as input and returns an int corresponding
 * to the score of the word according to the rules.
 */
int points(string str) {
    int n = str.size();

    if (n < 4) {
        return 0;
    }

    return n - 3;
}


/*
 * This function is a helper function. It takes a grid of chars (board) a set of
 * vectors of ints (previous), and two ints (row and col) as input. It returns a
 * set of vectors of ints containing the indexes of each possible next move.
 */
Set<Vector<int>> nextIndexes(Grid<char>& board, Set<Vector<int>>& previousPos,
                                                              int row, int col) {
    Set<Vector<int>> cubes;
    Vector<int> central = {row, col};

    // Iterating over the neighbors of the current cube
    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {
            Vector<int> current = {r, c};
            // If the cube is in the board and is not the central cube,
            // and if it is not a previously chosen cube
            if (board.inBounds(r, c) && current != central
                                     && (! previousPos.contains(current))) {
                Vector<int> indexes = {r, c};
                cubes.add(indexes);
            }
        }
    }
    return cubes;
}


/*
 * This function uses recursion to go over each word that can be made on the board
 * and evaluate the total score, which it returns as an int.
 */
int scoreBoardHelper(Grid<char>& board, Lexicon& lex, Set<string>& previousWords,
                     Set<Vector<int>> previousPos, string word, int row, int col) {
    // Base case: pruning - end path if no remaining possible word
    if (! lex.containsPrefix(word)) {
        return 0;
    }

    int sumScore = 0;

    // Add string to the score and to the previous words if it is a word and was not
    // previously entered
    if (lex.contains(word) && (! previousWords.contains(word))) {
        sumScore += points(word);
        previousWords.add(word);
    }

    // Possible next cubes indexes in a set of vectors
    Set<Vector<int>> next = nextIndexes(board, previousPos, row, col);

    // Update the Set of previous positions
    Vector<int> pos = {row, col};
    previousPos.add(pos);

    // For each next index, add the score of the recursive call to the sum
    for (Vector<int> coord : next) {
        int new_row  = coord[0];
        int new_col  = coord[1];
        char new_char = board.get(new_row, new_col);
        // we update the word and the coordinates in the recursive call
        sumScore += scoreBoardHelper(board, lex, previousWords, previousPos,
                                     word + new_char, new_row, new_col);
    }
    return sumScore;
}


/*
 * This function is a wrapper function. It takes a Grid of chars (board) and a
 * lexicon as input and returns the corresponding boggle score as an int.
 */
int scoreBoard(Grid<char>& board, Lexicon& lex) {
    int numRows  = board.numRows();
    int numCols  = board.numCols();
    int sumScore = 0;

    Set<string> previousWords;
    Set<Vector<int>> previousPos;

    // This loop through the grid allows the algorithm to run with all the
    // starting possibilities
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            // Word is initiated as beginning cube (char)
            string word = charToString(board.get(i, j));
            sumScore += scoreBoardHelper(board, lex, previousWords, previousPos,
                                         word, i, j);
        }
    }
    return sumScore;
}





/* * * * * * Test Cases * * * * * */

/* Test helper function to return shared copy of Lexicon. Use to
 * avoid (expensive) re-load of word list on each test case. */
static Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Load shared Lexicon, confirm number of words") {
    Lexicon lex = sharedLexicon();
    EXPECT_EQUAL(lex.size(), 127145);
}

PROVIDED_TEST("Test point scoring") {
    EXPECT_EQUAL(points("and"), 0);
    EXPECT_EQUAL(points("quad"), 1);
    EXPECT_EQUAL(points("quint"), 2);
    EXPECT_EQUAL(points("sextet"), 3);
    EXPECT_EQUAL(points("seventh"), 4);
    EXPECT_EQUAL(points("supercomputer"), 10);
}

PROVIDED_TEST("Test scoreBoard, board contains no words, score of zero") {
    Grid<char> board = {{'B','C','D','F'}, //no vowels, no words
                        {'G','H','J','K'},
                        {'L','M','N','P'},
                        {'Q','R','S','T'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 0);
}

PROVIDED_TEST("Test scoreBoard, board contains one word, score of 1") {
    Grid<char> board = {{'C','_','_','_'},
                        {'Z','_','_','_'},
                        {'_','A','_','_'},
                        {'_','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, alternate paths for same word, still score of 1") {
    Grid<char> board = {{'C','C','_','_'},
                        {'C','Z','C','_'},
                        {'_','A','_','_'},
                        {'R','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, small number of words in corner of board") {
    Grid<char> board = {{'L','I','_','_'},
                        {'M','E','_','_'},
                        {'_','S','_','_'},
                        {'_','_','_','_'}};
    Set<string> words = {"SMILE", "LIMES", "MILES", "MILE", "MIES", "LIME", "LIES", "ELMS", "SEMI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()),  2 + 2 + 2 + 1 + 1 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, small number of words") {
    Grid<char> board = {{'E','Z','R','R'},
                        {'O','H','I','O'},
                        {'N','J','I','H'},
                        {'Y','A','H','O'}};
    Set<string> words = { "HORIZON", "OHIA", "ORZO", "JOHN", "HAJI"};
    int nPoints = 0;
    for (string word: words) {
        nPoints += points(word);
    }
    EXPECT_EQUAL(nPoints, 4 + 1 + 1 + 1 + 1);
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), nPoints);
}

PROVIDED_TEST("Test scoreBoard, full board, medium number of words") {
    Grid<char> board = {{'O','T','H','X'},
                        {'T','H','T','P'},
                        {'S','S','F','E'},
                        {'N','A','L','T'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 76);
}

PROVIDED_TEST("Test scoreBoard, full board, large number of words") {
    Grid<char> board = {{'E','A','A','R'},
                        {'L','V','T','S'},
                        {'R','A','A','N'},
                        {'O','I','S','E'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 234);
}

STUDENT_TEST("Test scoreBoard, board contains one word, score of 1") {
    Grid<char> board = {{'D','_','_','_'},
                        {'_','I','_','_'},
                        {'_','_','C','_'},
                        {'_','_','_','E'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

STUDENT_TEST("Test scoreBoard, board contains one word, score of 1") {
    Grid<char> board = {{'C','R','_','_'},
                        {'_','_','E','_'},
                        {'_','_','A','_'},
                        {'_','_','M','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 3);
}
