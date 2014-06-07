//==============================================================================
//Name: Daryl Bennett
//Date: 11-21-13
//Lab #: Final
//==============================================================================
#include "hcs12.h"
#include "vectors12.h"
#include "main.h"
#include "mainuit.h"
#include "lcd.h"

void _uitoa(unsigned int value, char* string, unsigned char radix);
void LCDWriteLine(byte line, char* d);

unsigned char pattern[4] = {0x08,0x04,0x02,0x01};
unsigned char sound[4] = {20,40,80,99};

int sendPattern(int delayDiff){
    //Displays Pattern on LEDS
    int i;
    for(i=0;i<4;i++){
        PTB = pattern[i];
        makeSound(sound[i]);
        superTimer(delayDiff);
        PTB = 0x00;     //write zero to note subsequent blinks
        PWME = 0x00;    //kill sound
        superTimer(200);
    }
	return ;
}

int readPattern(){
    //Reads user input of SW's Then determines grade P/F
    int grade;
    unsigned char tempPattern[4];
    int i;
    int button;
    for(i=0;i<4;i++){
        //[SW3][SW2][SW1][SW0]
        while(PTH == 0xFF);
        switch(PTH){
            case 0xFE:
                //SW0
                makeSound(20);
                PTB = 0x01;
                tempPattern[i] = 0x01;
                makeSound(sound[i]);
                break;
            case 0xFD:
                //SW1
                makeSound(30);
                PTB = 0x02;
                tempPattern[i] = 0x02;
                makeSound(sound[i]);
                break;
            case 0xFB:
                //SW2
                makeSound(40);
                PTB = 0x04;
                tempPattern[i] = 0x04;
                makeSound(sound[i]);
                break;
            case 0xF7:
                //SW3
                makeSound(50);
                PTB = 0x08;
                tempPattern[i] = 0x08;
                makeSound(sound[i]);
                break;
        }
        superTimer(200);    //Delay to read next input
        PTB = 0x00;
        PWME = 0x00;    //kill sound
    }
    // Missing Equals() in embedded C :( Resort to for loop
    for(i=0;i<4;i++){
        if(tempPattern[i] != pattern[i]){
            grade = 0;
            break;
        }
        else{
            grade = 1;
        }
    }
    return grade;
}

int makePattern(){
    unsigned char oldPattern[4] = {0x08,0x04,0x02,0x01};
    int i;
    int j;
    for(i=0;i<4;i++){
        j = rand()%4;   //Random number between 0 and 3
        pattern[i] = oldPattern[j];
    }
    
    return;
}

int superTimer(int multiple){
    //Generates a time delay @ 1ms
    TSCR1 = 0x90;       //TCNT enable /ffcla
    TSCR2 = 0x03;       // Timer interrupter /prescaler
    TIOS |= OC0;        //Enable OC
    TC0 = TCNT + 3000;  // interrupt time

    int i;
    for(i = 0; i < multiple; i++){
        while(!(TFLG1 & C0F));
        TC0 += 3000;
    }
    TIOS &= ~OC0; //Disable OC 

    return;
}

int makeSound(int duty){
    //Generates sound via PWM
    PWMSCLA = 0x0C;	// 0x0C = 12
    PWMCLK  = 0x03;
    PWMSCLA = 0;
 	PWMPOL  = 0x03;
    PWMCTL = 0x00;

    PWMPER1 = 120;	// 120

    PWMDTY1 = duty;
    PWME    = 0x03;
    return;
}

int print(int statement,int value){
    //subfunction for uitoa
    if(statement == 1){
        char* scoreString = "Score:         ";
        _uitoa(value,&scoreString[7],10);
        LCDWriteLine(0x1,scoreString);
    }
    else if(statement == 2){
        //DEBUG
        char* debugString = "Debug:         ";
        _uitoa(value,&debugString[7],10);
        LCDWriteLine(0x1,debugString);
    }
    else{
        char* ahhhString = "ahhhhhh";
        LCDWriteLine(0x1,ahhhString);
    }
    return;
}

void setup(){
    //Init of all ports
    DDRB = 0xFF;    //LEDS
    DDRJ = 0x02;    //LED Cathode
    DDRH = 0x00;    //SW
    DDRP |= 0xFD;
    PTP &= 0xFD;

    //Timer
    TSCR1 = 0x80;   //Enable

    //LCD
    LCD_Init();
    
    return;
}

int main(){
    int pass;
    int score = 0;
    int delayDiff = 5000; //5s   
    setup();
    while(1){
        sendPattern(delayDiff);
        pass = readPattern();
        if(pass == 1){
            makeSound(95);      //passTune
            delayDiff -= 500;   //Increase difficulty -0.5s
            score += 10;
            makePattern();      //new pattern
        }
        else{
            makeSound(10);        //failTune();
            print(1,score);
            PWME = 0x00;    //kill sound
            break;
        }
    }
}
