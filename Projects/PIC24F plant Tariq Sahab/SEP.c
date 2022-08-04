//#include <P24HJ256GP610.h>
/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   SEP.c - 													    * *                  												* *             								                        *
\*******************************************************************/
#include <P24FJ256GA106.h>
#include <stdio.h>

#include "KCSDF.h"
#include "Common.h"

void UpdateLimits(void)
{
  C12PUSPtr->ERTCDT[0]=0x08;C12PUSPtr->ERTCDT[1]=0x09;
  C12PUSPtr->ERTCDT[2]=0x0F;C12PUSPtr->ERTCDT[3]=0x0C;
  C12PUSPtr->ERTCDT[4]=0x00;C12PUSPtr->ERTCDT[5]=0x00;

  C12PUSPtr->EPRSMS=0;
  C12PUSPtr->EVLVMS[0]=0; C12PUSPtr->EVLVMS[1]=0;
  C12PUSPtr->ESERDT=0xff;
  for(i=0; i<4; i++)
    C12PUSPtr->EVDYDT[i]=3;
  C12PUSPtr->EPDYDT=1;
  //*
  C12PUSPtr->ELMTDT[0]=0xFA00; 
  C12PUSPtr->ELMTDT[1]=0xF401; 
  C12PUSPtr->ELMTDT[2]=0xA00F; 
  //C12PUSPtr->ELMTDT[3]=0xAA05;	// Safty & Normal Difference = 5
  C12PUSPtr->ELMTDT[3]=0x1405;		// Safty & Normal Difference = 20 
  C12PUSPtr->ELMTDT[4]=0xDC05; 
  //*/
  /*
  C12PUSPtr->ELMTDT[0]=0x00FA; 
  C12PUSPtr->ELMTDT[1]=0x01F4; 
  C12PUSPtr->ELMTDT[2]=0x0FA0; 
  //C12PUSPtr->ELMTDT[3]=0x05AA;	// Safty & Normal Difference = 5
  C12PUSPtr->ELMTDT[3]=0x0514;		// Safty & Normal Difference = 20 
  C12PUSPtr->ELMTDT[4]=0x05DC; 
  */
  for(i=0; i<TOTMIN; i++) {
    for(j=0; j<7; j++) {
	  C12PUSPtr->MMCHMS[i][j]=0;
    }
	C12PUSPtr->MPRSMS[i]=0;
	C12PUSPtr->MVLVMS[i]=0;
	C12PUSPtr->MSERDT[i]=0xff;
    for(j=0; j<4; j++) {
	  C12PUSPtr->MVDYDT[i][j]=3;
    }
	C12PUSPtr->MPDYDT[i]=1;
	C12PUSPtr->MMDYDT[i]=16;
  }
  for(i=0; i<TOTMIN; i++) {
	C12PUSPtr->MCSPLDT[i][0]=0xD80E ;
	C12PUSPtr->MCSPLDT[i][1]=0xA00F ;
	C12PUSPtr->MCSPLDT[i][2]=0x8813 ;
	C12PUSPtr->MCSPLDT[i][3]=0x7017 ;

	C12PUSPtr->MVCPLDT[i][0]=0x5510;
	C12PUSPtr->MVCPLDT[i][1]=0xFD16;
  }
}

//void SendILC_SE(char Mods[])
/*
void SetILCMod(DataMF, ECasMF, Min, MinMF)
unsigned char DataMF, ECasMF, Min, MinMF;
{
  ILCMods[0]   |= DataMF;
  ILCMods[1]   |= ECasMF;
  ILCMods[Min] |= MinMF;
  if(DataMF || ECasMF || MinMF) ILCTxReq=1;
}
*/
void ProcessSlaveSPF_SE(void)
{
  t2Buf[0]=SPF_SE; t2Buf[1]=K_CMC; SRSE=0; t2Cnt=2;
}

void ProcessSlaveSBF_SE(void)
{
  t2Buf[0]=SBF_SE; t2Buf[1]=K_CMC; SRSE=SBF_SE; t2Cnt=2;
}

void ProcessSlaveACK_SE(void)
{
  t2Buf[0]=ACK_SE; t2Buf[1]=(RSE|K_CMC); SRSE=0; t2Cnt=2;
}

void ProcessSlaveRTY_SE(void)
{
  t2Buf[0]=RTY_SE; t2Buf[1]=K_CMC; SRSE=0; t2Cnt=2;
}

void ProcessSlaveRST_SE(void)
{
  t2Buf[0]=RST_SE; t2Buf[1]=K_CMC; SRSE=RST_SE; t2Cnt=2;
}

void ProcessSlaveCMF_SE(void)
{
  unsigned int num;
  unsigned char *cptr;

  //if(SlaveCMF==0) return 0;
  cptr=(unsigned char*)&num;
  num=CMF_SE_DataBytes; num <<= 3; num |= K_CMC;
  t2Buf[0] = 0x80; t2Buf[0] |= CMF_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  t2Buf[3] = SlaveCMF;								// Modifier  
  t2Cnt=4;
  if((SlaveCMF&0x80)!=0x80) MDelay=2;				// Delay for MSE Queue
  SlaveCMF=0; SRSE=CMF_SE;

  if(SlaveCMF&0x80) {								// CMF_SE for Electronics
    DRTmoCnt=0x8000; DRDataTmoCnt=0x8000;
    DRAckRecd=0; DRDataRecd=0;
    DRAck=0;  DRCnt=0; DRDelay=15;
    DRInProgress=0;
  }
  CONBMP[0]=CONBMP[1]=0xFF; CHSEFlag=1;				// 
}

void ProcessSlaveSFT_SE(void)
{
  unsigned int num;
  unsigned char *cptr;

  cptr=(unsigned char*)&num;
  num=SFT_SE_DataBytes; num <<= 3; num |= K_CMC;
  t2Buf[0] = 0x80; t2Buf[0] |= SFT_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  t2Buf[3] = 0xFF;									// Modifier 
  t2Buf[4] = 0xFF;									// Data
  t2Cnt=5; SRSE=SFT_SE;
}

/*
void ProcessSlaveALC_SE(void)
{
  unsigned int num;
  unsigned char *cptr;

  cptr=(unsigned char*)&num;
  num=ALC_SE_DataBytes; num <<= 3; num |= K_CMC;
  t2Buf[0] = 0x80; t2Buf[0] |= ALC_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  t2Buf[3] = 0x01;									// Modifier
  t2Cnt=ALC_SE_DataBytes+3; SRSE=ALC_SE;
  cptr=(unsigned char*)CMCPtr->FLODT;
  for(num=0; num<71; num++)
    t2Buf[4+num] = *cptr++;
}

void ProcessSlaveWLC_SE(void)
{
  unsigned int num;
  unsigned char *cptr;

  cptr=(unsigned char*)&num;
  num=WLC_SE_DataBytes; num <<= 3; num |= K_CMC;
  t2Buf[0] = 0x80; t2Buf[0] |= WLC_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  t2Buf[3] = 0x01;									// Modifier
  t2Cnt=WLC_SE_DataBytes+3; SRSE=WLC_SE;
  cptr=(unsigned char*)CMCPtr->FLODT;
  for(num=0; num<71; num++)
    t2Buf[4+num] = *cptr++;
}
*/

