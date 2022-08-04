#include <P24FJ256GA106.h>
#include "kcsdf.h"
#include "procval.h"
#include "common.h"

void ProcessValves(void)
{
  int C12CasNo;

  ProcValIni();
  if (!FFRATLReq) 
    ProcValData(MainCas);	   	    	// Process Main Cascade Valves Data
  for (C12CasNo=MR41; C12CasNo<=MS22; C12CasNo++) {
    ProcValData(C12CasNo);
  }
  SendCommand();	                    // Send Command to Electronics
  ValSendSE();  	                    // Send ATL/ILC S.E.
  Proc_ATLReq();
}

int ProcValData(int CasNo)
{
   unsigned char *PrvPtr, *CurPtr;
   unsigned char VSERStat=0 ;
   int CfgSts=0, BypStsChg=0, F_VlvStsChg=0, vflag=0, BypFlag=0 ;
   int VpdChg=0, CFGDefined=0;
   int ATLRTY_CT=0, VlvILC=0,VlvVcdVsd=0;
   int CBmpNo, ByteNo, CFGNo;

   if (PCCSEFlag) return 0;				// Valve Command in process
//   if (CMD_CT) return 0;
   if (F_DR80AfterVCmd) return 0;		// DR-80 after valve command in process
   if (VAD_CT[CasNo]>0) return 0;       // Valve Action Delay not over
//   Chk_VLV_Mask(CasNo);
//   if (F_AutLine==0) CK_CCD();
   VpdChg = Chk_VLV_VPD(CasNo); 	// Check ang change in valve vpd
   if (CasNo==MainCas) { VSERStat = C12PUSDT.ESERDT; CBmpNo=0;}
   else                { VSERStat = C12PUSDT.MSERDT[CasNo]; CBmpNo=CasNo+1;}
   if (VSERStat & VLV_MF) {		// Update Valve Status if VLV_SER Dis
     UpdPrvStsFromCurBuf(CasNo);
     return 0;
   }
   if (CasNo==MainCas) {
     for (ByteNo=0; ByteNo<2; ByteNo++) {
       if (C12DAT.EVLVCDT[ByteNo] != (C12DAT.EVLVPDT[ByteNo])) {
	     F_VlvStsChg=1; break;
       }
	 }
   }
   else {
     PrvPtr = &C12DAT.MVLVPDT[CasNo][0];
     CurPtr = &C12DAT.MVLVCDT[CasNo][0];
     for (ByteNo=0; ByteNo<2; ByteNo++) {
       if (*CurPtr++ != *PrvPtr++) {
	     F_VlvStsChg=1; break;
       }
     }
     BypFlag = Chk_BYP_VLV(CasNo);
     if (BypFlag && (TSTBITchar(BYPBMP,CasNo))) F_VlvStsChg=1;
   }
   if (!F_VlvStsChg) {
     VSC_CT[CasNo] = STOPCT;
     VPD_CT[CasNo] = STOPCT;
     if (CasNo==MainCas) C12DAT.ECATLDT=0;
     else C12DAT.MCATLDT[CasNo] = 0;
     UpdVpdPrvBmp(CasNo);
     return 0;
   }

   if (CasNo==MainCas) ATLRTY_CT = C12DAT.ECATLDT;
   else ATLRTY_CT = C12DAT.MCATLDT[CasNo];
   if (TSTBITchar(CONBMP,CBmpNo)) VlvILC=1;
   else if ((!TSTBITchar(CONBMP,CBmpNo)) && (ATLRTY_CT==0)) VlvILC=1;
   else VlvILC=0;
   if (VlvILC) {
     if (CasNo==MainCas) {
		/*if (VCD_CT[MainCas]) PORTE=0x01;
  		if (VSD_CT[MainCas]) PORTE=0x02;
   		if (VPD_CT[MainCas]) PORTE=0x04;
  		if (VSC_CT[MainCas]) PORTE=0x08;
  		if (VAD_CT[MainCas]) PORTE=0x10;*/

       if ((V_EVPDM[0] != V_EVPDS[0]) || VpdChg) vflag=1;
       if (!vflag) VPD_CT[CasNo] = STOPCT;
     }
     else {
       if (V_MVPDM[CasNo] != V_MVPDS[CasNo] || VpdChg) vflag=1;
       if (!vflag) {
	 BypStsChg = (V_EVPDM[1] ^ V_EVPDS[1]) & 0x7F;
	 if (BypStsChg || BYPBIT[CasNo])
	      {SETBITchar(BYPBMP,CasNo);}
	 else CLRBITchar(BYPBMP,CasNo);
	 VPD_CT[CasNo] = STOPCT;
       }
     }
     if (vflag) {
       if (VPD_CT[CasNo] < 0) VPD_CT[CasNo]=VPD_DL;
       if (VPD_CT[CasNo]) return 0;
       VPD_CT[CasNo]=STOPCT;
       if (ATLRTY_CT==0 || ATLRTY_CT>=4) {
	 UpdVpdPrvBmp(CasNo);
	 UpdPrvStsFromCurBuf(CasNo);
	 ILCMOD[CasNo] = ILCMOD[CasNo] | VPD_MF;
	 if (CasNo==MainCas) C12DAT.ECATLDT=0;
	 else C12DAT.MCATLDT[CasNo]=0;
	 //if (CasNo==MainCas && VSD_CT[MainCas]) PORTE=0x02;
	     return 0;
       }
       else VlvVcdVsd = 1;
     }
   }
   if (!VlvVcdVsd) {
     if (VSC_CT[CasNo] < 0) VSC_CT[CasNo]=VAD_D4[CasNo];
     if (VSC_CT[CasNo]) return 0;
     VSC_CT[CasNo]=STOPCT;
     if (CasNo==MainCas) {
       Form_VLV_ECFG(EVLVCMD[0]);
       CFGNo = C12DAT.ECCFGDT;
     }
     else {
       Form_VLV_MCFG(MVLVCMD[CasNo],CasNo);
       CFGNo = C12DAT.MCFGDT[CasNo];
     }
     if (!ChkCFG(CFGNo,CasNo)) CfgSts=1;
     else if (ChkCFG(CFGNo,CasNo)) { 
       if (TSTBITchar(CONBMP,CBmpNo)) CfgSts=1;
       else CfgSts=0;
     }
     if (CfgSts) {
       UpdPrvStsFromCurBuf(CasNo);
       if (CasNo==MainCas) ATLRTY_CT = C12DAT.ECATLDT;
       else ATLRTY_CT = C12DAT.MCATLDT[CasNo];
       if (ATLRTY_CT==0) ILCMOD[CasNo] |= VLV_MF;
       //if (CasNo==MainCas && VPD_CT[MainCas]) PORTE=0x04;
       if (CasNo==MainCas) C12DAT.ECATLDT=0;
       else C12DAT.MCATLDT[CasNo]=0;
       return 0;
     }
   }

   if (CasNo==MainCas)
     CFGDefined = Form_VLV_ECFG(C12DAT.EVLVCDT[0]);
   else
     CFGDefined = Form_VLV_MCFG(C12DAT.MVLVCDT[CasNo][0],CasNo);

// Process Valves VSD
   if (!CFGDefined) {
     if (VSD_CT[CasNo]) return 0;
     if (CasNo==MainCas) ATLRTY_CT = ++C12DAT.ECATLDT;
     else ATLRTY_CT = ++C12DAT.MCATLDT[CasNo];
     if (ATLRTY_CT >= 4) {
       PutCasToHWC(CasNo);
       UpdVlvCmdFromCurBuf(CasNo);
     }
     ATLMOD[CasNo] |= VSD_MF;
     SETBITchar(VADBMP,CasNo);			//Set Valve Action Delay Bit in Bitmap
     VSD_CT[CasNo] = VSD_TM;
     F_ValCmdReq = 1;
     F_VCDVSD[CasNo] = 1;  // flags to update prev status from cmd for ATL_SE
     UpdPrvStsFromCmd(CasNo);
     return 0;
   }

// Process Valves VCD
   if (VCD_CT[CasNo]) return 0;
   if (CasNo==MainCas) ATLRTY_CT = ++C12DAT.ECATLDT;
   else ATLRTY_CT = ++C12DAT.MCATLDT[CasNo];
   if (ATLRTY_CT >= 4) {
     PutCasToHWC(CasNo);
     UpdVlvCmdFromCurBuf(CasNo);
   }
   ATLMOD[CasNo] |= VCD_MF;
   SETBITchar(VADBMP,CasNo);			//Set Valve Action Delay Bit in Bitmap
   VCD_CT[CasNo] = VCD_TM;
   F_ValCmdReq = 1;
   F_VCDVSD[CasNo] = 1;
   UpdPrvStsFromCmd(CasNo);
   return 0;
}

