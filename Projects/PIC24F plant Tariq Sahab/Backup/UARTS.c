/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   UARTS.c - 														* *                  												* *             								                        *
/*******************************************************************/#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

void RemapUARTSPins()
{
  // Unlock Registers
  asm volatile ("MOV 	#OSCCON, w1  \n"
  				"MOV 	#0x46, 	w2 	 \n"
  				"MOV 	#0x57, 	w3 	 \n"
  				"MOV.b 	w2, 	[w1] \n"
  				"MOV.b 	w3, 	[w1] \n"
				"BCLR 	OSCCON,	#6"
				);

  RPINR18bits.U1RXR = 11;  			// Assign U1RX To Pin RP11 
  RPOR6bits.RP12R 	= 3 ;			// Assign U1TX To Pin RP12
  
  RPINR19bits.U2RXR = 4 ;  			// Assign U2RX To Pin RP4
  RPOR1bits.RP3R 	= 5 ;			// Assign U2TX To Pin RP3
  
  //RPINR17bits.U3RXR = 8 ;			// Assign U3RX To Pin RP8 
  //RPOR4bits.RP9R 	= 28;  			// Assign U3TX To Pin RP9

  RPINR27bits.U4RXR = 27;			// Assign U4RX To Pin RP27
  RPOR13bits.RP26R 	= 30;			// Assign U4TX To Pin RP26

  // Lock Registers
  asm volatile ("MOV #OSCCON, w1 \n"
				"MOV #0x46, w2 \n"
				"MOV #0x57, w3 \n"
				"MOV.b w2, [w1] \n"
				"MOV.b w3, [w1] \n"
				"BSET OSCCON, #6" 
				);
}

/******************************** CMC-FORCE COMMUNICATION DRIVER ***********************************/
ConfigureUARTS()
{
  ConfigureU1();
  ConfigureU2();
  ConfigureU4();
}

EnableUARTSRxINTRs()
{
  EnableU1RxINTRs();
  EnableU2RxINTRs(); 
  EnableU4RxINTRs(); 
}

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

/************************ CMC-ELECTONICS COMMUNICATION DRIVER **************************************/

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
		DRAckRecd=1; if((ERM&4)==4) DRAck=0x90;
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

/******************************** ERM COMMUNICATION DRIVER *****************************************/
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

