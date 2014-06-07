#include registers.inc
 org datastart
pass: FCC "%"
 db 0

 org prstart
 ldy #0000
 jmp start

#include Keypad.asm

#include Lcd.asm

start:
 jsr lcd_init

read:
 jsr read_key
 jsr num_to_char
 cmpb #$23 ;# delete character
 beq delete
 ;write
 stb 1,+Y
 bra lcdwrite

delete:
;remove %
 stb 1,-Y
 bra lcdwrite


lcdwrite:
 ;doesnt print multiple times....other then that works perfectly
 ;need to impliment some type of for loop to print % multiple times
 ldx #pass ;load string location
 jsr printLn1 ;print string
 bra read
 

 
