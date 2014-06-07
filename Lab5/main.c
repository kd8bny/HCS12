//===============================================================================
// Wytec Dragon12 DBUG-12 test program.
//
// By Jim Donelson 68hc12@jimdonelson.com
// Free tech support for Wytec Customers
//
// If you give this email out and it gets too much spam 
// it will be cut off.
//
//
// ***  Designed to run with DEBUG-12 ***
//
// GNU Version with Embedded GNU IDE.
//
// Starts at $2000 (see map file.)
//
//
// This assumes a vector forwardng table at $3E00 in
// DBUG-12 format. 
// Modify vectors according to AN1002 see below if not using DBUG-12
//
// What happens:
//
// - If you connect a scope to the DAC output, you will see a sine wave on channel A
//   and a ramp on channel B. SW 2-5 and or DIP SW 1-4 will multiply the
//   frequency of the waves.
// - The LED will count. SW 2-5 and/or DIP SW 1-4 will control the count rate.
// - The switches are written the single row LEDs
// - The pot will act as a brightenss countrol for the LED.
// - A song will play. If you lower DIP switch 8 is will only play once..
// - The current ADC value will be displayed on the LCD.
// - The 2nd line of the LCD will scroll. The brightness pot controls the scroll rate.
// - If you have the keypad connected, press keys will be displayed on the LCD
// - If it is a 20x4 LCD the lower two lines will have text displayed.
//
//===============================================================================

#include "main.h"


//#include "eepromburn.h"
#include "lcd.h"

//===============================================================================
//===============================================================================

void LCDWriteLine(byte line, char* d);
//===============================================================================
//===============================================================================

static volatile byte rtiCnt; // Forground loop cycle counter - rools over at 500ms
static volatile byte subCnt; // Counter to support rtiCnt
// Used for brightness control
static volatile  byte LEDDutyCycle = 128;    // How bright it should be.
static volatile  byte LEDytyCycleCounter=0;  // Counts ticks 
//===============================================================================
//===============================================================================

int t5delay;
int t6delay;          // T6 used to time the notes.
#ifdef miniDragon
#define TIME_SCALE  1
#else
#define TIMESCALE 4
#endif
int t6acc = 1000;     // T6 count down timer
int t6start = 1700;   // T6 reset value for a whole note.
int seconds = 0;
int song_index=0;     // index into the song array
byte song_counter = 0;// flag indicates song has played once.

char *initMessage = "Wytec Dragon12 GNU Build V1.1 Press Keypad, Turn Pot, Press SW's";
//===============================================================================
// Sine table (256 signed 8-bit values for sin(x) from 0 to 2PI)  
//===============================================================================

const signed char sinetable [256] = {
   0,    3,    6,    9,   12,   15,   18,   21, 
  24,   27,   30,   33,   36,   39,   42,   45, 
  48,   51,   54,   57,   59,   62,   65,   67, 
  70,   73,   75,   78,   80,   82,   85,   87, 
  89,   91,   94,   96,   98,  100,  102,  103, 
 105,  107,  108,  110,  112,  113,  114,  116, 
 117,  118,  119,  120,  121,  122,  123,  123, 
 124,  125,  125,  126,  126,  126,  126,  126, 
 126,  126,  126,  126,  126,  126,  125,  125, 
 124,  123,  123,  122,  121,  120,  119,  118, 
 117,  116,  114,  113,  112,  110,  108,  107, 
 105,  103,  102,  100,   98,   96,   94,   91, 
  89,   87,   85,   82,   80,   78,   75,   73, 
  70,   67,   65,   62,   59,   57,   54,   51, 
  48,   45,   42,   39,   36,   33,   30,   27, 
  24,   21,   18,   15,   12,    9,    6,    3, 
   0,   -3,   -6,   -9,  -12,  -15,  -18,  -21, 
 -24,  -27,  -30,  -33,  -36,  -39,  -42,  -45, 
 -48,  -51,  -54,  -57,  -59,  -62,  -65,  -67, 
 -70,  -73,  -75,  -78,  -80,  -82,  -85,  -87, 
 -89,  -91,  -94,  -96,  -98, -100, -102, -103, 
-105, -107, -108, -110, -112, -113, -114, -116, 
-117, -118, -119, -120, -121, -122, -123, -123, 
-124, -125, -125, -126, -126, -126, -126, -126, 
-126, -126, -126, -126, -126, -126, -125, -125, 
-124, -123, -123, -122, -121, -120, -119, -118, 
-117, -116, -114, -113, -112, -110, -108, -107, 
-105, -103, -102, -100,  -98,  -96,  -94,  -91, 
 -89,  -87,  -85,  -82,  -80,  -78,  -75,  -73, 
 -70,  -67,  -65,  -62,  -59,  -57,  -54,  -51, 
 -48,  -45,  -42,  -39,  -36,  -33,  -30,  -27, 
 -24,  -21,  -18,  -15,  -12,   -9,   -6,   -3
 } ;
 

