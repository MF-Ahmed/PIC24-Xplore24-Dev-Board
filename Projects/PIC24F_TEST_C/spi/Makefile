# MPLAB IDE generated this makefile for use with GNU make.
# Project: PIC_24F_SPI.mcp
# Date: Tue Oct 08 14:32:10 2013

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

PIC_24F_SPI.hex : PIC_24F_SPI.cof
	$(HX) "PIC_24F_SPI.cof"

PIC_24F_SPI.cof : adc_test_2.o
	$(CC) -mcpu=24FJ256GA106 "adc_test_2.o" -o"PIC_24F_SPI.cof" -Wl,-L"C:\Program Files\Microchip\MPLAB C30\lib",--script="C:\Program Files\Microchip\MPLAB C30\support\PIC24F\gld\p24FJ256GA106.gld",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,-Map="PIC_24F_SPI.map",--report-mem

adc_test_2.o : c:/program\ files/microchip/mplab\ c30/support/peripheral_24F/spi.h c:/program\ files/microchip/mplab\ c30/support/peripheral_24F/PPS.h c:/program\ files/microchip/mplab\ c30/support/PIC24F/h/p24FJ256GA106.h c:/program\ files/microchip/mplab\ c30/support/PIC24F/h/p24Fxxxx.h c:/program\ files/microchip/mplab\ c30/support/peripheral_24F/PIC24F_periph_features.h c:/program\ files/microchip/mplab\ c30/support/peripheral_24F/GenericTypeDefs.h c:/program\ files/microchip/mplab\ c30/support/peripheral_24F/uart.h c:/program\ files/microchip/mplab\ c30/support/PIC24F/h/P24FJ256GA106.h adc_test_2.c
	$(CC) -mcpu=24FJ256GA106 -x c -c "adc_test_2.c" -o"adc_test_2.o" -D__DEBUG -g -Wall

clean : 
	$(RM) "adc_test_2.o" "PIC_24F_SPI.cof" "PIC_24F_SPI.hex"

