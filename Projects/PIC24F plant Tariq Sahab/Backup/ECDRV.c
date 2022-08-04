/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   ECDRV.c - 														* *   																* *   CMC-ELECTONICS COMMUNICATION DRIVER          		            *
\*******************************************************************/#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

void _ISRFAST _U4RXInterrupt(void);
void _ISRFAST _U4RXInterrupt(void)
{
  int j;

  if(IFS5bits.U4RXIF) {
    IFS5bits.U4RXIF=0;								//Should be cleared by S/W
	if(DRDelay) {
	  *r3Ptr=U4RXREG;
	  r3Ptr=r3Buf; r3Cnt=1;
	  return 0;
	}
    *r3Ptr++=U4RXREG;
	if(U4STA&0x000E) EError=U4STA&0x000E;			//OERR, FERR, PERR
	//if(ERM) EError |= ERM&0x000E;					//Error Testing
	if(r3Cnt>0) r3Cnt--;
	if(r3Cnt==0) {
	  if((r3Buf[0]) && (DRCnt==0)) {
		DRAck=r3Buf[0]; 
		DRAckRecd=1; 
		//if((ERM&4)==4) DRAck=0x90;					//Error Testing Invalid DR
		if((DRAck>0x7F) && (DRDataTmoCnt<0)) {
		  if(DRAck==0x80) r3Cnt=33;
		  if(DRAck==0x81) r3Cnt=117;
		  if(DRAck==0x82) r3Cnt=953;
		  ESECnt=DRCnt=r3Cnt;
		  DRDataTmoCnt=30+1;						//Start DRDataTMO Counter of 3 Sec
		}
		else {
		  if(DRAck==0x7E || DRAck==0x7F) {
			r3Ptr=r3Buf; r3Cnt=1; DRCnt=0; 
		  }
		}
	  }
	  else {
		for(j=0; j<=DRCnt; j++) {
		  if(j<DRCnt-1) EDCS=(EDCS+r3Buf[j+1]);
		  ESEBuf[j]=r3Buf[j]; 
		}
		DRDataRecd=1; 
		//if((ERM&1)==1) ECS=ESEBuf[DRCnt]+1;
		//else ECS=ESEBuf[DRCnt];
		ECS=ESEBuf[DRCnt];
		if(ECS!=EDCS) ECSError=1;
		ECS=EDCS=0;
		r3Ptr=r3Buf; r3Cnt=1; DRCnt=0; 
	  }
	}
  }
}

void _ISRFAST _U4TXInterrupt(void);
void _ISRFAST _U4TXInterrupt(void)
{
  if(IFS5bits.U4TXIF) {
    IFS5bits.U4TXIF=0;					//Should be cleared by S/W
    U4TXREG = *t3Ptr++;
    if(t3Cnt>0) t3Cnt--;
    if(t3Cnt==0) {
	  IEC5bits.U4TXIE = 0;				//Disable U4Tx Interrupt
	  ETxInProgress=0;
	  t3Ptr=t3Buf;
    }
  }
} 


void EnableU4RxINTRs(void)
{
  /* clear IF flags */
  IFS5bits.U4RXIF = 0;
  IFS5bits.U4TXIF = 1;

  /* set priority */
  IPC22bits.U4RXIP = 1;
  IPC22bits.U4TXIP = 1;

  /* enable interrupts */
  IEC5bits.U4RXIE = 1;
  //IEC5bits.U4TXIE = 0;
}

void ConfigureU4(void)
{
  //BRGx=(FCY/16*BR)-1, where FCY=Fosc/2
  //In our case as Fosc=32MHz So, BRGx=((32000000/2)/16*BR)-1

  U4BRG=103;			//BR=9600	
  U4MODE=0x8004;		// UART4 Enable, 8 bit data, odd parity, one stop bit
  U4STA=0x8400;			// UART4 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
}

/************************************* End of File **************************************/
