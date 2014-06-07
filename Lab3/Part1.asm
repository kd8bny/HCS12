#include registers.inc

 org DATASTART
 liuno: FCC "Daryl"
 db 0
 lidose: FCC "Alger"
 db 0
 
 org PRSTART
 jmp start

#include Lcd.asm
start:
 jsr lcd_init
 ldx #liuno ;load string location
 jsr printLn1 ;print string
 ldx #lidose
 jsr printLn2