int Chk_VLV_VPD(int CasNo)
{
  unsigned char vpdchg, bypv ;
  int F_VVPD=0;

  if (CasNo==MainCas) {
    vpdchg = C12DAT.EVLVCDT[0] ^ C12DAT.EVLVCDT[1] ;
    V_EVPDM[0] = (~vpdchg) & 0x7F ;
    if (V_EVPDM[0] & V_EVPDS[0])
      F_VVPD = 1;			//prev 00/11 VPD and current 11/00 VPD
  }
  else {
    vpdchg = C12DAT.MVLVCDT[CasNo][0] ^ C12DAT.MVLVCDT[CasNo][1] ;
    V_MVPDM[CasNo] = (~vpdchg) & 0xFF ;
    if (V_MVPDM[CasNo] & V_EVPDS[CasNo])
      F_VVPD = 1;			//prev 00/11 VPD and current 11/00 VPD

    vpdchg = C12DAT.EVLVCDT[2] ^ C12DAT.EVLVCDT[3] ; /* Check Bypass VPD */
    vpdchg = (~vpdchg) & BYPBIT[CasNo] ;
    bypv = (~BYPBIT[CasNo]) & V_EVPDM[CasNo] ;
    V_EVPDM[1] = bypv | vpdchg ;
  }
  return F_VVPD;
}