/*
	Steps For Sending ATL_SE
	------------------------
	1.	Set cascade bitmap ATLCasBmp[2]
	2.	Set Modifiers VCD_MF, VSD_MF, FIM_MF in ATLECasMods
	3.	Set Modifiers VCD_MF, VSD_MF, REC_MF, VEV_MF in ATLMinMods[12]
	4.	Set ATLTxReq=1 to send ATL_SE

	NOTE:-
		1.	Step-1 is required in any case
		2.	Step-2 & 3 are required for ECascade and minies modifiers
		3.	Never use assignment statemaent for setting
			cascade bitmap or modifier but always OR the
			cascade bitmap or modifier e.g.
			SETBITchar(ATLCasBmp,1);
			ATLECasMods  |= FFR_MF;
			ATLMinMods[i]|= VLV_MF;
			Where i=0 to 11
*/
void ProcessSlaveATL_SE(void)
{
  unsigned int num;
  unsigned char *cptr;

  /***************Testing ATL_SE*****************/
/*
    //ATLDatMods|=CPS_MF; 
    ATLCasBmp[0] = 0xFF; ATLCasBmp[1] = 0x1F;	
	ATLECasMods|=(VCD_MF|VSD_MF|FIM_MF); 
    for(num=0; num<12; num++) {
	  ATLMinMods[num]|=(VCD_MF|VSD_MF|REC_MF|VEV_MF);
	}
*/
  /**********************************************/
  cptr=(unsigned char*)&num;
  num=ATL_SE_DataBytes; num <<= 3; num |= K_CMC;

  t2Buf[0] = 0x80; t2Buf[0] |= ATL_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  t2Buf[3] = CPS_MF;								// Modifier
  t2Buf[4] = ATLCasBmp[0]; t2Buf[5] = ATLCasBmp[1]&0x1F;	// Cascade bitmap(Main & Minies)
  t2Buf[6] = ATLECasMods;									// Main cascade modifiers
  for(num=0; num<12; num++) t2Buf[7+num] = ATLMinMods[num];	// Mini cascades modifiers

  cptr=(unsigned char*)C12Ptr->EFLODT;				// Main cascade data
  for(num=0; num<18; num++)
    t2Buf[19+num] = *cptr++;
  Form_VLV_ECFG(EVLVCMD[0]);						//shakeel
  t2Buf[37] = (unsigned char)C12Ptr->ECCFGDT;
  t2Buf[38] = (unsigned char)C12Ptr->ECATLDT;

  cptr=(unsigned char*)C12Ptr->MCSPDT;				// Mini cascades data
  for(num=0; num<96; num++)
    t2Buf[39+num] = *cptr++;
  cptr=(unsigned char*)C12Ptr->MVLVCDT;
  for(num=0; num<24; num++)
    t2Buf[135+num] = *cptr++;
  cptr=(unsigned char*)C12Ptr->MMCHCDT;
  for(num=0; num<168; num++)
    t2Buf[159+num] = *cptr++;

  for(num=0; num<12; num++)
    Form_VLV_MCFG(MVLVCMD[num]);					//shakeel
  cptr=(unsigned char*)C12Ptr->MCFGDT;
  for(num=0; num<12; num++)
    t2Buf[327+num] = *cptr++;

  cptr=(unsigned char*)C12Ptr->MCATLDT;
  for(num=0; num<12; num++)
    t2Buf[339+num] = *cptr++;

  t2Cnt=ATL_SE_DataBytes+3; SRSE=ATL_SE;
  ATLCasBmp[0]=ATLCasBmp[1]=ATLECasMods=0;
  for(num=0; num<12; num++) ATLMinMods[num]=0;
}

/*
	Steps For Sending ILC_SE
	------------------------
	1.	Set cascade bitmap ILCCasBmp[2]
	2.	Set Modifiers CPS_MF, SBOX_MF, ECRD_MF in ILCDatMods
	3.	Set Modifiers FFR_MF, VLV_MF, EHP_MF in ILCECasMods
	4.	Set Modifiers CSP_MF, VCP_MF, VLV_MF, MCH_MF in ILCMinMods[12]
	5.	Set ILCTxReq=1 to send ILC_SE

	NOTE:-
		1.	Step-1 is required in any case
		2.	Step-2 with CPS_MF is required for Step 3/4
		3.	Never use assignment statemaent for setting
			cascade bitmap or modifier but always OR the
			cascade bitmap or modifier e.g.
			SETBITchar(ILCCasBmp,1);
			ILCDatMods   |= CPS_MF;
			ILCECasMods  |= FFR_MF;
			ILCMinMods[i]|= VLV_MF;
			Where i=0 to 11
*/
int ProcessSlaveILC_SE(void)
{
  unsigned int num;
  unsigned char *cptr;
  /***************Testing ILC_SE*****************/
/*
    //ILCDatMods|=(CPS_MF|SBOX_MF|ECRD_MF);
    ILCCasBmp[0] = 0xFF; ILCCasBmp[1] = 0x1F;	
	ILCECasMods|=(FFR_MF|VLV_MF|EHP_MF);
    for(num=0; num<12; num++) {
	  ILCMinMods[num]|=(CSP_MF|VCP_MF|VLV_MF|MCH_MF);
	}
*/
  /**********************************************/

  t2Buf[0] = 0x80; t2Buf[0] |= ILC_SE;				// Frame type + SE code
  t2Buf[3] = ILCDatMods;							// Main Modifier

  if(t2Buf[3]&CPS_MF) {								// CPS_MF
    cptr=(unsigned char*)&num;
    num=ILC_CPSDataBytes; num <<= 3; num |= K_CMC;
    t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;					// Data size + Sender code
    t2Buf[4] = ILCCasBmp[0]; t2Buf[5] = ILCCasBmp[1]&0x1F;	// Cascade bitmap(12 minies + 1 C12)
    t2Buf[6] = ILCECasMods;									// Main cascade modifiers
    for(num=0; num<12; num++)								// Mini cascades modifiers
	  t2Buf[7+num] = ILCMinMods[num];
    cptr=(unsigned char*)C12Ptr->EFLODT;
    for(num=0; num<14; num++)
      t2Buf[19+num] = *cptr++;
    cptr=(unsigned char*)C12Ptr->EVLVPDT;
    for(num=0; num<4; num++)
      t2Buf[33+num] = *cptr++;
    cptr=(unsigned char*)C12Ptr->MCSPDT;
    for(num=0; num<96; num++)
      t2Buf[37+num] = *cptr++;
    cptr=(unsigned char*)C12Ptr->MVLVPDT;
    for(num=0; num<24; num++)
      t2Buf[133+num] = *cptr++;
    cptr=(unsigned char*)C12Ptr->MMCHCDT;
    for(num=0; num<168; num++)
      t2Buf[157+num] = *cptr++;
    t2Cnt=ILC_CPSDataBytes+3; 

    ILCCasBmp[0]=ILCCasBmp[1]=ILCECasMods=0;
    for(num=0; num<12; num++) ILCMinMods[num]=0;
    SRSE=ILC_SE;
	ILCDatMods &= 0xFE;
	if(ILCDatMods&0x07) ILCTxReq=1;
	return 0;
  }

  if(t2Buf[3]&SBOX_MF) {							// SBOX_MF
	//PORTE=t2Buf[3];
    cptr=(unsigned char*)&num;
    num=ILC_SBOXDataBytes; num <<= 3; num |= K_CMC;
    t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
    cptr=(unsigned char*)C12Ptr->MSBOXCDT;
    for(num=0; num<12; num++)
      t2Buf[4+num] = *cptr++;
    t2Cnt=ILC_SBOXDataBytes+3; 

    SRSE=ILC_SE;
	ILCDatMods &= 0xFD;
	if(ILCDatMods&0x07) ILCTxReq=1;
	return 0;
  }

  if(t2Buf[3]&ECRD_MF) {							// ECRD_MF
    cptr=(unsigned char*)&num;
    num=ILC_ECRDDataBytes; num <<= 3; num |= K_CMC;
    t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
    cptr=(unsigned char*)C12Ptr->MECRDCDT;
    for(num=0; num<6; num++)
      t2Buf[4+num] = *cptr++;
    t2Cnt=ILC_ECRDDataBytes+3; 

    SRSE=ILC_SE;
	ILCDatMods &= 0xFB;
	if(ILCDatMods&0x07) ILCTxReq=1;
	return 0;
  }
  return 0;
}

