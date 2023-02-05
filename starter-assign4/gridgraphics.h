#pragma once
#include "grid.h"

/**
 * The initForPopulationGrid function takes a Grid parameter representing
 * a population grid.  Call initForPopulationGrid once before starting
 * redistrict to configure the starting state of the animation.
 * The initial drawing is a grid of gray numbers on a white background.
 *
 * The optional pauseForClick parameter allows you to configure the
 * animation to pause after each redraw and require a click from the user
 * to continue. If not specified, pauseForClick defaults to false, indicating
 * the animation should proceed without pausing.
 */
void initForPopulationGrid(Grid<int>& grid, bool pauseForClick = false);


/**
 * The drawChosenRanges function takes a population grid (same grid as
 * earlier configured in call to initForPopulationGrid) and two GridLocationRanges.
 * The two ranges are drawn with a thick white border and filled with a
 * randomly-chosen color.
 *
 * If the animation has been configured to pauseForClick, animation will
 * pause after drawing these two ranges.
 *
 * It is an error to specify a range which is out of bounds for the grid.
 */
void drawChosenRanges(Grid<int>& grid, GridLocationRange range1, GridLocationRange range2);

/**
 * The undoChosenRanges function takes a population grid (the same one that
 * was earlier configured with initForPopulationGrid) and two GridLocationRanges.
 * The two ranges are removed from the drawing.
 *
 * It is an error to specify a range which is out of bounds for the grid.
 * or to attempt to undo a range that was not previously drawn via
 * `drawChosenRanges`.
 */
void undoChosenRanges(Grid<int>& grid, GridLocationRange range1, GridLocationRange range2);

