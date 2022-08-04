
LIST   P=PIC24FJ256GA106
#include "p24FJ256GA106.inc"
;__CONFIG   _XT_OSC & _WDT_OFF & _MC_MODE & _BODEN_ON
; Declaring the variables
count_1 equ 0x1a
count_2 equ 0x1b
count_3 equ 0x1c

;;;;;;;;;;;;;;;;;;;;Main Porgramme;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


org 0x00
initialize
BANKSEL DDRG						; Select bank which has DDRG
MOVLW b'01000000'					;
MOVWF DDRG							; Use RG7(Tx) as o/p & RG6(Rx) as input
BANKSEL SPBRG2						;
MOVLW 0X19							;
MOVWF SPBRG2						; Move 25 into SPBUG to Select a baud Rate of 9.6KBPS
BANKSEL TXSTA2						;	
MOVLW B'00100000'					;
MOVWF TXSTA2						; Asnychronous Mode,1 start & 1 Stop bit,8 databits,No parity
BANKSEL RCSTA2						;
MOVLW B'10010000'					;
MOVWF RCSTA2						; Serial Port Enabled, Continous Recive Enabled
BSF INTSTA,PEIE						; Enable Peripheral Interrupts
CALL Delay							; Wait for 500msec

receive
BANKSEL PIR2						;
BTFSS PIR2,RC2IF					; Check recieve Buffer if full=1 then 
goto $-1							; recieve data ELSE if =0 keep Checking it

MOVFP RCREG2,WREG					; Copy recieved data to WREG		
call Delay
call transmit
goto initialize
transmit							;
BANKSEL PIR2						;
BTFSS PIR2,TX2IF					; Check Transmitt Buffer
GOTO $-1							; If transmitt Buffer is Empty then Move character
MOVWF TXREG2						; to TXREG and Transmitt it & wait 1 sec 
CALL Delay							; If transmitt Buffer is Full then wait for it
RETURN								; to get Empty

Delay								; wait for 500mS
movlw 0xff
movwf count_1
movwf count_2
movlw 0x0A
b
decfsz count_1,1
goto b
decfsz count_2,1
goto b
decfsz count_3,1
goto b
return
end


					
