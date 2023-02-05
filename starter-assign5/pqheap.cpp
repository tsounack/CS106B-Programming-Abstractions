#include "pqheap.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "SimpleTest.h"
using namespace std;

const int INITIAL_CAPACITY = 10;
const int NONE = -1; // used as sentinel index

/*
 * The constructor initializes all of the member variables needed for
 * an instance of the PQHeap class. The allocated capacity
 * is initialized to a starting constant and a dynamic array of that
 * size is allocated. The count of filled slots is initially zero.
 */
PQHeap::PQHeap() {
    _numAllocated = INITIAL_CAPACITY;
    _elements = new DataPoint[_numAllocated](); // add parens to zero new memory
    _numFilled = 0;
}

/* The destructor is responsible for cleaning up any resources
 * used by this instance of the PQHeap class. The array
 * memory that was allocated for the PQHeap is deleted here.
 */
PQHeap::~PQHeap() {
    delete[] _elements;
}

/*
 * This private helper function moves the new element to
 * the right place in the array. It takes and returns no
 * argument.
 */
void PQHeap::bubbleUp() {
    // if the queue has one element, it is already sorted
    if (size() == 1) {
        return;
    }

    int ind_new    = size() - 1;
    int ind_parent = getParentIndex(ind_new);

    while (_elements[ind_new].priority <
           _elements[ind_parent].priority
           && ind_new != 0) {
        swapElements(ind_new, ind_parent);

        // It does not matter if there is no new parent:
        // ind_parent would just be NONE, and we would
        // exit the while loop right after.
        ind_new    = ind_parent;
        ind_parent = getParentIndex(ind_new);
    }
}

/*
 * This private helper function creates a new array twice
 * as large as the previous one and moves the data from the
 * old array to this new one. It takes and returns no argument.
 */
void PQHeap::enlarge() {
    // Creating new heap
    _numAllocated *= 2;
    DataPoint* elements2  = new DataPoint[_numAllocated]();

    // Copying the data in the new array
    for (int i = 0; i < size(); i++) {
        elements2[i] = _elements[i];
    }

    // Deleting old array
    delete[] _elements;
    _elements = elements2;
}

/*
 * This function takes a DataPoint elem as input, and adds
 * it to the heap in the right position. It resizes the
 * array if it is filled. This function returns nothing.
 */
void PQHeap::enqueue(DataPoint elem) {
    // If the array is filled, enlarge the array
    if (_numAllocated == _numFilled) {
        enlarge();
    }

    // Insert element onto the end of the array
    _elements[_numFilled] = elem;
    _numFilled++;

    bubbleUp();
}

/*
 * The element at index 0 is the most urgent (smallest priority value)
 * and the element in the last-filled index is the least urgent
 * (largest priority value). peek returns the first element (most
 * urgent, minimum priority value).
 */
DataPoint PQHeap::peek() const {
    if (isEmpty()) {
        error("Cannot access front element of empty heap!");
    }
    return _elements[0];
}

/*
 * This function is a helper for the trickleDown function. It takes
 * two ints (indexes of child and current) as argument. If the child
 * has a lower priority, it is swapped with current and its index is
 * returned (as an int) to be used as the new current. Otherwise
 * nothing is done and current's value is returned.
 */
int PQHeap::switchParentChild(int child, int current) {
    if (_elements[child].priority < _elements[current].priority) {
        swapElements(current, child);
        return child;
    }
    return current;
}

/*
 * This function is a helper for the enqueue function. It takes
 * no arguments and uses the trickle down algorithm to reorganize
 * the heap once an element is enqueued. It returns nothing.
 */
void PQHeap::trickleDown() {
    int current = 0;
    swapElements(current, size() - 1);
    _numFilled--;

    int childLeft  = getLeftChildIndex(current);
    int childRight = getRightChildIndex(current);

    // If both children exist, we can start trickling down
    while (childLeft != NONE && childRight != NONE) {
        int bestChild;
        // Ternary operator to find best child
        _elements[childLeft].priority < _elements[childRight].priority ?
                    bestChild = childLeft : bestChild = childRight;

        // switching with the best child if the priority is lower,
        // and assigning current to the corresponding index
        int newCurrent = switchParentChild(bestChild, current);

        // if the value does not change, the trickle down is over and
        // we break out of the while loop
        if (newCurrent == current) {
            break;
        }
        current = newCurrent;

        // recomputing the children's indexes
        childLeft  = getLeftChildIndex(current);
        childRight = getRightChildIndex(current);
    }

    // If there is no child left, we exit the function
    if (childLeft == NONE && childRight == NONE) {
        return;
    }

    // Otherwise swap if there is only one child and its priority is smaller
    if (childLeft == NONE) {
        current = switchParentChild(childRight, current);
    }
    else if (childRight == NONE) {
        current = switchParentChild(childLeft, current);
    }
}

