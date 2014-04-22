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

 // define setup parameters for OpenADC10
// 				Turn module on | ouput in integer | trigger mode timer | enable autosample
#define PARAM1  ADC_MODULE_ON | ADC_FORMAT_INTG32 | ADC_CLK_TMR | ADC_AUTO_SAMPLING_ON

// define setup parameters for OpenADC10
// 				ADC ref external    | disable offset test    | disable scan mode | perform 1 samples | use alternate mode
#define PARAM2  ADC_VREF_AVDD_AVSS | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_1 |  ADC_ALT_INPUT_OFF

// define setup parameters for OpenADC10
//				  use ADC internal clock | peripheral bus clock
#define PARAM3  ADC_CONV_CLK_Tcy2 | ADC_CONV_CLK_PB


// define setup parameters for OpenADC10
//               set AN0 as analog inputs
#define PARAM4	ENABLE_AN2_ANA

// define setup parameters for OpenADC10
// do not assign channels to scan
#define PARAM5	SKIP_SCAN_ALL

enum states { init } state;

/*****************************************
Global Variables
*****************************************/
uint32_t sec1000;   // Ms timer counter
uint32_t timer_a;   // Keeps tracks of timestamps
uint32_t a2dRead;

/*****************************************
 Interrupt Handlers
*****************************************/
void __ISR(_ADC_VECTOR, ipl1) ADCInterruptHandler(void) {
    if( INTGetFlag(INT_AD1)) {
        a2dRead = ReadADC10(0); // read the result of channel 0 conversion from the idle buffer
        //channel1 = ReadADC10(offset + 1); // read the result of channel 1 conversion from the idle buffer
        INTClearFlag(INT_AD1);
    }
}

void __ISR(_TIMER_3_VECTOR, ipl2) TimerInterruptHandler(void) {
    if( INT_SOURCE_TIMER(3)) {
        sec1000 += 10;
        INTClearFlag(INT_T3);
    }
}

void main() {
    DelayInit();
    OledInit();

    OpenTimer3(T3_ON            |
           T3_IDLE_CON      |
           T3_SOURCE_INT    |
           T3_PS_1_16       |
           T3_GATE_OFF,
           6250); // trigger every 10 ms = 100 Hz

    SetChanADC10(ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN2);
    OpenADC10(PARAM1, PARAM2, PARAM3, PARAM4, PARAM5);

    EnableADC10();

    INTSetVectorPriority(INT_ADC_VECTOR,INT_PRIORITY_LEVEL_1);
    INTClearFlag(INT_AD1);
    INTEnable(INT_AD1, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_2);
    INTClearFlag(INT_INT3);
    INTEnable(INT_T3, INT_ENABLED);
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    state = init;
    DisplaySplashScreen();

    while (1) {

        switch(state) {
        	case init: {

        	}
        	break;
            
        }
    }

    return;
}