void SendCPS_PDR()
{
  unsigned int num;
  unsigned char *cptr;

  cptr=(unsigned char*)C12Ptr->EFLODT;
  for(num=0; num<14; num++)
    t2Buf[4+num] = *cptr++;
  cptr=(unsigned char*)&C12Ptr->MCSPDT[0][0];
  for(num=0; num<96; num++)
    t2Buf[18+num] = *cptr++;
  cptr=(unsigned char*)C12Ptr->EVLVPDT;
  for(num=0; num<4; num++)
    t2Buf[114+num] = *cptr++;
  cptr=(unsigned char*)C12Ptr->MVLVPDT;
  for(num=0; num<24; num++)
    t2Buf[118+num] = *cptr++;
  cptr=(unsigned char*)C12Ptr->MMCHCDT;
  for(num=0; num<168; num++)
    t2Buf[142+num] = *cptr++;
  t2Cnt=PDR_SE_DataBytes+3; SRSE=PDR_SE;
} 

void SendMFRQ_PDR(unsigned char *tmp)
{
  int byte;

  for(byte=0; byte<312; byte++)
    t2Buf[4+byte] = *tmp++;
  t2Cnt=PDR_SE_DataBytes+6+3; SRSE=PDR_SE;
}

void ProcessSlavePDR_SE(void)
{
  unsigned int num;
  unsigned char *cptr, Mod;

  cptr=(unsigned char*)&num;
  t2Cnt=0; Mod=0;

  if(MSEBuf[3]==CPS_MF) num=PDR_SE_DataBytes;
  else num=PDR_SE_DataBytes+6; num <<= 3; num |= K_CMC;
  t2Buf[0] = 0x80; t2Buf[0] |= PDR_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  for(num=0; num<8; num++) {
    if(TSTBITchar(&MSEBuf[3],num)){
      SETBITchar(&Mod,num);
	}
    else {CLRBITchar(&Mod,num);}
  }
  t2Buf[3] = Mod;									// Modifier

  switch(Mod) {
	case  CPS_MF: SendCPS_PDR();  break;
	case MFR1_MF: cptr=(unsigned char*)C12Ptr->MMCHFRQ;
				  SendMFRQ_PDR(cptr);
				  break;
	case MFR2_MF: cptr=(unsigned char*)C12Ptr->MMCHFRQ2;
				  SendMFRQ_PDR(cptr);
				  break;
	case MFR3_MF: cptr=(unsigned char*)C12Ptr->MMCHFRQ3;
				  SendMFRQ_PDR(cptr);
				  break;
	case MFR4_MF: cptr=(unsigned char*)C12Ptr->MMCHFRQ4;
				  SendMFRQ_PDR(cptr);
				  break;
	default: break;
  }
}

