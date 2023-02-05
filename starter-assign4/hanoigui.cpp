/*
 * CS 106B/X, Marty Stepp
 * This file implements the GUI for the Towers of hanoi problem.
 */

#include "hanoigui.h"
#include <string>
#include "error.h"
#include "gobjects.h"
#include "stack.h"
#include "strlib.h"
#include "vector.h"
#include "map.h"

namespace HanoiGui {

static GWindow* window = nullptr;
static const int NUM_PEGS = 3;
static int DELAY_MS = 200;
static Vector<Vector<int>> disks;
static const Vector<string> DISK_COLORS {
    "red", "green", "yellow", "magenta", "cyan", "orange", "gray", "brown"
};
static Map<string, int> pegs {{"left", 0}, {"middle", 1}, {"right", 2}};


static bool OO_DRAW = true;
static bool WAIT_FOR_CLICK = false;

static void draw() {
    window->clearCanvas();

    // draw 3 pegs
    window->setColor("black");
    double pegX = 100;
    double pegY = 40;
    double pegWidth = 20;
    double pegHeight = window->getHeight() - 60;
    double pegGap = 150;

    for (int i = 0; i < NUM_PEGS; i++) {
        if (OO_DRAW) {
            GRect* rect = new GRect(pegX - pegWidth / 2, pegY, pegWidth, pegHeight);
            rect->setFilled(true);
            rect->setFillColor("black");
            window->add(rect);
        } else {
            window->setColor("black");
            window->fillRect(pegX - pegWidth / 2, pegY, pegWidth, pegHeight);
        }
        pegX += pegGap;
    }

    // draw disks
    double diskX = 100;
    for (int i = 0; i < NUM_PEGS; i++) {
        double diskWidthBase = 30;
        double diskWidthInc = 20;
        double diskHeight = 30;
        double diskY = pegY + pegHeight - diskHeight;
        for (int diskNum : disks[i]) {
            string color = DISK_COLORS[diskNum % DISK_COLORS.size()];

            double diskWidth = diskWidthBase + diskNum * diskWidthInc;

            if (OO_DRAW) {
                GRect* rect = new GRect(diskX - diskWidth / 2, diskY, diskWidth, diskHeight);
                rect->setFilled(true);
                rect->setFillColor(color);
                window->add(rect);
            } else {
                window->setColor(color);
                window->fillRect(diskX - diskWidth / 2, diskY, diskWidth, diskHeight);
            }

            diskY -= diskHeight;
        }
        diskX += pegGap;
    }
    window->repaint();

    // pause for animation
    if (WAIT_FOR_CLICK) {
        waitForClick();
    } else {
        pause(DELAY_MS);
    }
}

void initialize(int numDisks) {
    if (!window) {
        window = new GWindow(550, 300);
        window->setLocation(0, 0);
        window->setTitle("CS 106B Towers of Hanoi");
        window->setResizable(false);
        GObject::setAntiAliasing(false);
        window->setRepaintImmediately(false);
    }

    disks.clear(); // clear any previous contents

    // set up 3 empty pegs
    for (int i = 0; i < NUM_PEGS; i++) {
        disks.add(Vector<int>());
    }

    // all disks on first peg
    for (int i = numDisks; i >= 1; i--) {
        disks[0].add(i);
    }

    draw();
    window->show();
}

void moveOneDisk(int startPeg, int endPeg) {
    // check for various kinds of errors
    startPeg = isalpha(startPeg) ? startPeg - 'A' : startPeg;
    endPeg = isalpha(endPeg) ? endPeg - 'A' : endPeg;
    if (startPeg < 0 || startPeg > NUM_PEGS) {
        error("Invalid start peg: " + integerToString(startPeg));
    }
    if (endPeg < 0 || endPeg > NUM_PEGS) {
        error("Invalid end peg: " + integerToString(endPeg));
    }
    if (disks[startPeg].isEmpty()) {
        error("Peg " + integerToString(startPeg) + " has no disks");
    }
    if (!disks[endPeg].isEmpty() && disks[endPeg][disks[endPeg].size()-1] < disks[startPeg][disks[startPeg].size()-1]) {
        error("Disk on peg " + integerToString(startPeg)
              + " is too big to put onto peg " + integerToString(endPeg));
    }

    // no errors, so move the disk
    int disk = disks[startPeg].remove(disks[startPeg].size()-1);
    disks[endPeg].add(disk);

    // repaint the graphics to show new state
    draw();
}

// overloaded version that refers to pegs by string name
void moveOneDisk(string startPeg, string endPeg) {
    moveOneDisk(pegs[startPeg], pegs[endPeg]);
}

void setDelay(int ms) {
    DELAY_MS = ms;
}

int thirdPegNumber(int peg1, int peg2) {
    return 3 - peg1 - peg2;
}

void setWaitForClick(bool wait) {
    WAIT_FOR_CLICK = wait;
}

} // end namespace
