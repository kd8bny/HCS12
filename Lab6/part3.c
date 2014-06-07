//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-24-13
//Lab #: 6
//Part #: 2
//===============================================================================
#define GCC_TOOLCHAIN 1
#ifdef GCC_TOOLCHAIN
#include "main.h"
#include "mainuit.h"
#include "lcd.h"
#include "hcs12.h"
#endif

void _uitoa(unsigned int value, char* string, unsigned char radix);
void LCDWriteLine(byte line, char* d);



 int main(){
     LCD_Init();
     byte line = 0x1;
     char* duty = "Duty:     ";
     DDRB = 0xFF;       //ouput hbridge

 	 PWMSCLA = 0x0C;	// 0x0C = 12
     PWMCLK  = 0x33;
 	 PWMPOL  = 0x01;
     PWMCTL = 0x00;

     PWMPER1 = 100;	// 120

     PWMDTY1 = 50; //DTY/PER
     PWME    = 0x01;

     PORTB = 0x08;

    int keypadData;
    int keypadData1;
    int keypadData2;
    int keypadData3;
    DDRB = 0xFF;
    DDRJ = 0x02;

    for(;;){
        keypadData1 = ReadKey();
        keypadData1 = keypadData1*100;
        
        keypadData2 = ReadKey();
        keypadData2 = keypadData2*10;
        
        keypadData3 = ReadKey();
        keypadData = keypadData1+keypadData2+keypadData3;
        _uitoa(keypadData,&duty[6],10);
        LCDWriteLine(line,duty);
        PWMDTY1 = keypadData; //DTY/PER
        PORTB = 0x08;
        //asm("swi");
    }

}
