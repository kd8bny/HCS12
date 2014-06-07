
#include "main.h"
#include "lcd.h"

//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-3-13
//Lab #: 4
//Part #: 3
//===============================================================================

void LCDWriteLine(byte line, char* d);

//===============================================================================
//===============================================================================
// main
// Perform initialization and forground loop
// Called from startup.
//===============================================================================
//===============================================================================


int main(void)
{
    LCD_Init();   //This subroutine initializes the LCD
    //define
    byte line = 0x1;
    char* name = "Daryl";
    //DIP
    DDRH = 0; //input
    DDRB |= 0xFD; //ouput FF
    PTB &= 0xFD;

    //write LCD
    LCDWriteLine(line,name);

    //detect dip
    for(;;){
    PTB = PTH;

    }

}





