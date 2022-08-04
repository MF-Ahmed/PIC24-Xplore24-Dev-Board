/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   Common.c - 												    * *                  												* *             								                        *
\*******************************************************************/
#include <P24FJ256GA106.h>
#include "KCSDF.h"

struct c12dat {									// C12(B1) data structure
												// Main-Cascade data
  unsigned char		ECSFTDT		  		;		// SFT_SE data
  unsigned char		ECCFGDT		  		;		// Valves CFG number
  unsigned char		ECATLDT				;		// ATL retries counter
  unsigned char		MCFGDT[TOTMIN]		;		// Valves CFG number
  unsigned char		MCATLDT[TOTMIN]		;		// ATL retries counter
  unsigned short	EFLODT[2]			;		// Flows(Feed, Product)
  unsigned short	ECHBGDT[1]			;		// Cascade Hdr-BG (Waste)
  unsigned short	ECHPGDT[2]			;		// Cascade Hdr-PG (Dump, Evacuation)
  unsigned short	EEHPDT				;		// Evacuation Header Pressure-BG
  unsigned short	EEHPPGDT			;		// Evacuation Header Pressure-PG
  unsigned char		EVLVCDT[4]			;		// Valves current sts (pneumatic,Bypass)
  unsigned char		EVLVPDT[4]			;		// Valves previous sts(pneumatic,Bypass)
  unsigned short	EWFLODT[1]			;		// Cascade Flow(Waste)
												// Mini-Cascade data
  unsigned short	MCSPDT[TOTMIN][3]	;		// Cascade stage prs.(FR1,FR0,FS1)
  unsigned short	MVCPDT[TOTMIN]		;		// Vacuum Chamber Pressur
  unsigned char		MVLVCDT[TOTMIN][2]	;		// Valve current status
  unsigned short	MMCHFRQ[3][52]		;		// Mini# 01-03: 52 Mchs Frequencies
  unsigned short	MMCHFRQ2[3][52]		;		// Mini# 04-06: 52 Mchs Frequencies
  unsigned short	MMCHFRQ3[3][52]		;		// Mini# 07-09: 52 Mchs Frequencies
  unsigned short	MMCHFRQ4[3][52]		;		// Mini# 10-12: 52 Mchs Frequencies
  unsigned char		MVLVPDT[TOTMIN][2]	;		// Valves previous status
  unsigned char		MMCHCDT[TOTMIN][7]	;		// Machines Status  (Current data)
  unsigned char		MMCHCSDT[TOTMIN][7]	;		// Machines SlipSts (Current data)
  unsigned char		MMCHPDT[TOTMIN][7]	;		// Machines Status  (Previous data)
  unsigned char		MMCHPSDT[TOTMIN][7]	;		// Machines SlipSts (Previous data)
  unsigned char		MSBOXCDT[TOTMIN]	;		// Machines SlipBox (Current data)
  unsigned char		MSBOXPDT[TOTMIN]	;		// Machines SlipBox (Previous data)
  unsigned short	MECRDCDT[3]			;		// ElectCardsSts(VLV,PRS,MCH)CurData
  unsigned short	MECRDPDT[3]			;		// ElectCardsSts(VLV,PRS,MCH)PreData
  unsigned short	xxxxx[3]			;		// ElectCardsSts(VLV,PRS,MCH)PreData
};

struct c12pusdt {								// C12(B1) limits structure
  												// E Cascade Data
  unsigned char 	ERTCDT[6]			;		// E Cas date and time
  unsigned char 	EPRSMS   			;		// E Cas Pre gauges mask sts (EHP, FFR)
  unsigned char 	EVLVMS[2]			;		// E Cas Vlvs mask sts (1*EVLV + 1*SBP)
  unsigned char 	ESERDT   			;		// E Cas SER mask data (EHP, FFR, VLV)
  unsigned char 	EVDYDT[4]			;		// E Cas Valves action delay (Grp A B C D)
  unsigned char 	EPDYDT   			;		// E Cas Pressures S.Es. delays
  unsigned short	ELMTDT[3+2]			;		// E Cas limits (3*EHP + 2*FFR)
												// Mini Cascade Data
  unsigned char 	MMCHMS[TOTMIN][7]	;		// M Cas Machines mask status (52 Mchs)
  unsigned char 	MPRSMS[TOTMIN]		;		// M Cas Pre gauages mask sts (3*CSP, VCP)
  unsigned char 	MVLVMS[TOTMIN]		;		// M Cas Valves mask status
  unsigned char 	MSERDT[TOTMIN]		;		// M Cas SER mask data (REC CSP FIM VCP VLV MCH)
  unsigned char 	MVDYDT[TOTMIN][4]	;		// M Cas Valves action delay (Grp A B C D)
  unsigned char 	MPDYDT[TOTMIN]		;		// M Cas Pressures S.Es. delays
  unsigned char 	MMDYDT[TOTMIN]		;		// M Cas Machines  S.Es. delays
  unsigned short	MCSPLDT[TOTMIN][4]	;		// M Cas limits (NRM L1 L2 L3)
  unsigned short	MVCPLDT[TOTMIN][2]	;		// M Cas limits (NRM ACT)
};