//===============================================================================
// Musical note frequencys
//===============================================================================
#define c3		45866		// 261.63 Hz at 24 MHz
#define c3s		43293		// 277.18 Hz at 24 MHz
#define d3		40864		// 293.66 Hz at 24 MHz
#define d3s		38569		// 311.13 Hz at 24 MHz
#define e3		36404		// 329.63 Hz at 24 MHz
#define f3		34361		// 349.23 Hz at 24 MHz
#define f3s		32433		// 369.99 Hz at 24 MHz
#define g3		30613		// 391.99 Hz at 24 MHz
#define g3s		28894		// 415.31 Hz at 24 MHz
#define a3		27273		// 440.00 Hz at 24 MHz
#define a3s		25742		// 466.16 Hz at 24 MHz
#define b3		24297		// 493.88 Hz at 24 MHz

#define c4		22934		// 523.25 Hz at 24 MHz
#define c4s		21646		// 554.37 Hz at 24 MHz
#define d4		20431		// 587.33 Hz at 24 MHz
#define d4s		19285		// 622.25 Hz at 24 MHz
#define e4		18202		// 659.26 Hz at 24 MHz
#define f4		17181		// 698.46 Hz at 24 MHz
#define f4s		16216		// 739.99 Hz at 24 MHz
#define g4		15306		// 783.99 Hz at 24 MHz
#define g4s		14447		// 830.61 Hz at 24 MHz
#define a4		13636		// 880.00 Hz at 24 MHz
#define a4s		12871		// 932.32 Hz at 24 MHz
#define b4		12149		// 987.77 Hz at 24 MHz

#define c5		11467		// 1046.50 Hz at 24 MHz
#define c5s		10823		// 1108.73 Hz at 24 MHz
#define d5		10216		// 1174.66 Hz at 24 MHz
#define d5s		9642		// 1244.51 Hz at 24 MHz
#define e5		9101		// 1318.51 Hz at 24 MHz
#define f5		8590		// 1396.91 Hz at 24 MHz
#define f5s		8108		// 1479.98 Hz at 24 MHz
#define g5		7653		// 1567.98 Hz at 24 MHz
#define g5s		7225		// 1661.22 Hz at 24 MHz
#define a5		6818		// 1760.00 Hz at 24 MHz
#define a5s		6435		// 1864.66 Hz at 24 MHz
#define b5		6074		// 1975.53 Hz at 24 MHz

#define c6		5733		// 2093.00 Hz at 24 MHz
#define c6s		5412		// 2217.46 Hz at 24 MHz
#define d6		5109		// 2349.32 Hz at 24 MHz
#define d6s		4821		// 2489.02 Hz at 24 MHz
#define e6		4551		// 2637.02 Hz at 24 MHz
#define f6		4295		// 2793.83 Hz at 24 MHz
#define f6s		4054		// 2959.96 Hz at 24 MHz
#define g6		3827		// 3135.97 Hz at 24 MHz
#define g6s		3612		// 3322.44 Hz at 24 MHz
#define a6		3409		// 3520.00 Hz at 24 MHz
#define a6s		3218		// 3729.31 Hz at 24 MHz
#define b6		3037		// 3951.07 Hz at 24 MHz

