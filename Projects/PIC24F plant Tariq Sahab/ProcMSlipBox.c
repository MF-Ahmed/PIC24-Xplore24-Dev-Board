/********************************************************************\
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/********** *** ** * Send Machines Slip Box ILC SE * ** ** **********/
{
  ILCCasBmp[0] |= ChanBMP[0] ;	
  ILCCasBmp[1] |= ChanBMP[1] ;
  ILCDatMods |= SBOX_MF ;
  ILCTxReq = 1 ;
} 
/********** ** * End Of Send Machines Slip Box ILC SE * ** **********/

/********** *** ** * Process Machines Slip Box Data * ** ** **********/
  int	MinCasNo ;

  ChanBMP[0] = ChanBMP[1] = 0 ;
  if(FirstTimeRun & 0x04) {
    FirstTimeRun &=0xFB ;
    for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
      C12DAT.MSBOXPDT[MinCasNo] = C12DAT.MSBOXCDT[MinCasNo] ;
	}
    return(0) ;
  }
  /*** ** * Machines Slip Box * ** ***/
  for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
	if(!(C12PUSDT.MSERDT[MinCasNo] & MCH_MF)) {
      if(C12DAT.MSBOXCDT[MinCasNo] != C12DAT.MSBOXPDT[MinCasNo]) {
        SETBITchar(ChanBMP,MinCasNo) ;
      }
    }   
  }
  if(ChanBMP[0] || ChanBMP[1]) {
    SendMSlipBoxIlcSE() ;
    for(MinCasNo=0; MinCasNo<TOTMIN; MinCasNo++) {
      C12DAT.MSBOXPDT[MinCasNo]  = C12DAT.MSBOXCDT[MinCasNo]  ;
	}
  }
  return(0) ;
}
/********** ** * End Of Process Machines Slip Box Data * ** **********/

/*************** *** ** * End Of File * ** ** ***************/