void UpdPrvStsFromCurBuf(int CasNo)
{
// Update Valves Previous Status from Current Status
  short ByteNo;

  if (CasNo==MainCas)
    for (ByteNo=0; ByteNo<2; ByteNo++) //Update Main cascade Valve Prev Status
      C12DAT.EVLVPDT[ByteNo] = C12DAT.EVLVCDT[ByteNo] ;
  else {
    for (ByteNo=0; ByteNo<2; ByteNo++) //Update Mini cascade Valve Prev Status
      C12DAT.MVLVPDT[CasNo][ByteNo] = C12DAT.MVLVCDT[CasNo][ByteNo] ;
      UpdPrvBypFromCurBuf(CasNo);        //Update Bypass Valve Prev Status
  }
}

void UpdPrvBypFromCurBuf(int CasNo)
{
  unsigned char cbypsts, pbypsts;
  short ByteNo;

  for (ByteNo=2; ByteNo<4; ByteNo++) {
    cbypsts = C12DAT.EVLVCDT[ByteNo] & BYPBIT[CasNo] ;
    pbypsts = C12DAT.EVLVPDT[ByteNo] & (~BYPBIT[CasNo]) ;
    C12DAT.EVLVPDT[ByteNo] = cbypsts | pbypsts;
  }
}

void UpdPrvStsFromCmd(int CasNo)
{
// Update Valves Previous Status from Valves Command

  if (CasNo==MainCas) {                //Update Main cascade Valve Prev Status
    C12DAT.EVLVPDT[0] = EVLVCMD[0] & 0x7F;
    C12DAT.EVLVPDT[1] = (~EVLVCMD[0]) & 0x7F ;
  }
  else {                               //Update Mini cascade Valve Prev Status
    C12DAT.MVLVPDT[CasNo][0] = MVLVCMD[CasNo] ;
    C12DAT.MVLVPDT[CasNo][1] = (~MVLVCMD[CasNo]) & 0xFF;
    UpdPrvBypFromCmd(CasNo);           //Update Bypass Valve Prev Status
  }
}

void UpdPrvBypFromCmd(int CasNo)
{
  unsigned char pbypsts, cmdbypsts;

  cmdbypsts = EVLVCMD[1] & BYPBIT[CasNo] ;
  pbypsts = C12DAT.EVLVPDT[2] & (~BYPBIT[CasNo]) ;
  C12DAT.EVLVPDT[2] = cmdbypsts | pbypsts;

  cmdbypsts = (~EVLVCMD[1]) & BYPBIT[CasNo] ;
  pbypsts = C12DAT.EVLVPDT[3] & (~BYPBIT[CasNo]) ;
  C12DAT.EVLVPDT[3] = cmdbypsts | pbypsts;
}

