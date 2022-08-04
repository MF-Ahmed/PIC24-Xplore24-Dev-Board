/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Waseem Akhtar	30-April 2009                  		            * *                                                                  * *   ProcessECrdSBox.c - 	Process Electronics Cards & Machines 	* *                  		Slipp Box Data.							* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/***** *** ** * If any Valves Card Faulty * ** *** *****/
void SetValvesData()
{   
  int MinCasNo, ValNo ;

  ///// /// // / If Any C12 Valve Mask / // /// /////
  for(ValNo=0; ValNo<7; ValNo++) {
    if(TSTBITchar(&C12PUSDT.EVLVMS[0],ValNo)) {		// Valve is Mask
      if(TSTBITchar(&C12DAT.EVLVPDT[0],ValNo)) {	// Open  Status
        SETBITchar(&C12DAT.EVLVCDT[0],ValNo) ; 
	  }
      else {
        CLRBITchar(&C12DAT.EVLVCDT[0],ValNo) ; 
      }
      if(TSTBITchar(&C12DAT.EVLVPDT[1],ValNo)) {	// Close Status
        SETBITchar(&C12DAT.EVLVCDT[1],ValNo) ; 
	  }
      else {
        CLRBITchar(&C12DAT.EVLVCDT[1],ValNo) ; 
      }
    }
  }
  ///// /// // / If Any Bypass Valve Mask / // /// /////
  for(ValNo=0; ValNo<7; ValNo++) {
    if(TSTBITchar(&C12PUSDT.EVLVMS[1],ValNo)) {		// Valve is Mask
      if(TSTBITchar(&C12DAT.EVLVPDT[2],ValNo)) {	// Open  Status
        SETBITchar(&C12DAT.EVLVCDT[2],ValNo) ; 
	  }
      else {
        CLRBITchar(&C12DAT.EVLVCDT[2],ValNo) ; 
      }
      if(TSTBITchar(&C12DAT.EVLVPDT[3],ValNo)) {	// Close Status
        SETBITchar(&C12DAT.EVLVCDT[3],ValNo) ; 
	  }
      else {
        CLRBITchar(&C12DAT.EVLVCDT[3],ValNo) ; 
      }
    }
  }
  ///// /// // / If Any Minis Valve Mask / // /// /////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    for(ValNo=0; ValNo<8; ValNo++) {
      if(TSTBITchar(&C12PUSDT.MVLVMS[MinCasNo],ValNo)){			// Valve is Mask
        if(TSTBITchar(&C12DAT.MVLVPDT[MinCasNo][0],ValNo)) {	// Open  Status
          SETBITchar(&C12DAT.MVLVCDT[MinCasNo][0],ValNo) ; 
	    }
        else {
          CLRBITchar(&C12DAT.MVLVCDT[MinCasNo][0],ValNo) ; 
        }
        if(TSTBITchar(&C12DAT.MVLVPDT[MinCasNo][1],ValNo)) {	// Close Status
          SETBITchar(&C12DAT.MVLVCDT[MinCasNo][1],ValNo) ; 
	    }
        else {
          CLRBITchar(&C12DAT.MVLVCDT[MinCasNo][1],ValNo) ; 
        }
      }
    }
  }
  ////////// /// // / If C12 Valves Card Faulty / // /// ///////////
  if(TSTBITshort(&C12DAT.MECRDCDT[0],12)) {
    C12DAT.EVLVCDT[0] = C12DAT.EVLVPDT[0] ;
    C12DAT.EVLVCDT[1] = C12DAT.EVLVPDT[1] ;
  }
  ////////// /// // / If Bypass Valves Card Faulty / // /// ///////////
  if(TSTBITshort(&C12DAT.MECRDCDT[0],13)) {
    C12DAT.EVLVCDT[2] = C12DAT.EVLVPDT[2] ;
    C12DAT.EVLVCDT[3] = C12DAT.EVLVPDT[3] ;
  }
  ////////// /// // / If Minis Valves Card Faulty / // /// ///////////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    if(TSTBITshort(&C12DAT.MECRDCDT[0],MinCasNo)) {
      C12DAT.MVLVCDT[MinCasNo][0] = C12DAT.MVLVPDT[MinCasNo][0] ;
      C12DAT.MVLVCDT[MinCasNo][1] = C12DAT.MVLVPDT[MinCasNo][1] ;
	}
  }
}

