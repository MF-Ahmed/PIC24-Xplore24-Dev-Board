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

struct c12dat   C12DAT, *C12Ptr;
struct c12pusdt C12PUSDT, *C12PUSPtr;

char 		 Cs, ECS, EDCS, SE;
unsigned char RSE, SRSE, DR, DRAck, SlaveCMF;
//unsigned char ILCMods[14], ILCUMods[14];
unsigned char CONBMP[2], PCFGData[15], PVLVData[30], CCFGData[15], CVLVData[30];
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
unsigned char	ALLineSts, FirstTimeRun = 0x0F, ChanBMP[2], EINVDT = 0 ;
unsigned short	PEFLODT[2], PECHBGDT[1], PECHPGDT[2], PEEHPDT, PEEHPPGDT ;  //Previous Data
unsigned short	PMCSPDT[TOTMIN][3],	PMVCPDT[TOTMIN]	;						//Previous Data

		 char	MchSeDelay[TOTMIN][52] __attribute__((far));
unsigned short	PMMCHFRQ[3][52] __attribute__((far));unsigned short	PMMCHFRQ2[3][52] __attribute__((far));unsigned short	PMMCHFRQ3[3][52] __attribute__((far));unsigned short	PMMCHFRQ4[3][52] __attribute__((far));/********** *** ** * End For Common.c * ** *** **********/

/********** *** ** * Data Defined By Khalid Malik * ** *** **********/
//*///////////////////
unsigned char FFRCStatus = 0, FFRPStatus = 0,EHPPStatus = 0 , EHPCStatus = 0;
unsigned char  VCPCStatus[TOTMIN], VCPPStatus[TOTMIN],CSPPStatus[TOTMIN],CSPCStatus[TOTMIN] ;
unsigned char FFRATLReq, FFRSEReq, EHPSEReq, VCPSEReq;
char		  FFRSEDelay, VCPSEDelay[TOTMIN], CSPSEDelay[TOTMIN], EHPSEDelay ;
///////////////////*//********** *** ** * End For Common.c * ** *** **********/

/********************************* END OF FILE *******************************************/
