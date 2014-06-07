#include "hcs12.h"            // For EmbeddedGNU
typedef unsigned char byte;   // Also for EmbeddedGNU
//#include "mc9s12dg256.h"      // For CodeWarrior

#define TRUE 1
#define FALSE 0

byte Keypad();
byte ReadKey();

byte key_tmp;
static byte keyTable[] = {1,2,3,10,4,5,6,11,7,8,9,12,14,0,15,13};

byte Keypad()
{
 byte BCount = 0;
 volatile byte killtime = 0;
 byte keypadData = 0;
 byte retValue = 0;
 byte columnSelector = 0x80;
 byte columnData = 0;

    PUCR |= 1;
    DDRA = 0x0F;

    key_tmp = 0xF7;

    for (BCount = 15; BCount != 11; BCount--)
    {
        PORTA = key_tmp;

        killtime = 10;
        while (killtime > 0)
        {
            killtime--;
        }

        keypadData = PORTA & 0xF0;
        if (keypadData != 0xF0)
        {
            break;
        }
        key_tmp >>= 1;
        // I'm a little concerned that this is 100% accurate...
    }

    if (11 == BCount)
    {
        return 0xFF;
    }

    columnData = keypadData & columnSelector;
    while (0 != columnData)
    {
        columnSelector >>= 1;
        columnData = keypadData & columnSelector;
        BCount -= 4;
    }

    if (0 == keypadData)
    {
        return 0xFF;
    }

    retValue = keyTable[BCount];

    return retValue;
}


byte ReadKey()
{
    static volatile byte keyDown = FALSE;
    static volatile byte lastKey = 0xFF;
    byte thisKey = -1;
    byte retValue = 0;

    while(1)
    {
        // Try to read a key
        thisKey = Keypad();

        // See what we got...
        if (FALSE == keyDown)
        {
            // If we don't read 0xFF, that means we got a key press.
            // Record it for debouncing,
            // and set the state variable indicating the button is down,
            if (0xFF != thisKey)
            {
                keyDown = TRUE;
                lastKey = thisKey;
            }

            // The unwritten "else" here says that we scanned the keypad
            // found nothing, and we're going to keep scanning.
        }

        // If we're here, we think the key was already down.
        else
        {
            // If this press and the last press aren't the same,
            // it *should* be because we've let go of the key and
            // the scan is returning 0xFF now.
            // So we return the value,
            // and reset the state.
            if (thisKey != lastKey)
            {
                keyDown = FALSE;
                retValue = lastKey;
                lastKey = 0xFF;
                return retValue;
            }

            // The unwritten "else" here indicates that this == last,
            // so the button is bouncing or is still down.
        }
    }
}


/*
// This main function is an example of how to read
// the keypad continuously.
int main()
{
    byte keypadData = 0;
    DDRB = 0xFF;
    DDRJ = 0x02;

    while (1)
    {
        keypadData = ReadKey();
        PORTB = keypadData;
//        asm("swi");
    }

    return 0;
}
*/