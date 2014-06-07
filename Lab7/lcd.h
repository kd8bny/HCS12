//===============================================================================
// LCD Routines
//===============================================================================
typedef struct _scrollData
{
   word LCDstartDelay;
   word LCDcharDelay;
   word LCDdelayCounter;
   word LCDlength;
   byte LCDstate;
   char* LCDscrollData;
   int LCDindex;
} ScrollData;
void LCD_Init(void);
void LCDUpdateScroll(void);
void LCDWriteLine(byte line, char* d);
void LCDScrollLine(byte line, char* d );
ScrollData* LCDSetStartDelay( int which, word delay);
ScrollData* LCDSetCharDelay( int which, word delay);
void delay(byte ms);