void ProcessSlavePUS_SE(void)
{
  unsigned int num;
  unsigned char *cptr;

  cptr=(unsigned char*)&num;
  num=PUS_SE_DataBytes; num <<= 3; num |= K_CMC;
  t2Buf[0] = 0x80; t2Buf[0] |= PUS_SE;				// Frame type + SE code
  t2Buf[1] = *(cptr+1); t2Buf[2] = *cptr;			// Data size + Sender code
  t2Buf[3] = PUS_MF;//0x01;							// Modifier
  cptr=(unsigned char*)C12PUSPtr->ERTCDT;
  for(num=0; num<15; num++)
	t2Buf[4+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->ELMTDT;
  for(num=0; num<10; num++)
	t2Buf[19+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MMCHMS;
  for(num=0; num<84; num++)
	t2Buf[29+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MPRSMS;
  for(num=0; num<12; num++)
	t2Buf[113+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MVLVMS;
  for(num=0; num<12; num++)
	t2Buf[125+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MSERDT;
  for(num=0; num<12; num++)
	t2Buf[137+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MVDYDT;
  for(num=0; num<48; num++)
	t2Buf[149+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MPDYDT;
  for(num=0; num<12; num++)
	t2Buf[197+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MMDYDT;
  for(num=0; num<12; num++)
	t2Buf[209+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MCSPLDT;
  for(num=0; num<96; num++)
	t2Buf[221+num] = *cptr++;
  cptr=(unsigned char*)C12PUSPtr->MVCPLDT;
  for(num=0; num<48; num++)
	t2Buf[317+num] = *cptr++;

  t2Cnt=PUS_SE_DataBytes+3; SRSE=PUS_SE;
}

void DspToMaster(void)
{
  int i;
  char CS=0;

  sprintf((char *)t1Buf,"\n\rTo Master  :: "), t1Cnt=16;
  for(i=0; i<t2Cnt; i++) {
	sprintf((char *)&t1Buf[t1Cnt],"%02X ",t2Buf[i]);
	CS += t2Buf[i];
	t1Cnt += 3;
  }
  CS *= -1;
  sprintf((char *)&t1Buf[t1Cnt],"%02X ",CS&0xff); t1Cnt += 3;
  //if(t2Cnt==0) {sprintf((char *)&t1Buf[t1Cnt],"%02X ",SE&0xff); t1Cnt += 2;}
  t1Ptr=t1Buf; 
  IEC0bits.U1TXIE = 1; 	//Enable U1Tx Interrupt
}

void DspFromMaster(void)
{
  int i;

  t1Cnt=0;
  sprintf((char *)t1Buf,"\n\rFrom Master:: "), t1Cnt=16;
  for(i=0; i<3+DspMasterCnt; i++) {
	sprintf((char *)&t1Buf[t1Cnt],"%02X ",MSEBuf[i]);
	t1Cnt += 3;
  }
  DspMasterCnt=0;
  t1Ptr=t1Buf; 
  IEC0bits.U1TXIE = 1;	//Enable U1Tx Interrupt
}

int SendSE(void)
{
  if(ErmTxInProgress || MTxInProgress) return 0;				
  if((SE<0) || (SE>14))  return(0); 
  switch(SE) {
	case SPF_SE: ProcessSlaveSPF_SE(); break;
	case SBF_SE: ProcessSlaveSBF_SE(); break;
	case ACK_SE: ProcessSlaveACK_SE(); break;
	case RTY_SE: ProcessSlaveRTY_SE(); break;
	case RST_SE: ProcessSlaveRST_SE(); break;
	case CMF_SE: ProcessSlaveCMF_SE(); break;
	case SFT_SE: ProcessSlaveSFT_SE(); break;
	case ALC_SE: return 0;
	case ATL_SE: ProcessSlaveATL_SE(); break;
	case WLC_SE: return 0;
	case ILC_SE: ProcessSlaveILC_SE(); break;
	case PDR_SE: ProcessSlavePDR_SE(); break;
	case DSP_SE: return 0;
	case PUS_SE: ProcessSlavePUS_SE(); break;
  }
  if(SE) {
	if(t2Cnt==0) return 0;
	SE=0; Cs=0; t2Ptr=t2Buf; 
	if((ERM&2)==2) {ErmTxInProgress=1; DspToMaster();}
	t2Cnt++;						//For Checksum Transmission
	MTxInProgress=1;
	IEC1bits.U2TXIE = 1; 			//Enable U2Tx Interrupt
  }
  /*
  else {
	if(SlavePDRReq) {SlavePDRReq=0; SE=PDR_SE;}
	if(SlavePUSReq) {SlavePUSReq=0; SE=PUS_SE;}
	if(SlaveCMFReq)	{SlaveCMFReq=0; SE=CMF_SE;}
	if(ILCTxReq) {
	  ILCTxReq=0;
	  for(i=0; i<14; i++) {
		ILCMods[i]=ILCUMods[i];
		ILCUMods[i]=0;
	  }
	  SE=ILC_SE;
	}
  }
  */
  return 0;
}

void SendACK_SE(unsigned char CSE)
{
  RSE = CSE;
  //SlaveAckReq=1;
  SE=ACK_SE;
}

void ProcessMasterACK_SE(void)
{
  char ACKRSE,SenderCode;

  SenderCode=MSEBuf[1]&0x07;
  ACKRSE = (MSEBuf[1]>>3);
  //if(ACKRSE==SRSE) PORTD=ACKRSE;
  //else PORTD=0xFF;
  SRSE=0;
}

void ProcessMasterRTY_SE(unsigned char RTYSE)
{
  //SendACK_SE(RTY_SE<<3);
  SE=RTYSE;
}

void ProcessMasterRST_SE(void)
{
  SendACK_SE(RST_SE<<3);
  RST_SE_Recieved=1;
}

void ProcessMasterSER_SE(void)
{
  int i, j;

  /* Main Cascade SERs Enable/Disable */
  if(MSEBuf[5]&0x10) {
	for(i=0; i<7; i++) {
	  if(TSTBITchar(&MSEBuf[6],i)){
	    if(MSEBuf[6]&0x80) {
          CLRBITchar(&C12PUSPtr->ESERDT,i);
		}
	    else {SETBITchar(&C12PUSPtr->ESERDT,i);}
	  }
    }
  }

  /* Mini Cascades SERs Enable/Disable */
  else {
    for(i=0; i<TOTMIN; i++) {
	  if(TSTBITchar(&MSEBuf[4],i)) {
	    for(j=0; j<7; j++) {
	      if(TSTBITchar(&MSEBuf[7],j)){
	        if(MSEBuf[7]&0x80) {
              CLRBITchar(&C12PUSPtr->MSERDT[i],j);
		    }
	        else {SETBITchar(&C12PUSPtr->MSERDT[i],j);}
	      }
        }
	  }
	}
  }
  SendACK_SE(SER_SE<<3);
}

void ProcessMasterPMS_SE(void)
{
  int i;
  unsigned char dummy;

  /* Main Cascade Masking Enable/Disable */
  if(MSEBuf[5]&0x10) {
	for(i=0; i<2; i++) {						// Main cascade EHP,FFR masking ena/dis
	  if(TSTBITchar(&MSEBuf[6],i)){
	    if(MSEBuf[6]&0x80) {
          SETBITchar(&C12PUSPtr->EPRSMS,i);
		}
	    else {CLRBITchar(&C12PUSPtr->EPRSMS,i);}
	  }
    }
	if(MSEBuf[6]&VLV_MF){						// Main cascade Valves masking ena/dis
	  if(MSEBuf[8]<8) dummy=MSEBuf[8]-1;
	  else dummy=MSEBuf[8];
	  if(MSEBuf[6]&0x80) {
        SETBITchar(C12PUSPtr->EVLVMS,dummy);
      }
	  else {CLRBITchar(C12PUSPtr->EVLVMS,dummy);}
	}
  }
  
  /* Mini Cascades Masking Enable/Disable */
  else {
    for(i=0; i<TOTMIN; i++) {
	  if(TSTBITchar(&MSEBuf[4],i)) {
        if(MSEBuf[7]&VCP_MF) {			// VCP_MF
	      if(MSEBuf[7]&0x80) {
            SETBITchar(&C12PUSPtr->MPRSMS[i],3);
		  }
	      else {CLRBITchar(&C12PUSPtr->MPRSMS[i],3);}
	    }
        if(MSEBuf[7]&CSP_MF) {			// CSP_MF
	      if(MSEBuf[7]&0x80) {
            SETBITchar(&C12PUSPtr->MPRSMS[i],MSEBuf[8]-1);
		  }
	      else {CLRBITchar(&C12PUSPtr->MPRSMS[i],MSEBuf[8]-1);}
	    }
        if(MSEBuf[7]&VLV_MF) {			// VLV_MF
	      if(MSEBuf[7]&0x80) {
            SETBITchar(&C12PUSPtr->MVLVMS[i],MSEBuf[8]-1);
		  }
	      else {CLRBITchar(&C12PUSPtr->MVLVMS[i],MSEBuf[8]-1);}
	    }
        if(MSEBuf[7]&MCH_MF) {			// MCH_MF
	      if(MSEBuf[7]&0x80) {
            SETBITchar(&C12PUSPtr->MMCHMS[i][0],MSEBuf[8]-1);
		  }
	      else {CLRBITchar(&C12PUSPtr->MMCHMS[i][0],MSEBuf[8]-1);}
	    }
      }
    }
  }
  SendACK_SE(PMS_SE<<3);
}

void ProcessMasterUPD_SE(void)
{
  unsigned int i, *iptr;

  /* Main Cascade Update data */
  if(MSEBuf[5]&0x10) {
	if(MSEBuf[6]&EHP0_MF){
	  if(MSEBuf[10]==1) {
	    iptr = (unsigned int *)&MSEBuf[8];
	    C12PUSPtr->ELMTDT[0] = *iptr;
	  }
	  if(MSEBuf[10]==2) {
	    iptr = (unsigned int *)&MSEBuf[8];
	    C12PUSPtr->ELMTDT[1] = *iptr;
	  }
	  if(MSEBuf[10]==3) {
	    iptr = (unsigned int *)&MSEBuf[8];
	    C12PUSPtr->ELMTDT[2] = *iptr;
	  }
	}
	if(MSEBuf[6]&FFR_MF){
	  iptr = (unsigned int *)&MSEBuf[8];
	  C12PUSPtr->ELMTDT[3] = *iptr;
	  iptr = (unsigned int *)&MSEBuf[10];
	  C12PUSPtr->ELMTDT[4] = *iptr;
	}
	if(MSEBuf[6]&WCV_MF){
	  iptr = (unsigned int *)&MSEBuf[8];
	  FWFLODT[1] = *iptr;
	  WCVFlag=1;
	}
	if(MSEBuf[6]&RTC_MF){
	  for(i=0; i<6; i++)
		C12PUSPtr->ERTCDT[i]=MSEBuf[8+i];
	}
	if(MSEBuf[6]&VDY_MF){
	  for(i=1; i<5; i++){
	    if(MSEBuf[9]==i){
		  C12PUSPtr->EVDYDT[i-1]=MSEBuf[8];
		}
	  }
	}
	if(MSEBuf[6]&PDY_MF){
	  C12PUSPtr->EPDYDT=MSEBuf[8];
	}
	if(MSEBuf[6]&FCV_MF){
	  iptr = (unsigned int *)&MSEBuf[8];
	  FWFLODT[0] = *iptr;
	  FCVFlag=1;
	  //C12PUSPtr->ELMTDT[3] = *iptr;
	  //C12PUSPtr->ELMTDT[4] = *iptr;
	}
  }

  /* Mini Cascades Update Data */
  else {
    for(i=0; i<TOTMIN; i++) {
	  if(TSTBITchar(&MSEBuf[4],i)) {
        if(MSEBuf[7]&MDY_MF) {			
	      C12PUSPtr->MMDYDT[i]=MSEBuf[8];
		}
        if(MSEBuf[7]&CSP_MF) {			
	      if(MSEBuf[10]==1) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MCSPLDT[i][0] = *iptr;
	  	  }
	  	  if(MSEBuf[10]==2) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MCSPLDT[i][1] = *iptr;
	  	  }
	  	  if(MSEBuf[10]==3) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MCSPLDT[i][2] = *iptr;
	  	  }
	  	  if(MSEBuf[10]==4) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MCSPLDT[i][3] = *iptr;
	  	  }
		}
        if(MSEBuf[7]&VCP_MF) {			
	      if(MSEBuf[10]==1) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MVCPLDT[i][0] = *iptr;
	  	  }
	  	  if(MSEBuf[10]==2) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MVCPLDT[i][1] = *iptr;
	  	  }
		}
        if(MSEBuf[7]&VDY_MF) {			
	      if(MSEBuf[10]==2) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MVDYDT[i][1] = *iptr;
	  	  }
	  	  if(MSEBuf[10]==4) {
	        iptr = (unsigned int *)&MSEBuf[8];
	    	C12PUSPtr->MVDYDT[i][3] = *iptr;
	  	  }
		}
        if(MSEBuf[7]&PDY_MF) {			
	      C12PUSPtr->MPDYDT[i]=MSEBuf[8];
		}
	  }
	}
  }
  SendACK_SE(UPD_SE<<3);
}

void ProcessMasterPDR_SE(void)
{
  SendACK_SE(PDR_SE<<3);
  SlavePDRReq=1;
  //SE=PDR_SE;
}

void ProcessMasterPUS_SE(void)
{
  SendACK_SE(PUS_SE<<3);
  SlavePUSReq=1; 
  //SE=PUS_SE;
}
void ProcessMasterCMP_SE(void)
{
  int Err;
  unsigned char x[2];

  x[0]=MSEBuf[4]; x[1]=MSEBuf[5];
  Err = putcq(x);
  SendACK_SE(CMP_SE<<3);
}

void ProcessMasterHWC_SE(void)
{
  int Err;
  unsigned char x[2];

  x[0]=MSEBuf[4]; x[1]=MSEBuf[5];
  Err = puthq(x);
  SendACK_SE(HWC_SE<<3);
}

void ProcessMasterPCC_SE(void)
{
  int Err, i;
  unsigned char x[33];

  for(i=0; i<33; i++)
    x[i]=MSEBuf[3+i];
  Err = putpq(x);
  SendACK_SE(PCC_SE<<3);
}

int ProcessCMP_SE()
{
  int Err,i;
  unsigned char x[2];
 
  //if(CHSEFlag) return 0 ;
  do {
    Err = getcq(x);
    if(!Err) {
	  CHSEFlag=1;
	  if(TSTBITchar(x,12)){			//Main Cascade
        CLRBITchar(CONBMP,0);
        UpdVlvCmdFromCurBuf(MainCas);	//shakeel - Update valves command from curr buf
        UpdPrvStsFromCurBuf(MainCas);   //shakeel - Update valves prev buf from command
	  }
	  for(i=0; i<12; i++) {			//Min Cascade
		if(TSTBITchar(x,i)){
          CLRBITchar(CONBMP,i+1);
          UpdVlvCmdFromCurBuf(i);		//shakeel - Update valves command from curr buf
          UpdPrvStsFromCurBuf(i);       //shakeel - Update valves prev buf from command
		}
	  }
    }
  } while(!Err);
  return 0;
}

int ProcessHWC_SE() 
{
  int Err,i;
  unsigned char x[2];

  //if(CHSEFlag) return 0 ;
  do {
    Err = gethq(x);
    if(!Err) {
	  CHSEFlag=1;
	  if(TSTBITchar(x,12)){			//Main Cascade
        SETBITchar(CONBMP,0);
	  }
	  for(i=0; i<12; i++) {			//Min Cascade
		if(TSTBITchar(x,i)){
          SETBITchar(CONBMP,i+1);
		}
	  }
    }
  } while(!Err);
  return 0;
}

int ProcessPCC_SE()
{
  int Err,i,j,k=0,ii;
  unsigned char x[33];

  //if(PCCSEFlag) return 0 ;
  do {
    Err = getpq(x);
    if(!Err){
	  PCCSEFlag=1;
	  for(i=0; i<13; i++){
		if(TSTBITchar(&x[1],i)){
		  if(i==12){									// Main Cascade
			if(x[3]==CFG_MF){							// CFG_MF
			  for(j=0; j<13; j++) CCFGData[j]=x[5+j];	
			  for(j=0; j<2; j++) {
				if(CCFGData[0]) {
				  if(j==1) {CVLVData[j*2]=PVLVData[j*2];} //copy bypass valves previous data from prev buffer
				  else { 
                    CVLVData[j]=EOpenValCfg[CCFGData[j]-1];
					C12DAT.ECCFGDT = CVLVData[j] ;			//shakeel
                  }            
				}
			  }
              for(ii=0; ii<2; ii++)  {EVLVCMD[ii]=CVLVData[ii*2];}			// For Main cascade Valve Command open sts
			}
			if(x[3]==VLV_MF){								// VLV_MF
			  for(k=0; k<2; k++){
			    for(j=0; j<8; j++){
			      if(TSTBITchar(&x[k*2+5],j)){				// Open  Status
				    SETBITchar(&CVLVData[k*2+1],j)
				    SETBITchar(&CVLVData[k*2],j)
				  }
			      if(TSTBITchar(&x[k*2+6],j)){				// Close Status
				    CLRBITchar(&CVLVData[k*2],j)
				  }
			    }
			  }
			  for(ii=0; ii<2; ii++)  {EVLVCMD[ii]=CVLVData[ii*2];}			// For Main cascade Valve Command open sts
			}
			UpdPrvStsFromCmd(i);							//shakeel - Update Prv sts from cmd
            VActionDelay(i);									// shakeel - Start Valve Action Delay
		  }
		  else{												// Mini Cascades
			if(x[4]==CFG_MF){								// CFG_MF
			  for(j=0; j<13; j++) CCFGData[j]=x[5+j];	
			  for(j=1; j<13; j++) {
				if(CCFGData[j]) {
				  CVLVData[j*2+2]=MOpenValCfg[CCFGData[j]-1];
				}
			  }
			  MVLVCMD[i]=CVLVData[i*2+4];		// For Minies Valve Command open sts
			}			
			if(x[4]==VLV_MF){								// VLV_MF
			  for(j=0; j<8; j++){
			    if(TSTBITchar(&x[i*2+9],j)){				// Open  Status
				  SETBITchar(&CVLVData[i*2+4],j)
				}
			    if(TSTBITchar(&x[i*2+10],j)){				// Close Status
				  CLRBITchar(&CVLVData[i*2+4],j)
				  SETBITchar(&CVLVData[i*2+5],j)
				}
			  }
			  MVLVCMD[i]=CVLVData[i*2+4];		// For Minies Valve Command open sts
			}
			UpdPrvStsFromCmd(i);							//shakeel - Update Prv sts from cmd			
            VActionDelay(i);								// shakeel - Start Valve Action Delay
		  }
		}
	  }
    }
  } while(!Err);
  return 0;
}

int SendAck()
{
  //if(ErmTxInProgress || MTxInProgress) return 0;
  SlaveAckReq=0;
  SE=ACK_SE;
  return 0;
}

int SendPDR()
{
  if(ErmTxInProgress || MTxInProgress) return 0;
  SlavePDRReq=0;
  SE=PDR_SE;
  return 0;
}

int SendPUS()
{
  if(ErmTxInProgress || MTxInProgress) return 0;
  SlavePUSReq=0;
  SE=PUS_SE;
  return 0;
}

int SendCMF()
{
  if(ErmTxInProgress || MTxInProgress) return 0;
  SlaveCMFReq=0;
  SE=CMF_SE;
  return 0 ;
}

int SendILC()
{
  if(ErmTxInProgress || MTxInProgress) return 0;
  ILCTxReq=0;
  SE=ILC_SE;
  return 0 ;
}

int SendATL()
{
  if(ErmTxInProgress || MTxInProgress) return 0;
  ATLTxReq=0;
  SE=ATL_SE;
  return 0 ;
}

int ReportMCMFErrors(unsigned char MSe)
{
  if(MError) {
	if(MError&0x0002) {					// Send OVR SE to Master
	  SlaveCMF|=0x20;
	}
	if(MError&0x0004) {					// Send BRK SE to Master
	  SlaveCMF|=0x02;
	}
	if(MError&0x0008) {					// Send PAR SE to Master
	  SlaveCMF|=0x04;
	}
    SlaveCMFReq=1; 
	MError=0;
  }
  /*
  if(MCSError) {						// Send CSE SE to Master
	MCSError=0;
    SlaveCMF|=0x08;
  	SlaveCMFReq=1; 	
  }
  */
  if(MSe<1 || MSe>14) {					// Send ISE SE to Master
	SlaveCMF|=0x01;
	SlaveCMFReq=1;
  }
  if(MSETmoCnt==0) {					// Send TMO SE to Master
    MSETmoCnt=0x8000;
    SlaveCMF|=0x10;
  	SlaveCMFReq=1; 
  }
  return 0;
}

int AckMasterSE(void)
{
  unsigned char MSE;
 // int k;

  if(ErmTxInProgress || MTxInProgress) return 0;
  MasterSERec=0;
  if((ERM&2)==2) {ErmTxInProgress=1; DspFromMaster();}
  MSE=MSEBuf[0]&0x1F;
  if(MError || MCSError ||  MSETmoCnt==0 || MSE<1 || MSE>14) ReportMCMFErrors(MSE);
  //PORTE=MSEBuf[1]>>3;
  switch(MSE) {
	case CMP_SE: ProcessMasterCMP_SE(); 			break;
	case HWC_SE: ProcessMasterHWC_SE(); 			break;
	case PCC_SE: ProcessMasterPCC_SE(); 			break;
	case ACK_SE: ProcessMasterACK_SE(); 			break;
	case RTY_SE: ProcessMasterRTY_SE(MSEBuf[1]>>3); break;
	case RST_SE: ProcessMasterRST_SE(); 			break;
	case SER_SE: ProcessMasterSER_SE(); 			break;
	case PMS_SE: ProcessMasterPMS_SE(); 			break;
	case UPD_SE: ProcessMasterUPD_SE(); 			break;
	case PDR_SE: ProcessMasterPDR_SE(); 			break;
	case PUS_SE: ProcessMasterPUS_SE(); 			break;
	default: break;
  }
  return 0;
}

void ProcessMasterSE()
{
  ProcessCMP_SE();
  ProcessHWC_SE(); 
  ProcessPCC_SE();
}

void DspFromElectronics(void)
{
  int i;

  t1Cnt=0;
  if(ESEBuf[0]==0x82) ESECnt=100;
  sprintf((char *)t1Buf,"\n\rFrom Electronics:: "), t1Cnt=21;
  for(i=0; i<1+ESECnt; i++) {
	sprintf((char *)&t1Buf[t1Cnt],"%02X ",ESEBuf[i]);
	t1Cnt += 3;
  }
  ESECnt=0;
  t1Ptr=t1Buf; 
  IEC0bits.U1TXIE = 1;	//Enable U1Tx Interrupt
}

void DspToElectronics(void)
{
  int i;
//  char CS=0;

  sprintf((char *)t1Buf,"\n\rTo Electronics:: "), t1Cnt=19;
  for(i=0; i<t3Cnt; i++) {
	sprintf((char *)&t1Buf[t1Cnt],"%02X ",t3Buf[i]);
	//CS += t3Buf[i];
	t1Cnt += 3;
  }
  //CS *= -1;
  //if(t3Cnt>1) sprintf((char *)&t1Buf[t1Cnt],"%02X ",CS&0xff); t1Cnt += 2;
  t1Ptr=t1Buf; 
  IEC0bits.U1TXIE = 1; 	//Enable U1Tx Interrupt
}


/******************************* DR & DR Processing *********************************/

void ReportECMFErrors()
{
  if(EError) {
	if(EError&0x0002) {					// Send DR OVR SE to Master
	  SlaveCMF|=0xA0;
	}
	if(EError&0x0004) {					// Send DR BRK SE to Master
	  SlaveCMF|=0x82;
	}
	if(EError&0x0008) {					// Send DR PAR SE to Master
	  SlaveCMF|=0x84;
	}
    SlaveCMFReq=1; 
	EError=0;
  }
  if(DRDataTmoCnt==0 || DRTmoCnt==0) {		// Send DR/DRData TMO SE to Master
    DRDataTmoCnt=DRTmoCnt=0x8000;
    SlaveCMF|=0x90;
  	SlaveCMFReq=1; 
  }
  if(ECSError){							// Send DR CSE SE to Master
    ECSError=0;
    SlaveCMF|=0x88;
  	SlaveCMFReq=1; 
  }
  if((DRInProgress) && (DRAckRecd) && (DR!=DRAck)){		// Send DR ISE SE to Master
    DRTmoCnt=0x8000;
	SlaveCMF|=0x81;
	SlaveCMFReq=1;
	DRAckRecd=0;
  }
  if((!DRInProgress) && (DRAckRecd || DRDataRecd)){		// Send DR ISE SE to Master
    DRTmoCnt=0x8000;
	SlaveCMF|=0x81;
	SlaveCMFReq=1;
	DRAckRecd=DRDataRecd=0;
  }
//return 0 ;
}


void UpDateDRData()
{
  int i, j;
  unsigned char *cptr;

  // Valves Data of Main & Mini Cascades
  if(DR==0x80) {
	if(DR80Cnt==0) {
	  DR80Cnt=1;
      F_DR80AfterVCmd = 0 ;   //shakeel
	  for(i=0; i<28; i++) {PVLVData[i]=CVLVData[i]= ~ESEBuf[i+3];}
	  for(i=0; i<2; i++)  {EVLVCMD[i]=CVLVData[i*2];}			// For Main  Valve Command open sts
	  for(i=0; i<12; i++) {MVLVCMD[i]=CVLVData[i*2+4];}			// For Minies Valve Command open sts
	  for(i=0; i<2; i++) {
	    C12Ptr->EVLVPDT[i*2]   = ESEBuf[i*2+4] ;
	    C12Ptr->EVLVPDT[i*2+1] = ESEBuf[i*2+3];
	  }
	  for(i=0; i<12; i++){
	    C12Ptr->MVLVPDT[i][0]=ESEBuf[i*2 +8];
	    C12Ptr->MVLVPDT[i][1]=ESEBuf[i*2 +7];
	  }
	}
	for(i=0; i<2; i++) {
	  C12Ptr->EVLVCDT[i*2]   = ESEBuf[i*2+4] ;
	  C12Ptr->EVLVCDT[i*2+1] = ESEBuf[i*2+3];
	}
	for(i=0; i<12; i++){
	  C12Ptr->MVLVCDT[i][0]=ESEBuf[i*2 +8];
	  C12Ptr->MVLVCDT[i][1]=ESEBuf[i*2 +7];
	}

    if(ESEBuf[32] & 0x80)	ESEBuf[32] &= 0x7F ; // for Electronics MSB = 1 LDC OK
    else 					ESEBuf[32] |= 0x80 ; // for C & A 		MSB = 0 LDC OK

	//C12Ptr->MECRDCDT[0]  = ESEBuf[31]<<8;		//lower byte
	//C12Ptr->MECRDCDT[0] |= ESEBuf[32];		//higher byte
	C12Ptr->MECRDCDT[0]  = ESEBuf[32]<<8;		//lower byte
	C12Ptr->MECRDCDT[0] |= ESEBuf[31];			//higher byte
  }

  // Analogue Data of Main & Mini Cascades
  if(DR==0x81) {
	cptr=(unsigned char*)C12Ptr->EFLODT;		// Main cascade data
	for(i=0; i<7; i++) {
	  *cptr++=ESEBuf[i*2+4];
	  *cptr++=ESEBuf[i*2+3];
	}

    for(j=0; j<TOTMIN; j++) {
	  cptr=(unsigned char*)&C12Ptr->MCSPDT[j][0];		// Mini cascade data
	  for(i=0; i<3; i++) {
	    *cptr++ = ESEBuf[i*2+20+j*8];
	    *cptr++ = ESEBuf[i*2+19+j*8];
	  }
	  cptr=(unsigned char*)&C12Ptr->MVCPDT[j];			// Mini cascade data
	  *cptr++ = ESEBuf[26+j*8];
	  *cptr++ = ESEBuf[25+j*8];
	}

	//C12Ptr->MECRDCDT[1]  = ESEBuf[115]<<8;	//lower byte
	//C12Ptr->MECRDCDT[1] |= ESEBuf[116];		//higher byte
	C12Ptr->MECRDCDT[1]  = ESEBuf[116]<<8;		//lower byte
	C12Ptr->MECRDCDT[1] |= ESEBuf[115];			//higher byte
  }

  // Mini Cascades Machines Data
  if(DR==0x82) {
	C12Ptr->MSBOXCDT[0]=ESEBuf[3];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ[0][0];	
	for(i=0; i<14; i++) {
	  *cptr++ = ESEBuf[i*2+5];
	  *cptr++ = ESEBuf[i*2+4];
	}

	C12Ptr->MSBOXCDT[1]=ESEBuf[32];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ[1][0];
	for(i=0; i<30; i++) {
	  *cptr++ = ESEBuf[i*2+34];
	  *cptr++ = ESEBuf[i*2+33];
	}

	C12Ptr->MSBOXCDT[2]=ESEBuf[93];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ[2][0];
	for(i=0; i<52; i++) {
	  *cptr++ = ESEBuf[i*2+95];
	  *cptr++ = ESEBuf[i*2+94];
	}

	C12Ptr->MSBOXCDT[3]=ESEBuf[198];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ2[0][0];
	for(i=0; i<44; i++) {
	  *cptr++ = ESEBuf[i*2+200];
	  *cptr++ = ESEBuf[i*2+199];
	}

	C12Ptr->MSBOXCDT[4]=ESEBuf[287];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ2[1][0];
	for(i=0; i<44; i++) {
	  *cptr++ = ESEBuf[i*2+289];
	  *cptr++ = ESEBuf[i*2+288];
	}

	C12Ptr->MSBOXCDT[5]=ESEBuf[376];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ2[2][0];
	for(i=0; i<44; i++) {
	  *cptr++ = ESEBuf[i*2+378];
	  *cptr++ = ESEBuf[i*2+377];
	}

	C12Ptr->MSBOXCDT[6]=ESEBuf[465];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ3[0][0];
	for(i=0; i<44; i++) {
	  *cptr++ = ESEBuf[i*2+467];
	  *cptr++ = ESEBuf[i*2+466];
	}

	C12Ptr->MSBOXCDT[7]=ESEBuf[554];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ3[1][0];
	for(i=0; i<40; i++) {
	  *cptr++ = ESEBuf[i*2+556];
	  *cptr++ = ESEBuf[i*2+555];
	}

	C12Ptr->MSBOXCDT[8]=ESEBuf[635];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ3[2][0];
	for(i=0; i<48; i++) {
	  *cptr++ = ESEBuf[i*2+637];
	  *cptr++ = ESEBuf[i*2+636];
	}

	C12Ptr->MSBOXCDT[9]=ESEBuf[732];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ4[0][0];
	for(i=0; i<48; i++) {
	  *cptr++ = ESEBuf[i*2+734];
	  *cptr++ = ESEBuf[i*2+733];
	}

	C12Ptr->MSBOXCDT[10]=ESEBuf[829];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ4[1][0];
	for(i=0; i<32; i++) {
	  *cptr++ = ESEBuf[i*2+831];
	  *cptr++ = ESEBuf[i*2+830];
	}

	C12Ptr->MSBOXCDT[11]=ESEBuf[894];
	cptr=(unsigned char*)&C12Ptr->MMCHFRQ4[2][0];
	for(i=0; i<28; i++) {
	  *cptr++ = ESEBuf[i*2+896];
	  *cptr++ = ESEBuf[i*2+895];
	}

	//C12Ptr->MECRDCDT[2]  = ESEBuf[951]<<8;	//lower byte
	//C12Ptr->MECRDCDT[2] |= ESEBuf[952];		//higher byte
	C12Ptr->MECRDCDT[2]  = ESEBuf[952]<<8;		//lower byte
	C12Ptr->MECRDCDT[2] |= ESEBuf[951];			//higher byte
  }
  ProcessECrdSBox() ;	// Waseem Akhtar
  ProcMsECsPccData() ;	// Waseem Akhtar
  ProcAnaData() ;		// Khalid Malik
  ProcessValves() ;		// Shakeel Muzaffar
}

void ProcessMCVCmd_SE()
{
  int i;
  char lcs=0, *cptr;
  
  DR=0x7E;
  t3Buf[0]=0; t3Buf[1]=7;					//Counter
  cptr= (char *)&FWFLODT[0];
  t3Buf[3]=*cptr++; t3Buf[2]=*cptr; 
  if(FCVFlag) {FCVFlag=0; t3Buf[3]|=0x80;}
  cptr= (char *)&FWFLODT[1]; 
  t3Buf[5]=*cptr++; t3Buf[4]=*cptr;  
  if(WCVFlag) {WCVFlag=0; t3Buf[5]|=0x80;}
  for(i=0; i<6; i++) lcs+=t3Buf[i]; t3Buf[6]=lcs;
  t3Cnt=7; t3Ptr=t3Buf;
  //if((ERM&1)==1) {ErmTxInProgress=1; DspToElectronics();}					//testing
  IEC5bits.U4TXIE = 1; 					//Enable U4Tx Interrupt
}

void ProcessCMP_HWC_PCC_SE()
{
  int i;
  char lcs=0;

  if(CHSEFlag) CHSEFlag=0;
  if(PCCSEFlag) PCCSEFlag=0;
  t3Buf[0]=0; t3Buf[1]=19;				//Counter
  t3Buf[2]=CONBMP[0]; t3Buf[3]=CONBMP[1];
  //t3Buf[2]=CONBMP[1]; t3Buf[3]=CONBMP[0];
  //t3Buf[4]=C12Ptr->EVLVCDT[0]; t3Buf[5]=C12Ptr->EVLVCDT[2];
  //for(i=0; i<12; i++) t3Buf[6+i]=C12Ptr->MVLVCDT[i][0];
  //for(i=0; i<14; i++) t3Buf[4+i]=CVLVData[i*2];
  for(i=0; i<2; i++)  {t3Buf[4+i]=EVLVCMD[i];}			// For Main  Valve Command open sts
  for(i=0; i<12; i++) {t3Buf[6+i]=MVLVCMD[i];}			// For Minies Valve Command open sts
  for(i=0; i<18; i++) lcs+=t3Buf[i]; t3Buf[18]=lcs;
  t3Cnt=19; t3Ptr=t3Buf;
  //if((ERM&1)==1) {ErmTxInProgress=1; DspToElectronics();}					//testing
  IEC5bits.U4TXIE = 1; 					//Enable U4Tx Interrupt
}

void ProcessDRData()
{
  if((DRDataRecd) && (DR==DRAck) && (DRDataTmoCnt>0) && (EError==0)) {
    if((ERM&1)==1) {ErmTxInProgress=1; DspFromElectronics();}
    DRDataRecd=0; DRAckRecd=0; DRAck=0; DRCnt=0;
	DRDataTmoCnt=0x8000; DRTmoCnt=0x8000;
    UpDateDRData();
    DR++; if(DR>0x82) DR=0x80;
    DRInProgress=0;
	DRDelay=3;
  }
  else {
	ReportECMFErrors();
  }
}

void ProcessDRAck()
{
  if((DR==DRAck) && DRTmoCnt>0) {
	DRTmoCnt=0x8000;						//Stop  DRTMO Counter
	if(DRAck==0x7F) {ProcessCMP_HWC_PCC_SE(); DRInProgress=0;DRAckRecd=DRDataRecd=0;DRDelay=3;}
	if(DRAck==0x7E) {ProcessMCVCmd_SE(); DRInProgress=0;DRAckRecd=DRDataRecd=0;DRDelay=3;}
  }
  else {

  }
}

int SendDR()
{
  if(DRInProgress || ErmTxInProgress || DRDelay) return 0;
  if((DRTmoCnt<0) && (DRDataTmoCnt<0)) {
    DRInProgress=1;
	if(CHSEFlag || PCCSEFlag)  {DR=0x7F;}
	else if(FCVFlag || WCVFlag){DR=0x7E;}
	else {
	  if(DR==0x7E || DR==0x7F) DR=0x80;
	}
	t3Buf[0]=DR; t3Cnt=1; t3Ptr=t3Buf; DRCnt=0;
	if((ERM&1)==1) {ErmTxInProgress=1; DspToElectronics();}
	r3Cnt=1; r3Ptr=r3Buf;
    DRTmoCnt=10+1; 							//Start DRTMO Counter of 1 Sec
    IEC5bits.U4TXIE = 1; 					//Enable U4Tx Interrupt
  }
  return 0 ;
}

void GetEData()
{
  SendDR();
  ProcessDRAck();
  ProcessDRData(); 
}

/***************************** End of FILE *********************************/

/********************* TESTING ********************/
/*
	if((ERM&2)==2){
	  t1Cnt=0;
  	  sprintf((char *)t1Buf,"\n\rt1="), t1Cnt=5;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",t1Buf[1195+k]);
	    t1Cnt += 3;
  	  }
  	  sprintf((char *)&t1Buf[t1Cnt],"\n\rt2="), t1Cnt+=5;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",t2Buf[495+k]);
	    t1Cnt += 3;
  	  }
  	  sprintf((char *)&t1Buf[t1Cnt],"\n\rt3="), t1Cnt+=5;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",t3Buf[45+k]);
	    t1Cnt += 3;
  	  }
  	  sprintf((char *)&t1Buf[t1Cnt],"\n\rr2="), t1Cnt+=5;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",r2Buf[95+k]);
	    t1Cnt += 3;
  	  }
  	  sprintf((char *)&t1Buf[t1Cnt],"\n\rr3="), t1Cnt+=5;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",r3Buf[995+k]);
	    t1Cnt += 3;
  	  }
  	  sprintf((char *)&t1Buf[t1Cnt],"\n\rMBuf="), t1Cnt+=7;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",MSEBuf[95+k]);
	    t1Cnt += 3;
  	  }
  	  sprintf((char *)&t1Buf[t1Cnt],"\n\rEBuf="), t1Cnt+=7;
  	  for(k=0; k<5; k++) {
	    sprintf((char *)&t1Buf[t1Cnt],"%02X ",ESEBuf[995+k]);
	    t1Cnt += 3;
  	  }

	  t1Ptr=t1Buf;
	  IEC0bits.U1TXIE = 1;	//Enable U1Tx Interrupt
	}
*/
	/****************************************************/

