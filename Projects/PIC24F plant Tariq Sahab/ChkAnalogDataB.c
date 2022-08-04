/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Khalid Mehmood		20-April 2009                  		        * *                                                                  * *   ProcAnalogData.c - 	Process Analog data of FFR EHP CSP VCP	* *                  												* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

#define	Hig	1
#define Nrm	0
#define Wrn 1
#define Act 2
#define Mnl 2
#define Mjl 3
#define Eme 4

char MinChanBMP[2];
unsigned short MinNo ;

/********************************CHK FFR Routines ******************/
/********** *** ** * Set FFR ILC SE Modifiers * ** ** **********/void SetFFRILC_SE(void)
{
  ILCCasBmp[1] |= 0x10 ;
  ILCDatMods |= CPS_MF ;
  ILCECasMods |= FFR_MF ;
  ILCTxReq = 1 ;
}
 
/*****************  Chech FFR SE Request ******************/
void  SendFFRSE()
{
  SetFFRILC_SE();
  if(!(CONBMP[0] & 0x01)) {				//CMP Control
    if(FFRCStatus == Hig){
      FFRATLReq = 1;
    }
  }
}

/********** *** ** * Check Feed Flow Rate * ** ** **********/
void ChkFFR()
{
  unsigned short FFRValue, FFRNrmLmt, FFRHigLmt,PEFLODT[0] ;
  
  FFRSEReq = 0 ;

  if(!(C12PUSDT.ESERDT & FFR_MF)) {	   					//If FFR SER enable
    FFRValue  = C12DAT.EFLODT[0] ;
    FFRNrmLmt = C12PUSDT.ELMTDT[3] ;
    FFRHigLmt = C12PUSDT.ELMTDT[4] ;
    SwapData(&FFRValue) ;
	SwapData(&FFRNrmLmt) ;
	SwapData(&FFRHigLmt) ;

    if(FFRValue >= FFRHigLmt) {		// FFR Safty Limit, 
      FFRCStatus = Hig ;
    }

    if(FFRValue <= FFRNrmLmt) {		// FFr Normal Limit
      FFRCStatus = Nrm ; 
    }
    if((FFRCStatus) != (FFRPStatus)){
      if(FFRSEDelay == 0) {					 // If delay is completed
        FFRSEReq = 1;				  	     // Set SE Req flag
        FFRSEDelay = -1;					 // Set No delay
      }
      if(FFRSEDelay < 0) {				     // If there is no delay
        //FFRSEDelay = 11 ;				     // Set Delay 10 for one second 
        FFRSEDelay = 5 ;				     // Set Delay
      } 
    }
    else  {
      if(FFRSEDelay >= 0) {
        FFRSEDelay = -1 ;
        FFRSEReq = 0 ;
      }
    }
    if(FFRSEReq & 0x00FF) {
      FFRPStatus = FFRCStatus ;
      FFRSEReq = 0 ;
      SendFFRSE();
      PEFLODT[0] = C12DAT.EFLODT[0] ;
    }
  }  
}
/*************** *** ** * End Of FFR Routines * ** ** ***************/

/************************ EHP Routines ***************************/
/*****************  Send EHP SE Request *****************/
void SendEHPSE()
{  
  ILCCasBmp[1] |= 0x10 ;
  ILCDatMods |= CPS_MF ;
  ILCECasMods |= EHP_MF ;
  ILCTxReq = 1 ;
}

