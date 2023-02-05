#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "SimpleTest.h"  // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * This function is implemented iteratively.
 */
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    string result = "";

    while (! messageBits.isEmpty()) {
        EncodingTreeNode* cur = tree;

        // dequeue until a leaf is found, and store its char in result
        while (! cur->isLeaf()) {
            Bit bit = messageBits.dequeue();
            if (bit == 0) cur = cur->zero;
            if (bit == 1) cur = cur->one;
        }

        result += charToString(cur->ch);
    }

    return result;
}

/**
 * This function is a helper function for the unflattenTree function.
 * Using pass by reference for cur allows us to keep track of the tree.
 */
void unflattenTreeHelper(Queue<Bit>& treeShape, Queue<char>& treeLeaves,
                                                 EncodingTreeNode* &cur) {
    Bit bit = treeShape.dequeue();

    if (bit == 0) {
        cur = new EncodingTreeNode(treeLeaves.dequeue());
        return;
    }

    if (bit == 1) {
        cur = new EncodingTreeNode(nullptr, nullptr);
        unflattenTreeHelper(treeShape, treeLeaves, cur->zero);
        unflattenTreeHelper(treeShape, treeLeaves, cur->one);
    }
}


/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the queues are well-formed and represent
 * a valid encoding tree.
 *
 * Your implementation may change the queue parameters however you like. There
 * are no requirements about what they should look like after this function
 * returns.
 *
 */
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    EncodingTreeNode* front = nullptr;
    unflattenTreeHelper(treeShape, treeLeaves, front);
    return front;
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * deallocateTree call is required to prevent memory leaks.
 */
string decompress(EncodedData& data) {
    EncodingTreeNode* tree = unflattenTree(data.treeShape, data.treeLeaves);
    string result = decodeText(tree, data.messageBits);
    deallocateTree(tree);
    return result;
}

/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * It raises an error if there are not at least two distinct chars.
 */
EncodingTreeNode* buildHuffmanTree(string text) {
    Map<char, int> wordMap;
    PriorityQueue<EncodingTreeNode*> wordPQueue;

    // filling the map
    for (char ch : text) {
        wordMap[ch]++;
    }
    // filling the queue
    for (char ch : wordMap) {
        EncodingTreeNode* chLeaf = new EncodingTreeNode(ch);
        wordPQueue.enqueue(chLeaf, wordMap[ch]);
    }
    if (wordPQueue.size() < 2) {
        error("The input text does not have at least two distinct characters.");
    }

    // constructing the tree until there is one element left in the queue
    while (wordPQueue.size() > 1) {
        // leaf 1 and its priority
        int priority1 = wordPQueue.peekPriority();
        EncodingTreeNode* leaf1 = wordPQueue.dequeue();
        // leaf 2 and its priority
        int priority2 = wordPQueue.peekPriority();
        EncodingTreeNode* leaf2 = wordPQueue.dequeue();
        // creating new node and enqueuing it
        EncodingTreeNode* node = new EncodingTreeNode(leaf1, leaf2);
        wordPQueue.enqueue(node, priority1 + priority2);
    }

    // the last element is the tree
    return wordPQueue.dequeue();
}

/**
 * Helper function for the encodeText function. It uses recursion and traverses the tree
 * once to build a map that associates each character with its encoded bit sequence.
 */
void mapTree(EncodingTreeNode* tree, Map<char, Vector<Bit>> &map, Vector<Bit> bitPath) {
    if (tree->isLeaf()) {
        map[tree->getChar()] = bitPath;
        return;
    }

    mapTree(tree->zero, map, bitPath + 0);
    mapTree(tree->one,  map, bitPath + 1);
}


/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 *
 * This function uses iteration.
 */
Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    Map<char, Vector<Bit>> map;
    Vector<Bit> bitPath;
    mapTree(tree, map, bitPath);

    Queue<Bit> result;
    for (char ch : text) {
        for (Bit bit : map[ch]) {
            result.enqueue(bit);
        }
    }
    return result;
}

/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 *
 * This function uses recursion.
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    if (tree->isLeaf()) {
        treeShape.enqueue(0);
        treeLeaves.enqueue(tree->getChar());
        return;
    }

    treeShape.enqueue(1);
    flattenTree(tree->zero, treeShape, treeLeaves);
    flattenTree(tree->one,  treeShape, treeLeaves);
}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
EncodedData compress(string messageText) {
    // creating tree
    EncodingTreeNode* tree = buildHuffmanTree(messageText);

    // creating treeShape and treeLeaves queues
    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(tree, treeShape, treeLeaves);

    // creating messageBits queue
    Queue<Bit> messageBits = encodeText(tree, messageText);

    // removing the tree from memory
    deallocateTree(tree);

    return {treeShape, treeLeaves, messageBits};
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    EncodingTreeNode* r  = new EncodingTreeNode('R');
    EncodingTreeNode* s  = new EncodingTreeNode('S');
    EncodingTreeNode* n3 = new EncodingTreeNode(r, s);
    EncodingTreeNode* e  = new EncodingTreeNode('E');
    EncodingTreeNode* n2 = new EncodingTreeNode(n3, e);
    EncodingTreeNode* t  = new EncodingTreeNode('T');
    EncodingTreeNode* n1 = new EncodingTreeNode(t, n2);
    return n1;
}

void deallocateTree(EncodingTreeNode* t) {
    if (t == nullptr) return;

    deallocateTree(t->zero);
    deallocateTree(t->one);
    delete t;
}

bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    // if one is a nullptr and not the other: false
    if ((a == nullptr && b != nullptr) || (a != nullptr && b == nullptr)) {
        return false;
    }

    // if both are nullptr, the function will directly return true
    if (a != nullptr && b != nullptr) {

        // if both are leaves but don't have the same char: false
        if (a->isLeaf() && b->isLeaf()) {
            if (a->getChar() != b->getChar()) {
                return false;
            }
        }

        // if both are nodes, recursive call (until the leaves are reached)
        else if (!a->isLeaf() && !b->isLeaf()) {
            if (! areEqual(a->zero, b->zero)) return false;
            if (! areEqual(a->one, b->one))   return false;
        }

        // if one is a leaf and the other a node: false
        else {
            return false;
        }
    }

    return true;
}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("Testing deallocateTree") {
    EncodingTreeNode* tree = createExampleTree();
    deallocateTree(tree);
}

STUDENT_TEST("Testing areEqual") {
    EncodingTreeNode* tree1 = nullptr;
    EncodingTreeNode* tree2 = nullptr;
    EXPECT(areEqual(tree1, tree2));

    EncodingTreeNode* leaf1 = new EncodingTreeNode('A');
    EncodingTreeNode* leaf2 = new EncodingTreeNode('B');
    tree1 = new EncodingTreeNode(leaf1, leaf2);
    EXPECT(! areEqual(tree1, tree2));

    tree2 = tree1;
    EXPECT(areEqual(tree1, tree2));

    tree2 = createExampleTree();
    EXPECT(! areEqual(tree1, tree2));

    deallocateTree(tree1);

    tree1 = createExampleTree();
    EXPECT(areEqual(tree1, tree2));

    deallocateTree(tree1);
    deallocateTree(tree2);
}

STUDENT_TEST("Testing flattenTree") {
    EncodingTreeNode* tree = createExampleTree();
    Queue<Bit> treeShape;
    Queue<char> treeLeaves;
    flattenTree(tree, treeShape, treeLeaves);
    EncodingTreeNode* tree2 = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, tree2));
    deallocateTree(tree);
    deallocateTree(tree2);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman",
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