#define c7		2867		// 4186.01 Hz at 24 MHz
#define c7s		2706		// 4434.92 Hz at 24 MHz
#define d7		2554		// 4698.64 Hz at 24 MHz
#define d7s		2411		// 4978.03 Hz at 24 MHz
#define e7		2275		// 5274.04 Hz at 24 MHz
#define f7		2148		// 5587.66 Hz at 24 MHz
#define f7s		2027		// 5919.92 Hz at 24 MHz
#define g7		1913		// 6271.93 Hz at 24 MHz
#define g7s		1806		// 6644.88 Hz at 24 MHz
#define a7		1705		// 7040.00 Hz at 24 MHz
#define a7s		1609		// 7458.63 Hz at 24 MHz
#define b7		1519		// 7902.13 Hz at 24 MHz
#define c8		1		// for rest note

//===============================================================================
// Quick look up table - saves bytes for a song.
//===============================================================================
unsigned int notes[]={
		c3,c3s,d3,d3s,e3,f3,f3s,g3,g3s,a3,a3s,b3,
		 0,0,0,0,		// We do this so the upper nibble is octave of a note
		c4,c4s,d4,d4s,e4,f4,f4s,g4,g4s,a4,a4s,b4,
		 0,0,0,0,
		c5,c5s,d5,d5s,e5,f5,f5s,g5,g5s,a5,a5s,b5,
		 0,0,0,0,
		c6,c6s,d6,d6s,e6,f6,f6s,g6,g6s,a6,a6s,b6,
		 0,0,0,0,
		c7,c7s,d7,d7s,e7,f7,f7s,g7,g7s,a7,a7s,b7,
		 0,0,0,0,
		c8
};
// Indexes of notes.
#define note_c		0
#define note_cs		1
#define note_d		2
#define note_ds		3
#define note_e		4
#define note_f		5
#define note_fs		6
#define note_g		7
#define note_gs		8
#define note_a		9
#define note_as		10
#define note_b		11


#define WHOLE	      1
#define HALF	      2
#define QUARTER     4
#define EIGHTH      8
#define SIXTEENTH   16

#define REST        0xfe
#define SONG_END    0xff

byte song[]={
//   Declare a song.....
		0x20+note_e,EIGHTH,
		0x20+note_ds,EIGHTH,
		0x20+note_e,EIGHTH,
		0x20+note_ds,EIGHTH,
		0x20+note_e,EIGHTH,
		0x10+note_b,EIGHTH,
		0x20+note_d,EIGHTH,
		0x20+note_c,EIGHTH,
		0x10+note_a,QUARTER,
		0x00+note_e,EIGHTH,
		0x00+note_a,EIGHTH,
		0x10+note_c,EIGHTH,
		0x10+note_e,EIGHTH,
		0x10+note_a,EIGHTH,
		0x10+note_b,QUARTER,
		0x00+note_gs,EIGHTH,
		0x10+note_d,EIGHTH,
		0x10+note_e,EIGHTH,
		0x10+note_gs,EIGHTH,
		0x10+note_b,EIGHTH,
		0x20+note_c,QUARTER,
		0x00+note_e,EIGHTH,
		0x00+note_a,EIGHTH,
		0x10+note_e,EIGHTH,
		REST,HALF,
		REST,QUARTER,
		SONG_END,SONG_END,
};
//===============================================================================
// Converstion routines.
//===============================================================================
#define NUMBER_OF_DIGITS 16
void _uitoa(unsigned int value, char* string, unsigned char radix)
{
  unsigned char index, i;
  index = NUMBER_OF_DIGITS;
  i = 0;

  do {
    string[--index] = '0' + (value % radix);
    if ( string[index] > '9') string[index] += 'A' - '9' - 1;
    value /= radix;
  } while (value != 0);

  do {
    string[i++] = string[index++];
  } while ( index < NUMBER_OF_DIGITS );

  //string[i] = 0; /* string terminator */
}

