/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        adcDrv1.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx/PIC24Hxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Settu D 			03/09/06  First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* This file contains two functions - initAdc1(), initTmr3(), _initDma0()
* and _DMA0Interrupt functions
**********************************************************************/

#if defined(__dsPIC33F__)
#include "p33fxxxx.h"
#elif defined(__PIC24H__)
#include "p24hxxxx.h"
#endif

#include "adcDrv1.h"
#include "dsp.h"
#include "tglPin.h"


/*=============================================================================
initAdc1() is used to configure A/D to convert channel 5 on Timer event. 
It generates event to DMA on every sample/convert sequence. ADC clock is configured at 625Khz 
=============================================================================*/
void initAdc1(void)
{
        //AD1CON1 Register
        AD1CON1bits.FORM = 3;		// Data Output Format: Signed Fraction (Q15 format)
        AD1CON1bits.SSRC = 2;		// Sample Clock Source: GP Timer starts conversion
        AD1CON1bits.ASAM = 1;		// ADC Sample Control: Sampling begins immediately after conversion
 		AD1CON1bits.AD12B = 1;		// 12-bit 1-channel operation
		AD1CON1bits.ADDMABM=1; 		// DMA buffers are built in conversion order mode

        //AD1CON2 Register
        AD1CON2bits.SMPI=0;			// Increment DMA address every 1 sample/conversion
		AD1CON2bits.BUFM=1;

        //AD1CON3 Register        
		AD1CON3bits.ADRC=0;			// ADC Clock is derived from Systems Clock
		AD1CON3bits.ADCS = 63;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us (625Khz)
									// ADC Conversion Time for 12-bit Tc=14*Tad = 22.4us 
									// ADC Conversion Time for 10-bit Tc=12*Tab = 19.2us						

		AD1CON4=0;					// Allocate 1 words of buffer to each analog input
									// This register is not used in conversion order mode
									// This is required only in the scatter/gather mode



        //AD1CHS0: A/D Input Select Register
        AD1CHS0bits.CH0SA=5;		// MUXA +ve input selection (AIN5) for CH0
		AD1CHS0bits.CH0NA=0;		// MUXA -ve input selection (Vref-) for CH0

        //AD1PCFGH/AD1PCFGL: Port Configuration Register
		AD1PCFGL=0xFFFF;
		AD1PCFGH=0xFFFF;
        AD1PCFGLbits.PCFG5 = 0;		// AN7 as Analog Input

        //AD1CSSH/AD1CSSL: A/D Input Scan Selection Register
        AD1CSSH = 0x0000;
		AD1CSSL = 0x0000;			// Channel Scan is disabled, default state
        
        IFS0bits.AD1IF = 0;			// Clear the A/D interrupt flag bit
        IEC0bits.AD1IE = 0;			// Do Not Enable A/D interrupt 
        AD1CON1bits.ADON = 1;		// Turn on the A/D converter	

        tglPinInit();

}

/*=============================================================================  
Timer 3 is setup to time-out every 125 microseconds (8Khz Rate). As a result, the module 
will stop sampling and trigger a conversion on every Timer3 time-out, i.e., Ts=125us. 
At that time, the conversion process starts and completes Tc=14*Tad periods later.

When the conversion completes, the module starts sampling again. However, since Timer3 
is already on and counting, about (Ts-Tc)us later, Timer3 will expire again and trigger 
next conversion. 
=============================================================================*/
void initTmr3() 
{
        TMR3 = 0x0000;
        PR3 = SAMPPRD;
        IFS0bits.T3IF = 0;
        IEC0bits.T3IE = 0;

        //Start Timer 3
        T3CONbits.TON = 1;

}


// DMA0 configuration
// Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM 
// AMODE: Regigster indirect with register increment
// MODE: Continuous, Ping-Pong Mode
// IRQ: ADC Interrupt
// ADC stores results stored alternatively between DMA_BASE[0]/DMA_BASE[16] on every 16th DMA request 

unsigned int dma0BufSel=0;

void initDma0(void)
{
	DMA0CON=0x0002;					// Configure DMA in Register indirect mode for (AD1CON1bits.ADDMABM=1)
	DMA0PAD=0x0300;
	DMA0CNT=15;						// 16 DMA request
	DMA0STA=0x0000;					// BUFFER A: DMA_BASE[0] to DMA_BASE[15]		
	DMA0STB=0x0020;					// BUFFER B: DMA_BASE[16] to DMA_BASE[31]
	DMA0CONbits.CHEN=1;
	DMA0REQ=13;
	dma0BufSel=0;

	IFS0bits.DMA0IF = 0;			//Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1;			//Set the DMA interrupt enable bit

}


/*=============================================================================
_DMA0Interrupt(): ISR name is chosen from the device linker script.
=============================================================================*/
//extern int _DMA_BASE;
unsigned int adcBuff[256],adcCntr=0;

void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
int i = 0; 
volatile unsigned int * adcPtr;
  

		dma0BufSel^=1;				// Toggle this variable on every interrupt
   		
		if(dma0BufSel==0) {			// DMA is filling buffer pointed by DMA0STA
  			adcPtr = (unsigned int *)(&_DMA_BASE + 0x10);
		} else {					// DMA is filling buffer pointed by DMA0STB
			adcPtr = (unsigned int *)(&_DMA_BASE);	
		}

	    for (i=0;i<16;i++) {
                adcBuff[adcCntr]= *adcPtr++;
				adcCntr++; 
       }
				
		if(adcCntr==256)
			adcCntr=0;

		tglPin();					// Toggle PORTA, BIT0	
        IFS0bits.DMA0IF = 0;		//Clear the DMA0 Interrupt Flag
}










