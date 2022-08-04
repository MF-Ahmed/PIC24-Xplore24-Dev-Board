#include <P24FJ256GA106.h>
#include "kcsdf.h"
#include "common.h"
#include "procval.h"

void ProcessValves(void)
{
  int CasNo;

  ProcValIni(); 
  PORTE=0x80;
  ProcVal(MainCas);	          	        // Process Main Cascade Valves Data
  for (CasNo=R41; CasNo<=S22; CasNo++) 
    ProcVal(CasNo);                 	// Process Mini Cascade Valves Data
  
  SendCommand();	                    // Send Command to Electronics
  ValSendSE();  	                    // Send ATL/ILC S.E.
  PORTE=0x00;
}

int ProcVal(int CasNo)
{
   int VlvStsChg=0,F_VlvStsChg=0, vflag=0, CNo;
   int VcdVsdFlag, VpdChg, ByteNo, CFGDefined;
   unsigned char SERStat, *PrvPtr, *CurPtr;

//   if (F_VCMD) return 0;
//   if (F_VCMD80) return 0;
//   if (VAD_CT[CasNo]>0) return 0;
//   Chk_VLV_Mask(CasNo);

   VpdChg = Chk_VLV_VPD(CasNo); 	// Check ang change in valve vpd
   if (CasNo==MainCas) SERStat = C12PUSDT.ESERDT;
   else                SERStat = C12PUSDT.MSERDT[CasNo];
   if (SERStat & VLV_MF) {		// Update Valve Status if VLV_SER Dis
     UpdPrvStsFromCurBuf(CasNo);
 //test    return 0;
   }
   if (CasNo==MainCas) {
     for (ByteNo=0; ByteNo<2; ByteNo++)
       if (C12DAT.EVLVCDT[ByteNo] != (C12DAT.EVLVPDT[ByteNo])) {
	     F_VlvStsChg=1; break;
       }
   }
   else {
     PrvPtr = &C12DAT.MVLVPDT[CasNo][0];
     CurPtr = &C12DAT.MVLVCDT[CasNo][0];
     for (ByteNo=0; ByteNo<2; ByteNo++)
       if (*CurPtr++ != *PrvPtr++) {
	     F_VlvStsChg=1; break;
       }
     if (Chk_BYP_VLV(CasNo)) F_VlvStsChg=1;
   }
   if (!F_VlvStsChg) {
     VSC_CT[CasNo] = STOPCT;
     VPD_CT[CasNo] = STOPCT;
     ATLRTY_CT[CasNo] = 0;
     UpdVpdPrvBmp(CasNo);
 //test    return 0;
   }
   if (ATLRTY_CT[CasNo]==0) {
     if (CasNo==MainCas) {
       if ((V_EVPDM[0] != V_EVPDS[0]) || VpdChg) vflag=1;
       if (!vflag) VPD_CT[CasNo] = STOPCT;
     }
     else {
       if (V_MVPDM[CasNo] != V_MVPDS[CasNo] || VpdChg) vflag=1;
       VlvStsChg = (V_EVPDM[1] ^ V_EVPDS[1]) & 0x7F;
//       if (VlvStsChg) ADJ_BYP_VLV();
       if (VlvStsChg & BYPBIT[CasNo]) { SETBITchar(BYPBMP,CasNo); }
       else                           CLRBITchar(BYPBMP,CasNo);
       VPD_CT[CasNo] = STOPCT;
     }
     if (vflag) {
       if (VPD_CT[CasNo] == -1) VPD_CT[CasNo]=VPD_DL;
       if (VPD_CT[CasNo]) return 0;
       VPD_CT[CasNo]=STOPCT;
//       if (F_HWC[CasNo] || ATLRTY_CT[CasNo]==0) {
       if (CasNo==MainCas) CNo = 0;
       else                CNo = CasNo;
       if (TSTBITchar(CONBMP,CNo) || ATLRTY_CT[CasNo]==0) {
	 UpdVpdPrvBmp(CasNo);
	 UpdPrvStsFromCurBuf(CasNo);
	 ILCMOD[CasNo] = ILCMOD[CasNo] | VPD_MF;
	 return 0;
       }
       else {
	 if (ATLRTY_CT[CasNo]>=4) ATLRTY_CT[CasNo] = 0;
	 else VcdVsdFlag = 1;
       }
     }
   }
   if (!VcdVsdFlag) {
     if (VSC_CT[CasNo] == -1) VSC_CT[CasNo]=VAD_D4[CasNo];
     if (VSC_CT[CasNo]) return 0;
     VSC_CT[CasNo]=STOPCT;
     if (CasNo==MainCas) CNo = 0;
     else                CNo = CasNo;
//     if (F_HWC[CasNo]) {
     if (TSTBITchar(CONBMP,CNo)) {
       UpdPrvStsFromCurBuf(CasNo);
       if (ATLRTY_CT[CasNo]==0) ILCMOD[CasNo] |= VLV_MF;
       ATLRTY_CT[CasNo] = 0;
     }
   }
   if (CasNo==MainCas) 
     CFGDefined = Chk_VLV_ECFG(EVLVCMD[0]);
   else
     CFGDefined = Chk_VLV_MCFG(MVLVCMD[CasNo],CasNo);
   if (!CFGDefined) {
     if (VSD_CT[CasNo]) return 0;
     ATLRTY_CT[CasNo]++ ;
     if (ATLRTY_CT[CasNo] >= 4) {
       PutCasToHWC(CasNo);
       UpdVlvCmdFromCurBuf(CasNo);
     }
     ATLMOD[CasNo] |= VSD_MF;
     SBITVAD(CasNo);
     VSD_CT[CasNo] = VSD_TM;
     F_VCMD = 1;
     F_VCDVSD[CasNo] = 1;
     return 0;
   }
   if (VCD_CT[CasNo]) return 0;
   ATLRTY_CT[CasNo]++ ;
   if (ATLRTY_CT[CasNo] >= 4) {
     PutCasToHWC(CasNo);
     UpdVlvCmdFromCurBuf(CasNo);
   }
   ATLMOD[CasNo] |= VCD_MF;
   SBITVAD(CasNo);
   VCD_CT[CasNo] = VCD_TM;
   F_VCMD = 1;
   F_VCDVSD[CasNo] = 1;
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
    bypv = ~BYPBIT[CasNo] & V_EVPDM[CasNo] ;
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
    C12DAT.EVLVPDT[0] = EVLVCMD[0] ;
    C12DAT.EVLVPDT[1] = (~EVLVCMD[0]) ;
  }
  else {                               //Update Mini cascade Valve Prev Status
    C12DAT.MVLVPDT[CasNo][0] = MVLVCMD[CasNo] ;
    C12DAT.MVLVPDT[CasNo][1] = ~MVLVCMD[CasNo];
    UpdPrvBypFromCmd(CasNo);           //Update Bypass Valve Prev Status
  }
}