#ifndef _itoa
void _itoa(int value, char* string, unsigned char radix)
{
  if (value < 0 && radix == 10) {
    *string++ = '-';
    value = -value;
  }
  _uitoa(value, string, radix);
}
#endif
//===============================================================================
// Dragon12/miniDragon+ - 7 Segment LED Support
//
// Connected to Port H
// 7 0-6 lines of Port H used.
// Initialization:
// Port H needs to be set to output.
//   DDRH = 0xff;          // Init Port H LED set porth pins to output 
//===============================================================================

//
// LED Segment Decoder Table
//
byte segm_ptrn[]={   // segment pattern
//0    1    2    3    4    5    6    7
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,     //0-7
//8   9   A    B     C    d    E   F
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,     //8-F
//G   H     J   n     o   o
0x3d,0x76,0x74,0x1e,0x38,0x54,0x63,0x5c,	 //10-17
//blk  -   =    =     =    =
0x00,0x01,0x48,0x41,0x09,0x49	             //20-23
};

// Used by the brightness controller.
//static byte LEDValue;
#ifdef miniDragon
void LEDOut( byte value  ) 
{                                                      // This is done so bit 7 is untouched and can be used.
      PTH &= 0x80;    // Clear out lower 7 bits so can or in others.
      PTH |= LEDValue =  segm_ptrn[value];  
}

#endif

//===============================================================================
// Dragon12/miniDragon12+  Keypad
// 4x4 Key Pad Support
//
//  Connected to PORTA 8 bits used.
//
//  Port Set Up:
//  DDRA = 0x0f;          // pa0-pa3 are outputs, pa4-pa7 are inputs
//  PUCR = PUCR | 1;      // Init Port A enable pullups on porta for Key Pad.
//===============================================================================

char KPMessage[]="Key : X         ";


#define KBDEBOUNCE   1      // How many times the kb must be equal.
#define ROW_MASK     0xf0   // Mask for the row data.
#define COL1  0xe0
#define COL2  0xd0
#define COL3  0xb0
#define COL4  0x70

//===============================================================================
// Keypad - These tables map row/cols to numbers 0-15.
//===============================================================================
// define this if you want to plug the keypad in the other way.
#ifndef INV_KEYPAD
byte _kbdecoder[4][4]={
{0xf,0xb,0x7,0x3},
{0xe,0xa,0x6,0x2},
{0xd,0x9,0x5,0x1},
{0xc,0x8,0x4,0x0},
};
#else
byte _kbdecoder[4][4]={
{0x0,0x1,0x2,0x3},
{0x4,0x5,0x6,0x7},
{0x8,0x9,0xa,0xb},
{0xc,0xd,0xe,0xf},
};
#endif
static volatile byte in_digit   = 0;

//===============================================================================
// Keypad - Private variables used for the keypad.
//===============================================================================
byte ScanKeyPad()
{
  byte d;
  byte rowsel = 0xf7; //pa3=low, pa0-pa2=high
  byte i;
  byte input;
  byte j;
  static volatile byte last_digit = -1;
  static volatile byte kbdebounce = 0;

            
  DDRA = 0x0f; // pa0-pa3 are outputs, pa4-pa7 are inputs

  // Scan the keypad by lowering one row line at a time.
  // then checking if a col went low.
  for( i = 0, rowsel = 0xf7 ; i < 4 ; ++i)
  {
     PORTA = rowsel;
     for( d = 0 ; d < 16 ; ++d )  // small delay to settle.
        ;                      
     input = PORTA & ROW_MASK;
     
     if( input != ROW_MASK )      // a line was lo, so key pressed
        break;
     rowsel >>= 1;                // select the next row...
  }
  d = 255;
  if( i < 4 )  // Did we find any lows?
  {
       // if 2 keys are pressed, then none will match.
      switch(input) 
      {
        case COL1: j = 3; break;
        case COL2: j = 2; break;
        case COL3: j = 1; break;
        case COL4: j = 0; break;
        default:
            j = 255;;
      }
      // Have a match ?
      if( j != 255 )
        d =  _kbdecoder[i][j];
   }
  // Did anything decode ?
  if( d != 255 )
  {
      if( last_digit == d    )
      {
        if(kbdebounce != 0xff)
          ++kbdebounce;
      }
      else 
      {
        kbdebounce = 0;
        last_digit = d;
      }
      //
      // Valid key press detected ?
      //
      if(KBDEBOUNCE == kbdebounce ) 
      {
        kbdebounce = 0xff;  // means we have seen this key press  
//------------------------------------------------------
// Application specific code 
//
        if( 9 == in_digit  ) 
        {
           while( in_digit--)
              KPMessage[6+in_digit] = 0x20;
           in_digit = 0;
        }
        // Convert to Hex ASCII      
        KPMessage[6+in_digit] = d | 0x30;        
        if( d > 9 )
           KPMessage[6+in_digit] = (d-10) + 'A';
           
        LCDScrollLine(2,&KPMessage[0]);
        in_digit++;
        
         if( 4 == in_digit )
           KPMessage[6+(in_digit++)] = ':';
//------------------------------------------------------                         

        return d;
      }
  }           
  else
  {
    // Nothing found reset debouce.
    last_digit = -1;
    kbdebounce = 0;
  }
    
  return -1;        
         
}



