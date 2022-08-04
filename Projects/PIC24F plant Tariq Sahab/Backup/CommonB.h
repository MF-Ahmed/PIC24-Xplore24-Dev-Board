/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   Common.h - 													* *                  												* *             								                        *
\*******************************************************************/
//#include <P24FJ256GA106.h>
//#include "KCSDF.h"

#define	CR	13
#define	LF	10

#define	CMF_SE_DataBytes	1	
#define	SFT_SE_DataBytes	2	
#define	ALC_SE_DataBytes	72
#define	WLC_SE_DataBytes	72
#define	ILC_CPSDataBytes	322
#define	ILC_SBOXDataBytes	13
#define	ILC_ECRDDataBytes	7
#define	ATL_SE_DataBytes	348
#define	PDR_SE_DataBytes	307 
#define	PUS_SE_DataBytes	362
#define	ms_62				4065
#define	ms_100				6550
#define	ms_125				8200
#define	ms_200				13100
#define	ms_1000				65535
//*/////////////////
extern struct c12dat {							// C12(B1) data structure
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

extern struct c12pusdt {						// C12(B1) limits structure
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
///////////////////*/

extern struct c12dat   C12DAT, *C12Ptr;
extern struct c12pusdt C12PUSDT, *C12PUSPtr;

extern char 		 Cs, ECS, EDCS, SE;
extern unsigned char RSE, SRSE, DR, DRAck, SlaveCMF;
//extern unsigned char ILCMods[14], ILCUMods[14];
extern unsigned char CONBMP[2], PCFGData[15], PVLVData[30], CCFGData[15], CVLVData[30];
extern unsigned char MOpenValCfg[6], EOpenValCfg[18];
extern unsigned char *t1Ptr, *t2Ptr, *t3Ptr, *r1Ptr, *r2Ptr, *r3Ptr, *Endr3Ptr, *Endt3Ptr;
extern unsigned char ILCCasBmp[2], ILCDatMods, ILCECasMods, ILCMinMods[12];
extern unsigned char ATLCasBmp[2], ATLDatMods, ATLECasMods, ATLMinMods[12];
extern unsigned char MSEBuf[100];// __attribute__((far));

extern int 			CHSEFlag, PCCSEFlag, CHSEInProgress, PCCSEInProgress, DRInProgress;
extern int 			MCVCmdInProgress, ErmTxInProgress, MTxInProgress, ETxInProgress;
extern int 			DRDataRecd, DRAckRecd, SlaveCMFReq, MasterSERec, FCVFlag, WCVFlag;
extern int 			RST_SE_Recieved, SlavePDRReq, SlaveAckReq, SlavePUSReq;
extern int 			ATLTxReq, ILCTxReq, ERM, LEDFlag, SPFValue, SBFValue;

extern int			DRTmoCnt, DRDataTmoCnt, VlvCmdTmoCnt, MSETmoCnt, DRDelay;
extern int			DR80Cnt, ECSError, MCSError, MDelay;
extern unsigned int FWFLODT[2];						// Cascade Flow(Feed, Waste)
extern unsigned int	VFrameCnt, DspMasterCnt, ESECnt, EError, MError, SlaveCMFSend;
extern unsigned int	DRCnt, Tmr1Cnt, t1Cnt, r1Cnt, t2Cnt, r2Cnt, t3Cnt, r3Cnt;
extern unsigned long i,j;

/********** *** ** * Data Defined By Waseem Akhtar * ** *** **********/ 
extern	unsigned char ALLineSts, FirstTimeRun, ChanBMP[2], EINVDT ;
extern	unsigned short	PEFLODT[2], PECHBGDT[1], PECHPGDT[2], PEEHPDT, PEEHPPGDT ;  //Previous Data
extern	unsigned short	PMCSPDT[TOTMIN][3],	PMVCPDT[TOTMIN]	;						//Previous Data

extern			 char	MchSeDelay[TOTMIN][52] __attribute__((far));
extern unsigned short	PMMCHFRQ[3][52] __attribute__((far)) ;extern unsigned short	PMMCHFRQ2[3][52] __attribute__((far));extern unsigned short	PMMCHFRQ3[3][52] __attribute__((far));extern unsigned short	PMMCHFRQ4[3][52] __attribute__((far));
/********** *** ** * End For Common.h * ** *** **********/

/********** *** ** * Data Defined By Khalid Malik * ** *** **********/
//*////////////////////////
extern unsigned char FFRCStatus , FFRPStatus ,EHPPStatus , EHPCStatus;
extern unsigned char  VCPCStatus[TOTMIN], VCPPStatus[TOTMIN],CSPPStatus[TOTMIN],CSPCStatus[TOTMIN] ;
extern unsigned char FFRATLReq, FFRSEReq, EHPSEReq, VCPSEReq ;
extern char		  FFRSEDelay, VCPSEDelay[TOTMIN], CSPSEDelay[TOTMIN], EHPSEDelay ;
//////////////////////*//********** *** ** * End For common.h * ** *** **********/

extern unsigned char t1Buf[1200] __attribute__((far));
//extern unsigned char *t1Ptr __attribute__((far));
extern unsigned char r1Buf[100] __attribute__((far));
//extern unsigned char *r1Ptr __attribute__((far));
extern unsigned char t2Buf[500] __attribute__((far));
//extern unsigned char *t2Ptr __attribute__((far));
extern unsigned char r2Buf[100] __attribute__((far));
//extern unsigned char *r2Ptr __attribute__((far));
extern unsigned char t3Buf[50] __attribute__((far));
//extern unsigned char *t3Ptr __attribute__((far));
extern unsigned char r3Buf[1000] __attribute__((far));
//extern unsigned char *r3Ptr __attribute__((far));
extern unsigned char ESEBuf[1000] __attribute__((far));

//extern void FillData() ;
extern void SwapData() ;
extern void initqs() ;
extern void DspToMaster() ;
extern void ConfigADC() ;
extern int SendPDR() ;
extern int SendPUS() ;
extern int SendCMF() ;
extern int SendILC() ;
extern int SendATL() ;
extern int AckMasterSE() ;
extern int putcq() ;
extern int puthq() ;
extern int putpq() ;
extern int getcq() ;
extern int gethq() ;
extern int getpq() ;
extern void GetEData() ;
extern void ProcDigitalData() ;
extern void ProcAnaData() ;



/********************************* END OF FILE *******************************************/