/***************************************/
void ChkEHP()
{
  unsigned short EHPValue, EHPNrmLmt, EHPWrnLmt , EHPActLmt , PEEHPDT ;

  EHPSEReq = 0;
  if(!(C12PUSDT.ESERDT & 0x01)) {					//If FFR SER enable 
    EHPValue  = C12DAT.EEHPDT  ;
    EHPNrmLmt = C12PUSDT.ELMTDT[0] ;
    EHPWrnLmt = C12PUSDT.ELMTDT[1] ;
    EHPActLmt = C12PUSDT.ELMTDT[2] ;
    SwapData(&EHPValue) ;
	SwapData(&EHPNrmLmt) ;
	SwapData(&EHPWrnLmt) ;
    SwapData(&EHPActLmt) ;

    if(EHPValue  >= EHPActLmt) {
      EHPCStatus = Act ; 
    }
    else if(EHPValue  >= EHPWrnLmt ) {
      if(EHPPStatus == Act)
        EHPCStatus = Act ;
      else
        EHPCStatus = Wrn ;
    } 
    else if(EHPValue  <= EHPNrmLmt) {
      EHPCStatus = Nrm ;
    }  
    if(EHPCStatus != EHPPStatus){
      if(EHPSEDelay == 0){ 						   // If delay is completed
        EHPSEReq = 1;						       // Set SE Req flag
        EHPSEDelay = -1;					       // Set No delay
      }
      if(EHPSEDelay < 0) {				           // If there is no delay
        //EHPSEDelay = 11 ;			 	           // Set Delay
        EHPSEDelay = 3 ;			 	           // Set Delay
      }
    }
    else {
      if(EHPSEDelay > 0) {
        EHPSEDelay = -1;
        EHPSEReq = 0;	
      }
    }
  }
  if(EHPSEReq & 0x00FF) {
   SendEHPSE();
   PEEHPDT = C12DAT.EEHPDT  ;
   EHPPStatus = EHPCStatus ;
   EHPSEReq = 0 ;
  }
}
/*************** *** ** * End Of EHP Routines * ** ** ***************/

/*--------------------VCP Routines Starts here -------------------*/
/*****************  Send EHP SE Request *****************/
void SendVCPSE()
{  
  ILCCasBmp[0] |= MinChanBMP[0] ;
  ILCCasBmp[1] |= MinChanBMP[1] ;

  ILCDatMods |= CPS_MF ;
  for(MinNo = 0;MinNo<12;MinNo++) {
    if(TSTBITchar(MinChanBMP,MinNo))
      ILCMinMods[MinNo] |= VCP_MF ;
  }
  ILCTxReq = 1 ;
}

/********** *** ** * Check Vaccum chamber Pressure * ** ** **********/
void ChkVCP()
{
  unsigned short VCPValue, VCPNrmLmt, VCPActLmt ,PMVCPDT[MinNo];
 
  VCPSEReq = 0 ;
  MinChanBMP[0] =  MinChanBMP[1]  = 0 ;

  for(MinNo = 0;MinNo<12;MinNo++) {
    if(!(C12PUSDT.MSERDT[MinNo] & VCP_MF)) {	   		//If VCP SER enable
      VCPValue  = C12DAT.MVCPDT[MinNo] ;
      VCPNrmLmt = C12PUSDT.MVCPLDT[MinNo][0] ;
      VCPActLmt = C12PUSDT.MVCPLDT[MinNo][1] ;
      SwapData(&VCPValue) ;
	  SwapData(&VCPNrmLmt) ;
	  SwapData(&VCPActLmt) ;

      if(VCPValue >= VCPActLmt) {		             	// FFR Safty Limit, 
        VCPCStatus[MinNo] = Hig ;
      }

      if(VCPValue <= VCPNrmLmt) {	                 	// FFr Normal Limit
        VCPCStatus[MinNo] = Nrm ; 
      }
      if((VCPCStatus[MinNo]) != (VCPPStatus[MinNo])){
        if(VCPSEDelay[MinNo] == 0) {			       // If delay is completed
          SETBITchar(MinChanBMP,MinNo);
          VCPPStatus[MinNo] = VCPCStatus[MinNo] ;
          PMVCPDT[MinNo] = C12DAT.MVCPDT[MinNo] ;
          VCPSEDelay[MinNo] = -1;					   // Set No delay
        }
        if(VCPSEDelay[MinNo] < 0) {				       // If there is no delay
          //VCPSEDelay = 11 ;				           // Set Delay 10 for one second 
          VCPSEDelay[MinNo] = 5 ;				       // Set Delay
        } 
      }
      else  {
        if(VCPSEDelay[MinNo] >= 0) {
          VCPSEDelay[MinNo] = -1 ;
        }
      }
    }
  }
  if(MinChanBMP[0] || MinChanBMP[1]) {
    SendVCPSE();
  }    
}
/*************** *** ** * End Of VCP Routines * ** ** ***************/

/*--------------------CSP Routines Starts here -------------------*/
/*****************  Send CSP SE Request *****************/

