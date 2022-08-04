/********** *** ** * Data Defined By Khalid Malik * ** *** **********/
/*///////////////////

unsigned char FFRCStatus = 0, FFRPStatus = 0,EHPPStatus = 0 , EHPCStatus = 0;
unsigned char  VCPCStatus[TOTMIN], VCPPStatus[TOTMIN],CSPPStatus[TOTMIN],CSPCStatus[TOTMIN] ;
unsigned char FFRATLReq, FFRSEReq, EHPSEReq, VCPSEReq;
char		  FFRSEDelay, VCPSEDelay[TOTMIN], CSPSEDelay[TOTMIN], EHPSEDelay ;
///////////////////*//********** *** ** * End For Common.c * ** *** **********/

/********** *** ** * Data Defined By Khalid Malik * ** *** **********/
/*////////////////////////

extern unsigned char FFRCStatus , FFRPStatus ,EHPPStatus , EHPCStatus;
extern unsigned char  VCPCStatus[TOTMIN], VCPPStatus[TOTMIN],CSPPStatus[TOTMIN],CSPCStatus[TOTMIN] ;
extern unsigned char FFRATLReq, FFRSEReq, EHPSEReq, VCPSEReq ;
extern char		  FFRSEDelay, VCPSEDelay[TOTMIN], CSPSEDelay[TOTMIN], EHPSEDelay ;
/*////////////////////////********** *** ** * End For common.h * ** *** **********/

/*************** *** ** * MAIN Routine * ** ** **************/int SwapData(short *SPtr) ;
int ChkGaugNumHig(short,short,short,short) ;
int ChkGaugNumLow(short,short,short,short) ;

int SwapData(short *SPtr) 
{
return(0);
}
int main()
{
  ProcAnaData();
 
  return(0) ;
}
/*************** *** ** * End Of File * ** ** ***************/