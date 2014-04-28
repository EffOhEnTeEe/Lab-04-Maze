/************************************************
Function Implementations
************************************************/

#include <plib.h>
//#include "OledGrph.h"
#include "Maze.h"

uint32_t _lastPress;                // Used to keep track of the last button press for debouncing

void DisplaySplashScreen() {
    // Print the splash screen before starting game
    OledClearBuffer();
    OledSetCursor(0,0);
    OledPutString("FC");
    OledSetCursor(3,1);
    OledPutString("Cerebot");
    OledSetCursor(2,2);
    OledPutString("Maze Puzzle");
    OledUpdate();
}

void DisplayModeSelect() {
	// Print the mode select screen
	OledClearBuffer();
	OledSetCursor(0,0);
	OledPutString("Enter Interface");
	OledSetCursor(0,2);
	OledPutString("Button 1 = SPI");
	OledSetCursor(0,3);
	OledPutString("Button 2 = I2C");
	OledUpdate();
}

/**********************************************
Button Debouncing Functions
**********************************************/

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

/**********************************************
Maze Printing
**********************************************/

void PrintMaze() {
    //OledClearBuffer();
    const int grid[]={27,30,44,47,70,73,107,110,127,
                      50,53,79,82,90,93,127,
                      20,23,67,70,106,109,127};
    OledMoveTo(0,0);OledDrawRect(127,31);
    uint32_t xc = 0;
    uint32_t yc = 7;
    uint32_t ind = 0;
    OledMoveTo(xc,yc);
    while(1) {
    OledLineTo(grid[ind],yc);
    if (grid[ind++]==127) {
    yc+=8;
    OledMoveTo(0,yc);
    if(yc>30) break; //we're done
    }
    else OledMoveTo(grid[ind++],yc);
    }
    OledMoveTo(64,0);
    OledLineTo(64,23);
    OledMoveTo(40,7);
    OledLineTo(40,15);
    OledMoveTo(85,7);
    OledLineTo(85,23);
    OledMoveTo(96,23);
    OledLineTo(96,31);
    OledUpdate();
}

/**********************************************
Collision Detection Functions
**********************************************/

//Print the player character to the OLED without updating the display
void PrintPlayer( uint32_t xco, uint32_t yco ) {
    OledMoveTo( xco, yco );
    OledDrawPixel();
    OledMoveTo( xco+1, yco );
    OledDrawPixel();
    OledMoveTo( xco, yco+1 );
    OledDrawPixel();
    OledMoveTo( xco+1, yco+1 );
    OledDrawPixel();
}

/**********************************************
Collision Detection Functions
**********************************************/

// Function checks if the pixel behind 
int CheckLeft( uint32_t* leftPos) {
    return OledGetPixel( (*leftPos) - 1 );     // Check the position behind the player's current position
}


/**********************************************
SPI Helper Function Implementations
**********************************************/

// Initializes SPI communications
void SPIAccelInit() {
	int garbage;

    SpiChnOpen(SPI_CHANNEL4, 
               SPI_OPEN_MSTEN       |
           	   SPI_OPEN_CKP_HIGH    | 
               SPI_OPEN_ENHBUF, 
               2);

    SpiChnPutC(SPI_CHANNEL4, 0x80);

    garbage = SpiChnGetC(SPI_CHANNEL4);

    SPIAccelWriteToReg(0x2C, 0x0A);
    SPIAccelWriteToReg(0x2D, 0x08);
}


int SPIAccelRead(int address) {
	int reading;
	PORTFCLR = BIT_12;
	SpiChnPutC(SPI_CHANNEL4, 0x80 + address);
	reading = SpiChnGetC(SPI_CHANNEL4);
	SpiChnPutC(SPI_CHANNEL4, 0xFF);
	reading = SpiChnGetC(SPI_CHANNEL4);
	PORTFSET = BIT_12;

	return reading;
}

float SPIAccelGetCoor(int address) {
	float result;
    int data0, data1;

    data0 = SPIAccelRead(address);
    data1 = SPIAccelRead(address + 1);

    data1 = 0x0000FFFF & ( data0 | (data1 << 8) ); // Use data2 the upper 8 bits, data1 the lower 8 bits

    if(data1 & 0x8000)              // Sign extension CHANGE ME
      data1 = data1 | 0xFFFF0000;

    data1 = (data1 ^ 0xFFFFFFFF) + 1; // Converting to signed magnitude CHANGE ME

    result = (data1 * -100 / 256);

    return result;
}

void SPIAccelWriteToReg(int address, int data) {
	PORTFCLR = BIT_12;

	SpiChnPutC(SPI_CHANNEL4, address);
	SpiChnGetC(SPI_CHANNEL4);
	SpiChnPutC(SPI_CHANNEL4, data);
	SpiChnGetC(SPI_CHANNEL4);

	PORTFSET = BIT_12;
}