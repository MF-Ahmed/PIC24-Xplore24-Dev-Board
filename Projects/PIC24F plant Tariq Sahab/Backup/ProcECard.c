/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Waseem Akhtar	14-April 2009                  		            * *                                                                  * *   ProcECards.c - 	Process Machines Data						* *                  												* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/********** *** ** * Send Electronic Cards ILC SE * ** ** **********/void SendECardsIlcSE(void)
{
  ILCCasBmp[0] |= ChanBMP[0] ;	
  ILCCasBmp[1] |= ChanBMP[1] ;
  ILCDatMods |= ECRD_MF ;
  ILCTxReq = 1 ;
} 
/********** ** * End Of Send Electronic Cards ILC SE * ** **********/

/********** *** ** * Process Electronic Cards Data * ** ** **********/int ProECardsData() {
  			int		ii ;
  unsigned 	char	 HwcChanBMP[2] ; 

  ChanBMP[0] = ChanBMP[1] = 0 ;
  HwcChanBMP[0] = HwcChanBMP[1] = 0 ;

  if(FirstTimeRun & 0x02) {
    FirstTimeRun &= 0x05 ;
/*///////////////////////
    // Auto Local Line Status
    if(TSTBITshort(&C12DAT.MECRDCDT[0],14)) { 
      CLRBITshort(&C12DAT.MECRDCDT[0],14) ;   
      ALLineSts = 1 ;
    }
    else
      ALLineSts = 0 ;   
/////////////////*/
    //C12DAT.MECRDCDT[0] |= 0x8000 ;	//For Local DC Power OK
    for(ii=0; ii<3; ii++) {      
      C12DAT.MECRDPDT[ii] = C12DAT.MECRDCDT[ii]  ;
	}
    return(0) ;
  }

/*///////////////////////
  // Auto Local Line Status
  if(TSTBITshort(&C12DAT.MECRDCDT[0],14)) { 
    CLRBITshort(&C12DAT.MECRDCDT[0],14) ;   
    ALLineSts = 1 ;
  }
  else
    ALLineSts = 0 ;   
/////////////////*/

  //// // / Valves Electronic Cards / // ////
  // Minis Valves Electronic Cards
  for(ii=0; ii<12; ii++) {
    if(!(C12PUSDT.MSERDT[ii] & VLV_MF)) { 			// If VLV SE of Mini Enable
      if(TSTBITshort(&C12DAT.MECRDCDT[0],ii)) {		// If VLV Card of Mini faulty
        SETBITchar(ChanBMP,ii) ;
		if(!(TSTBITchar(CONBMP,ii))) {				// If Mini in Computer Control
          SETBITchar(HwcChanBMP,ii) ;
		}
        C12PUSDT.MSERDT[ii] |= VLV_MF ;	// Disable SE 
 	  }
      // Currently Cards are OK	 Previously Cards were Faulty
	  else if(TSTBITshort(&C12DAT.MECRDPDT[0],ii)) {
        SETBITchar(ChanBMP,ii) ;
      }   
    } 
  }
  // C12 Valves Electronic Card
  if(!(C12PUSDT.ESERDT & VLV_MF)) {  				// If VLV SE of C12 Enable
    if(TSTBITshort(&C12DAT.MECRDCDT[0],12)) {		// If VLV Card of C12 faulty
      SETBITchar(ChanBMP,12) ;
	  if(!(TSTBITchar(CONBMP,12))) {				// If C12 in Computer Control
		HwcChanBMP[0] = 0xFF ; HwcChanBMP[1] = 0x1F ;
      }   
      C12PUSDT.ESERDT |= VLV_MF ;	// Disable Valves SE 
    }
    else if(TSTBITshort(&C12DAT.MECRDPDT[0],12)) {
      SETBITchar(ChanBMP,12) ;
    }
  }
//*///////////////
  // Bypass Valves Electronic Card
  if(TSTBITshort(&C12DAT.MECRDCDT[0],13)) {
    for(ii=0; ii<12; ii++) {
      if(!(C12PUSDT.MSERDT[ii] & VLV_MF)) { 
        SETBITchar(ChanBMP,ii) ;
        C12PUSDT.MSERDT[ii] |= VLV_MF ;	// Disable SE 
      }
      if(!(TSTBITshort(CONBMP,ii))) { 
        SETBITchar(HwcChanBMP,ii) ;
      }
    }
  } 
  else if(TSTBITshort(&C12DAT.MECRDPDT[0],13)) {
    //ChanBMP[0] = 0xFF ; ChanBMP[1] = 0x0F ;
    for(ii=0; ii<12; ii++) {
      if(!(C12PUSDT.MSERDT[ii] & VLV_MF)) { 
        SETBITchar(ChanBMP,ii) ;
      }
    }
  }
/////////////*/

//*//////////////
  // Local DC Supply Status
  if(!(C12PUSDT.ESERDT & LDC_MF)) {  				// If LDC SE of C12 Enable
    if(TSTBITshort(&C12DAT.MECRDCDT[0],15)) {		// If LDC Failed
      SETBITchar(ChanBMP,12) ;
      C12PUSDT.ESERDT |= LDC_MF ;					// Disable LDC SE 
    }
    else if(TSTBITshort(&C12DAT.MECRDPDT[0],15)) {	// If LDC Ok & previously Failed
      SETBITchar(ChanBMP,12) ;
    }
  } 
///////////////*/

//*//////////////
  ///////// Pressures Electronic Cards /////////
  // Minis Pressures Electronic Cards
  for(ii=0; ii<12; ii++) {
    if(!(C12PUSDT.MSERDT[ii] & (CSP_MF | VCP_MF))) {
      // Currently Cards are Faulty	 Previously Cards were OK
      if(TSTBITshort(&C12DAT.MECRDCDT[1],ii)) {
        SETBITchar(ChanBMP,ii) ;
		if(!(TSTBITchar(CONBMP,ii))) {
          SETBITchar(HwcChanBMP,ii) ;
		}
        C12PUSDT.MSERDT[ii] |= (CSP_MF | VCP_MF) ;	// Disable SE 
 	  }
      // Currently Cards are OK	 Previously Cards were Faulty
	  else if(TSTBITshort(&C12DAT.MECRDPDT[1],ii)) {
        SETBITchar(ChanBMP,ii) ;
      }   
    }
  } 
  // C12 Pressures Electronic Card
  if(!(C12PUSDT.ESERDT & (FFR_MF | EHP0_MF))) {
    if(TSTBITshort(&C12DAT.MECRDCDT[1],12)) {
      SETBITchar(ChanBMP,12) ;
	  if(!(TSTBITchar(CONBMP,12))) {
		HwcChanBMP[0] = 0xFF ; HwcChanBMP[1] = 0x1F ;
      }   
      C12PUSDT.ESERDT |= (FFR_MF | EHP0_MF) ;	// Disable SE 
    }
    else if(TSTBITshort(&C12DAT.MECRDPDT[1],12)) {
      SETBITchar(ChanBMP,12) ;
    }
  }
/////////////*/

//*//////////////
  ///////// Machines Electronic Cards /////////
  // Minis Machines Electronic Cards
  for(ii=0; ii<12; ii++) {
    if(!(C12PUSDT.MSERDT[ii] & MCH_MF)) {
      // Currently Cards are Faulty	 Previously Cards were OK
      if(TSTBITshort(&C12DAT.MECRDCDT[2],ii)) {
        SETBITchar(ChanBMP,ii) ;
        C12PUSDT.MSERDT[ii] |= MCH_MF ;	// Disable SE 
 	  }
      // Currently Cards are OK	 Previously Cards were Faulty
	  else if(TSTBITshort(&C12DAT.MECRDPDT[2],ii)) {
        SETBITchar(ChanBMP,ii) ;
      }   
    }
  }  
/////////////*/
  /////////////////////////////////////////////////////
  if(ChanBMP[0] || ChanBMP[1]) {
    SendECardsIlcSE() ;
    if(HwcChanBMP[0] || HwcChanBMP[1]) {
	  CONBMP[0] |= HwcChanBMP[0] ;
      CONBMP[1] |= HwcChanBMP[1] ;
      CHSEFlag = 1 ; // Set this flag after change of control Status CMP/HWC
	}
    C12DAT.MECRDPDT[0] = C12DAT.MECRDCDT[0]  ;
    C12DAT.MECRDPDT[1] = C12DAT.MECRDCDT[1]  ;
    C12DAT.MECRDPDT[2] = (C12DAT.MECRDCDT[2] & 0x0FFF) ;
  }
  return(0) ;
}
/********** ** * End Of Process Electronic Cards Data * ** **********/

/*************** *** ** * End Of File * ** ** ***************/