void UpdVlvCmdFromCurBuf(int CasNo)
{
// Update Valves Command from Valves Current Buffer

  if (CasNo==MainCas) {                //Update Main cascade Valve Command
    EVLVCMD[0] = C12DAT.EVLVCDT[0] ;
    EVLVCMD[0] &= (~C12DAT.EVLVCDT[1]);
  }
  else {                               //Update Mini cascade Valve Command
    MVLVCMD[CasNo] = C12DAT.MVLVCDT[CasNo][0] ;
    MVLVCMD[CasNo] &= (~C12DAT.MVLVCDT[CasNo][1]) ;
    UpdBypCmdFromCurBuf(CasNo);        //Update Bypass Valve Command
  }
}

void UpdBypCmdFromCurBuf(int CasNo)
{
  unsigned char cmdbypsts, curbypsts;

  curbypsts = C12DAT.EVLVCDT[2] & BYPBIT[CasNo] ;
  cmdbypsts = EVLVCMD[1] & (~BYPBIT[CasNo]) ;
  EVLVCMD[1] = cmdbypsts | curbypsts;

  cmdbypsts = (~EVLVCMD[1]) & (~BYPBIT[CasNo]) ;
  curbypsts = C12DAT.EVLVCDT[3] & BYPBIT[CasNo] ;
  EVLVCMD[1] = (~(cmdbypsts | curbypsts)) & 0X7F;
}

void Chk_VLV_Mask(int CasNo)
{
// Previous and current status of valves are compared and if any change
//  occurs and corresponding valves are not masked then current  status
//  is updated.If valves are masked then previous status of that valves
//  becomes the current status.

  unsigned char *CurVPtr, *PrvVPtr, *MaskVPtr;

  if (CasNo==MainCas) {
    CurVPtr = &C12DAT.EVLVCDT[0];     // Main Cascade Valve Current data
    PrvVPtr = &C12DAT.EVLVPDT[0];     // Main Cascade Valve Previous data
    MaskVPtr = &V_EMask[0];            // Main cascade mask table
    MaskData(CurVPtr,PrvVPtr,MaskVPtr);
  }
  else {
    CurVPtr = &C12DAT.MVLVCDT[CasNo][0]; // Mini Cascade Valve Current data
    PrvVPtr = &C12DAT.MVLVPDT[CasNo][0]; // Mini Cascade Valve Previous data
    MaskVPtr = &V_MMask[CasNo];           // Mini cascade mask table
    MaskData(CurVPtr,PrvVPtr,MaskVPtr);
    CurVPtr = &C12DAT.EVLVCDT[2];     // Bypass Valve Current data
    PrvVPtr = &C12DAT.EVLVPDT[2];     // Bypass Valve Previous data
    MaskVPtr = &V_EMask[1];            // Bypass Valve mask table
    MaskData(CurVPtr,PrvVPtr,MaskVPtr);
  }
}

void MaskData(unsigned char *CurVPtr, unsigned char *PrvVPtr, unsigned char *MaskVPtr)
{
 short ByteNo;
 unsigned char ValStat[2];

  for (ByteNo = 0; ByteNo < 2; ByteNo++) {
    ValStat[ByteNo] = *CurVPtr & ~(*MaskVPtr) ;
    *CurVPtr++ = ValStat[ByteNo] | (*PrvVPtr++ & (*MaskVPtr)) ;
  }
}

int Chk_BYP_VLV(int CasNo)
{
 int ByteNo, chgflag=0;
 unsigned char BypVSts, CurSts, PrvSts;

 for (ByteNo=2; ByteNo<4; ByteNo++) {
   CurSts = C12DAT.EVLVCDT[ByteNo] & 0x7F ;
   PrvSts = C12DAT.EVLVPDT[ByteNo] & 0x7F ;
   if (CurSts != PrvSts) {
      BypVSts = (CurSts ^ PrvSts) & 0x7F;
      if (BypVSts) {
        if (BypVSts & BYPBIT[CasNo]) {
	      {SETBITchar(BYPBMP,CasNo)}; chgflag=1;
        }
        else { CLRBITchar(BYPBMP,CasNo); }
	  }
   }
 }
 return chgflag;
}

