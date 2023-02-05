#include "listnode.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;

/*
 * This function is a helper function for runSort. It takes a pointer to a
 * list node as argument. The function then iteratively goes through all nodes
 * and if a node is found to be out of order, it is disconnected from the link.
 * The function returns the pointer to this new front node (nullptr if none is
 * found).
 */
ListNode* splitRun(ListNode* front) {
    ListNode* previous = front;
    ListNode* newFront = nullptr;

    for (ListNode *cur = front->next; cur != nullptr; cur = cur->next) {
        // if a node is out of order, we break it off from the link,
        // set the newFront as its pointer and break out of the for loop
        if (previous->data > cur->data) {
            previous->next = nullptr;
            newFront = cur;
            break;
        }
        previous = cur;
    }

    return newFront;
}

/*
 * This function is a helper function for the merge function.
 * It takes three listnode pointers as input, using them to
 * add a new element to a results list. It returns nothing.
 */
void addTail(ListNode* &front, ListNode* &tail, ListNode* elem) {
    // if the results list is empty
    if (front == nullptr) {
        front = elem;
        tail  = front;
    }
    else {
        tail->next = elem;
        tail = tail->next;
    }
}

/*
 * This function is a helper function for the runSort function.
 * It takes two listnode pointers as input, merging the associated
 * lists in a sorted list which it returns as a listnode pointer.
 */
ListNode* merge(ListNode* front1, ListNode* front2) {
    ListNode* results = nullptr;
    ListNode* tail    = nullptr;
    ListNode* cur1    = front1;
    ListNode* cur2    = front2;
    ListNode* nxtpointer;

    // add elements until both lists are 'empty'
    while (cur1 != nullptr || cur2 != nullptr) {
        // both lists are full
        if (cur1 != nullptr && cur2 != nullptr) {
            // ternary operator to point to the next element to add
            nxtpointer = (cur1->data < cur2->data) ? cur1 : cur2;
        }
        // one list is empty
        else {
            nxtpointer = (cur1 != nullptr) ? cur1 : cur2;
        }
        addTail(results, tail, nxtpointer);

        // 'increment' cur1 or cur2
        if (nxtpointer == cur1) {
            cur1 = cur1->next;
        }
        else {
            cur2 = cur2->next;
        }
    }
    return results;
}


/*
 * This function takes a listnode pointer as argument, and
 * sorts the associated list. It returns nothing.
 */
void runSort(ListNode* &front) {
    ListNode* results = nullptr;
    ListNode* front2;

    while (front != nullptr) {
        // front2 is the remainder
        front2 = splitRun(front);
        results = merge(results, front);
        front = front2;
    }
    front = results;
}


/*
 * This function is a helper function for quickSort. It takes four
 * listnodes as input (all by reference), and sorts the first list
 * in the three other lists. It returns nothing.
 */
void partition(ListNode* &front, ListNode* &equal,
               ListNode* &less, ListNode* &greater) {
    ListNode* pivot = front;
    ListNode* nextLink;
    equal = nullptr;
    less = nullptr;
    greater = nullptr;

    if (front == nullptr) {
        return;
    }

    // Not exactly conventional to modify the for-expression in the loop,
    // but assigning it at the beginning of the loop allows us to keep
    // track of the elements
    for (ListNode *cur = front; cur != nullptr; cur = nextLink) {
        nextLink = cur->next;
        if (cur->data == pivot->data) {
            cur->next = equal;
            equal = cur;
        }
        else if (cur->data < pivot->data) {
            cur->next = less;
            less = cur;
        }
        else {
            cur->next = greater;
            greater = cur;
        }
    }
    // This ensures that front is now empty (otherwise the first element
    // would remain). I am not sure if this is a requirement for this
    // assignment since there is no duplication of objects and thus no
    // memory loss, but I'm adding it in case the auto tests take this
    // in account.
    front = nullptr;
}


