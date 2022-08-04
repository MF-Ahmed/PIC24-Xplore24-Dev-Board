/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   UARTS.c - 														* *                  												* *   CMC-Master Communication Driver		                        *
\*******************************************************************/#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/*
	1. If r2Cnt=0 note the following comments
	2. Either full Packet or 1st three bytes are recieved
	3. If not VFrame then full packet is recieved
	4. If VFrame & VFrameCnt=0 then VFrameCnt is recieved & not the packet
	5. If VFrame & VFrameCnt!=0 then full VFrame is recieved
*/

//void __attribute__((__interrupt__)) _U2RXInterrupt(void);
//void __attribute__((__interrupt__)) _U2RXInterrupt(void)

void _ISRFAST _U2RXInterrupt(void);
void _ISRFAST _U2RXInterrupt(void)
{
  int i;
  char lcs=0;

  if(IFS1bits.U2RXIF) {
    IFS1bits.U2RXIF=0;		//Should be cleared by S/W
	if(MDelay) {
	  *r2Ptr=U2RXREG;
	  r2Ptr=r2Buf; r2Cnt=3; VFrameCnt=0;
	  return 0;
	}
    *r2Ptr++ = U2RXREG;
	if(U2STA&0x000E) MError=U2STA&0x000E;			//OERR, FERR, PERR
	//if(ERM) MError |= ERM&0x000E;					//Error Testing
	if(r2Cnt==3 && VFrameCnt==0) MSETmoCnt=30+1;	//MSE TMO for 3 Sec.
	if(r2Cnt>0) r2Cnt--;
	if(r2Cnt==0) {
	  if((r2Buf[0]&0x80) && (VFrameCnt==0)){
		VFrameCnt  = r2Buf[2]>>3;		//13-bit bytes count (lower 5 bits)
		VFrameCnt |= r2Buf[1]<<5;		//                   (higher 8 bits)
		DspMasterCnt=r2Cnt=VFrameCnt;
	  }
	  else {
		for(i=0; i<3+VFrameCnt; i++) {MSEBuf[i]=r2Buf[i]; lcs+=MSEBuf[i];}
		//if(lcs) {PORTE=lcs; lcs=0; MCSError=1;}
		if(lcs) {lcs=0; MCSError=1;}
		MasterSERec=1; r2Ptr=r2Buf; r2Cnt=3; VFrameCnt=0; 
		MSETmoCnt=0x8000;									//Stop MSE TMO 
  		//if((ERM&2)==2) PORTE |= MSEBuf[0]&0x1F;
	  }
	}	
  }
}

//void __attribute__((__interrupt__)) _U2TXInterrupt(void);
//void __attribute__((__interrupt__)) _U2TXInterrupt(void)

void _ISRFAST _U2TXInterrupt(void);
void _ISRFAST _U2TXInterrupt(void)
{
  if(IFS1bits.U2TXIF) {
    IFS1bits.U2TXIF=0;		//Should be cleared by S/W
    if(t2Cnt==1) {
	  IEC1bits.U2TXIE = 0;	//Disable U2Tx Interrupt
	  Cs *= -1; U2TXREG = Cs; 
	  t2Cnt=0; t2Ptr=t2Buf; 
	  MTxInProgress=0;
	}
	else {
	  Cs += *t2Ptr;
      U2TXREG = *t2Ptr++;
      if(t2Cnt>0) t2Cnt--;
	}
  }
} 

void EnableU2RxINTRs(void)
{
  /* clear IF flags */
  IFS1bits.U2RXIF = 0;
  IFS1bits.U2TXIF = 1;

  /* set priority */
  IPC7bits.U2RXIP = 5;
  IPC7bits.U2TXIP = 5;

  /* enable interrupts */
  IEC1bits.U2RXIE = 1;
  //IEC1bits.U2TXIE = 0;
}

void ConfigureU2(void)
{
  //BRGx=(FCY/16*BR)-1, where FCY=Fosc/2
  //In our case as Fosc=32MHz So, BRGx=((32000000/2)/16*BR)-1=103.16

  U2BRG=103;			//BR=9600	
  //U2BRG=207;			//BR=4800	
  //U2MODE=0x8000;		//UART2 Enable, 8 bit data, no parity, one stop bit
  U2MODE=0x8004;		//UART2 Enable, 8 bit data, odd parity, one stop bit
  U2STA=0x8400;			//UART2 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
}
/******************************************** End of FILE ******************************************/
