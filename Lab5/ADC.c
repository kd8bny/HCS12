//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-3-13
//Lab #: 4
//Part #: 3
//===============================================================================
#include "hcs12.h"
unsigned int reading = 0;
//void LCDWriteLine(byte line, char* d);


int main()
{
    //LCD set
    //LCD_Init();   //This subroutine initializes the LCD
    //define
    //byte line = 0x1;
    //char* value = "Daryl";
    int value = 0;
    
    DDRH = 0;           //input SW
    DDRB |= 0xFD;       //ouput LED
    PTB &= 0xFD;        //set output
    
	ATD0CTL2 = 0x80;	// Power up the ATD
	ATD0CTL3 = 0x40;	// 4 conversions
	ATD0CTL3 = 0x05;	// x12 divider, minimum sample time
						// No CTL4 init, so 10-bit results
	ATD0CTL5 = 0x86;	// Right-justified, starting Ch. 6
    int oldReading = 0;     // Set initial value
    while(1){
    ATD0CTL5 = 0x86;	// Right-justified, starting Ch. 6
	while (0 == (ATD0STAT0 & 0x80));
          /*reading += ATD0DR0H;	// Read each result register
          reading += ATD0DR1H;
          reading += ATD0DR2H;
          reading += ATD0DR3H;
          reading /= 4;		// And average them.*/
          reading = ATD0DR0;
          if(PTH != 0x0){
                  oldReading = reading;
                  PTB = reading;    //Set out put to read value
	              asm("swi");			// Return to DBUG12
          }
          else{
                 reading = 0;
                 PTB = reading;    //Set out put to read value
                 }
   }
}
