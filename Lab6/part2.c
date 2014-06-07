//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-24-13
//Lab #: 6
//Part #: 2
//===============================================================================
#include "hcs12.h"

 int main(){
     DDRB = 0xFF;       //ouput hbridge

 	 PWMSCLA = 0x0C;	// 0x0C = 12
     PWMCLK  = 0x33;
 	 PWMPOL  = 0x01;
     PWMCTL = 0x00;

     PWMPER0 = 100;	// 120

     PWMDTY0 = 50; //DTY/PER
     PWME    = 0x01;
     
     PORTB = 0x08;
     
 	 while(1);

}