void SendCSPSE()
{ 
  short MiniNum ; 

  ILCCasBmp[0] |= MinChanBMP[0] ;
  ILCCasBmp[1] |= MinChanBMP[1] ;

  ILCDatMods |= CPS_MF ;
  for(MiniNum = 0;MiniNum<12;MiniNum++) {
    if(TSTBITchar(MinChanBMP,MiniNum)) {
      CSPPStatus[MiniNum] = CSPCStatus[MiniNum]  ;	 
      ILCMinMods[MiniNum] |= CSP_MF ;
    }
  }
  ILCTxReq = 1 ;
}

/***** *** ** * Set CSP Current Status * ** *** *****/
void SetCSPCurSts(short MinCasNo)
{
  unsigned short CSPLmt1, CSPLmt2 , CSPLmt3 , CSPNrmLmt, CSPValue ;
  short BNo ;

  CSPNrmLmt = C12PUSDT.MCSPLDT[MinCasNo][0] ;
  CSPLmt1   = C12PUSDT.MCSPLDT[MinCasNo][1] ;
  CSPLmt2   = C12PUSDT.MCSPLDT[MinCasNo][2] ;
  CSPLmt3   = C12PUSDT.MCSPLDT[MinCasNo][3] ;

  SwapData(&CSPNrmLmt) ;
  SwapData(&CSPLmt1) ;
  SwapData(&CSPLmt2) ;
  SwapData(&CSPLmt3) ;
  for(BNo=0; BNo<3; BNo++) {
    CSPValue = C12DAT.MCSPDT[MinCasNo][BNo] ;
	SwapData(&CSPValue) ;                   
    if(CSPValue  >= CSPLmt3){
      SETBITshort(&CSPCStatus[MinCasNo],BNo);
    }
    else if(CSPValue  >= CSPLmt2){
      SETBITshort(&CSPCStatus[MinCasNo],BNo+3);
    }
    else if(CSPValue  >= CSPLmt1){
      SETBITshort(&CSPCStatus[MinCasNo],BNo+6);
    }
    else if(CSPValue  > CSPNrmLmt){
      SETBITshort(&CSPCStatus[MinCasNo],BNo+9);
    }
    else if(CSPValue  <= CSPNrmLmt){
      SETBITshort(&CSPCStatus[MinCasNo],BNo+12);
    }
  }
}

/***** *** ** * Set CSP Current Status According to Previous Status  * ** *** *****/
SetCSPCurStsAcPreSts(short MinCasNo)
{
  short BNo ;

  // If  CurSts = All Gauges NRM 
  if(CSPCStatus[MinCasNo] == 0x7000) {
    return(0) ;
  }
  if(CSPCStatus[MinCasNo] == 0x0007) {
    return(0) ;
  }
  for(BNo=0; BNo<3; BNo++) {
    // If  CurSts = EME 
    if(TSTBITshort(&CSPCStatus[MinCasNo],BNo)) {
      continue ;
    }
    // If  CurSts = MJL
    if(TSTBITshort(&CSPCStatus[MinCasNo],BNo+3)) {
      if(TSTBITshort(&CSPPStatus[MinCasNo],BNo)) {			// If  PreSts = EME
        SETBITshort(&CSPCStatus[MinCasNo],BNo) ;				// SET CurSts = EME
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+3) ;
      }
      continue ;
    }
    // If  CurSts = MNL
    if(TSTBITshort(&CSPCStatus[MinCasNo],BNo+6)) {
      if(TSTBITshort(&CSPPStatus[MinCasNo],BNo)) {			// If  PreSts = EME
        SETBITshort(&CSPCStatus[MinCasNo],BNo) ;				// SET CurSts = EME
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+6) ;
      }
      else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+3)) {		// If  PreSts = MJL
        SETBITshort(&CSPCStatus[MinCasNo],BNo+3) ;			// SET CurSts = MJL
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+6) ;
      }
      continue ;
    }
    // If  CurSts < MNL & CurSts > NRM
    if(TSTBITshort(&CSPCStatus[MinCasNo],BNo+9)) {
      if(TSTBITshort(&CSPPStatus[MinCasNo],BNo)) {			// If  PreSts = EME
        SETBITshort(&CSPCStatus[MinCasNo],BNo) ;				// SET CurSts = EME
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+9) ;
      }
      else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+3)) {		// If  PreSts = MJL
        SETBITshort(&CSPCStatus[MinCasNo],BNo+3) ;			// SET CurSts = MJL
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+9) ;
      }
      else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+6)) {		// If  PreSts = MNL
        SETBITshort(&CSPCStatus[MinCasNo],BNo+6) ;			// SET CurSts = MNL
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+9) ;
      }
      else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+12)) {	// If  PreSts = MNL
        SETBITshort(&CSPCStatus[MinCasNo],BNo+12) ;			// SET CurSts = MNL
        CLRBITshort(&CSPCStatus[MinCasNo],BNo+9) ;
      }
      continue ;
    }
    // If  CurSts <= NRM  
    if(TSTBITshort(&CSPCStatus[MinCasNo],BNo+12)) {
    //  if(CSPCStatus != 0x7000) {
        if(TSTBITshort(&CSPPStatus[MinCasNo],BNo)) {		// If  PreSts = EME
          SETBITshort(&CSPCStatus[MinCasNo],BNo) ;				// SET CurSts = EME
          CLRBITshort(&CSPCStatus[MinCasNo],BNo+12) ;
        }
        else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+3)) {		// If  PreSts = MJL
          SETBITshort(&CSPCStatus[MinCasNo],BNo+3) ;			// SET CurSts = MJL
          CLRBITshort(&CSPCStatus[MinCasNo],BNo+12) ;
        }
        else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+6)) {		// If  PreSts = MNL
          SETBITshort(&CSPCStatus[MinCasNo],BNo+6) ;			// SET CurSts = MNL
          CLRBITshort(&CSPCStatus[MinCasNo],BNo+12) ;
        }
        else if(TSTBITshort(&CSPPStatus[MinCasNo],BNo+12)) {	// If  PreSts = MNL
          SETBITshort(&CSPCStatus[MinCasNo],BNo+12) ;			// SET CurSts = MNL
          CLRBITshort(&CSPCStatus[MinCasNo],BNo+9) ;
        }
        continue ;
      //} 
    }
  }
}