/*
 * This function is a helper function for quickSort. It takes two
 * listnodes as argument (one by reference), and appends the second
 * list to the first list. It returns nothing.
 */
void concatenate(ListNode* &front1, ListNode* front2) {
    // if list 1 is empty, point to list2.
    if (front1 == nullptr) {
        front1 = front2;
        return;
    }

    ListNode* end = front1;
    while (end->next != nullptr) {
        end = end->next;
    }
    // Once the end of list 1 is reached, make its end
    // point to list 2.
    end->next = front2;
}


/*
 * This function takes a listnode as argument and sorts the
 * corresponding list using recursivity. It returns nothing.
 */
void quickSort(ListNode*& front) {
    // base case: 0 and 1 element. They can be put together
    // because if front is nullptr, the compiler will only
    // read the first condition (and not the second which would
    // cause an error).
    if (front == nullptr || front->next == nullptr) {
        return;
    }

    ListNode* equal;
    ListNode* less;
    ListNode* greater;

    partition(front, equal, less, greater);

    // recursive calls
    quickSort(less);
    quickSort(greater);

    // concatenate results
    concatenate(less, equal);
    concatenate(less, greater);
    front = less;
}


/* * * * * * Test Code Below This Point * * * * * */

/*
 * We have provided this utility function as a convenience. This
 * function traverses the linked list represented by the provided
 * front pointer and prints out the data stored in the list along
 * the way in a convenient format. You may find it to be helpful to
 * use this function as a debugging aid.
 */
void printList(ListNode* front) {
   cout << "{";
   for (ListNode *cur = front; cur != nullptr; cur = cur->next) {
       cout << cur->data;
       if (cur->next != nullptr) {
           cout << ", ";
       }
   }
   cout << "}" << endl;
}

/*
 * This utility function deallocates the memory for all the nodes in a
 * given linked list. It can be used to recycle the memory allocated
 * during a test case and avoid memory leaks.
 */
void deallocateList(ListNode* front) {
    // If the list is empty, nothing needs to be deleted
    if (front == nullptr) {
        return;
    }

    ListNode* cur = front;
    for (ListNode* next = front->next; next != nullptr; next = next->next) {
        delete cur;
        cur = next;
    }
    // if there is only one link (front), this deletes it.
    // if there are multiple links, this deletes the last one.
    delete cur;
}

/*
 * This utility function is given a vector of values and constructs
 * an equivalent linked list containing the same values in the same order.
 * It can be used to construct a linked list to use as an input to
 * a function being tested.
 */
ListNode* createList(Vector<int> values) {
    int n = values.size();
    // if values is an empty vector
    if (n == 0) {
        return nullptr;
    }

    ListNode* front = new ListNode(values[0], nullptr);
    ListNode* previous = front;

    // for each value, we create a new node and link it to the previous one
    for (int i = 1; i < n; i++) {
        ListNode* current = new ListNode(values[i], nullptr);
        previous->next = current;
        previous = current;
    }
    return front;
}

/*
 * This utility function compares a linked list and a vector for
 * equivalence and returns true if both contain the same values in the
 * same order. It can be used to confirm a linked list's contents match
 * the expected.
 */
bool areEquivalent(ListNode* front, Vector<int> v) {
    int n = v.size();
    ListNode* current = front;

    for (int i = 1; i <= n; i++) {
        int data = current->data;
        // test if the data is not the same
        if (data != v[i - 1]) {
            return false;
        }

        // test if current's next point is nullptr while the end of the
        // list is not reached yet
        if (current->next == nullptr && i != n) {
            return false;
        }
        current = current->next;
    }

    // test if all the list has been looped through but there are still links
    if (current != nullptr) {
        return false;
    }

    return true;
}

