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

enum states { init, modeSelect } state;

/*****************************************
Global Variables
*****************************************/
uint32_t sec1000;   // Millisecond timer counter
uint32_t timer_a;   // Keeps tracks of timestamps

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

    DisplaySplashScreen();		// Print splash screen
    state = init;				// Set state machine to init

    while (1) {

        switch(state) {
        	case init: {
        		// Stay in the init stage until BTN1 is pressed then display the mode select screen
        		if(buttonOnePress()) {
        			DisplayModeSelect();
        			state = modeSelect;
        		}
        	}
        	break;

        	case modeSelect: {

        		if(buttonOnePress()) {
        			//state = btn2;
        		}

        		if(buttonTwoPress()) {
        			//state = btn3;
        		}
        	}
        	break;            
        }
    }

    return;
}