/*
 * This file was written by Thomas Sounack for the CS106B class
 */
#include <cctype>
#include <fstream>
#include <string>
#include "console.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/* This function is intended to return a string which
 * includes only the letter characters from the original
 * (all non-letter characters are excluded)
 *
 * The issue with this function was that the first character
 * was always stored in the solution. It was therefore never
 * tested, and was not removed if it was a non-letter. It
 * also caused issues with empty inputs, as the result was
 * not initialized well. This issue was fixed by initializing
 * the result as an empty string instead of the first character
 * of the input.
 */
string lettersOnly(string s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}

/* This function converts each letter in the input string
 * into a number following the soundex table. It returns them
 * in a string.
 */
string encoder(string s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
        // We convert the character to an uppercase to have less cases to test
        char letter = toupper(s[i]);
        if (letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U' || letter == 'H' || letter == 'W' || letter == 'Y') {
            result += '0';
        }
        if (letter == 'B' || letter == 'F' || letter == 'P' || letter == 'V') {
            result += '1';
        }
        if (letter == 'C' || letter == 'G' || letter == 'J' || letter == 'K' || letter == 'Q' || letter == 'S' || letter == 'X' || letter == 'Z') {
            result += '2';
        }
        if (letter == 'D' || letter == 'T') {
            result += '3';
        }
        if (letter == 'L') {
            result += '4';
        }
        if (letter == 'M' || letter == 'N') {
            result += '5';
        }
        if (letter == 'R') {
            result += '6';
        }
    }
    return result;
}


/* This function takes a string of digits and coalesces
 * adjacent duplicate digits form the code
 */
string coalescer(string s) {
    string result = "";
    // Initialising an empty character to keep the last character in memory during the loop
    char last = '\0';
    for (int i = 0; i < s.length(); i++) {
        // If the selected character from the input is not the same as the previous one, add this character to the output string
        if (s[i] != last) {
            result += s[i];
        }
        last = s[i];
    }
    return result;
}

/* This function takes a string of digits (code) as well as a string
 * of letters (name), and outputs the code replacing the first digit
 * of the code with the first letter of the name in uppercase.
 */
string firstDigitToUppercase(string s, string name) {
    // This code returns an error when the inputs are empty,
    // the following condition ensures this does not happen.
    if (name != "" && s != "") {
        // We want to have the first letter of the name as an uppercase
        char firstLetter = toupper(name[0]);
        // We want to have all the code except the first digit
        string result = firstLetter + s.substr(1);
        return result;
    }
    // If the inputs are empty, the function returns an empty string
    else {
        string result = "";
        return result;
    }
}


/* This function takes a string (code) and outputs the same
 * string of digits without the zeros.
 */
string discardZeros(string s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
        // If the character is not 0, we add it to the return string.
        if (s[i] != '0') {
            result += s[i];
        }
    }
    return result;
}

/* This function takes a string (code). If the length is
 * less than 4, it padds with zero(s). If the length is more than 4,
 * it truncates the excess.
 */
string lengthFour(string s) {
    // If the length of s is less than 4, we add zeros
    if (s.length() < 4) {
        while (s.length() != 4) {
            s.append("0");
        }
    }
    // If the length of s is more than 4, we truncate
    if (s.length() > 4) {
        s.erase(4);
    }
    return s;
}

/* This function takes a string (name), and returns the associated
 * Soundex code using all the helper functions defined above.
 */
string soundex(string s) {
    // We remove the non-letters
    string letters = lettersOnly(s);
    // We encode the letters
    string encoded = encoder(letters);
    // We coalesce adjacent duplicate digits
    string noDuplicates = coalescer(encoded);
    // We replace the first digit of the code with the first letter of the
    // original name, converting to uppercase
    string firstDigitUppercase = firstDigitToUppercase(noDuplicates, letters);
    // We discard any zeros from the code
    string noZeros = discardZeros(firstDigitUppercase);
    // We make the code exactly length 4 by padding with zeros or truncating
    // the excess
    string code = lengthFour(noZeros);
    return code;
}


/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        readEntireFile(in, allNames);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found. \n" << endl;

    // The names read from file are now stored in Vector allNames

    // Initializing the surname. The value is not empty to allow the while
    // loop to initiate.
    string surname = "Initial";

    // Each run of this loop corresponds to an interaction of the user with
    // the soundex search program, and the program returning the information
    // to the user.
    while (surname != "") {
        // Prompting user for a surname (return will exit the loop).
        surname = getLine("Enter a surname (RETURN to quit):");

        // If the name is empty, we do not run any search and skip
        // to exiting the loop
        if (surname != "") {
            // Compute soundex code of the surname
            string soundexCode = soundex(surname);

            // Printing the soundex code
            cout << "Soundex code is " << soundexCode << endl;

            // Initializing the results vector
            Vector<string> results;

            // Looping through the vector
            for (int i = 0; i < allNames.size(); i++) {
                // If a name is found that has the same soundex
                // code, add it to the Names vector.
                if (soundex(allNames[i]) == soundexCode) {
                    results.add(allNames[i]);
                }
            }

            // Sorting the vector
            results.sort();

            // Printing the results
            cout << "Matches from database: {";

            for (int i = 0; i < results.size(); i++) {
                cout << "\"" << results[i] << "\"";
                // Adding a coma if it is not the last result
                if (i != results.size() - 1) {
                    cout << ", ";
                }
            }
            cout << "} \n" << endl;
        }
    }

    // Now that the while loop is closed, we display the stop message
    cout << "All done!" << endl;
}


