#include <iostream>
#include "console.h"
#include "sorting.h"
#include "labyrinth.h"
#include "SimpleTest.h"
using namespace std;

/*
 * You are free to edit the main in any way that works
 * for your testing/debugging purposes.
 * We will supply our own main() during grading.
 */
int main() {
    if (runSimpleTests(SELECTED_TESTS)) {
        return 0;
    }
    cout << "All done, exiting" << endl;
    return 0;
}


// Do not remove or edit below this line. It is here to confirm that your code
// conforms to the expected function prototypes needed for grading
void confirmFunctionPrototypes() {
    ListNode* p = nullptr;
    runSort(p);
    quickSort(p);
    MazeCell* m = nullptr;
    isPathToFreedom(m, "", {});
}
