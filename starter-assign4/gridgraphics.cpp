/*
 * File: gridgraphics.cpp
 * ----------------------
 * @author Julie Zelenski, Fall 2022
 * Implementation of the graphics support routines. You are
 * welcome to read over this code, but you will not need to edit it.
 */

#include "gridgraphics.h"
#include "gbutton.h"
#include "gcolor.h"
#include "gfont.h"
#include "gwindow.h"
#include "strlib.h"
#include "vector.h"
#include <iomanip>
using namespace std;

static bool gInitialized;
static GWindow *gWindow;
static GButton *gButton;
static int gCellSize;
static string gFontDesc;
static Grid<int> gGrid;
static bool gPauseForClick;

struct GraphicalRange {
    GridLocationRange range;
    GRectangle bounds;
    string color;
};
static Vector<GraphicalRange> gChosen;

static void initialize() {
    gWindow = new GWindow(1, 1);
    gWindow->setVisible(false);
    gWindow->setWindowTitle("Redistrict");
    gWindow->setRepaintImmediately(false);
    gWindow->setAutoRepaint(false);
    gWindow->setExitOnClose(false);
    gWindow->setLocation(10, 10);
    gWindow->setResizable(false);
    gWindow->setBackground("White");
    gButton = new GButton("Paused, click to continue");
    gButton->setActionListener([]() { gButton->setVisible(false);});
    gWindow->addToRegion(gButton, GWindow::REGION_SOUTH);
    gButton->setVisible(false);
    gInitialized = true;
}

void pauseForClick() {
    if (!gInitialized) return;
    gButton->setVisible(true);
    while (gButton->isVisible()) pause(100);
}

static GDimension getTextBounds(string text, string font) {
    gWindow->setFont(font);
    QFontMetrics metrics(GFont::toQFont(font));
    return GDimension { double(metrics.horizontalAdvance(QString::fromStdString(text))), double(metrics.ascent()) };
}

// This goopy piece of code is needed to account for platform differences in
// how fonts are sized. Pay no attention.
static void configureFontSize(double width, double height, string text) {
    GDimension bounds;
    int fontSize = 1;
    do {
        gFontDesc = "Helvetica-" + integerToString(++fontSize);
        bounds = getTextBounds(text, gFontDesc);
    } while (bounds.height < height/2 && bounds.width < width/2);
}

static void changeDimensions(int numRows, int numCols) {
    static const int kMinWindowSize = 240, kMaxWindowSize = 720;
    static const int kDefaultCellSize = 70;

    if (!gInitialized) initialize();
    gWindow->setVisible(false);
    gCellSize = kDefaultCellSize;
    int minDimension = min(numRows, numCols);
    int maxDimension = max(numRows, numCols);
    gCellSize = min(max(kDefaultCellSize, kMinWindowSize/minDimension), kMaxWindowSize/maxDimension);
    gWindow->setCanvasSize(numCols*gCellSize, numRows*gCellSize + 30); // 30 for button in south
    configureFontSize(gCellSize, gCellSize, "##");
    gWindow->setVisible(true);
}

static GRectangle rectForRange(GridLocationRange range) {
    int numCols = range.endCol() - range.startCol() + 1;
    int numRows = range.endRow() - range.startRow() + 1;
    return GRectangle(range.startCol()*gCellSize, range.startRow()*gCellSize, numCols*gCellSize, numRows*gCellSize);
}

static string nextColor(bool reset = false) {
    static const int start = 20, end = 200, mid = (start+end)/2;
    static const string colors[] = {
        GColor::convertRGBToColor(end,start,start), //red
        GColor::convertRGBToColor(end,mid,start),   //orange
        GColor::convertRGBToColor(end,end,start),   //yellow
        GColor::convertRGBToColor(start,end,start), //green
        GColor::convertRGBToColor(start,end,end),   //aqua
        GColor::convertRGBToColor(start,mid,end),   //blue
        GColor::convertRGBToColor(mid,start,end),   //purple
        GColor::convertRGBToColor(end,start,end)};  //pink
    static const int nColors = sizeof(colors)/sizeof(*colors);
    static int n;
    if (reset) n = -1;
    return colors[++n % nColors];
}

