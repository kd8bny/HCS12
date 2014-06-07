#include registers.inc
 org datastart
 
 ldaa #$FF
 staa DDRB
 ldaa #0
 staa DDRH

 org prstart

 LOOP:
 ldab ptiH
 stab PORTB
 Bra LOOP