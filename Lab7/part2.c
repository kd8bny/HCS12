#include "hcs12.h"
#include "vectors12.h"
#include "main.h"
#include "mainuit.h"
#include "lcd.h"
#define __interrupt   __attribute__((interrupt))

void _uitoa(unsigned int value, char* string, unsigned char radix);
void LCDWriteLine(byte line, char* d);

unsigned int period = 0;
unsigned int value;
unsigned int edge0 = 0;
unsigned int edge1 = 0;

__interrupt void IC3_ISR(){
	TFLG1 |= 0x08;
    edge1 = TC3;
    
    //Find freq
    period = edge1-edge0;
    edge0 = edge1;

    value = 24000/period;

    
	return;
}

int main(){
    LCD_Init();
    byte line = 0x1;
    char* freq = "FREQ:     ";

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

    _uitoa(value,&freq[6],10);
    LCDWriteLine(line,freq);

    }

	return 0;
}

