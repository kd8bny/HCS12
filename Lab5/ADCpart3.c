//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-3-13
//Lab #: 4
//Part #: 3
//===============================================================================
#define GCC_TOOLCHAIN 1
#ifdef GCC_TOOLCHAIN
#include "main.h"
#include "lcd.h"
#include "hcs12.h"
#endif

unsigned int reading = 0;
unsigned int oldReading = 0;     // Set initial value
unsigned int readingtemp = 0;
void _uitoa(unsigned int value, char* string, unsigned char radix);
void LCDWriteLine(byte line, char* d);


int main()
{
    //LCD set
    LCD_Init();   //This subroutine initializes the LCD
    //define
    byte line = 0x1;
    char* value = "ADC:      mA";
    int voltage;
    int current;
    
    DDRB |= 0xFD;       //ouput LED
    PTB &= 0xFD;        //set output
    
	ATD0CTL2 = 0x80;	// Power up the ATD
	ATD0CTL3 = 0x40;	// 4 conversions
	ATD0CTL3 = 0x05;	// x12 divider, minimum sample time
						// No CTL4 init, so 10-bit results
	ATD0CTL5 = 0x85;	// Right-justified, starting Ch. 6

    while(1){
    ATD0CTL5 = 0x85;	// Right-justified, starting Ch. 6
	while (0 == (ATD0STAT0 & 0x80));
          reading = ATD0DR0;       //read result register
          if(reading != oldReading){
                  oldReading = reading;
                  
                  voltage = (oldReading*5);
                  current = voltage/30;
                  _uitoa(current,&value[4],10);
                  LCDWriteLine(line,value);
                  //PTB = oldReading;    //Set out put to read value
	             //asm("swi");			// Return to DBUG12
          }
          else{
                 oldReading = 0;
                 LCDWriteLine(line,value);
                 //PTB = oldReading;    //Set out put to read value
                 }
   }
}
