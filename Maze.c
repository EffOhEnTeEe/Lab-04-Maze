/************************************************
Function Implementations
************************************************/

#include "Maze.h"

void DisplaySplashScreen() {
    // Print the splash screen before starting game
    OledSetCursor(0,0);
    OledPutString("FC");
    OledSetCursor(3,1);
    OledPutString("Cerebot");
    OledSetCursor(2,2);
    OledPutString("Maze Puzzle");
    OledUpdate();
}
