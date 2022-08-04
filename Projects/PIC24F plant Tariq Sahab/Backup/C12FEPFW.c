/******************************************************************** *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   C12FEPFW.c - 													* *                  												* *             								                        *
 ********************************************************************/
#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

int  SendSE();
int  ProcessMasterSE();
void RemapUARTSPins();
void DoInit();
void ConfigureT1();
void ConfigureT2();
void Delay(int);
void ConfigureU1();
void ConfigureU2();
void ConfigureU4();
void EnableU1RxINTRs();
void EnableU2RxINTRs(); 
void EnableU4RxINTRs(); 
void ConfigureUARTS();
//void EnableUARTSRxINTRs();
void UpdateLimits();

void DoInit(void)
{
  r1Ptr=r1Buf; r1Cnt=0; t1Ptr=t1Buf; t1Cnt=0;				//ERM Communication
  r2Ptr=r2Buf; r2Cnt=3; t2Ptr=t2Buf; t2Cnt=0;				//Master Communication
  r3Ptr=r3Buf; r3Cnt=1; t3Ptr=t3Buf; t3Cnt=0; 				//Electronics Communication
  Endr3Ptr= &r3Buf[1000]; Endt3Ptr= &t3Buf[50];
  DRCnt=0; SlaveCMF=0; SlaveCMFReq=0; SlaveAckReq=0;DRAck=ERM=0;
  MasterSERec=0;  VFrameCnt=0;  SlavePDRReq=0; SlavePUSReq=0;
   i=0; j=0; Tmr1Cnt=0;
  DspMasterCnt=0; RST_SE_Recieved=0;
  ATLTxReq=ILCTxReq=0;
  DRDataRecd=DRAckRecd=0;ECS=EDCS=ESECnt=0;
  FCVFlag=WCVFlag=MCVCmdInProgress=0; 
  DRInProgress=ErmTxInProgress=MTxInProgress=ETxInProgress=0;
  DRTmoCnt=DRDataTmoCnt=VlvCmdTmoCnt=MSETmoCnt=0x8000; DR=0x80; DR80Cnt=0;
  CHSEFlag=PCCSEFlag=CHSEInProgress=PCCSEInProgress=0;
  CONBMP[0]=0xFF; CONBMP[1]=0x1F; LEDFlag=1; DRDelay=0;ECSError=MCSError=0;
  SPFValue=SBFValue=0;
  TRISE=0;							// Set PORTE for output
  TRISF=0xFFFF;						// Set PORTF for input
  PORTE=0;
  FWFLODT[0]=FWFLODT[1]=0; EError=MError=0;; SlaveCMFSend=0;
  ILCCasBmp[0]=ILCCasBmp[1]=ILCDatMods=ILCECasMods=0;
  for(i=0; i<12; i++) ILCMinMods[i]=0;
  ATLCasBmp[0]=ATLCasBmp[1]=ATLDatMods=ATLECasMods=0;
  for(i=0; i<12; i++) ATLMinMods[i]=0;
  //for(i=0; i<14; i++) ILCMods[i]=ILCUMods[i]=0; 
  for(i=0; i<15; i++) PCFGData[i]=CCFGData[i]=0;
  for(i=0; i<30; i++) PVLVData[i]=CVLVData[i]=0;

  for(i=0; i<1200; i++) t1Buf[i]=0xFF;
  for(i=0; i<500; i++)  t2Buf[i]=0xFF;
  for(i=0; i<50; i++)   t3Buf[i]=0xFF;
  for(i=0; i<100; i++)  r1Buf[i]=0xFF;
  for(i=0; i<100; i++)  r2Buf[i]=0xFF;
  for(i=0; i<1000; i++) r3Buf[i]=0xFF;
  for(i=0; i<1000; i++) ESEBuf[i]=0xFF;
  for(i=0; i<100; i++)  MSEBuf[i]=0xFF;
  for(i=0; i<TOTMIN; i++){
    VCPSEDelay[i] = -1 ;
    CSPSEDelay[i] = -1 ;
    for(j=0; j<52; j++) MchSeDelay[i][j] = -1;
  }
  FFRSEDelay = - 1 ;
  EHPSEDelay = - 1 ;
  initqs(); 
  C12Ptr = &C12DAT;
  C12PUSPtr = &C12PUSDT;
  //INTCON1bits.NSTDIS=1;			// Nesting interrupts are disabled
}

