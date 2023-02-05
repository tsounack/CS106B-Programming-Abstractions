/*
 * File edited by Thomas Sounack
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "SimpleTest.h"
using namespace std;

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

/*
 * This function takes a GWindow, three GPoints and one int as argument, and returns
 * the total count of black triangles drawn while drawing a Sierpinski triangle of
 * the specified order using recursion.
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    // base case
    if (order == 0) {
        fillBlackTriangle(window, one, two, three);
        return 1;
    }

    // new coordinates (middle of each point)
    GPoint new_one = GPoint((one.x + two.x) / 2, (one.y + two.y) / 2);
    GPoint new_two = GPoint((two.x + three.x) / 2, (two.y + three.y) / 2);
    GPoint new_three = GPoint((three.x + one.x) / 2, (three.y + one.y) / 2);

    // the three calls correspond to the three triangles drawn in the existing one(s)
    return drawSierpinskiTriangle(window, one, new_one, new_three, order - 1) +
    drawSierpinskiTriangle(window, new_one, two, new_two, order - 1) +
    drawSierpinskiTriangle(window, new_three, new_two, three, order - 1);
}


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all your testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}

