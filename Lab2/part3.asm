#include registers.inc
 org datastart
Count ds 1

 org prstart
 ldab #$00
 stab DDRH
 staa Count

itLOOP:
 ldab ptiH
 cmpb #$FD
 bne itLOOP; branch if ab is equal

switchgo:
 ldaa Count
 adda #1
 staa Count
 nop
 bra itLOOP; back to main loop