//===============================================================================
//  miniDragon Switches
//  The switches go low when pressed or jumped.
//===============================================================================
#ifdef miniDragon
#define SW1  0x10
#define SW2  0x08
#define SW3  0x20
#define SW4  0x40 
#define SWMASK  (SW1 + SW2 + SW3 + SW4)
#define SWPORT    PORTAD0
#else
#define SW2    0x8
#define SW3    0x4
#define SW4    0x2 
#define SW5    0x1
#define SWMASK 0xff
#define SWPORT PTH

#define DIPSW8 0x80
#define DIPSW7 0x40
#define DIPSW6 0x20
#define DIPSW5 0x10
#define DIPSW4 0x8
#define DIPSW3 0x4
#define DIPSW2 0x2
#define DIPSW1 0x1
#endif
volatile byte switches;
//===============================================================================
//  Return Raw switches
//===============================================================================
byte ReadSwitches() 
{
 
      switches = SWPORT;
      return switches;
}

//===============================================================================
// Return TRUE if switch is pressed.
//===============================================================================
byte ReadSwitch(byte which) 
{
  switches = SWPORT & SWMASK;
  return 0 == (SWPORT & which);
}

//===============================================================================
// ADC
// miniDragon+ has a pot on channel 7. We use
// this for controlling the brightness of the 7-seg LEDs.
//===============================================================================
 // ADC Defines.
#define MULTI_MODE    0x10
#define SINGLE_MODE   0
#define SCAN_MODE     0x20
#define NO_SCAN_MODE	0
#define POT_CHANNEL_NUM   0x7		// reading input from AN07
#define SCF           0x80      //Sequence Complete Flag in ATD0STAT0

byte ADTValue;
char LCDMessage[32]="ADC 07 =       ";