void UpdVpdPrvBmp(int CasNo)
{
  unsigned char cvpdsts, pvpdsts;

  if (CasNo==MainCas) V_EVPDS[0] = V_EVPDM[0];
  else {
    V_MVPDS[CasNo] = V_MVPDM[CasNo];
    cvpdsts = V_EVPDM[1] & BYPBIT[CasNo];
    pvpdsts = V_EVPDS[1] & (~BYPBIT[CasNo]);
    V_EVPDS[1] = cvpdsts | pvpdsts;
  }
}

int Form_VLV_ECFG(unsigned char CurCFG)
{
  int CFGNo, ECFGNo=0;

  for (CFGNo=0; CFGNo<18; CFGNo++) {
    if (MainCFG[CFGNo] == CurCFG) {
      ECFGNo = CFGNo+1; break;
    }
  }
  C12DAT.ECCFGDT = ECFGNo;

  return ECFGNo;
}

int Form_VLV_MCFG(unsigned char CurCFG, int CasNo)
{
  int CFGNo, CasCFGNo=0;

  for (CFGNo=0; CFGNo<6; CFGNo++) {
    if (MinCFG[CFGNo] == CurCFG) {
      CasCFGNo = CFGNo+1; break;
    }
  }
  C12DAT.MCFGDT[CasNo] = CasCFGNo;

  return CasCFGNo;
}

int ChkCFG(int ActTobeTaken,int CasNo)
{
// if CurCFG=ReqCFG CFGSts=0 else 1
  int CFGSts=1;

  if (CasNo==MainCas) {
    Form_VLV_ECFG(C12DAT.EVLVCDT[0]);
    if ((ActTobeTaken==C12DAT.ECCFGDT) || ActTobeTaken==RECIRC_CFG) 
      CFGSts=0;
  }
  else {
    Form_VLV_MCFG(C12DAT.MVLVCDT[CasNo][0],CasNo);
    if (ActTobeTaken==C12DAT.MCFGDT[CasNo] || ActTobeTaken==RECIRC_CFG) 
      CFGSts=0;
  }
  return CFGSts;
}

void PutCasToHWC(int CasNo)
{
 short CBmpNo;

  if (CasNo==MainCas) CBmpNo = 0;
  else                CBmpNo = CasNo+1;
  SETBITchar(CONBMP,CBmpNo);
}

void StartVAD(int CasNo)
{
  if (TSTBITchar(VADBMP,CasNo))  {
    VActionDelay(CasNo);
    CLR_VPD(CasNo);
    CLRBITchar(VADBMP,CasNo);		        //Clear Valve Action Delay Bit in Bitmap
  }
}

void VActionDelay(int CasNo)
{
    if (CasNo==MainCas) {
      EVLVCMD[0] &= 0x7F;
      if (EVLVCMD[0]==EGROUP[0])
	    VAD_CT[CasNo] = VAD_D1[CasNo]; 	// Group-1 Valves
      else if (EVLVCMD[0]==EGROUP[1])
	    VAD_CT[CasNo] = VAD_D2[CasNo];      // Group-2 Valves
      else  VAD_CT[CasNo] = VAD_D3[CasNo];      // Group-3 Valves
    }
    else VAD_CT[CasNo] = VAD_D2[CasNo];         // For Minis, all valves DPs
    F_DR80AfterVCmd=1;
}

void CLR_VPD(int CasNo)
{
  if (CasNo==MainCas) V_EVPDS[0]=0;
  else {
    V_MVPDS[CasNo]=0;
    V_EVPDS[1] = V_EVPDS[1] & (~BYPBIT[CasNo]);
  }
}