/***** *** ** * If any Analog Card Faulty * ** *** *****/
void SetAnalogData()
{   
  int MinCasNo ;

  ///// /// // / If Any C12 Analog Pressure Mask / // /// /////
  ////// If C12 FFR Mask //////
  if(C12PUSDT.EPRSMS & FFR_MF)
    C12DAT.EFLODT[0] = PEFLODT[0] ;
  ////// If C12 EHP Mask //////
  if(C12PUSDT.EPRSMS & EHP0_MF)
    C12DAT.EEHPDT = PEEHPDT ;
  ///// /// // / If Any Mini Analog Pressure Mask / // /// /////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    // If Mini CSP---FR1 Mask //
    if(C12PUSDT.MPRSMS[MinCasNo] & bit0)
      C12DAT.MCSPDT[MinCasNo][0] = PMCSPDT[MinCasNo][0] ;
    // If Mini CSP---FR0 Mask //
    if(C12PUSDT.MPRSMS[MinCasNo] & bit1)
      C12DAT.MCSPDT[MinCasNo][1] = PMCSPDT[MinCasNo][1] ;
    // If Mini CSP---FS1 Mask //
    if(C12PUSDT.MPRSMS[MinCasNo] & bit2)
      C12DAT.MCSPDT[MinCasNo][2] = PMCSPDT[MinCasNo][2] ;
    // If Mini VCP Mask //
    if(C12PUSDT.MPRSMS[MinCasNo] & bit3)
      C12DAT.MVCPDT[MinCasNo]    = PMVCPDT[MinCasNo] ;
  }
  ////////// /// // / If C12 Analog Card Faulty / // /// ///////////
  if(TSTBITshort(&C12DAT.MECRDCDT[1],12)) {
    C12DAT.EFLODT[0]  = PEFLODT[0] ;
    C12DAT.EFLODT[1]  = PEFLODT[1] ;
    C12DAT.ECHBGDT[0] = PECHBGDT[0] ;
    C12DAT.ECHPGDT[0] = PECHPGDT[0] ;
    C12DAT.ECHPGDT[1] = PECHPGDT[1] ;
    C12DAT.EEHPDT     = PEEHPDT ;
    C12DAT.EEHPPGDT   = PEEHPPGDT ; 
  }
  else {
    //PEFLODT[0]  = C12DAT.EFLODT[0]  ;
    PEFLODT[1]  = C12DAT.EFLODT[1]  ;
    PECHBGDT[0] = C12DAT.ECHBGDT[0] ;
    PECHPGDT[0] = C12DAT.ECHPGDT[0] ;
    PECHPGDT[1] = C12DAT.ECHPGDT[1] ;
    //PEEHPDT     = C12DAT.EEHPDT     ;
    PEEHPPGDT   = C12DAT.EEHPPGDT   ; 
  }
  ////////// /// // / If Minis Analog Card Faulty / // /// ///////////
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
    if(TSTBITshort(&C12DAT.MECRDCDT[1],MinCasNo)) {
      C12DAT.MCSPDT[MinCasNo][0] = PMCSPDT[MinCasNo][0] ;
      C12DAT.MCSPDT[MinCasNo][1] = PMCSPDT[MinCasNo][1] ;
      C12DAT.MCSPDT[MinCasNo][2] = PMCSPDT[MinCasNo][2] ;
      C12DAT.MVCPDT[MinCasNo]    = PMVCPDT[MinCasNo] ;
	}
  }
}

/***** *** ** * Process Machines Frequency Data * ** ** *****/void SetMacsFrqData() {
  int MinCasNo, MacNo, StartMacNo ;
  short	MCasTotMach[12] = {14,30,52,44,44,44,44,40,48,48,32,28} ;

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
    if(TSTBITshort(&C12DAT.MECRDCDT[2],MinCasNo)) {
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
  //return(0) ;
}
/***** *** ** * Process Electronics Cards & Slipp Box Data * ** *** *****/
int ProcessECrdSBox()
{
  int MinCasNo, MacNo, ii ;

  //*////////////////////
  if(FirstTimeRun & 0x08) {
    FirstTimeRun &= 0x07 ;

    ///////// Copy Valve Previous Data to Current Data   
    C12DAT.EVLVPDT[0] = C12DAT.EVLVCDT[0] ;
    C12DAT.EVLVPDT[1] = C12DAT.EVLVCDT[1] ;
    C12DAT.EVLVPDT[2] = C12DAT.EVLVCDT[2] ;
    C12DAT.EVLVPDT[3] = C12DAT.EVLVCDT[3] ;
    for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
      C12DAT.MVLVPDT[MinCasNo][0] = C12DAT.MVLVCDT[MinCasNo][0] ;
      C12DAT.MVLVPDT[MinCasNo][1] = C12DAT.MVLVCDT[MinCasNo][1] ;
    }

    ///////// Copy Analog Previous Data to Current Data   
    PEFLODT[0]  = C12DAT.EFLODT[0]  ;
    PEFLODT[1]  = C12DAT.EFLODT[1]  ;
    PECHBGDT[0] = C12DAT.ECHBGDT[0] ;
    PECHPGDT[0] = C12DAT.ECHPGDT[0] ;
    PECHPGDT[1] = C12DAT.ECHPGDT[1] ;
    PEEHPDT     = C12DAT.EEHPDT     ;
    PEEHPPGDT   = C12DAT.EEHPPGDT   ; 
    for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
      PMCSPDT[TOTMIN][0] = C12DAT.MCSPDT[TOTMIN][0] ;
      PMCSPDT[TOTMIN][1] = C12DAT.MCSPDT[TOTMIN][1] ;
      PMCSPDT[TOTMIN][2] = C12DAT.MCSPDT[TOTMIN][2] ;
      PMVCPDT[TOTMIN]    = C12DAT.MVCPDT[TOTMIN] ;
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
    return(0) ;
  }
  ///////////////*/
  SetAnalogData() ; 
  SetValvesData() ;
  SetMacsFrqData() ;
  return(0) ;
}
/*************** *** ** * End Of File * ** ** ***************/