//===============================================================================
// Trigger, wait, then read a channel and return 8-bit value..
//===============================================================================
byte ReadADC(byte channel) 
{
  byte status;

  // This resets the conversion done flag and starts a new conversion.
  ATD0CTL5 = SINGLE_MODE+NO_SCAN_MODE+channel;
  
  for (;;) 
  {
     status = ATD0STAT0;
     if( status & SCF )
       break;
  }
  
  status = ATD0DR7H;     // Just the hi byte. Ignore LSB's
  ADTValue = status;
  
  return status;
 
}
//===============================================================================
// Dragon12 DAC Support
// LTC1661 Dual DAC Connected to SPI0
// See the pdf for this part.
//===============================================================================
// Set up SPI0 to talk to the DAC
// PM6 = LD/CS for the DAC. This enables shifting in the data and loads the output.
#define DAC_LD      0x40  // PM6 is the load control fo r the DAC
void InitSPI0DAC() 
{
   DDRM |= DAC_LD;  // Set PM6 to output.
   PTM  |= DAC_LD;   // Set PM6 high to deselect
   
   // Enable SPI0, Set Master Mode,
   SPI0CR1 = 0x50;   
   
   SPI0CR2 = 2;
   
   // Set baud rate to 12Mhz @ 24Mz Bus speed
   // When 0 it will be bus clock/2
   SPI0BR  = 0x00; 
   
   WOMS    = 0;    // Port S Pull ups enabled
}
#define SPTEF   0x20
#define SPIF    0x80
//===============================================================================
// Send a byte to SPI0
//===============================================================================
void WriteSPI0(byte data )
{
  while( !(SPI0SR & SPTEF) )  // Wait for done.
    ;  
  
  SPI0DR = data;

  while( !(SPI0SR & SPIF) )   
   ;
  
  data = SPI0DR;              // Reset the SPIF flag.  
}
//===============================================================================
// Write a value to the DAC
// 0= Channel A 1=Channel B
//===============================================================================
#define LOAD_DACA   0x90
#define LOAD_DACB   0xa0
void WriteDAC( byte channel, byte data) 
{
// The output data to DAC is 16 bits:
// A3|A2|A1|A0|D9|D8|D7|D6  D5|D4|D3|D2|D1|D0|xx|xx  xx= Don't care
// A3-A0=Command  9=Load DAC A, 0xA=Load DAC B
// We only use the upper 8 bits.

   byte outData;
   outData = (channel == 0) ? LOAD_DACA : LOAD_DACB; // Set command
   outData |= ((data >> 4) & 0xf);
   
   // Need to raise LD first.
   PTM |= DAC_LD;      
   // Enable the chip for loading
   PTM &= ~DAC_LD;
   
   WriteSPI0( outData );
   outData = (data <<4) &0xf0;
   WriteSPI0( outData );
   PTM |= DAC_LD;
   
}
//===============================================================================
//===============================================================================
// main
// Perform initialization and forground loop
// Called from startup.
//===============================================================================
//===============================================================================
 __interrupt void RealTimeInterrupt(void);
#define LED_COUNT_RATE   100
volatile static int LEDCountRate = LED_COUNT_RATE;
volatile static byte dipswitch;

