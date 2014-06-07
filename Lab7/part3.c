#include "hcs12.h"
#include "vectors12.h"
#include "main.h"
#include "mainuit.h"
#include "lcd.h"
#define __interrupt   __attribute__((interrupt))

void _uitoa(unsigned int value, char* string, unsigned char radix);
void LCDWriteLine(byte line, char* d);

unsigned int period = 0;
unsigned int rpmValue = 0;
unsigned int dutyValue = 0;
unsigned int edge0 = 0;
unsigned int edge1 = 0;



__interrupt void IC3_ISR(){
	TFLG1 |= 0x08;

    edge1 = TC3;

    //Find freq
    period = edge1-edge0;
    edge0 = edge1;

    rpmValue = (24000000/period)*60;
    asm("swi");

    
   	return;
}
int dutyCycle(){
    int keypadData;
    int keypadData1;
    int keypadData2;
    int keypadData3;
    DDRB = 0xFF;
    DDRJ = 0x02;

        keypadData1 = ReadKey();
        keypadData1 = keypadData1*100;

        keypadData2 = ReadKey();
        keypadData2 = keypadData2*10;

        keypadData3 = ReadKey();
        keypadData = keypadData1+keypadData2+keypadData3;

        PWMDTY1 = keypadData; //DTY/PER


    return keypadData;

}

int main(){
    LCD_Init();
    char* dutyString = "Duty:     ";
    char* rpmString = "RPM:         ";

//////////////    //duty  //////////////////////////////////////////////
     DDRB = 0xFF;       //ouput hbridge

 	 PWMSCLA = 0x0C;	// 0x0C = 12
     PWMCLK  = 0x33;
 	 PWMPOL  = 0x01;
     PWMCTL = 0x00;

     PWMPER1 = 100;	// 120

     //PWMDTY1 = dutyValue;
     PWME    = 0x01;

     PORTB = 0x08;

//////////////////////    //read //////////////////////////////////////////
    DDRT &= 0x00;
	// Enable the Timer System
	TSCR1 = 0x80;

	// Make IC channel
	TIOS = 0x00;

	//rising edge only
	TCTL4 = 0x40;//01

	//Enable IC3 interrupts
    TIE = 0x08;//01

	UserTimerCh3 = (unsigned short)&IC3_ISR;
	asm("cli");


	// Spin forever
	while (1){
	//dutyValue = dutyCycle();
	//PWMDTY1 = dutyValue;
    _uitoa(dutyValue,&dutyString[6],10);
    _uitoa(rpmValue,&rpmString[5],10);
    LCDWriteLine(0x1,dutyString);
    LCDWriteLine(0x2,rpmString);


    //delay(100);
    }

	return 0;
}
