/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Waseem Akhtar	08-April 2009                  		            * *                                                                  * *   ProcMac.c - 	Process Machines Data							* *                  												* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/***** *** ** * Send Machines ILC SE * ** ** *****/void SendMacsIlcSE(void)
{
  int MinCasNo ;
  ILCCasBmp[0] |= ChanBMP[0] ;	
  ILCCasBmp[1] |= ChanBMP[1] ;
  ILCDatMods |= CPS_MF ;
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    if(TSTBITchar(ChanBMP,MinCasNo)) {
      ILCMinMods[MinCasNo] |= MCH_MF ;
    }
  }	
  ILCTxReq = 1 ;
}
/***** ** * End Of Send Machines ILC SE * ** *****/

/***** *** ** * Swap short Data Bytes * ** ** *****/void SwapData(unsigned short *SPtr)
{
  return 0;
  unsigned char *CPtr, Dummy1, Dummy2 ;

  CPtr = ( unsigned char *)SPtr ;
  Dummy1 = *CPtr ;
  Dummy2 = *(CPtr+1) ;
  *CPtr     = Dummy2 ;
  *(CPtr+1) = Dummy1 ;
 }
/***** ** * End Of Swap short Data Bytes * ** *****/

/***** *** ** * Process Machines Frequency Data * ** ** *****/int ProMacFrqData() {
  int MinCasNo, MacNo, ii, jj ;
  int MacCrash, MacSlip, MacReinstall ;  unsigned short	MchFrq ;
  short	MCasTotMach[12] = {14,30,52,44,44,44,44,40,48,48,32,28} ;

  if(FirstTimeRun & 0x01) {
    FirstTimeRun &= 0xFE ;
    for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
      for(ii=0; ii<7; ii++) {
        C12DAT.MMCHPDT[MinCasNo][ii]  = C12DAT.MMCHCDT[MinCasNo][ii]  ;
        C12DAT.MMCHPSDT[MinCasNo][ii] = C12DAT.MMCHCSDT[MinCasNo][ii] ;
      }
	}
    return(0) ;
  }

  ChanBMP[0] = ChanBMP[1] = 0 ;
  for(jj=0; jj<4; jj++) {
    for(ii=0; ii<3; ii++) {
      MinCasNo = ii + (jj*3) ;
//*///////////////////////
 	  // If Inv A Fail		// R4-1, R2-1
      if((EINVDT & 0x01) && ((MinCasNo ==  0) || (MinCasNo ==  2))) continue ;
 	  // If Inv B Fail		// R1-1, R1-2
      if((EINVDT & 0x02) && ((MinCasNo ==  3) || (MinCasNo ==  4))) continue ;
 	  // If Inv C Fail		// R0-2, R0-3 
      if((EINVDT & 0x04) && ((MinCasNo ==  6) || (MinCasNo ==  7))) continue ;
 	  // If Inv D Fail		// R3-1, R0-1
      if((EINVDT & 0x08) && ((MinCasNo ==  1) || (MinCasNo ==  5))) continue ;
 	  // If Inv E Fail		// S2-1, S2-2
      if((EINVDT & 0x10) && ((MinCasNo == 10) || (MinCasNo == 11))) continue ;
 	  // If Inv F Fail		// S1-1, S1-2
      if((EINVDT & 0x20) && ((MinCasNo ==  8) || (MinCasNo ==  9))) continue ;
///////////////////////*/
      MacCrash = MacSlip = MacReinstall = 0 ;
	  if(!(C12PUSDT.MSERDT[MinCasNo] & MCH_MF)) {
        for(MacNo=0; MacNo<MCasTotMach[MinCasNo]; MacNo++) {
          if(!(TSTBITchar(C12PUSDT.MMCHMS[MinCasNo],MacNo))) {            if(jj==0)	   MchFrq = C12DAT.MMCHFRQ[ii][MacNo]  ; //For Minis	R4-1, R3-1, R2-1
            else if(jj==1) MchFrq = C12DAT.MMCHFRQ2[ii][MacNo] ; //For Minis	R1-1, R1-2, R0-1
            else if(jj==2) MchFrq = C12DAT.MMCHFRQ3[ii][MacNo] ; //For Minis	R0-2, R0-3, S1-1
            else if(jj==3) MchFrq = C12DAT.MMCHFRQ4[ii][MacNo] ; //For Minis	S1-2, S2-1, S2-2
            //SwapData(&MchFrq) ;
            //////////////// Machine Crashed /////////////
            if(MchFrq <= 400) {
		      //PORTE = 0x80 ;
	  	      if(MchSeDelay[MinCasNo][MacNo] == 0) {
		        SETBITchar(C12DAT.MMCHCDT[MinCasNo],MacNo) ;
		        CLRBITchar(C12DAT.MMCHCSDT[MinCasNo],MacNo) ;
		        MacCrash = 1 ;
		        MchSeDelay[MinCasNo][MacNo] = -1 ;    	      }
		      else if(MchSeDelay[MinCasNo][MacNo] < 0) {
		        if(!(TSTBITchar(C12DAT.MMCHPDT[MinCasNo],MacNo))) {
		          MchSeDelay[MinCasNo][MacNo] = C12PUSDT.MMDYDT[MinCasNo]+1 ;
    	        }        		      }
		      else if(MchSeDelay[MinCasNo][MacNo] > 0) {
		        if(TSTBITchar(C12DAT.MMCHPDT[MinCasNo],MacNo)){
			      MchSeDelay[MinCasNo][MacNo] = -1 ;		        }
		      }
            }
            //////////////// Machine Slipped /////////////
            else if(MchFrq <= 1020) {
		      if(MchSeDelay[MinCasNo][MacNo] == 0) {
		        SETBITchar(C12DAT.MMCHCSDT[MinCasNo],MacNo) ;
		        CLRBITchar(C12DAT.MMCHCDT[MinCasNo],MacNo) ;
		        MacSlip = 1 ;
		        MchSeDelay[MinCasNo][MacNo] = -1 ;		      }
		      else if(MchSeDelay[MinCasNo][MacNo] < 0) {
		        if(!(TSTBITchar(C12DAT.MMCHPSDT[MinCasNo],MacNo))) {
			      MchSeDelay[MinCasNo][MacNo] = C12PUSDT.MMDYDT[MinCasNo]+1 ;
                }        		      }
		      else if(MchSeDelay[MinCasNo][MacNo] > 0) {
		        if(TSTBITchar(C12DAT.MMCHPSDT[MinCasNo],MacNo)){
			      MchSeDelay[MinCasNo][MacNo] = -1 ;		        }
		      }
            }
            //////////////// Machine Reinstalled /////////////
            else if(MchFrq >= 1030) {
		      if(MchSeDelay[MinCasNo][MacNo] == 0) {
		        CLRBITchar(C12DAT.MMCHCSDT[MinCasNo],MacNo) ;
		        CLRBITchar(C12DAT.MMCHCDT[MinCasNo],MacNo) ;
		        MacReinstall = 1 ;
		        MchSeDelay[MinCasNo][MacNo] = -1 ;		      }
		      else if(MchSeDelay[MinCasNo][MacNo] < 0) { 
		        if(TSTBITchar(C12DAT.MMCHPDT[MinCasNo],MacNo) || 
				   TSTBITchar(C12DAT.MMCHPSDT[MinCasNo],MacNo)) {
			      MchSeDelay[MinCasNo][MacNo] = C12PUSDT.MMDYDT[MinCasNo]+1 ;
		        }  		      }
		      else if(MchSeDelay[MinCasNo][MacNo] > 0) {
		        if(!(TSTBITchar(C12DAT.MMCHPDT[MinCasNo],MacNo)) &&
				   !(TSTBITchar(C12DAT.MMCHPSDT[MinCasNo],MacNo))) {
			      MchSeDelay[MinCasNo][MacNo] = -1 ;		        }
		      }
            }
 		  }
        }
	  }
      if(MacCrash+MacSlip+MacReinstall) {
        SETBITchar(ChanBMP,MinCasNo) ;
      } 
    }
  }
  if((ChanBMP[0]) || (ChanBMP[1])) {
    SendMacsIlcSE() ;
    for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
      if(TSTBITchar(ChanBMP,MinCasNo)) {
        for(ii=0; ii<7; ii++) {
          C12DAT.MMCHPDT[MinCasNo][ii]  = C12DAT.MMCHCDT[MinCasNo][ii]  ;
          C12DAT.MMCHPSDT[MinCasNo][ii] = C12DAT.MMCHCSDT[MinCasNo][ii] ;
	    }
	  }
	}
    ///////// Copy Machines Frequency Previous Data to Current Data   
    for(ii=0; ii<3; ii++) {
      for(MacNo=0; MacNo<52; MacNo++) {
        PMMCHFRQ[ii][MacNo]  = C12DAT.MMCHFRQ[ii][MacNo]  ;
        PMMCHFRQ2[ii][MacNo] = C12DAT.MMCHFRQ2[ii][MacNo] ;
        PMMCHFRQ3[ii][MacNo] = C12DAT.MMCHFRQ3[ii][MacNo] ;
        PMMCHFRQ4[ii][MacNo] = C12DAT.MMCHFRQ4[ii][MacNo] ;
      }
	}
  }
  return(0) ;
}
/***** ** * End Of Process Machines Frequency Data * ** *****/

//extern	int	ProMacFrqData() ;
extern	int	ProECardsData() ;extern	int ProMSlipBoxData();extern	int PccSeInvSts() ;

void ProcMsECsPccData(void)
{
  PccSeInvSts() ;
  ProMSlipBoxData() ;
  ProECardsData() ;
  ProMacFrqData() ;
}/*************** *** ** * End Of File * ** ** ***************/