/******************************* Check CSP Status ******************/
void ChkCSP()
{
  short BNo ;

  MinChanBMP[0] =  MinChanBMP[1]  = 0 ;
  //*////////////////
  if(FirstTimeRun & 0x10) {
    FirstTimeRun &= 0xEF ;
    for(MinNo=0; MinNo<TOTMIN; MinNo++) {
      CSPCStatus[MinNo] = 0 ;
      SetCSPCurSts(MinNo) ;
      for(BNo=0; BNo<3; BNo++) {
		if(TSTBITshort(&CSPCStatus[MinNo],BNo+9)) {
		  SETBITshort(&CSPCStatus[MinNo],BNo+12) ;
		  CLRBITshort(&CSPCStatus[MinNo],BNo+9) ;
 	    }
	  }
      CSPPStatus[MinNo] = CSPCStatus[MinNo]  ;	 
	}
  }
  ///////////////*/
  for(MinNo=0; MinNo<TOTMIN; MinNo++) {
    if(!(C12PUSDT.MSERDT[MinNo] & CSP_MF)) {	//If CSP SER enable 
      CSPCStatus[MinNo] = 0 ;
      SetCSPCurSts(MinNo) ;
	  SetCSPCurStsAcPreSts(MinNo) ;
      if(CSPCStatus[MinNo] != CSPPStatus[MinNo]) {
        if(CSPSEDelay[MinNo] == 0) {			       // If delay is completed         
          CSPSEDelay[MinNo] = -1;                      // Set No delay
          SETBITchar(MinChanBMP,MinNo);    	 			// New by waseem              
        }
        if(CSPSEDelay[MinNo] < 0) {				       // If there is no delay
          //CSPSEDelay = 11 ;				           // Set Delay 10 for one second 
          CSPSEDelay[MinNo] = 5 ;				       // Set Delay
        } 
      }
      else{
        if(CSPSEDelay[MinNo] >= 0) {
          CSPSEDelay[MinNo] = -1 ;
        }
      }
    }
  }
  if(MinChanBMP[0] || MinChanBMP[1]) {
    SendCSPSE();
  }
}
 
/*************** *** ** */ 
void ProcAnaDataN(void)
{
  ChkFFR() ;
  ChkEHP() ;
  ChkVCP() ;
  ChkCSP() ;
}
/************************** End of File *********************/