/*
 * This function returns the value of the most urgent element and removes
 * it from the heap. It then uses a trickle down algorithm to rebalance
 * the heap.
 */
DataPoint PQHeap::dequeue() {
    DataPoint urgent = peek();

    trickleDown();

    return urgent;
}

/*
 * Returns true if the queue contains no elements, false otherwise
 */
bool PQHeap::isEmpty() const {
    return size() == 0;
}

/*
 * The count of enqueued elements is tracked in the
 * member variable _numFilled.
 */
int PQHeap::size() const {
    return _numFilled;
}

/*
 * Updates internal state to reflect that the queue is empty, i.e. count
 * of filled slots is reset to zero. The array memory stays allocated
 * at current capacity. The entries in the element array do not need to
 * be cleared; those slots will be overwritten when additional values
 * are enqueued.
 */
void PQHeap::clear() {
    _numFilled = 0;
}

/*
 * Prints the contents of internal array for debugging purposes.
 */
void PQHeap::printDebugInfo(string msg) const {
    cout << msg << endl;
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << _elements[i] << endl;
    }
}

/*
 * We strongly recommend implementing this helper function, which
 * should traverse the heap array and ensure that the heap property
 * holds for all elements in the array. If elements are found that
 * violate the heap property, an error should be thrown.
 */
