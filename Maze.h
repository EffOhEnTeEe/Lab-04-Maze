/* 
 * File:   maze.h
 * Author: fonte
 *
 * Created on April 22, 2014, 3:14 PM
 */

#ifndef MAZE_H
#define	MAZE_H

#include <stdint.h>
#include "OledGrph.h"

void DisplaySplashScreen();		// Display the splash screen with my initials, "Cerebot," and "Maze puzzle" on three lines
void DisplayModeSelect();		// Display the mode select screen allowing selection of SPI or I2C communication
int buttonOnePress();			// BTN1 debouncing
int buttonTwoPress();			// BTN2 debouncing
int buttonThreePress();			// BTN3 debouncing
// not implemented, might not be the best solution
int CheckLeft( uint32_t* );					// Check pixel left of the player
int CheckRight( uint32_t* );				// Check pixel right of the player
int CheckTop( uint32_t* );					// Check pixel above the player
int CheckBottom( uint32_t* );				// Check pixel below the player

/*************************************
SPI Helper Function Prototypes
*************************************/
void SPIAccelInit();
int SPIAccelRead(int address);
float SPIAccelGetCoor(int address);
void SPIAccelWriteToReg(int address, int data);


#endif	/* MAZE_H */

