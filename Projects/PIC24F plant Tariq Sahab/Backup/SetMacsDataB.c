/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Waseem Akhtar	30-April 2009                  		            * *                                                                  * *   SetMacsData.c - 	Process Machines Data						* *                  												* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/***** *** ** * Process Machines Frequency Data * ** ** *****/int SetMacsFrqData() {
  int MinCasNo, MacNo, StartMacNo, ii ;
  short	MCasTotMach[12] = {14,30,52,44,44,44,44,40,48,48,32,28} ;

  if(FirstTimeRun & 0x08) {
    FirstTimeRun &= 0x07 ;
    for(ii=0; ii<3; ii++) {
      for(MacNo=0; MacNo<52; MacNo++) {
        PMMCHFRQ[ii][MacNo]  = C12DAT.MMCHFRQ[ii][MacNo]  ;
        PMMCHFRQ2[ii][MacNo] = C12DAT.MMCHFRQ2[ii][MacNo] ;
        PMMCHFRQ3[ii][MacNo] = C12DAT.MMCHFRQ3[ii][MacNo] ;
        PMMCHFRQ4[ii][MacNo] = C12DAT.MMCHFRQ4[ii][MacNo] ;
      }
	}
    return(0) ;
  }
  ////////// /// // / If any Inv. failed [A, B, C, D, E, F] //////
  if(EINVDT & 0x01) {		// If Inverter A Failed
    for(MacNo=0; MacNo<52; MacNo++) {
      C12DAT.MMCHFRQ[0][MacNo] = PMMCHFRQ[0][MacNo] ;		//	R4-1
      C12DAT.MMCHFRQ[2][MacNo] = PMMCHFRQ[2][MacNo] ;		//	R2-1
    }
  }
  if(EINVDT & 0x02) {		// If Inverter B Failed
    for(MacNo=0; MacNo<52; MacNo++) {
      C12DAT.MMCHFRQ2[0][MacNo] = PMMCHFRQ2[0][MacNo] ;		// R1-1
      C12DAT.MMCHFRQ2[1][MacNo] = PMMCHFRQ2[1][MacNo] ;		// R1-2
    }
  }
  if(EINVDT & 0x04) {		// If Inverter C Failed
    for(MacNo=0; MacNo<52; MacNo++) {
      C12DAT.MMCHFRQ3[0][MacNo] = PMMCHFRQ3[0][MacNo] ;		// R0-2 
      C12DAT.MMCHFRQ3[1][MacNo] = PMMCHFRQ3[1][MacNo] ;		// R0-3 
    }
  }
  if(EINVDT & 0x08) {		// If Inverter D Failed
    for(MacNo=0; MacNo<52; MacNo++) {
      C12DAT.MMCHFRQ[1][MacNo]  = PMMCHFRQ[1][MacNo]  ;		// R3-1
      C12DAT.MMCHFRQ3[2][MacNo] = PMMCHFRQ3[2][MacNo] ;		// R0-1
    }
  }
  if(EINVDT & 0x10) {		// If Inverter E Failed
    for(MacNo=0; MacNo<52; MacNo++) {
      C12DAT.MMCHFRQ4[1][MacNo] = PMMCHFRQ4[1][MacNo] ;		// S2-1
      C12DAT.MMCHFRQ4[2][MacNo] = PMMCHFRQ4[2][MacNo] ;		// S2-2
    }
  }
  if(EINVDT & 0x20) {		// If Inverter F Failed
    for(MacNo=0; MacNo<52; MacNo++) {
      C12DAT.MMCHFRQ3[2][MacNo] = PMMCHFRQ3[2][MacNo] ;		// S1-1
      C12DAT.MMCHFRQ4[0][MacNo] = PMMCHFRQ4[0][MacNo] ;		// S1-2
    }
  }

  ////////// /// // / If any Machines Slip Box Faulty / // /// ///////////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    if(C12DAT.MSBOXCDT[MinCasNo]) {
	  StartMacNo = (C12DAT.MSBOXCDT[MinCasNo]-1) * 8 ;
	  //for(MacNo=StartMacNo; MacNo<MCasTotMach[MinCasNo]; MacNo++) {
      for(MacNo=StartMacNo; MacNo<52; MacNo++) {
        if(MinCasNo < 3) 
          C12DAT.MMCHFRQ[MinCasNo-0][MacNo]  = PMMCHFRQ[MinCasNo-0][MacNo]  ;
        else if(MinCasNo < 6) 
          C12DAT.MMCHFRQ2[MinCasNo-3][MacNo] = PMMCHFRQ3[MinCasNo-3][MacNo] ;
        else if(MinCasNo < 9) 
          C12DAT.MMCHFRQ3[MinCasNo-6][MacNo] = PMMCHFRQ3[MinCasNo-6][MacNo] ;
        else 
          C12DAT.MMCHFRQ3[MinCasNo-9][MacNo] = PMMCHFRQ3[MinCasNo-9][MacNo] ;
	  }
	}
  }
  ////////// /// // / If any Machines Card Faulty / // /// ///////////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    if(TSTBITshort(C12DAT.MECRDCDT,MinCasNo)) {
	  for(MacNo=0; MacNo<MCasTotMach[MinCasNo]; MacNo++) {
        if(MinCasNo < 3) 
          C12DAT.MMCHFRQ[MinCasNo-0][MacNo]  = PMMCHFRQ[MinCasNo-0][MacNo]  ;
        else if(MinCasNo < 6) 
          C12DAT.MMCHFRQ2[MinCasNo-3][MacNo] = PMMCHFRQ3[MinCasNo-3][MacNo] ;
        else if(MinCasNo < 9) 
          C12DAT.MMCHFRQ3[MinCasNo-6][MacNo] = PMMCHFRQ3[MinCasNo-6][MacNo] ;
        else 
          C12DAT.MMCHFRQ3[MinCasNo-9][MacNo] = PMMCHFRQ3[MinCasNo-9][MacNo] ;
	  }
	}
  }
  ////////// /// // / If any Machine Mask / // /// ///////////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    for(MacNo=0; MacNo<MCasTotMach[MinCasNo]; MacNo++) {
      if(TSTBITchar(&C12PUSDT.MMCHMS[MinCasNo][0],MinCasNo)) {
        if(MinCasNo < 3) 
          C12DAT.MMCHFRQ[MinCasNo-0][MacNo]  = PMMCHFRQ[MinCasNo-0][MacNo]  ;
        else if(MinCasNo < 6) 
          C12DAT.MMCHFRQ2[MinCasNo-3][MacNo] = PMMCHFRQ3[MinCasNo-3][MacNo] ;
        else if(MinCasNo < 9) 
          C12DAT.MMCHFRQ3[MinCasNo-6][MacNo] = PMMCHFRQ3[MinCasNo-6][MacNo] ;
        else 
          C12DAT.MMCHFRQ3[MinCasNo-9][MacNo] = PMMCHFRQ3[MinCasNo-9][MacNo] ;
	  }
	}
  }
  return(0) ;
}  
/*************** *** ** * End Of File * ** ** ***************/