STUDENT_TEST("Testing splitRun") {
    // Manually construct testList 3 -> 5 -> 5 -> 4 ->10 -> 4
    Vector<int> values = {3, 5, 5, 4, 10, 4};
    ListNode* testList = createList(values);

    ListNode* testList2 = splitRun(testList);
    ListNode* testList3 = splitRun(testList2);
    ListNode* testList4 = splitRun(testList3);


    EXPECT(areEquivalent(testList, {3, 5, 5}));
    EXPECT(areEquivalent(testList2, {4, 10}));
    EXPECT(areEquivalent(testList3, {4}));
    EXPECT_EQUAL(testList4, nullptr);

    deallocateList(testList);
    deallocateList(testList2);
    deallocateList(testList3);
}

STUDENT_TEST("Testing merge") {
    Vector<int> values = {2, 4, 5};
    ListNode* testList1 = createList(values);

    ListNode* testList2 = new ListNode(1, nullptr);

    ListNode* merged = merge(testList1, testList2);

    EXPECT(areEquivalent(merged, {1, 2, 4, 5}));

    deallocateList(merged);

    values = {2, 4, 5};
    testList1 = createList(values);
    testList2 = nullptr;

    merged = merge(testList1, testList2);

    EXPECT(areEquivalent(merged, {2, 4, 5}));

    deallocateList(merged);
}

STUDENT_TEST("Testing runSort") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {4, 3, 2, 1};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);

    values = {};
    list = createList(values);
    runSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {1};
    list = createList(values);
    runSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {37, 53, 21, 1, 0, 23, 444, 100};
    list = createList(values);
    runSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {3, 9, 10, 0, 1, 88, 32, 12};
    list = createList(values);
    runSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {-4, 8, 9, 0, -1, -100, 33};
    list = createList(values);
    runSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Testing partition") {
    // Manually construct testList 7 -> 5 -> 5 -> 4 ->10 -> 4
    Vector<int> values = {7, 5, 5, 4, 10, 4};
    ListNode* testList = createList(values);

    ListNode* testList2 = nullptr;
    ListNode* testList3 = nullptr;
    ListNode* testList4 = nullptr;

    partition(testList, testList2, testList3, testList4);

    EXPECT(areEquivalent(testList, {}));
    EXPECT(areEquivalent(testList2, {7}));
    EXPECT(areEquivalent(testList3, {4, 4, 5, 5}));
    EXPECT(areEquivalent(testList4, {10}));

    deallocateList(testList2);
    deallocateList(testList3);
    deallocateList(testList4);


    values = {6, 6, 6};
    testList = createList(values);
    partition(testList, testList2, testList3, testList4);
    EXPECT(areEquivalent(testList, {}));
    EXPECT(areEquivalent(testList2, {6, 6, 6}));
    EXPECT(areEquivalent(testList3, {}));
    EXPECT(areEquivalent(testList4, {}));
    deallocateList(testList2);
    deallocateList(testList3);
    deallocateList(testList4);

    values = {};
    testList = createList(values);
    partition(testList, testList2, testList3, testList4);
    EXPECT(areEquivalent(testList, {}));
    EXPECT(areEquivalent(testList2, {}));
    EXPECT(areEquivalent(testList3, {}));
    EXPECT(areEquivalent(testList4, {}));
    deallocateList(testList2);
    deallocateList(testList3);
    deallocateList(testList4);

    values = {1};
    testList = createList(values);
    partition(testList, testList2, testList3, testList4);
    EXPECT(areEquivalent(testList, {}));
    EXPECT(areEquivalent(testList2, {1}));
    EXPECT(areEquivalent(testList3, {}));
    EXPECT(areEquivalent(testList4, {}));
    deallocateList(testList2);
    deallocateList(testList3);
    deallocateList(testList4);
}

