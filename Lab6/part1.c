//===============================================================================
//Fill out the following:
//Name: Daryl Bennett
//Date: 10-24-13
//Lab #: 6
//Part #: 1
//===============================================================================
#include "hcs12.h"

 int main(){
     DDRP |= 0xFD;       //ouput LPWM
     PTP &= 0xFD;        //set output
     
 	 PWMSCLA = 0x0C;	// 0x0C = 12
     PWMSCLB = 0x0C;	// 0x0C = 12
     PWMCLK  = 0x33;
 	 PWMPOL  = 0x03;
     PWMCTL = 0x00;

     PWMPER0 = 100;	// 120
     PWMPER1 = 100;	// 120

     PWMDTY0 = 37; //DTY/PER
     PWMDTY1 = 75;
     PWME    = 0x03;

 	 while(1) ;
 	 
}
 
 