volatile static word seg7value = 0x0123;
volatile static byte LEDSvalue =0xAA;
int main1(void)
{
  word LED = 0;
    
  asm("sei");           // disable the global interrupts
  
#ifdef  GCC_TOOLCHAIN
  UserRTI       = (unsigned short)&RealTimeInterrupt;  // 3e70
  UserTimerCh3 =  (unsigned short)&Timer3Interrupt;
  UserTimerCh4 =  (unsigned short)&Timer4Interrupt;
  UserTimerCh5  = (unsigned short)&Timer5Interrupt;    // 3e64
  UserTimerCh6  = (unsigned short)&Timer6Interrupt;    // 3e62
#endif
  // Port set ups.
  DDRH = 0x0;          // set porth pins to input DIP Switches 
  dipswitch = PTH;     // Read the DIP Switches.
  // LEDs row of LEDs and 7 segs share Port B
  // PJ1 select the row of LEDs
  // PTP[0:4] control the 7 segs. Set Lo to select.
  DDRB = 0xff;         // Port B output to LEDS
  DDRJ |= 0x02;        // PJ1 enables the row of LEDS, so make it output.
  
  DDRP |= 0xf;         // PP0-PP3 Are digit enables for the 7 segs. Set to output.
  PERP |= 0xf;         // Enable pull ups on PP0-PP3.
  PTP  |= 0xf;         // Turn off all the digits for now.           
  PTJ  &= ~0x2;        // Set PJ1 to 0 to turn on LEDS.
  
 // At this point what ever you write to PORTB will 
 // show up on the single row of LEDs
 // After we start running, then stuff will be muxed instead.
   
  PORTB = dipswitch;  
  
  LCD_Init();
  LCDWriteLine(1,"Ready...");
   
  PUCR = PUCR | 1;      // enable pullups on porta for Key Pad.

  // ATD block set up.
  ATD0CTL2 = 0x80;      // Enable power up mode for ADC Block
  ATD0CTL3 = 0x40;      // Set 8 bit conversions.
    
  // Set the enable digital input for the switches SW1-SW4
  ATD0DIEN  = 0xff;

//  LEDOut(LED);

  rtiCnt = 0;           // roll over counter for forground loop.      
  
  // Set the Real Time Clock Rate
  RTICTL = 0x10;        // 1/(16Mz/2^10) = 64 us or 15.6 ticks per ms      
                        // 244us or 4 ticks/ms
  CRGINT |= 0x80;       // enable the interrupt for RTI 
  
  InitSPI0DAC();

  // Set up timer 5 for the speaker.
  t5delay = 1000;
  t6delay = 3000; // 1ms
  t6acc = t6start;
  TSCR1 = 0x90;               // Enable TCNT and fast clear
  TSCR2 = 0x03;               // Set prescaler to 1:8
  TIOS  |= TIOS_IOS6_MASK;    // Enable OC6 for song timer
  TIOS  |= TIOS_IOS4_MASK;    // Enable Timer 4 for muxing
  TIOS  |= TIOS_IOS3_MASK;    // Enable Timer 3 for DAC waveforms
    
//  TIOS  |= TIOS_IOS5_MASK;  // Disable the speaker until song starts.  
  TCTL1 = 5;                  // Set toggle mode for OC5 and OC4
  
  TC5 = TCNT + t5delay;       // Init the comnpare registers
  TC6 = TCNT;                 // This also resets the inturrupt.
  TIE |= TIOS_IOS5_MASK ;     // Enable the timer interrupt bits.
  TIE |= TIOS_IOS6_MASK;
  TIE |= TIOS_IOS4_MASK;
  TIE |= TIOS_IOS3_MASK;

//  LEDOut(2);
// New code
//  TDBG12printf("\nTEST for miniDragon12+ Hello World!\n");


 // Init the EEProm and Flash Burn clocks
 //
 #ifdef miniDragon
 FCLKDIV = ((16000/200/8)-1)+0x40; // Flash CLK = 200 kHz
 ECLKDIV = ((16000/200/8)-1)+0x40; // Eeprom CLK = 200 kHz
#else
 FCLKDIV = ((4000/200)-1); // Flash CLK = 200 kHz
 ECLKDIV = ((4000/200)-1); // Eeprom CLK = 200 kHz
#endif



  asm("cli");                 // enable the global interrupts

 // TDBG12printf("\nTEST for Dragon12 Hello World!\n");
  LCDWriteLine(1,"Ready...");
  LCDSetCharDelay(2, 0x2000);
  LCDScrollLine(2,initMessage);
  LCDWriteLine(3,"ABCDEFGHIJKLMNOPQRST"); 
  LCDWriteLine(4,"!@#$%^&*(){}[]:;?><"); 

  for (;;) {
    
 // Main forground loop.
 // Do things at different intervals.
    LEDSvalue = PTH; // Read the dip switches
    LCDUpdateScroll();
    if (rtiCnt >= LEDCountRate)
    {
      ++LED;   
      seg7value = LED; // set the value in the 7 segs.

      LEDCountRate = LED_COUNT_RATE;

      if(ReadSwitch( SW5) )
        LEDCountRate >>= 1;
        
      if(ReadSwitch( SW2) )
        LEDCountRate >>= 1;
        
      if(ReadSwitch( SW3) )
        LEDCountRate >>= 1;
        
      if(ReadSwitch( SW4) )
        LEDCountRate >>= 1;
       rtiCnt = 0;  
    }
    
    if(0 == rtiCnt % 10 ) 
    {
  
      LEDDutyCycle = ReadADC(POT_CHANNEL_NUM);
      
      _uitoa(LEDDutyCycle,&LCDMessage[9],10);
      LCDWriteLine(1,&LCDMessage[0]); 
 
       if( LEDDutyCycle > 80 )
      {
        LCDSetCharDelay(2, LEDDutyCycle * 50);
      }
    }

     if(  0 == rtiCnt % 2 ) 
     {
       ScanKeyPad();  
     }
  }
} 


//===============================================================================
//  RealTimeInterrupt
//
// - Led duty cycle
// - Counters for the foreground loop
// - LCD delay counter.
//===============================================================================
byte _digit_enable[]={
  ~0x8,~4,~2,~1
};

