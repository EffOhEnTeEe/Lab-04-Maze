/* 
 * File:   maze.h
 * Author: fonte
 *
 * Created on April 22, 2014, 3:14 PM
 */

#ifndef MAZE_H
#define	MAZE_H

#include <stdint.h>

uint32_t _lastPress;				// Used to keep track of the last button press for debouncing
uint32_t SPI_Select;				// FLag for SPI selected
uint32_t I2C_Select;				// Flag for I2C selected
uint32_t Init_Display_Flag;			// Flag for Displaying the splash screen on init

void DisplaySplashScreen();		// Display the splash screen with my initials, "Cerebot," and "Maze puzzle" on three lines
void DisplayModeSelect();		// Display the mode select screen allowing selection of SPI or I2C communication
int buttonOnePress();			// BTN1 debouncing
int buttonTwoPress();			// BTN2 debouncing
int buttonThreePress();			// BTN3 debouncing

/*************************************
SPI Helper Function Prototypes
*************************************/
void SPIAccelInit();
int SPIAccelRead(int address);
float SPIAccelGetCoor(int address);
void SPIAccelWriteToReg(int address, int data);


#endif	/* MAZE_H */