char 		 Cs, ECS, EDCS, SE;
unsigned char RSE, SRSE, DR, DRAck, SlaveCMF;
//unsigned char ILCMods[14], ILCUMods[14];
unsigned char EVLVCMD[2], MVLVCMD[12];
unsigned char CONBMP[2], PCFGData[15], PVLVData[30], CCFGData[15], CVLVData[30];
//unsigned char EVLVCMD[2], MVLVCMD[12];
unsigned char MOpenValCfg[6], EOpenValCfg[18];
unsigned char *t1Ptr, *t2Ptr, *t3Ptr, *r1Ptr, *r2Ptr, *r3Ptr, *Endr3Ptr, *Endt3Ptr;
unsigned char ILCCasBmp[2], ILCDatMods, ILCECasMods, ILCMinMods[12];
unsigned char ATLCasBmp[2], ATLDatMods, ATLECasMods, ATLMinMods[12];
unsigned char MSEBuf[100];// __attribute__((far));

unsigned char MOpenValCfg[6]  = {0xE0,  0xF0, 0xE8, 0x17,   0x0F,   0x07} ;
							   //RECIRC VEVAC DEVAC	NORMALE	NORMALD	NORMAL

unsigned char EOpenValCfg[18] = { 0x00,   0x40,  0x38,   0x78,  0x34,  0x74, 0x1C,  0x5C, 0x2A, 
								//RECIRC* RECIRC PWDEVC* PWDEVC	PWEVC* PWEVC PDEVC*	PDEVC WDEVC*
								  0x6A, 0x06,   0x46,  0x0E,  0x4E, 0x07,   0x47,  0x0F,   0x4F
								//WDEVC FINTDC*	FINTDC FINTR* FINTR	NORMDC* NORMDC NORMAL* NORMAL
						   		} ;

int			CHSEFlag, PCCSEFlag, CHSEInProgress, PCCSEInProgress, DRInProgress;
int 		MCVCmdInProgress, ErmTxInProgress, MTxInProgress, ETxInProgress;
int 		DRDataRecd, DRAckRecd, SlaveCMFReq, MasterSERec, FCVFlag, WCVFlag;
int 		RST_SE_Recieved, SlavePDRReq, SlavePUSReq, SlaveAckReq;
int			ILCTxReq, ATLTxReq, ERM, LEDFlag, SPFValue, SBFValue;

int 		DRTmoCnt, DRDataTmoCnt, VlvCmdTmoCnt, MSETmoCnt, DRDelay;
int			DR80Cnt, ECSError, MCSError, MDelay;
unsigned int FWFLODT[2];						// Cascade Flow(Feed, Waste)
unsigned int VFrameCnt, DspMasterCnt, ESECnt, EError, MError, SlaveCMFSend;
unsigned int DRCnt, Tmr1Cnt, t1Cnt, r1Cnt, t2Cnt, r2Cnt, t3Cnt, r3Cnt;
unsigned long i,j;

unsigned char t1Buf[1200] __attribute__((far));
//unsigned char *t1Ptr __attribute__((far));
unsigned char r1Buf[100] __attribute__((far));
//unsigned char *r1Ptr __attribute__((far));
unsigned char t2Buf[500] __attribute__((far));
//unsigned char *t2Ptr __attribute__((far));
unsigned char r2Buf[100] __attribute__((far));
//unsigned char *r2Ptr __attribute__((far));
unsigned char t3Buf[50] __attribute__((far));
//unsigned char *t3Ptr __attribute__((far));
unsigned char r3Buf[1000] __attribute__((far));
//unsigned char *r3Ptr __attribute__((far));
unsigned char ESEBuf[1000] __attribute__((far));

/********** *** ** * Data Defined By Waseem Akhtar * ** *** **********/ 
unsigned char	ALLineSts, FirstTimeRun = 0xFF, ChanBMP[2], EINVDT = 0 ;
unsigned short	PEFLODT[2], PECHBGDT[1], PECHPGDT[2], PEEHPDT, PEEHPPGDT ;  //Previous Data
unsigned short	PMCSPDT[TOTMIN][3],	PMVCPDT[TOTMIN]	;						//Previous Data

		 char	MchSeDelay[TOTMIN][52] __attribute__((far));