void PQHeap::validateInternalState() const {
    /*
     * If there are more elements than spots in the array, we have a problem.
     */
    if (_numFilled > _numAllocated) error("Too many elements in not enough space!");

    /* Loop over the array and compare priority of parent and children.
     * If a children has larger priority than its parent, report the problem
     * using error.
     */
    for (int i = 1; i < size(); i++) {
        int parentIndex = getParentIndex(i);

        // Verify if the order is respected
        if (_elements[parentIndex].priority > _elements[i].priority) {
            printDebugInfo("validateInternalState");
            error("PQArray has elements out of order at index " + integerToString(i));
        }
    }
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the parent of the
 * specified child index. If this child has no parent, return
 * the sentinel value NONE.
 */
int PQHeap::getParentIndex(int child) const {
    if (child == 0) {
        return NONE;
    }

    return (child - 1) / 2;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the left child of the
 * specified parent index. If this parent has no left child, return
 * the sentinel value NONE.
 */
int PQHeap::getLeftChildIndex(int parent) const {
    int index = 2 * parent + 1;

    if (index >= _numFilled) {
        return NONE;
    }

    return index;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the right child of the
 * specified parent index. If this parent has no right child, return
 * the sentinel value NONE.
 */
int PQHeap::getRightChildIndex(int parent) const {
    int index = 2 * parent + 2;

    if (index >= _numFilled) {
        return NONE;
    }

    return index;
}

/*
 * We strongly recommend implementing this helper function that swaps
 * array elements. There is a swapElements member function implemented
 * in PQAarray that you start from. Be sure to include error bounds-checking
 * it is an invaluable defense against indexing mistakes!
 */
void PQHeap::swapElements(int indexA, int indexB) {
    DataPoint tmp = _elements[indexA];
    _elements[indexA] = _elements[indexB];
    _elements[indexB] = tmp;
}


/* * * * * * Test Cases Below This Point * * * * * */

// This is testing the global implementation of PQHeap
STUDENT_TEST("PQHeap example from writeup") {
    PQHeap pq;

    pq.enqueue( { "Zoe", -3 } );
    pq.enqueue( { "Elmo", 10 } );
    pq.enqueue( { "Bert", 6 } );
    EXPECT_EQUAL(pq.size(), 3);
    pq.printDebugInfo("After enqueue 3 elements");

    pq.enqueue( { "Kermit", 5 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.printDebugInfo("After enqueue one more");

    DataPoint removed = pq.dequeue();
    DataPoint expected = { "Zoe", -3 };
    EXPECT_EQUAL(removed, expected);
    pq.printDebugInfo("After dequeue one");
}

// This ensures that the enqueue function works correctly
STUDENT_TEST("PQHeap, enqueue only, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "e", 2.718 });
    pq.validateInternalState();
    pq.enqueue({ "pi", 3.14 });
    pq.validateInternalState();
    pq.enqueue({ "phi", 1.618 });
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 3);
}

// This ensures that the size, clear and isEmpty functions work
STUDENT_TEST("PQHeap: operations size/isEmpty/clear") {
    PQHeap pq;

    EXPECT(pq.isEmpty());
    pq.clear();
    EXPECT_EQUAL(pq.isEmpty(), pq.size() == 0);
    pq.enqueue({ "", 7 });
    EXPECT_EQUAL(pq.size(), 1);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 2);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 3);
    pq.clear();
    pq.validateInternalState();
    EXPECT(pq.isEmpty());
    EXPECT_EQUAL(pq.size(), 0);
}

// This ensures that dequeue and peek work correctly when using an
// empty queue
STUDENT_TEST("PQHeap: dequeue or peek on empty pqueue raises error") {
    PQHeap pq;
    DataPoint point = { "Programming Abstractions", 106 };

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.dequeue();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.clear();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

// This ensures that the dequeue function (and thus the trickle down
// algorithm) does not cause errors.
STUDENT_TEST("PQHeap, dequeue, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "e", 2.718 });
    pq.enqueue({ "pi", 3.14 });
    pq.enqueue({ "phi", 1.618 });

    for (int i = 0; i < 3; i++) {
        pq.dequeue();
        pq.validateInternalState();
    }
}

// This ensures that the Heap is able to enlarge itself correctly
STUDENT_TEST("PQHeap, test enlarge array memory") {
    for (int size = 5; size <= 500; size *= 5) {
        PQHeap pq;

        for (int i = 1; i <= size; i++) {
            pq.enqueue({"", double(i) });
        }
        pq.validateInternalState();


        for (int i = 1; i <= size; i++) {
            DataPoint expected = {"", double(i) };
            EXPECT_EQUAL(pq.dequeue(), expected);
        }
    }
}

// This ensures that the overall structures works well by combining the
// use of enqueues and dequeues
STUDENT_TEST("PQHeap, sequence of mixed operations") {
    PQHeap pq;
    int size = 30;
    double val = 0;

    for (int i = 0; i < size; i++) {
        pq.enqueue({"", --val });
    }
    val = 0;
    for (int i = 0; i < pq.size(); i++) {
        DataPoint front = pq.peek();
        EXPECT_EQUAL(pq.dequeue(), front);
        pq.enqueue( { "", ++val });
    }
    EXPECT_EQUAL(pq.size(), size);
    val = 0;
    while (!pq.isEmpty()) {
        DataPoint expected = { "", ++val };
        EXPECT_EQUAL(pq.dequeue(), expected);
    }
}

// This is a stress test ensuring that the structures behaves well under heavy load.
STUDENT_TEST("PQHeap stress test, cycle many random elements in and out") {
    PQHeap pq;
    int n = 0, maxEnqueues = 1000;
    double sumEnqueued = 0, sumDequeued = 0;

    setRandomSeed(42); // make test behavior deterministic

    DataPoint mostUrgent = { "", 0 };
    pq.enqueue(mostUrgent);
    while (true) {
        if (++n < maxEnqueues && randomChance(0.9)) {
            DataPoint elem = { "", randomInteger(-10, 10) + 0.5 };
            if (elem.priority < mostUrgent.priority) {
                mostUrgent = elem;
            }
            sumEnqueued += elem.priority;
            pq.enqueue(elem);
        } else {
            DataPoint elem = pq.dequeue();
            sumDequeued += elem.priority;
            EXPECT_EQUAL(elem, mostUrgent);
            if (pq.isEmpty()) break;
            mostUrgent = pq.peek();
        }
    }
    EXPECT_EQUAL(sumEnqueued, sumDequeued);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("PQHeap example from writeup, validate each step") {
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }, { "A", 5 }, { "B", 3 }, { "K", 7 }, { "G", 2 },
        { "V", 9 }, { "T", 1 }, { "O", 8 }, { "S", 6 } };

    pq.validateInternalState();
    for (DataPoint dp : input) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}
