#define TOTCAS	13
#define TOTMIN	12
#define MainCas	12
#define STOPCT	-1
#define DR_DL	31				// 1 tick=100ms, 3 sec
#define MGROUP  0xFF            //Mini Cascade Group - DPs group 3 sec

// Default Values
#define FFRNormal	0x05AA		// FFR Normal Limit (1.45v)
#define FFRSafety	0x05DC		// FFR Safety Limit (1.50v)
#define EHPNormal	0x00FA		// EHP Normal Limit (0.25 Torr)
#define EHPLimit1	0x01F4		// EHP Limit1 (0.5 Torr)
#define EHPLimit2	0x0FA0		// EHP Limit2 (4.0 Torr)
#define CSPNormal	0x0ED8		// CSP Normal (3.8 Torr)
#define CSPLimit1	0x0FA0		// CSP Limit1 (4.0 Torr)
#define CSPLimit2	0x1388		// CSP Limit2 (5.0 Torr)
#define CSPLimit3	0x1770		// CSP Limit3 (6.0 Torr)
#define VCPNormal	0x1055		// VCP Normal (2.0E-1 MBar)
#define VCPLimit1	0x16FD		// VCP Limit1 (1.0E+0 MBar)
#define VSD_TM      51      	// VSD action delay(5s) in sec  
#define VCD_TM      51      	// VCD action delay(5s) in sec 
#define VPD_DL      51      	// VPD S.Es.  delay(5s) in sec
#define VPD_MF      0x10    	// Valves position discrepancy

#define MR41		0
#define MS22		11
#define R4B			0x01
#define R3B			0x02
#define R2B			0x04
#define R1B			0x08
#define R0B			0x10
#define S1B			0x20
#define S2B			0x40

// Valve Local Counters
char VAD_CT[TOTCAS];
char ATLRTY_CT[TOTCAS];
char VSD_CT[TOTCAS];
char VCD_CT[TOTCAS];
char FIM_DL;
// Valve External Counters
extern char CMD_CT;
extern char VPD_CT[TOTCAS];
extern char VSC_CT[TOTCAS];

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
 
int F_VCMD;
int F_HWC[TOTCAS];
int F_VCMD80;
int F_AutLine;
int F_CCA;
int F_CCD;							//Send CCD Occured/Removed SE Flag
int V_ECFGNo;
int F_VCDVSD[TOTCAS];
int V_MCFGNo[TOTMIN];

unsigned char BYPBIT[TOTMIN]={R4B,R3B,R2B,R1B,R1B,R0B,R0B,R0B,S1B,S1B,S2B,S2B};

unsigned char MCFG[6] =  {   // Mini Cascade Valves configurations table
			     0xE0,   // 1 - RECIRC
			     0xF0,   // 2 - VEVAC
			     0xE8,   // 3 - DEVAC
			     0x17,   // 4 - NORMALE
			     0x0F,   // 5 - NORMALD
			     0x07    // 6 - NORMAL
			    };

unsigned char ECFG[18] = {   // Main Cascade Valves configurations table
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

// Prototypes 
int  ProcVal(int);
int  Chk_VLV_VPD(int CasNo);
void UpdPrvStsFromCurBuf(int CasNo);
void UpdPrvBypFromCurBuf(int CasNo);
void UpdPrvStsFromCmd(int CasNo);
void UpdPrvBypFromCur(int CasNo);
void UpdPrvStsFromCmd(int CasNo);
void UpdPrvBypFromCmd(int CasNo);
void UpdVlvCmdFromCurBuf(int CasNo);
void UpdBypCmdFromCurBuf(int CasNo);
void Chk_VLV_Mask(int CasNo);
void MaskData(unsigned char *CurVPtr, unsigned char *PrvVPtr, unsigned char *MaskVPtr);
int Chk_BYP_VLV(int CasNo);
void UpdVpdPrvBmp(int CasNo);
int Chk_VLV_ECFG(unsigned char CurCFG);
int Chk_VLV_MCFG(unsigned char CurCFG, int CasNo);
void PutCasToHWC(int CasNo);
void SBITVAD(int CasNo);
void StartVAD(int CasNo);
void CLR_VPD(int CasNo);
void IniVar(void);
void CK_CCD(void);
void SETCON(void);
void ProcValIni(void);
void SendCommand(void);
void ValSendSE(void);

extern unsigned char ILCCasBmp[2], ILCDatMods, ILCECasMods, ILCMinMods[TOTMIN];
extern unsigned char ATLCasBmp[2], ATLDatMods, ATLECasMods, ATLMinMods[12];
extern int ILCTxReq, ATLTxReq;