static void drawCenteredText(GPoint center, double width, double height, string text) {
    GDimension bounds = getTextBounds(text, gFontDesc);
    gWindow->drawString(text, center.x + (width-bounds.width)/2, center.y + height - (height-bounds.height)/2);
}

static void drawPopulationLabel(GridLocation loc) {
    GPoint center(loc.col*gCellSize, loc.row*gCellSize);
    drawCenteredText(center, gCellSize, gCellSize, integerToString(gGrid[loc]));
}

static void drawBaseGrid() {
    gWindow->clear();
    gWindow->setColor("Light Gray");
    for (const auto& cur: gGrid.locations()) {
        drawPopulationLabel(cur);
    }
}

void initForPopulationGrid(Grid<int> &g, bool pauseOnRedraw) {
    if (!gInitialized) initialize();
    gPauseForClick = pauseOnRedraw;
    if (g.numRows() != gGrid.numRows() || g.numCols() != gGrid.numCols()) {
        changeDimensions(g.numRows(), g.numCols());
    }
    gGrid = g;
    gChosen.clear();
    nextColor(true); // reset color to start of wheel
    drawBaseGrid();
    gWindow->repaint();
    if (gPauseForClick) pauseForClick();
}

static void drawRange(GraphicalRange gr) {
    gWindow->setColor(gr.color);
    gWindow->fillRect(gr.bounds);
    gWindow->setColor("White");
    gWindow->setLineWidth(5);
    gWindow->drawRect(gr.bounds);
    for (GridLocation cur: gr.range) {
        drawPopulationLabel(cur);
    }
}

void addRange(GridLocationRange range, string color) {
    if (range.isEmpty()) error("Attempt to drawChosenRange on empty range!");
    if (!gGrid.locations().contains(range.startLocation()) || !gGrid.locations().contains(range.endLocation()))
        error("Attempt to drawChosenRange on range [" + range.toString() + "] that is outside grid!");
    GraphicalRange gr = {range, rectForRange(range), color};
    gChosen.add(gr);
    drawRange(gr);
}

void drawChosenRanges(Grid<int>& grid, GridLocationRange range1, GridLocationRange range2) {
    if (!gInitialized || gGrid != grid) return;
    string color = nextColor(); // re-use same color for both halves of one split
    addRange(range1, color);
    addRange(range2, color);
    gWindow->repaint();
    if (gPauseForClick) pauseForClick();
}

bool discardRange(GridLocationRange r) {
    for (int i = gChosen.size()-1; i >= 0; i--) {
        GridLocationRange cur = gChosen[i].range;
        if (cur.startLocation() == r.startLocation() && cur.endLocation() == r.endLocation()) {
            gChosen.remove(i);
            return true;
        }
    }
    error("Attempt to undoChosenRange on range [" + r.toString() + "] that was not previously drawn.");
    return false;
}

void undoChosenRanges(Grid<int>& grid, GridLocationRange range1, GridLocationRange range2) {
    if (!gInitialized || gGrid != grid) return;
    discardRange(range1);
    discardRange(range2);
    // some previous drawn ranges are now uncovered, rather than try to figure out which
    // just redraw entire stack
    drawBaseGrid();
    for (int i = 0; i < gChosen.size(); i++) {
        drawRange(gChosen[i]);
    }
    gWindow->repaint();
}

void printDistricts() {
    if (gGrid.isEmpty()) error("printDistricts called without previous call to drawPopulationGrid");

    for (int row = 0; row < gGrid.numRows(); row++) {
        for (int col = 0; col < gGrid.numCols(); col++) {
            cout << "+----";
        }
        cout << "+" << endl;
        for (int col = 0; col < gGrid.numCols(); col++) {
            cout << "| " << right << setw(2) << setfill(' ') << gGrid[row][col] << " ";
        }
        cout << "|" << endl;
    }
    for (int col = 0; col < gGrid.numCols(); col++) {
        cout << "+----";
    }
    cout << "+" << endl;
}
