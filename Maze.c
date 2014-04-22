/************************************************
Function Implementations
************************************************/

#include <plib.h>
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

int buttonOnePress() {
    int cur = PORTG & 0x40;
    if (cur == _lastPress) return 0;
    DelayMs(2);
    cur = PORTG & 0x40;
    if (cur == _lastPress) return 0;
    _lastPress = cur;
    if (cur) return 1;
    return 0;
}

int buttonTwoPress() {
    int cur = PORTG & 0x80;
    if (cur == _lastPress) return 0;
    DelayMs(2);
    cur = PORTG & 0x80;
    if (cur == _lastPress) return 0;
    _lastPress = cur;
    if (cur) return 1;
    return 0;
}

int buttonThreePress() {
    int cur = PORTA & 0x01;
    if (cur == _lastPress) return 0;
    DelayMs(2);
    cur = PORTA & 0x01;
    if (cur == _lastPress) return 0;
    _lastPress = cur;
    if (cur) return 1;
    return 0;
}