void UpdPrvBypFromCmd(int CasNo)
{
  unsigned char pbypsts, cmdbypsts;

  cmdbypsts = EVLVCMD[0] & BYPBIT[CasNo] ;
  pbypsts = C12DAT.EVLVPDT[0] & (~BYPBIT[CasNo]) ;
  C12DAT.EVLVPDT[2] = cmdbypsts | pbypsts;

  cmdbypsts = (~EVLVCMD[0]) & BYPBIT[CasNo] ;
  pbypsts = C12DAT.EVLVPDT[1] & (~BYPBIT[CasNo]) ;
  C12DAT.EVLVPDT[3] = cmdbypsts | pbypsts;
}

void UpdVlvCmdFromCurBuf(int CasNo)
{
// Update Valves Command from Valves Current Buffer

  if (CasNo==MainCas) {                //Update Main cascade Valve Command
    EVLVCMD[0] = C12DAT.EVLVCDT[0] ;
    EVLVCMD[0] &= ~C12DAT.EVLVCDT[1];
  }
  else {                               //Update Mini cascade Valve Command
    MVLVCMD[CasNo] = C12DAT.MVLVPDT[CasNo][0] ;
    MVLVCMD[CasNo] &= ~C12DAT.MVLVPDT[CasNo][1] ;
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
  EVLVCMD[1] = ~(cmdbypsts | curbypsts);
}

void Chk_VLV_Mask(int CasNo)
{
// Previous and current status of valves are compared and if any change
//  occurs and corresponding valves are not masked then current  status
//  is updated.If valves are masked then previous status of that valves
//  becomes the current status.

  unsigned char *CurVPtr, * PrvVPtr, *MaskVPtr;

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
 unsigned char BypVSts;

 for (ByteNo=2; ByteNo<4; ByteNo++) {
   if (C12DAT.EVLVCDT[ByteNo] != C12DAT.EVLVPDT[ByteNo]) {
      BypVSts = C12DAT.EVLVCDT[ByteNo] ^ C12DAT.EVLVPDT[ByteNo];
      BypVSts &= 0x7F;
      if (BypVSts & BYPBIT[CasNo]) {
	    SETBITchar(BYPBMP,CasNo); chgflag=1;
      }
      else { CLRBITchar(BYPBMP,CasNo); }
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

int Chk_VLV_ECFG(unsigned char CurCFG)
{
  int CFGNo, ECFGNo=0;

  for (CFGNo=0; CFGNo<19; CFGNo++) {
    if (ECFG[CFGNo] == CurCFG) {
      ECFGNo = CFGNo; break;
    }
  }
  V_ECFGNo = ECFGNo;
  return ECFGNo;
}

int Chk_VLV_MCFG(unsigned char CurCFG, int CasNo)
{
  int CFGNo, CasCFGNo=0;

  for (CFGNo=1; CFGNo<7; CFGNo++) {
    if (MCFG[CFGNo] == CurCFG) {
      CasCFGNo = CFGNo; break;
    }
  }
  V_MCFGNo[CasNo] = CasCFGNo;
  return CasCFGNo;
}

void PutCasToHWC(int CasNo)
{
 short CNo;

  if (CasNo==MainCas) CNo = 0;
  else                CNo = CasNo;
  SETBITchar(CONBMP,CNo);
  F_HWC[CasNo]=1;
}

void SBITVAD(int CasNo)
{
  SETBITchar(VADBMP,CasNo);			//Set Valve Action Delay Bit in Bitmap
}

void StartVAD(int CasNo)
{
  if (TSTBITchar(VADBMP,CasNo)) {
    if (CasNo==MainCas) {
      if (EVLVCMD[0]==EGROUP[0])
	    VAD_CT[CasNo] = VAD_D1[CasNo]; 	// Group-1 Valves
      else if (EVLVCMD[0]==EGROUP[1])
	    VAD_CT[CasNo] = VAD_D2[CasNo];      // Group-2 Valves
      else  VAD_CT[CasNo] = VAD_D3[CasNo];      // Group-3 Valves
    }
    else VAD_CT[CasNo] = VAD_D2[CasNo];         // For Minis, all valves DPs
    CLRBITchar(VADBMP,CasNo);		        //Clear Valve Action Delay Bit in Bitmap
    CLR_VPD(CasNo);
  }
}

void CLR_VPD(int CasNo)
{
  if (CasNo==MainCas) V_EVPDS[0]=0;
  else {
    V_MVPDS[CasNo]=0;
    V_EVPDS[1] = V_EVPDS[1] & (~BYPBIT[CasNo]);
  }
}

void IniVar(void)
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
    CSERSts[CasNo] = 0x20;		// Disable all Parameter SER
    if (CasNo==MainCas) VAD_D1[CasNo] = 51 ; 	// 5+1 sec (1 tick=100ms)
    else                VAD_D1[CasNo] = 31 ; 	// 3+1 sec
    VAD_D2[CasNo] = 31;				// 3+1 sec
    if (CasNo==MainCas) VAD_D3[CasNo] = 31 ; 	// 3+1 sec
    else                VAD_D3[CasNo] = 11 ; 	// 1+1 sec
    VAD_D4[CasNo] = 51;				// 5+1 sec
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
    F_HWC[CasNo]=1;
    SETBITchar(CONBMP,CasNo);
    VAD_CT[CasNo] = 0;
    VSD_CT[CasNo] = 0;
    VCD_CT[CasNo] = 0;
    ATLRTY_CT[CasNo] = 0;
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
  for (ByteNo=0; ByteNo<4; ByteNo++)
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
  Chk_VLV_Mask(MainCas);
  Chk_VLV_VPD(MainCas); 			// Check ang change in valve vpd
  UpdVpdPrvBmp(MainCas);
  for (CasNo=0; CasNo<TOTCAS; CasNo++)
    Chk_VLV_MCFG(MVLVCMD[CasNo],CasNo);

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
   ATLBMP[i] = 0;
   ILCBMP[i] = 0;
 }
}

void SendCommand()
{
  int CNo, CasNo;

  F_AutLine=1;
  if (F_VCMD) {
    F_VCMD = 0;
    for (CNo=0; CNo<TOTCAS; CNo++) {
      if (CNo==MainCas) CasNo = 0;
      else              CasNo = CNo;
      if (!TSTBITchar(CONBMP,CasNo)) {
	F_AutLine = 0; break;
      }
    }
    CMD_CT = DR_DL;
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
    }
    if (ATLMOD[CasNo]) {
      ATLTxReq = 1;
      SETBITchar(ATLCasBmp,CasNo);
      if (CasNo==MainCas)
	ATLECasMods |= ATLMOD[CasNo];
      else
	ATLMinMods[CasNo] |= ATLMOD[CasNo];
    }
  }
}



