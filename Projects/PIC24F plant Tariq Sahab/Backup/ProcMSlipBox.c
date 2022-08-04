/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Waseem Akhtar	17-April 2009                  		            * *                                                                  * *   ProcMSlipBox.c - 	Process Machines Data						* *                  												* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/********** *** ** * Send Electronic Cards ILC SE * ** ** **********/void SendMSlipBoxIlcSE(void)
{
  ILCCasBmp[0] |= ChanBMP[0] ;	
  ILCCasBmp[1] |= ChanBMP[1] ;
  ILCDatMods |= SBOX_MF ;
  ILCTxReq = 1 ;
} 
/********** ** * End Of Send Electronic Cards ILC SE * ** **********/

/********** *** ** * Process Electronic Cards Data * ** ** **********/int ProMSlipBoxData() {
  int	ii ;

  ChanBMP[0] = ChanBMP[1] = 0 ;
//*////////////////
  if(FirstTimeRun & 0x04) {
    FirstTimeRun &=0x03 ;
    for(ii=0; ii<TOTMIN; ii++) {
      C12DAT.MSBOXPDT[ii]  = C12DAT.MSBOXCDT[ii]  ;
	}
    return(0) ;
  }
///////////////*/
   /*** ** * Valves Electronic Cards * ** ***/
  // Minis Valves Electronic Cards
  for(ii=0; ii<TOTMIN; ii++) {
    if(C12DAT.MSBOXCDT[ii] != C12DAT.MSBOXPDT[ii]) {
      SETBITchar(ChanBMP,ii) ;
    }
  }
  if(ChanBMP[0] || ChanBMP[1]) {
    SendMSlipBoxIlcSE() ;
     for(ii=0; ii<TOTMIN; ii++) {
      C12DAT.MSBOXPDT[ii]  = C12DAT.MSBOXCDT[ii]  ;
	}
  }
  return(0) ;
}
/********** ** * End Of Process Electronic Cards Data * ** **********/

/*************** *** ** * End Of File * ** ** ***************/