void RemapUARTSPins()
{
  // Unlock Registers
  asm volatile ("MOV 	#OSCCON, w1  \n"
  				"MOV 	#0x46, 	w2 	 \n"
  				"MOV 	#0x57, 	w3 	 \n"
  				"MOV.b 	w2, 	[w1] \n"
  				"MOV.b 	w3, 	[w1] \n"
				"BCLR 	OSCCON,	#6"
				);

  RPINR18bits.U1RXR = 11;  			// Assign U1RX To Pin RP11 
  RPOR6bits.RP12R 	= 3 ;			// Assign U1TX To Pin RP12
  
  RPINR19bits.U2RXR = 4 ;  			// Assign U2RX To Pin RP4
  RPOR1bits.RP3R 	= 5 ;			// Assign U2TX To Pin RP3
  
  //RPINR17bits.U3RXR = 8 ;			// Assign U3RX To Pin RP8 
  //RPOR4bits.RP9R 	= 28;  			// Assign U3TX To Pin RP9

  RPINR27bits.U4RXR = 27;			// Assign U4RX To Pin RP27
  RPOR13bits.RP26R 	= 30;			// Assign U4TX To Pin RP26

  // Lock Registers
  asm volatile ("MOV #OSCCON, w1 \n"
				"MOV #0x46, w2 \n"
				"MOV #0x57, w3 \n"
				"MOV.b w2, [w1] \n"
				"MOV.b w3, [w1] \n"
				"BSET OSCCON, #6" 
				);
}

void ConfigureUARTS()
{
  ConfigureU1();
  ConfigureU2();
  ConfigureU4();
}

void EnableUARTSRxINTRs()
{
  EnableU1RxINTRs();
  EnableU2RxINTRs(); 
  EnableU4RxINTRs(); 
}
/*
PrintADCValues(int ADC)
{
  unsigned char *cptr;

  if(ErmTxInProgress || MTxInProgress) return 0;
  cptr= &ADC;
  t2Buf[1]= *cptr++; t2Buf[0]= *cptr; 
  t2Cnt=2; t2Ptr=t2Buf;
  if((ERM&2)==2) {ErmTxInProgress=1; DspToMaster();}
  while(t1Cnt!=0);
}
*/
int PrintADCValues()
{
  unsigned char *cptr;

  if(ErmTxInProgress || MTxInProgress) return 0;
  cptr= (unsigned char *)&SPFValue;
  t2Buf[1]= *cptr++; t2Buf[0]= *cptr; 
  cptr= (unsigned char *)&SBFValue;
  t2Buf[3]= *cptr++; t2Buf[2]= *cptr; 
  t2Cnt=4; t2Ptr=t2Buf;
  if((ERM&2)==2) {ErmTxInProgress=1; DspToMaster();}
  return(0) ;
}

int main()
{
  MainLoop:

  RemapUARTSPins();
  DoInit();
  ConfigureT1();
  ConfigureT2();
  Delay(20);
  ConfigureUARTS();
  EnableUARTSRxINTRs();
  ConfigADC();
  UpdateLimits();
  //FillData() ;

  /* Slave to Master SEs */
  SE=RST_SE;
  //SE=SPF_SE;
  //SE=SBF_SE;
  //SE=ACK_SE;
  //SE=RTY_SE;
  //SE=CMF_SE;
  //SE=SFT_SE;
  //SE=ATL_SE;
  //SE=ILC_SE;  
  //SE=PDR_SE;
  //SE=PUS_SE;

  //ILCTxReq=1;	//ILC_SE testing
  //ILCDatMods|=(CPS_MF|SBOX_MF|ECRD_MF);
  //ATLTxReq=1;	//ATL_SE testing
  //ATLDatMods|=CPS_MF; 

  while(1) {
	//ClrWdt();
	ERM=PORTF; 
	if(!ERM) PORTE=0;
    SendSE();
	if(SlavePDRReq) SendPDR();
	if(SlavePUSReq) SendPUS();
	if(SlaveCMFReq) SendCMF();
	if(ILCTxReq)	SendILC();
	if(ATLTxReq)	SendATL();
	if(MasterSERec) AckMasterSE();
	else ProcessMasterSE();
	GetEData();

	//SPFValue=ReadADC(0x0505);
	//SPFValue=ReadADC(0x0606);
	//PrintADCValues(SPFValue);
    //Delay(2);

	//SBFValue=ReadADC(0x0606);
	//SBFValue=ReadADC(0x0707);
    //Delay(2);
	//PrintADCValues(SBFValue);
	//PrintADCValues();
    //Delay(2);

	if(RST_SE_Recieved) break; 
  }
  goto MainLoop;
}

/**************************************** END OF FILE ***************************************/
