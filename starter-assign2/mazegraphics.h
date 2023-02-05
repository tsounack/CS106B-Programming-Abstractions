/**
 * File: mazegraphics.h
 * --------------------
 * @author Julie Zelenski, Fall 2022
 * Defines functions to draw a maze in graphics window.
 */

#pragma once
#include <string>
#include "grid.h"


/**
 * The drawMaze function takes a Grid parameter and displays as a
 * "maze" of corridors and walls.  A grid location that is true is
 * an open corridor (drawn in white), false is a wall (drawn in dark gray).
 */
void drawMaze(const Grid<bool>& g);


/**
 * The highlightPath function takes a path (Vector of GridLocation) and
 * marks the path on the maze by drawing a filled dot on each location.
 * The fill color of the dots is set by the color parameter.  Color can
 * be a standard name such as "red", "blue", "green" or a hex color string "#33ff33"
 *
 * highlightPath expects to mark path locations on top of an already
 * drawn maze. It is an error to call highlightPath without having previously
 * called the drawMaze function.
 *
 * The optional third argument is the number of milliseconds to pause
 * after drawing. This can be used to slow the animation so you can watch
 * while trying to debug.
 */
void highlightPath(Vector<GridLocation>& p, std::string color, int msecsToPause = 0);


/**
 * The printMaze function can optionally be used to output a text version
 * of the maze for debugging or accessbility.
 * First call drawMaze and highlightPath to draw maze in graphics window, then
 * follow with a call to printMaze to output the corresponding text version.
 * In the text form, the character '@' is used for wall, space for corridor,
 * and '+' marks highlighted locations on the path.
 *
 * Example output:
 *                     +  @  @     @  @
 *                     +  +  +  +  +  @
 *                     @  @  @  @  +  @
 *                                 +  +
 */
void printMaze();

