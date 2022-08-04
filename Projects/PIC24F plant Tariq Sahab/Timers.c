/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   Timers.c - 													* *                  												* *             								                        *
\*******************************************************************/

//#include <P24HJ256GP610.h>
#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/*
1.	Timer decrements the value
2.	Prescale 1:256 means timer will be incremented/decremented 
	after every 256th impulse of clock
3.	If PR1=65535(max. val.) and PreScale=1:256 then interrupt
	of the timer will be generated after every one second
4.	Timer Interrupt(in sec.)=[PRI*PS*1.9075]/Frq. of clock
	e.g. if PRI=65535, PS=256, Frq. of clock=32000000(32 MHz)
	[65535*256*1.9075]/32000000=1(Sec.)
5.	Therefore PR1 = Freq. of clock/[PS*1.9075]
6.	e.g.	PR1 = [32000000*1Sec]/[256*1.9075]=65531
7.	if we decrease the value of PS then interrupt time
	of timer will be smaller
8.	For 62ms PR1=4063, For 100ms PR1=6550, For 1s(1000ms) PR1=65535
*/

void _ISRFAST _T1Interrupt(void);
void _ISRFAST _T1Interrupt(void)
{
  if(IFS0bits.T1IF == 1) {
	IFS0bits.T1IF=0;         		// Clear IF bit 
	if(Tmr1Cnt>0) 		{Tmr1Cnt--; 	}
	if(DRTmoCnt>0) 		{DRTmoCnt--; 	}
	if(DRDataTmoCnt>0) 	{DRDataTmoCnt--;}
	if(VlvCmdTmoCnt>0)	{VlvCmdTmoCnt--;}
	if(DRDelay>0)		{DRDelay--;		}
	if(MSETmoCnt>0)		{MSETmoCnt--;	}
	if(MDelay>0)		{MDelay--;		}
	if(FFRSEDelay>0)	{FFRSEDelay--;	}
	if(EHPSEDelay>0)	{EHPSEDelay--;	}
    for(i=0; i<TOTMIN; i++){
      if(VCPSEDelay[i] > 0) 
        VCPSEDelay[i]--;
      if(CSPSEDelay[i] > 0) 
        CSPSEDelay[i]--;
    }

    for(i=0; i<13; i++){
	  if(VAD_CT[i] > 0) VAD_CT[i]--;
	  if(VSC_CT[i] > 0) VSC_CT[i]--;
	  if(VPD_CT[i] > 0) VPD_CT[i]--;
	  if(VSD_CT[i] > 0) VSD_CT[i]--;
	  if(VCD_CT[i] > 0) VCD_CT[i]--;
	}
    if(FIM_DL > 0) FIM_DL--;
    if(CMD_CT > 0) CMD_CT--;
  }
}



void _ISRFAST _T2Interrupt(void);
void _ISRFAST _T2Interrupt(void)
{
  int ii, jj, i;

  if(IFS0bits.T2IF == 1) {
	IFS0bits.T2IF=0;         		// Clear IF bit 
	for(ii=0; ii<TOTMIN; ii++) {
	  for(jj=0; jj<52; jj++) {
		if(MchSeDelay[ii][jj]>0)
          MchSeDelay[ii][jj]-- ;
	  }	  
	}
    /*for(i=0; i<13; i++){
	  if(VAD_CT[i] > 0) VAD_CT[i]--;
	  if(VSC_CT[i] > 0) VSC_CT[i]--;
	  if(VPD_CT[i] > 0) VPD_CT[i]--;
	  if(VSD_CT[i] > 0) VSD_CT[i]--;
	  if(VCD_CT[i] > 0) VCD_CT[i]--;
	}
    if(FIM_DL > 0) FIM_DL--;
    if(CMD_CT > 0) CMD_CT--;*/
  }
}

void ConfigureT1()
{
  TMR1  = 0;          				// Reset Timer1 to 0x0000 
  PR1   = ms_100;      				// Assigning Period to Timer period register 
  T1CONbits.TCKPS=0x03;				// Clock Prescale 0x00=1:1, 0x01=1:8, 0x02=1:64, 0x03=1:256 
  T1CONbits.TON = 1;     			// Timer1 on 
  IFS0bits.T1IF = 1;        		// Set IF bit 
  IPC0bits.T1IP = 6;    			// Assigning Interrupt Priority 
  IEC0bits.T1IE = 1; 				// Enable Interrupt
}

void ConfigureT2()
{
  TMR2  = 0;          				// Reset Timer2 to 0x0000 
  PR2   = ms_1000;      			// Assigning Period to Timer period register 
  T2CONbits.TCKPS=0x03;				// Clock Prescale 0x00=1:1, 0x01=1:8, 0x02=1:64, 0x03=1:256 
  T2CONbits.TON = 1;     			// Timer2 on 
  IFS0bits.T2IF = 1;        		// Set IF bit 
  IPC1bits.T2IP = 6;    			// Assigning Interrupt Priority 
  IEC0bits.T2IE = 1; 				// Enable Interrupt
}

int Delay(int del)
{
  Tmr1Cnt=del;
  while(Tmr1Cnt>0);					// Wait for delay to be over
  return 0;
}

/********************************* END OF FILE *******************************************/
/*
void _ISRFAST _T1Interrupt(void);
void _ISRFAST _T1Interrupt(void)
{
  if(IFS0bits.T1IF == 1) {
	IFS0bits.T1IF=0;         		// Clear IF bit 
	Tmr1Cnt--;
  }
}

Delay(int del)
{
  Tmr1Cnt=del;
  TMR1  = 0;          				// Reset Timer1 to 0x0000 
  PR1   = 65535;      				// Assigning Period to Timer period register 
  T1CONbits.TCKPS=0x03;				// Clock Prescale 0x00=1:1, 0x01=1:8, 0x02=1:64, 0x03=1:256 
  T1CONbits.TON = 1;     			// Timer1 on 
  IFS0bits.T1IF = 1;        		// Set IF bit 
  //IPC0bits.T1IP = 7;    			// Assigning Interrupt Priority 
  IEC0bits.T1IE = 1; 				// Enable Interrupt  
  while(Tmr1Cnt>0);					// Wait for Cnt to be zero
  IEC0bits.T1IE = 0; 				// Disable Interrupt 
  T1CONbits.TON = 0;     			// Timer1 off 
  IFS0bits.T1IF = 0;        		// Clear IF bit 	
  return 0;
}
*/
