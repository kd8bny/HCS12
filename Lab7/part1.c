//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-24-13
//Lab #: 7
//Part #: 1
//===============================================================================
#include "hcs12.h"
#include "vectors12.h"
#define __interrupt   __attribute__((interrupt))

__interrupt void ISR(void){

	CRGFLG |= 0x80;
    //if(count == 100){ //100ms
    //    count = 0;
        PORTB ^= 1;
    //}
    //else{
    //count+=count;
    //}
	return;
}

//unsigned int count;

int main(){


    RTICTL = 0x6c; //prescaler
    CRGINT = 0x80;
    //CRGFLG = 0x80;

	DDRB = 0xFF;  // Set up Port B
	PORTB = 1;

    //count = 0;                       g
	asm("cli");  // Globally enable interrupts

	// Spin forever
	UserRTI = (unsigned short)&ISR;
	while (1);
	asm("swi");


	return 0;
}