void IniValVar(void)
{
  short CasNo,ByteNo;

  for (ByteNo=0; ByteNo<2; ByteNo++) {
    V_EVPDM[ByteNo]=0;
    V_EVPDS[ByteNo]=0;
//    VLV_SM[ByteNo]=0;
    V_EMask[ByteNo]=0;
    BYPBMP[ByteNo]=0;
  }

  for (CasNo=0; CasNo<TOTCAS; CasNo++) {
    V_MVPDM[CasNo]=0;
    V_MVPDS[CasNo]=0;
    if (CasNo!=MainCas) {
      V_MMask[CasNo]=0;
    }
    //CSERSts[CasNo] = 0x20;		// Disable all Parameter SER
    if (CasNo==MainCas) VAD_D1[CasNo] = 51 ; 	// 5+1 sec (1 tick=100ms)
    else                VAD_D1[CasNo] = 31 ; 	// 3+1 sec
    VAD_D2[CasNo] = 31;				// 3+1 sec
    if (CasNo==MainCas) VAD_D3[CasNo] = 31 ; 	// 3+1 sec
    else                VAD_D3[CasNo] = 11 ; 	// 1+1 sec
    VAD_D4[CasNo] = 51;				// 5+1 sec

/*
    if (CasNo==MainCas) VAD_D1[CasNo] = 6 ; 	// 5+1 sec (1 tick=100ms)
    else                VAD_D1[CasNo] = 4 ; 	// 3+1 sec
    VAD_D2[CasNo] = 4;				// 3+1 sec
    if (CasNo==MainCas) VAD_D3[CasNo] = 4 ; 	// 3+1 sec
    else                VAD_D3[CasNo] = 2 ; 	// 1+1 sec
    VAD_D4[CasNo] = 6;				// 5+1 sec
*/
//    if (CasNo!=MainCas) CSP_DL[CasNo] = 11 ;  // 1+1 sec
//    if (CasNo!=MainCas) MCHSE_DL[CasNo] = 320;// 32 sec
/*    if (CasNo!=MainCas) {
      C12PUSDT.MCSPLDT[CasNo][0] = CSPNormal;
      C12PUSDT.MCSPLDT[CasNo][1] = CSPLimit1;
      C12PUSDT.MCSPLDT[CasNo][2] = CSPLimit2;
      C12PUSDT.MCSPLDT[CasNo][3] = CSPLimit3;
      C12PUSDT.MVCPLDT[CasNo][0] = VCPNormal;
      C12PUSDT.MVCPLDT[CasNo][1] = CSPLimit1;
    }*/
//    SETBITchar(CONBMP,CasNo);
    VAD_CT[CasNo] = 0;
    VSD_CT[CasNo] = 0;
    VCD_CT[CasNo] = 0;
//    ATLRTY_CT[CasNo] = 0;
    if (CasNo==MainCas) C12DAT.ECATLDT=0;
    else C12DAT.MCATLDT[CasNo] = 0;
    VSC_CT[CasNo] = STOPCT;
    VPD_CT[CasNo] = STOPCT;
  }
  FIM_DL = STOPCT;

/*C12PUSDT.ELMTDT[0] = EHPNormal;
  C12PUSDT.ELMTDT[1] = EHPLimit1;
  C12PUSDT.ELMTDT[2] = EHPLimit2;
  C12PUSDT.ELMTDT[3] = FFRNormal;
  C12PUSDT.ELMTDT[4] = EHPLimit1;
  CONBMP[0] = 0xFF; CONBMP[1] = 0x1F;
*/
  /*for (ByteNo=0; ByteNo<4; ByteNo++)
    C12DAT.EVLVPDT[ByteNo] = C12DAT.EVLVCDT[ByteNo];
  for (ByteNo=0; ByteNo<12; ByteNo++) {
    for (i=0; i<12; i++)
      C12DAT.MVLVPDT[ByteNo][i] = C12DAT.MVLVCDT[ByteNo][i];
  }
  for (ByteNo=0,i=0; ByteNo<2; ByteNo++, i++) {
    EVLVCMD[ByteNo] = C12DAT.EVLVCDT[i++];
    EVLVCMD[ByteNo] &= (~C12DAT.EVLVCDT[i]);   	// Update Valves Command
  }
  Chk_VLV_ECFG(EVLVCMD[0]);
  //Chk_VLV_Mask(MainCas);
  //Chk_VLV_VPD(MainCas); 			// Check ang change in valve vpd
  UpdVpdPrvBmp(MainCas);
  for (CasNo=0; CasNo<TOTCAS; CasNo++)
    Chk_VLV_MCFG(MVLVCMD[CasNo],CasNo);
*/
  F_AutLine=1;
//  F_CCA=1;
//  SETCON();
//  CCDPSTS = 0xC0 ;
}

