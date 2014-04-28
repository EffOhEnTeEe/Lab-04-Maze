/*
 * File:   main.c
 * Author: Fonte Clanton
 *
 *
 */

#define _PLIB_DISABLE_LEGACY
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <plib.h>
#include "delay.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "PmodOLED.h"
#include "Maze.h"


#pragma config ICESEL       = ICS_PGx1  // ICE/ICD Comm Channel Select
#pragma config DEBUG        = OFF       // Debugger Disabled for Starter Kit

#pragma config FNOSC        = PRIPLL	// Oscillator selection
#pragma config POSCMOD      = XT	// Primary oscillator mode
#pragma config FPLLIDIV     = DIV_2	// PLL input divider
#pragma config FPLLMUL      = MUL_20	// PLL multiplier
#pragma config FPLLODIV     = DIV_1	// PLL output divider
#pragma config FPBDIV       = DIV_8	// Peripheral bus clock divider
#pragma config FSOSCEN      = OFF	// Secondary   oscillator enable

#define PERIPHERAL_CLOCK 10000000
//#define FUNCTION_POINTER
//#define ROW_BY_ROW
//#define PRINT_MAZE

enum states { init, modeSelect, buttonIsPressed, setup, playGame } state;

// This struct will hold the the position variables
// for the player icon
struct _Player {
	uint32_t upLeft;// = 128;
	uint32_t upRight;// = 129;
	uint32_t botLeft;// = 256;
	uint32_t botRight;// = 257;
    uint32_t x_position;
    uint32_t y_position;
};
typedef struct _Player Player;

/*****************************************
Global Variables
*****************************************/
uint32_t sec1000;   					   // Millisecond timer counter
uint32_t timer_a;   					   // Keeps tracks of timestamps
uint32_t SPI_Select;                // FLag for SPI selected
uint32_t I2C_Select;                // Flag for I2C selected
uint32_t Init_Display_Flag;         // Flag for Displaying the splash screen on init
uint32_t row;
uint32_t col;

const uint32_t MAX_PLAYABLE_ROW = 3;  // Maximum playable Y position
const uint32_t MAX_PLAYABLE_COL = 15; // Maximum playable X position

const char PLAYER_ICON[] = { 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const int player_icon = 0x00;

Player thePlayer;

/*****************************************
 Interrupt Handlers
*****************************************/
void __ISR(_TIMER_2_VECTOR, ipl2) TimerInterruptHandler(void) {
    if( INT_SOURCE_TIMER(2) ) {		// Verify source of interrupt
        sec1000++;					// Increment timer counter
        INTClearFlag(INT_T2);		// Acknowledge Interrupt
    }
}

void main() {
    DelayInit();
    OledInit();
	int X, Y;
    thePlayer.x_position = 2;
    thePlayer.y_position = 2;

#ifdef FUNCTION_POINTER 	// function pointer stuff
    int (*checkPos)(void);


    checkPos = CheckLeft;
    checkPos(); // CheckLeft();

    checkPos = CheckRight;
  	checkPos(); // CheckRight();
#endif


    TRISFCLR = BIT_12;

    /*************************************
    User-Defined Characters
    *************************************/
    OledDefUserChar(player_icon, PLAYER_ICON);

    DDPCONbits.JTAGEN = 0;		// Disable JTAG controller so BTN3 can be used as input

    OpenTimer2(T2_ON            |
	           T2_IDLE_CON      |
	           T2_SOURCE_INT    |
	           T2_PS_1_16       |
	           T2_GATE_OFF,
	           625); // trigger every 1 ms = 1 kHz [ 10MHz/16/625 = 1000 ]

    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_1);
    INTClearFlag(INT_T2);
    INTEnable(INT_T2, INT_ENABLED);
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    state = init;				// Set state machine to init

    while (1) {

    	if(buttonThreePress()) {
    		state = init;
    	}

        switch(state) {
        	case init: {

        		if(!Init_Display_Flag) {
        			DisplaySplashScreen();		// Display the splash screen
        			Init_Display_Flag = 1;		// Set flag
	        		
	        		SPI_Select = 0;				// Reset flags for SPI and I2C select
	        		I2C_Select = 0;
        		}

        		// Stay in the init stage until BTN1 is pressed then display the mode select screen
        		if(buttonOnePress()) {
        			DisplayModeSelect();
        			Init_Display_Flag = 0;			// Reset flag
        			state = buttonIsPressed;
        		}

        	}
        	break;

        	// Special case for the transition from init to modeSelect so that BTN1 is not read too quickly.
        	// When that happens the mode select screen is only displayed for a brief window that doesn't
        	// allow the user enough time to make a choice
        	case buttonIsPressed: {
                if(!buttonOnePress()) {
                   state = modeSelect;
                }
        	}

        	case modeSelect: {

        		if(buttonOnePress()) {
        			SPI_Select = 1;			// SPI selected
        			SPIAccelInit();
        			state = setup;
        			OledClearBuffer();
        			OledUpdate();
        		}

        		else if(buttonTwoPress()) {
        			I2C_Select = 1;			// I2C selected
        			// I2C helper functions go here
        			state = setup;
        			OledClearBuffer();
        			OledUpdate();
        		}

        	}
        	break;

        	// setup case here that will print the maze and character
        	case setup: {
#ifdef PRINT_MAZE
        		OledClearBuffer();
        		PrintMaze();	// Print the maze to the OLED
#endif
        		state = playGame;
        	}

        	case playGame: {
    			X = SPIAccelGetCoor(0x32);
   				Y = SPIAccelGetCoor(0x34);

        		// If SPI was selected
        		if(SPI_Select) {
                    PrintPlayer( &thePlayer.x_position, &thePlayer.y_position );
                    PrintMaze();
                    OledUpdate();
                    OledClearBuffer();

#ifdef ROW_BY_ROW // Row by row and column by column
       				if( X < -25 && row < 3 ) {
       					OledSetCursor(col, ++row);
		                OledClearBuffer();
		                OledPutChar(player_icon);
		                OledUpdate();
       				}

       				else if ( X > 25 && row > 0 ) {
       					OledSetCursor(col, --row);
		                OledClearBuffer();
		                OledPutChar(player_icon);
		                OledUpdate();
       				}

       				if( Y < - 25 && col < 15 ) {
       					OledSetCursor(++col, row);
		                OledClearBuffer();
		                OledPutChar(player_icon);
		                OledUpdate();	
       				}

       				else if( Y > 25 && col > 0 ) {
       					OledSetCursor(--col, row);
		                OledClearBuffer();
		                OledPutChar(player_icon);
		                OledUpdate();
       				}

       				DelayMs(100);
#endif
        		}

        		else if(I2C_Select) {
        			OledSetCursor(0,0);
        			OledPutString("You got to I2C!");
        			OledUpdate();

        		}
        	}
        }
    }

    return;
}