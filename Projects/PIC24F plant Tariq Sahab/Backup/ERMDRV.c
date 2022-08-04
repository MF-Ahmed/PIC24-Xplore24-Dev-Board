/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   UARTS.c - 														* *                  												* *   ERM COMMUNICATION DRIVER				                        *
\*******************************************************************/#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

void _ISRFAST _U1RXInterrupt(void);
void _ISRFAST _U1RXInterrupt(void)
{
  if(IFS0bits.U1RXIF) {
    IFS0bits.U1RXIF=0;		//Should be cleared by S/W
/*
    *r1Ptr++=U1RXREG;
	r1Cnt++;
	r1Flag=1;  
*/
  }
}

void _ISRFAST _U1TXInterrupt(void);
void _ISRFAST _U1TXInterrupt(void)
{
  if(IFS0bits.U1TXIF) {
    IFS0bits.U1TXIF=0;		//Should be cleared by S/W
    U1TXREG = *t1Ptr++;
    if(t1Cnt>0) t1Cnt--;
    if(t1Cnt==0) {
	  IEC0bits.U1TXIE = 0;	//Disable U1Tx Interrupt
	  t1Ptr=t1Buf;
	  ErmTxInProgress=0;
    }
  }
} 

void EnableU1RxINTRs(void)
{
  /* clear IF flags */
  IFS0bits.U1RXIF = 0;
  IFS0bits.U1TXIF = 1;

  /* set priority */
  IPC2bits.U1RXIP = 7;
  IPC3bits.U1TXIP = 7;

  /* enable interrupts */
  //IEC0bits.U1RXIE = 1;
  //IEC0bits.U1TXIE = 0;
}

void ConfigureU1(void)
{
  //BRGx=(FCY/16*BR)-1, where FCY=Fosc/2
  //In our case as Fosc=32MHz So, BRGx=((32000000/2)/16*BR)-1

  U1BRG=103;			//BR=9600	
  //U1BRG=207;			//BR=4800	
  U1MODE=0x8004;		// UART1 Enable, 8 bit data, odd parity, one stop bit
  U1STA=0x8400;			// UART1 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
}

/******************************************** End of FILE ******************************************/
