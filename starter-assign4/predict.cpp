/*
 * File edited by Thomas Sounack
 */
#include <iostream>
#include "backtracking.h"
#include "lexicon.h"
#include "set.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

// keypad is a program-wide constant that stores the Map from digit char to
// its associated set of possible letters
static const Map<char, Set<char>> keypad = {
    {'2', {'a','b','c'} }, {'3', {'d','e','f'} }, {'4', {'g','h','i'} },
    {'5', {'j','k','l'} }, {'6', {'m','n','o'} }, {'7', {'p','q','r','s'} },
    {'8', {'t','u','v'} }, {'9', {'w','x','y','z'} } };


/*
 * This function takes a string (digits), a set of strings (suggestions), a lexicon
 * (lex) and a string (word) as input, and returns nothing.
 * It uses recursion to add all the possible suggestions to the set and ends paths
 * early if no corresponding words will be found.
 */
void predictHelper(string digits, Set<string>& suggestions, Lexicon& lex, string word) {
    // Base case: if all numbers have been processed, add the words
    // associated with the sequence to suggestions and end path
    if (digits.size() == 0) {
        if (lex.contains(word)) {
            suggestions.add(word);
            return;
        }
    }

    // Base case: pruning - end path if no remaining possible word
    if (! lex.containsPrefix(word)) {
        return;
    }

    // Set of letters associated with this call
    Set<char> letters = keypad[digits[0]];

    // Remove digit from digits
    digits.erase(0,1);

    // Recursive call for each of the letters
    for (char letter : letters) {
        predictHelper(digits, suggestions, lex, word + letter);
    }
}



/*
 * This function is the wrapper function for predictHelper. It takes a string (digits),
 * a set of strings (suggestions) and a lexicon (lex) and returns nothing.
 */
void predict(string digits, Set<string>& suggestions, Lexicon& lex) {
    // Define empty word string
    string word = "";

    // Wrapper function: call function
    predictHelper(digits, suggestions, lex, word);
}



//     AUTO COMPLETE EXTENSION


/*
 * This function takes a string (digits), a set of strings (suggestions), a lexicon
 * (lex) and a string (word) as input, and returns nothing.
 * It uses recursion to add all the possible suggestions to the set and ends paths
 * early if no corresponding words will be found.
 */
void autoCompleteHelper(string digits, Set<string>& suggestions, Lexicon& lex, string word) {
    // Base case: pruning - end path if no remaining possible word
    if (! lex.containsPrefix(word)) {
        return;
    }


    Set<char> letters;

    // If the input is not completely processed, we only use
    // its letters to form the chain
    if (digits.size() > 0) {
        letters = keypad[digits[0]];
        digits.erase(0,1);
    }

    // Otherwise we can add any letter
    else {
        // We add the current word to suggestions if it exists
        if (lex.contains(word)) {
            suggestions.add(word);
        }

        for (int i = 2; i <= 9; i++) {
            char number = stringToChar(integerToString(i));
            letters.unionWith(keypad[number]);
        }
    }


    // Recursive call for each of the possible letters
    for (char letter : letters) {
        autoCompleteHelper(digits, suggestions, lex, word + letter);
    }
}



/*
 * This function is the wrapper function for predictHelper. It takes a string (digits),
 * a set of strings (suggestions) and a lexicon (lex) and returns nothing.
 */
void autoComplete(string digits, Set<string>& suggestions, Lexicon& lex) {
    // Define empty word string
    string word = "";

    // Wrapper function: call function
    autoCompleteHelper(digits, suggestions, lex, word);
}


/* * * * * * Test Cases * * * * * */

/* Test helper function to return shared copy of Lexicon. Use to
 * avoid (expensive) re-load of word list on each test case. */
static Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Predict intended words for digit sequence 6263, example from writeup") {
    string digits = "6263";
    Set<string> expected = {"name", "mane", "oboe"};
    Set<string> suggestions;

    predict(digits, suggestions, sharedLexicon());
    EXPECT_EQUAL(suggestions, expected);
}

PROVIDED_TEST("What words can I spell from my phone number?") {
    string digits = "3228679";  // my old phone number :-)
    Set<string> expected = {"factory"};
    Set<string> suggestions;

    predict(digits, suggestions, sharedLexicon());
    EXPECT_EQUAL(suggestions, expected);
}

STUDENT_TEST("New words!") {
    string digits = "43556";
    string expected = "hello";
    Set<string> suggestions;

    predict(digits, suggestions, sharedLexicon());
    EXPECT(suggestions.contains(expected));

    digits = "3663";
    expected = "food";
    Set<string> suggestions2;

    predict(digits, suggestions2, sharedLexicon());
    EXPECT(suggestions2.contains(expected));
}

STUDENT_TEST("Auto complete extension") {
    string digits = "435";
    string expected = "hello";
    string expected2 = "hell";
    Set<string> suggestions;

    autoComplete(digits, suggestions, sharedLexicon());
    EXPECT(suggestions.contains(expected));
    EXPECT(suggestions.contains(expected2));

    digits = "3663";
    expected = "domestic";
    expected2 = "fomenting";
    Set<string> suggestions2;

    autoComplete(digits, suggestions2, sharedLexicon());
    EXPECT(suggestions2.contains(expected));
}