/* * * * * * Test Cases * * * * * */


PROVIDED_TEST("Test exclude of punctuation, digits, and spaces") {
    string s = "O'Hara";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "tldr");
}


PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}

// TODO: add your test cases here

STUDENT_TEST("Finding the bug for lettersOnly: empty input") {
    string s = "";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "");
}

STUDENT_TEST("Finding the bug for lettersOnly: input with only non-letters") {
    string s = "--_--";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "");
}

STUDENT_TEST("Finding the bug for lettersOnly: first char is a non-letter") {
    string s = "-Thomas";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "Thomas");
}

STUDENT_TEST("Testing the encoder function: all the letters of the alphabet (mix of upper- and lowercase)") {
    string s = "AbCdeFghiJKlmnOpQrSTuvwXyZ";
    string result = encoder(s);
    EXPECT_EQUAL(result, "01230120022455012623010202");
}

STUDENT_TEST("Testing the encoder function: empty input") {
    string s = "";
    string result = encoder(s);
    EXPECT_EQUAL(result, "");
}

STUDENT_TEST("Testing the coalescer function: empty input and one digit input") {
    string s = "";
    string result = coalescer(s);
    EXPECT_EQUAL(result, "");
    s = "1";
    result = coalescer(s);
    EXPECT_EQUAL(result, "1");
}

STUDENT_TEST("Testing the coalescer function: input is multiple times the same digit") {
    string s = "0000000";
    string result = coalescer(s);
    EXPECT_EQUAL(result, "0");
}

STUDENT_TEST("Testing the coalescer function: random inputs") {
    string s = "222025";
    string result = coalescer(s);
    EXPECT_EQUAL(result, "2025");
    s = "231002312458894";
    result = coalescer(s);
    EXPECT_EQUAL(result, "2310231245894");
}

STUDENT_TEST("Testing the firstDigitToUppercase function: empty inputs") {
    string s = "";
    string name = "";
    string result = firstDigitToUppercase(s, name);
    EXPECT_EQUAL(result, "");
}

STUDENT_TEST("Testing the firstDigitToUppercase function: single digit as code input") {
    string s = "8";
    string name = "pikachu";
    string result = firstDigitToUppercase(s, name);
    EXPECT_EQUAL(result, "P");
}

STUDENT_TEST("Testing the firstDigitToUppercase function: random inputs") {
    string s = "2305923";
    string name = "thomas";
    string result = firstDigitToUppercase(s, name);
    EXPECT_EQUAL(result, "T305923");
    s = "9523095";
    name = "FLORE";
    result = firstDigitToUppercase(s, name);
    EXPECT_EQUAL(result, "F523095");
}

STUDENT_TEST("Testing the discardZeros function: empty and zeros input") {
    string s = "";
    string result = discardZeros(s);
    EXPECT_EQUAL(result, "");
    s = "000000";
    result = discardZeros(s);
    EXPECT_EQUAL(result, "");
}

STUDENT_TEST("Testing the discardZeros function: random input") {
    string s = "23200340220";
    string result = discardZeros(s);
    EXPECT_EQUAL(result, "2323422");
    s = "0920093930";
    result = discardZeros(s);
    EXPECT_EQUAL(result, "929393");
}

STUDENT_TEST("Testing the lengthFour function: input with a length of 4") {
    string s = "T320";
    string result = lengthFour(s);
    EXPECT_EQUAL(result, "T320");
}

STUDENT_TEST("Testing the lengthFour function: input with a length > 4") {
    string s = "T3202341242";
    string result = lengthFour(s);
    EXPECT_EQUAL(result, "T320");
}

STUDENT_TEST("Testing the lengthFour function: input with a length < 4, empty and not empty") {
    string s = "T3";
    string result = lengthFour(s);
    EXPECT_EQUAL(result, "T300");
    s = "";
    result = lengthFour(s);
    EXPECT_EQUAL(result, "0000");
}

STUDENT_TEST("Testing the soundex function: two questions from homework") {
    string s = "Angelou";
    string result = soundex(s);
    EXPECT_EQUAL(result, "A524");
    s = "Thomas";
    result = soundex(s);
    EXPECT_EQUAL(result, "T520");
}

STUDENT_TEST("Testing the soundex function: empty input") {
    string s = "";
    string result = soundex(s);
    EXPECT_EQUAL(result, "0000");
}
