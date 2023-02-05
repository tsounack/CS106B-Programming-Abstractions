// File edited by Thomas Sounack

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;



// This function takes a string as argument, and returns the clean
// version of it (alphanumerical only, letters in lowercase) using
// the alnum and tolower functions.
string cleanToken(string s) {
    string result = "";
    for(int i = 0; i < s.length(); i++) {
        if (isalnum(s[i])) {
            result += tolower(s[i]);
        }
    }
    return result;
}

// This function takes a string (text) as argument, and returns the
// set of unique tokens included in the string
Set<string> gatherTokens(string text) {
    Set<string> tokens;

    // We split the string using the spaces
    Vector<string> splitText = stringSplit(text, " ");

    for (string text : splitText) {
        string token = cleanToken(text);
        // We add the token if it is not empty
        if (token != "") {
            tokens.add(token);
        }
    }
    return tokens;
}

// This function takes a string (file) and a Map (index) as input,
// reads the file, fills the map as an inverted index and returns
// the number of url in the file.
int buildIndex(string dbfile, Map<string, Set<string>>& index) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines;
    readEntireFile(in, lines);

    int n = lines.size();
    int counter = 0;

    // By going in increments of 2, we can process both the urls
    // and the tokens at the same time.
    for (int i = 0; i < n; i += 2) {
        string url = lines[i];
        counter++;

        Set<string> tokens = gatherTokens(lines[i + 1]);

        for (string token : tokens) {
            index[token].add(url);
        }
    }
    return counter;
}

// This function takes a Map (the index) and a string (query) and uses operations
// to determine the correct output string (urls associated with the query) and
// return it.
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;

    // This splits the query using the spaces
    Vector<string> splitQuery = stringSplit(query, " ");

    for (string query : splitQuery) {

        // This cleans the tokens and separates them if needed
        for (string token : gatherTokens(query)) {

            // We search the affiliated urls
            Set<string> queryUrls = index.get(token);

            // If the first char is a +, we intersect
            if (startsWith(query, "+")) {
                result.intersect(queryUrls);
            }
            // If the first char is a -, we compute the difference
            else if (startsWith(query, "-")) {
                result.difference(queryUrls);
            }
            // Otherwise we compute the union
            else {
                result.unionWith(queryUrls);
            }
        }
    }
    return result;
}

// This function takes a string (file name of db) as argument and implements
// a console program that enables the user to request searches in the
// database.
void searchEngine(string dbfile) {

    cout << "Stand by while building index..." << endl;

    Map<string, Set<string>> index;

    // Building the index and computes the number of urls and words
    int nb_urls  = buildIndex(dbfile, index);
    int nb_words = index.size();

    cout << "Indexed " << nb_urls << " pages containing " << nb_words
         << " unique terms. \n" << endl;

    string query = "initial";
    while (query != "") {
        query = getLine("Enter query sentence (RETURN/ENTER to quit):");

        if (query != "") {
            Set<string> result = findQueryMatches(index, query);

            cout << "Found " << result.size() << " matching pages" << endl;
            cout << result << "\n" << endl;
        }
    }
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings of letters and digits") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS106B"), "cs106b");
}

PROVIDED_TEST("cleanToken on strings containing punctuation") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("they're"), "theyre");
}

PROVIDED_TEST("cleanToken on string of only punctuation") {
    EXPECT_EQUAL(cleanToken("#$^@@.;"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 6 unique tokens, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 12);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


STUDENT_TEST("cleanToken on empty string") {
    EXPECT_EQUAL(cleanToken(""), "");
}

STUDENT_TEST("cleanToken on special chars") {
    EXPECT_EQUAL(cleanToken(":./.&(!§)"), "");
}

STUDENT_TEST("cleanToken on string with letters, numbers and chars") {
    EXPECT_EQUAL(cleanToken("Th&o§mAS°:/"), "thomas");
}

STUDENT_TEST("gatherTokens from empty string") {
    Set<string> tokens = gatherTokens(" ");
    EXPECT_EQUAL(tokens.size(), 0);
}

STUDENT_TEST("gatherTokens from french string") {
    Set<string> tokens = gatherTokens("I love baguette and croissant <3");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("baguette"));
}

STUDENT_TEST("buildIndex from website.txt, 36 pages") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    EXPECT_EQUAL(nPages, 36);
    EXPECT(index.containsKey("explore"));
    EXPECT(index.containsKey("fundamentals"));
}

STUDENT_TEST("buildIndex from website.txt, time verification") {
    Map<string, Set<string>> index;
    TIME_OPERATION(1, buildIndex("res/website.txt", index));
}

STUDENT_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> stringNotInDB = findQueryMatches(index, "aozdinzaidn");
    EXPECT_EQUAL(stringNotInDB.size(), 0);
    Set<string> complexRequest = findQueryMatches(index, "one two +fish - milk");
    EXPECT_EQUAL(complexRequest.size(), 2);
    Set<string> oneWord = findQueryMatches(index, "R*ù%ed");
    EXPECT_EQUAL(oneWord.size(), 2);
    buildIndex("res/website.txt", index);
    Set<string> specialCharBetween = findQueryMatches(index, "expect_error");
    EXPECT_EQUAL(specialCharBetween.size(), 4);
}