STUDENT_TEST("Testing concatenate") {
    // Manually construct testList 7 -> 5 -> 5 -> 4 ->10 -> 4
    Vector<int> values = {7, 5, 5, 4, 10, 4};
    Vector<int> values2 = {2, 4, 8};
    ListNode* testList = createList(values);
    ListNode* testList2 = createList(values2);

    concatenate(testList, testList2);

    EXPECT(areEquivalent(testList, {7, 5, 5, 4, 10, 4, 2, 4, 8}));
    deallocateList(testList);


    values = {7, 5, 5, 4, 10, 4};
    values2 = {};
    testList = createList(values);
    testList2 = createList(values2);
    concatenate(testList, testList2);
    EXPECT(areEquivalent(testList, {7, 5, 5, 4, 10, 4}));
    deallocateList(testList);

    values = {};
    values2 = {7, 5, 5, 4, 10, 4};
    testList = createList(values);
    testList2 = createList(values2);
    concatenate(testList, testList2);
    EXPECT(areEquivalent(testList, {7, 5, 5, 4, 10, 4}));
    deallocateList(testList);

    values = {};
    values2 = {};
    testList = createList(values);
    testList2 = createList(values2);
    concatenate(testList, testList2);
    EXPECT(areEquivalent(testList, {}));
    deallocateList(testList);
}

STUDENT_TEST("Testing quickSort") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1, 2, 3, 4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);


    values = {};
    list = createList(values);
    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {1};
    list = createList(values);
    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {4, 3, 2, 1};
    list = createList(values);
    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {3, 9, 10, 0, 1, 88, 32, 12};
    list = createList(values);
    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {-4, 8, 9, 0, -1, -100, 33};
    list = createList(values);
    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);

    values = {4, 4};
    list = createList(values);
    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Time trial runsort") {
    int startSize = 1000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }
        TIME_OPERATION(n, runSort(list));
        deallocateList(list);
    }
}

STUDENT_TEST("Time trial runsort best") {
    int startSize = 10000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = i;
            list = new ListNode(v[i], list);
        }
        TIME_OPERATION(n, runSort(list));
        deallocateList(list);
    }
}

STUDENT_TEST("Time trial quickSort") {
    int startSize = 10000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }
        TIME_OPERATION(n, quickSort(list));
        deallocateList(list);
    }
}

STUDENT_TEST("Time trial quickSort worse") {
    int startSize = 1000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = 0; i <= n-1; i++) {
            v[i] = i;
            list = new ListNode(v[i], list);
        }
        TIME_OPERATION(n, quickSort(list));
        deallocateList(list);
    }
}

STUDENT_TEST("This is a skeleton to demonstrate an end-to-end student test.") {
    /* Initialize vector to sequence of values */
    Vector<int> values = {1, 2, 3, 4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    runSort(list);

    /* Sort the equivalent vector */
    values.sort();

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

PROVIDED_TEST("Confirm correctness of test utility functions.") {
    /* Manually construct testList 1 -> 2 -> 3 */
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);

    /* Test equality comparison function. */
    EXPECT(areEquivalent(testList, {1, 2, 3}));
    EXPECT(!areEquivalent(testList, {1, 2}));
    EXPECT(!areEquivalent(testList, {1, 2, 3, 4}));
    EXPECT(!areEquivalent(testList, {1, 3, 2}));

    /* Test create list function. */
    ListNode* studentList = createList({1, 2, 3, 4});
    printList(studentList);
    ListNode* cur = studentList;
    for (int i = 1; i <= 4; i++) {
        EXPECT(cur != nullptr);
        EXPECT_EQUAL(cur->data, i);
        cur = cur->next;
    }

    /* Test deallocate list functions. There should be no memory leaks from this test. */
    deallocateList(studentList);
    deallocateList(testList);
}

/*
 * The provided test case demonstrates a comparsion between the same
 * algorithm (quicksort) operating on a linked list versus a vector.
 */
PROVIDED_TEST("Time linked list quicksort vs vector quicksort") {
    int startSize = 50000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }

        /* NOTE: This test does not check correctness of the linked list sort
         * function. It only times the two operations to compare relative speed. */
        TIME_OPERATION(n, quickSort(list));
        TIME_OPERATION(n, v.sort());        /* Standard vector sort operation is backed
                                               with quicksort algorithm. */

        deallocateList(list);
    }
}