unsigned short	PMMCHFRQ[3][52] __attribute__((far));unsigned short	PMMCHFRQ2[3][52] __attribute__((far));unsigned short	PMMCHFRQ3[3][52] __attribute__((far));unsigned short	PMMCHFRQ4[3][52] __attribute__((far));
/********** *** ** * Data Defined By Khalid Malik * ** *** **********/
//*///////////////////
unsigned char	 FFRCStatus = 0, FFRPStatus = 0,EHPPStatus = 0 , EHPCStatus = 0;
unsigned char	VCPCStatus[TOTMIN], VCPPStatus[TOTMIN] ;
unsigned short  CSPPStatus[TOTMIN], CSPCStatus[TOTMIN] ;
unsigned char	FFRATLReq, FFRSEReq, EHPSEReq, VCPSEReq;
unsigned char   CSPATLReq[TOTMIN], CSPATLMOD[TOTMIN];
char		  	FFRSEDelay, VCPSEDelay[TOTMIN], CSPSEDelay[TOTMIN], EHPSEDelay ;
///////////////////*/
/********** *** ** * Data Defined By Shakeel Muzaffar * ** *** ******/
// Valve Local Counters
char VAD_CT[TOTCAS];
char VSD_CT[TOTCAS];
char VCD_CT[TOTCAS];
char FIM_DL;
char CMD_CT;
char VPD_CT[TOTCAS];
char VSC_CT[TOTCAS];

// Valve Action Delays
char VAD_D1[TOTCAS];				// MDV action delay D=5s
char VAD_D2[TOTCAS];                // MFV,WDV,DPs action dly D=3s
char VAD_D3[TOTCAS];                // MPV,MWV action delay D=1s
char VAD_D4[TOTCAS];                // Valves S.Es. delay D=5s

unsigned char V_EVPDM[2];    		// Main Cascade VPD current bitmap
unsigned char V_EVPDS[2];           // Main Cascade VPD previous bitmap
unsigned char ATLBMP[2];    		// ATL S.E. Bitmap
unsigned char ILCBMP[2];            // ILC S.E. Bitmap
unsigned char VADBMP[2];            // Valve Action Delay Bitmap
unsigned char V_MVPDM[TOTCAS];      // Mini Cascade VPD current bitmap
unsigned char V_MVPDS[TOTCAS];      // Mini Cascade VPD previous bitmap
unsigned char VLV_SM[2];           	// Valves SER Enabel/Disable Status
unsigned char V_EMask[2];     		// Main Casacade Valves mask bitmap
unsigned char V_MMask[TOTMIN];     	// Mini Casacade Valves mask bitmap
unsigned char BYPBMP[2];			// Bypass Valve Status Change Bitmap
unsigned char ILCMOD[TOTCAS];       // ILC S.E. Modifiers 
unsigned char ATLMOD[TOTCAS];       // ATL S.E. Modifiers 
unsigned char CCDCSTS;
unsigned char CCDPSTS;
 
int F_ValCmdReq;
int F_HWC[TOTCAS];
int F_DR80AfterVCmd;
int F_AutLine;
int F_CCA;
int F_CCD;							//Send CCD Occured/Removed SE Flag
int F_VCDVSD[TOTCAS];
int V_MCFGNo[TOTMIN];

unsigned char BYPBIT[TOTMIN]={R4B,R3B,R2B,R1B,R1B,R0B,R0B,R0B,S1B,S1B,S2B,S2B};

unsigned char MinCFG[6] =  {   // Mini Cascade Valves configurations table
			     0xE0,   // 1 - RECIRC
			     0xF0,   // 2 - VEVAC
			     0xE8,   // 3 - DEVAC
			     0x17,   // 4 - NORMALE
			     0x0F,   // 5 - NORMALD
			     0x07    // 6 - NORMAL
			    };

unsigned char MainCFG[18] = {   // Main Cascade Valves configurations table
			     0x00,   //  1 - RECIRC*
			     0x40,   //  2 - RECIRC
			     0x38,   //  3 - PWDEVC*
			     0x78,   //  4 - PWDEVC
			     0x34,   //  5 - PWEVC*
			     0x74,   //  6 - PWEVC
			     0x1C,   //  7 - PDEVC*
			     0x5C,   //  8 - PDEVC
			     0x2A,   //  9 - WDEVC*
			     0x6A,   // 10 - WDEVC
			     0x06,   // 11 - FINTDC*
			     0x46,   // 12 - FINTDC
			     0x0E,   // 13 - FINTR*
			     0x4E,   // 14 - FINTR
			     0x07,   // 15 - FINTR*
			     0x47,   // 16 - FINTR
			     0x0F,   // 17 - NORMAL*
			     0x4F    // 18 - NORMAL
			    } ;

unsigned char EGROUP[3] = 
              { 0x08,    // Main Cascade Group1 - MDV group 5 sec
			    0x71,    // Main Cascade Group2 - MFV,WDV,DPs(PDV,MEV) group 3 sec
			    0x06     // Main Cascade Group3 - MPV,MWV group 1 sec
			  };

/********** *** ** * End For Common.c * ** *** **********/

struct c12dat   C12DAT, *C12Ptr;
struct c12pusdt C12PUSDT, *C12PUSPtr;

/********************************* END OF FILE *******************************************/