void CK_CCD(void)
{
  F_CCD = 0;
  SETCON();
  if (CCDCSTS ^ CCDPSTS) {
    CCDPSTS = CCDCSTS; F_CCD=1;
  }
}

void SETCON(void)
{
  CCDCSTS = 0;
  if (F_AutLine) CCDCSTS |= 0x40;   // Set CC Command in CCD current Status
   if (F_CCA)    CCDCSTS |= 0x80;   // Set CC Acknowledgement in CCD current Status
}


void ProcValIni()
{
 int CasNo, i; 

 for (CasNo=0; CasNo<TOTCAS; CasNo++) {
   ATLMOD[CasNo] = 0;
   ILCMOD[CasNo] = 0;
 }
for (i=0; i<2; i++) {
     BYPBMP[i] = 0;
//   ATLBMP[i] = 0;
//   ILCBMP[i] = 0;
 }
}

void SendCommand()
{
  int CNo, CasNo;

  F_AutLine=1;
  if (F_ValCmdReq) {
    F_ValCmdReq = 0;
    for (CasNo=0; CasNo<TOTCAS; CasNo++) {
      if (CasNo==MainCas) CNo = 0;
      else              CNo = CasNo+1;
      if (!TSTBITchar(CONBMP,CNo)) {
        F_AutLine = 0; break;
      }
    }
    //CMD_CT = DR_DL;
    CMD_CT = 10;
    F_DR80AfterVCmd = 1  ;
    PCCSEFlag = 1 ;
  }
}

void ValSendSE()
{
  int CasNo;

  for (CasNo=0; CasNo<TOTCAS; CasNo++) {
    if (ILCMOD[CasNo]) {
      ILCTxReq = 1;
      SETBITchar(ILCCasBmp,CasNo);
      if (CasNo==MainCas)
        ILCECasMods |= ILCMOD[CasNo];
      else
        ILCMinMods[CasNo] |= ILCMOD[CasNo];
      ILCDatMods |= CPS_MF ;
    }
    if (ATLMOD[CasNo]) {
      ATLTxReq = 1;
      SETBITchar(ATLCasBmp,CasNo);
      if (CasNo==MainCas)
	    ATLECasMods |= ATLMOD[CasNo];
      else
	    ATLMinMods[CasNo] |= ATLMOD[CasNo];
      ATLDatMods |= CPS_MF ;
    }
  }
}


void Proc_ATLReq()
{
  int MCas, CFGNo=0;

  if (FFRATLReq) {
    FFRATLReq = 0;
    if (TSTBITchar(&EVLVCMD[0],FFRBitNo)) {
      EVLVCMD[0] &= 0xFE;					// Close MFV in Main Cascade
      UpdPrvStsFromCmd(MainCas);
      F_ValCmdReq = 1;
      SETBITchar(VADBMP,MainCas);			    //Set Valve Action Delay Bit in Bitmap
      SendCommand();	                    // Send Command to Electronics
      VActionDelay(MainCas); 
      SETBITchar(ATLCasBmp,MainCas);
      ATLECasMods |= FIM_MF;
      ATLDatMods |= CPS_MF ;
      ATLTxReq = 1;
    }
    else SetFFRILC_SE();
  }

  for (MCas=0; MCas<TOTMIN; MCas++) {
    if (CSPATLReq[MCas]) {
      CSPATLReq[MCas] = 0;
      F_ValCmdReq = 1;
      if (CSPATLMOD[MCas]==VEV_MF) CFGNo=2;
      else if (CSPATLMOD[MCas]==REC_MF) CFGNo=1;
      Change_VLV_MCFG(CFGNo, MCas);
      SETBITchar(VADBMP,MCas);			//Set Valve Action Delay Bit in Bitmap
      SendCommand();	                    // Send Command to Electronics
      VActionDelay(MainCas); 
      SETBITchar(ATLCasBmp,MCas);
      ATLMinMods[MCas] |= CSPATLMOD[MCas];
      ATLTxReq = 1;
    }
//    else SetCSPILC_SE();
  }
}

void Change_VLV_MCFG(int CFGNo, int CasNo)
{
  MVLVCMD[CasNo] = MinCFG[CFGNo-1];			//Change required CFG
  Form_VLV_MCFG(MVLVCMD[CasNo],CasNo);
  UpdPrvStsFromCmd(CasNo);

}