volatile word LEDMuxTimer = 256;
volatile byte digit_enable = 0;
volatile char muxCounter = 0;

__interrupt void RealTimeInterrupt(void)
{
  
  ++subCnt;
  
  if( 156/4 == subCnt )   
  {
     rtiCnt++;   
     subCnt = 0;
  }
  
  CRGFLG = 0x80; // clear rti flag         
}
byte WaveCntr = 0; 
byte skipcount = 2;

__interrupt void Timer3Interrupt(void) 
{
   int s;
   s = sinetable[WaveCntr];
   s += 128;
   
   WriteDAC(0,(byte)s);
   WriteDAC(1,WaveCntr);
  
   if( (PTH & 0xf) != 0xf )
     skipcount  = PTH & 0xf ;
   else
     skipcount = 1;

   // Skip will multiply the frequency bygoing thru the table faster.
   WaveCntr += skipcount; 

   TC3 += 150;
}

int Timer4Counts = 300;
int Timer4Subcnt = 0;

__interrupt void Timer4Interrupt(void) 
{
  byte digit;
  
    ++Timer4Subcnt;
    
    if( Timer4Subcnt % 16 ) {
      
    if(  muxCounter > 3  ) // Output the LEDS
    {
       PTP  |= 0xf;         // Turn all 7 seg. digits off 
       PTJ  &= ~0x2;        // Set PJ1 to 0 to turn on LEDS.
       PORTB = LEDSvalue;  
    }
    else 
    { // 0-3 = digits
       PTJ  |= 0x2;        // Set PJ1 to 1 to turn off single LEDS.
    
       digit = (seg7value >> (char) (4*(muxCounter))) & 0xf;
       PORTB = segm_ptrn[digit];
       if( LEDDutyCycle > 0){
        
       PTP &= 0xf0;      
       PTP  |= _digit_enable[muxCounter];                   
       }
       digit_enable = _digit_enable[muxCounter];
           
    }
    ++muxCounter;
    
    // It's 8 to allow the single leds to be a bit brighter.
    if(muxCounter == 6)
      muxCounter = 0;    
    }
    if( Timer4Subcnt == 255 ) 
    {
       PTP &= 0xf0;      
       PTP  |= digit_enable;
       Timer4Subcnt = 0;                   
       
    }
    if(Timer4Subcnt > (LEDDutyCycle))
    {
       // Shut the LEDS off for brightness.
       PTP  |= 0xf;
    }
    
    
    TC4 += Timer4Counts;
}
//===============================================================================
// Timer 6 interrupt
// Sequence the song and set up timer 5 for the note to be played.
//===============================================================================
__interrupt void Timer6Interrupt(void) 
{
    unsigned int freq;
    int duration;
    byte note;
    
    --t6acc;
    
     if( t6acc == 0 )
     {
        // Look up note and duration for next note.
        note = song[song_index++];  
        duration =  song[song_index++];
        
        // if there is a jumper on SW4, then the song will only play 1 time.
        if( note == REST || note == SONG_END ||
                   (!(switches & DIPSW8) && song_counter >= 1))
        {
            TIOS  &= ~TIOS_IOS5_MASK;     // Shut off the speaker
            
        } else 
        {
           TIOS  |= TIOS_IOS5_MASK;       // Speaker on
           freq = notes[note]/8;          // 4,6,12
        }
        
        t6acc = t6start/duration;
        
        t5delay = freq;
        TC5 += t5delay;
        if(  song[song_index] == 0xff )
        {
           song_counter=1; 
           song_index = 0;
        }
     }

// Uncomment to get rid of tune...     
//    TIOS  &= ~TIOS_IOS5_MASK;     // Shut off the speaker


    // Reset the interrupt flag and set up next count.
    // If you don't write to the TC register, the int will not reset.
    TC6 += t6delay;
}

//===============================================================================
// Generate a square wave on PT5 pin 16
// It is set up to toggle, so it makes a nice square wave.
//===============================================================================

int LEDBrightness = 0;

__interrupt void Timer5Interrupt(void)
{

               TC5 += t5delay;
}


