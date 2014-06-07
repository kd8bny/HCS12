#include registers.inc
 org datastart

 
 org prstart

 jmp buttonPress

#include Keypad.asm

buttonPress:
 ldaa #$FF
 staa DDRB ;
 jsr read_key
 stab PORTB
 bra buttonPress; back to main loop