//*************************************************************************************
#include <p24FJ256GA106.h>
//#include "adc_test.h"


//_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2);
//_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_XT & FNOSC_PRI &IOL1WAY_ON);
//_CONFIG3( WPCFG_WPCFGDIS & WPDIS_WPDIS);

_CONFIG1(0x7F7F);
_CONFIG2(0xF2FD);
_CONFIG3(0xFFFF);

unsigned char sys[]={"\r\nSystem Power fail occured \r\n X"};
unsigned char stby[]={"\r\nStandbye Power fail occured\r\n X"};
unsigned char *sysp,*stbyp;
int count;

//*************************************************************************************
//*************************************************************************************
//********************* FUNCTION PROTOTPYES *******************************************

int Rx_UARTS(unsigned char);
void USART2_TX(unsigned char);
void RemapPins(void);
void config_Ports(void);
void Tx_UARTS(volatile unsigned char,volatile int);
void config_USARTs(void);
void ConfigADC(void);
// ADC reff = 3.3v resolation = 10bits --> 1 bit = 3.3v/1024 = 3.2mV 
// e.g if hex value is (3F8) = 1016 * 3.2mV = 3.25V
// e.g if input  is 2V the hex value read is  2V/3.2mV =  625 = (271)
//*************************************************************************************
//*************************************************************************************
/////////////////////////// USART-1 ------------> ERM/////////////////////////////////
////////////////////////USART-2 ------------> Current Loop////////////////////////////
///////////////////////////USART-3 ------------> Electronics//////////////////////////
////////////////////////////USART-4 ------------> Tx/Rx///////////////////////////////
//*************************************************************************************
//*************************************************************************************
#define  ERM   1
#define  CL    2
#define  ELE   3
#define  TxRx  4
volatile unsigned int adcval;
volatile unsigned char adcvalL,adcvalH,chno;


int main(void)
 {
	    
    RemapPins();
	//config_Ports();
	config_USARTs();
    ConfigADC();
	sysp=&sys;stbyp=&stby;
  adcval=ReadADC(6);
    //
    //Tx_UARTS(TxRx,adcvalH);
	//Tx_UARTS(TxRx,adcvalL);
   	while(1){

   
    adcvalL=adcval;
   adcvalH=adcval>>8;

    Tx_UARTS(TxRx,adcvalH);
	Tx_UARTS(TxRx,adcvalL);
     DelayMs();
   //adcval=ReadADC(7);
            //adcvalL=adcval;
    		//adcvalH=adcval>>8;
    		//Tx_UARTS(TxRx,adcvalH);
			//Tx_UARTS(TxRx,adcvalL);
			//DelayMs();DelayMs();DelayMs();DelayMs();
			//DelayMs();DelayMs();DelayMs();DelayMs();DelayMs();DelayMs();DelayMs();DelayMs();
		    //count=Rx_UARTS(TxRx);
			//if(count=='A' || count =='a'){
			//	while(*sysp!='X'){ 
			//	 Tx_UARTS(TxRx,*sysp++);  
			//	}
			//	sysp=&sys;stbyp=&stby;
			//	count=0;
			//}  			
	        //if(count=='B' || count == 'b'){
			//	while(*stbyp!='X'){  
			//	Tx_UARTS(TxRx,*stbyp++);
			//	}
              //  sysp=&sys;stbyp=&stby;
			///	count=0;
			//} 			
  } 
return 0;
}
void RemapPins(void)
{
  // Unlock Registers
  asm volatile ("MOV 	#OSCCON, w1  \n"
  				"MOV 	#0x46, 	w2 	 \n"
  				"MOV 	#0x57, 	w3 	 \n"
  				"MOV.b 	w2, 	[w1] \n"
  				"MOV.b 	w3, 	[w1] \n"
				"BCLR 	OSCCON,	#6"
				);
//**************************************************************

  RPINR18bits.U1RXR = 11;  			// Assign U1RX To Pin RP11 
  RPOR6bits.RP12R 	= 3 ;			// Assign U1TX To Pin RP12
  
  RPINR19bits.U2RXR = 4 ;  			// Assign U2RX To Pin RP4
  RPOR1bits.RP3R 	= 5 ;			// Assign U2TX To Pin RP3
  
  RPINR17bits.U3RXR = 8 ;			// Assign U3RX To Pin RP8 
  RPOR4bits.RP9R 	= 28;  			// Assign U3TX To Pin RP9


  RPINR27bits.U4RXR = 27;			// Assign U4RX To Pin RP27
  RPOR13bits.RP26R 	= 30;			// Assign U4TX To Pin RP26

//***************************************************************
// Lock Registers
  asm volatile ("MOV #OSCCON, w1 \n"
				"MOV #0x46, w2 \n"
				"MOV #0x57, w3 \n"
				"MOV.b w2, [w1] \n"
				"MOV.b w3, [w1] \n"
				"BSET OSCCON, #6" 
				);
}
//*************************************************************************************
void config_Ports(void)
{
//TRISD=8; //RD3=Data pin no 51 input Data INPUT
//PORTD=0;
//TRISE=0x0000;

}
//*************************************************************************************

void config_USARTs(void)
{							 

//BRGx=(FCY/16*BR)-1, where FCY=Fosc/2
  //In our case as Fosc=32MHz So, BRGx=((32000000/2)/16*BR)-1

  U1BRG=103;			//BR=9600	
  U1MODE=0x8004;		// UART1 Enable, 8 bit data, odd parity, one stop bit
  U1STA=0x8400;			// UART1 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
  
  U3BRG=103;	//BR=9600
  U3MODEbits.PDSEL1=1;  	// 8 bit data, odd parity, one stop bit
  U3MODEbits.UARTEN=1;	// ENABLE TX,RX
  U3STAbits.UTXEN=1;

  //U3STAbits.UTXISEL1=1;
  //U3BRG=103;			//BR=9600	
  // U3MODE=0x8004;		// UART4 Enable, 8 bit data, odd parity, one stop bit
  //U3STA=0x8400;			// UART4 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full


  U2BRG=103;			//BR=9600
  U2MODE=0x8004;		// UART4 Enable, 8 bit data, odd parity, one stop bit
  U2STA=0x8400;			// UART4 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full  

  //U4BRG=103;			//BR=9600	
  //U4MODE=0x8004;		// UART4 Enable, 8 bit data, odd parity, one stop bit
  //U4STA=0x8400;			// UART4 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
   
  U4BRG=103;	//BR=9600
  U4MODEbits.PDSEL1=1;  	// 8 bit data, odd parity, one stop bit
  U4MODEbits.UARTEN=1;	// ENABLE TX,RX
  U4STAbits.UTXEN=1;
}

//************************************************************************************
//************************************************************************************

void DelayMs(void)
{
unsigned long f;
for(f=0;f<50000;f++);

}
//************************************************************************************
//************************************************************************************


int Rx_UARTS(unsigned char uno)
{ 
 switch(uno){
  case 1:
    while(!U1STAbits.URXDA);
    return U1RXREG; 
    break;
  case 2:
    while(!U2STAbits.URXDA);
    return U2RXREG; 
    break;
  case 3:
    while(!U3STAbits.URXDA);
    return U3RXREG; 
    break;
  case 4:
   while(!U4STAbits.URXDA);
   return U4RXREG;
    break;
 }

}
//************************************************************************************
//************************************************************************************




void _ISRFAST _ADC1Interrupt(void)
{
   while(!IFS0bits.AD1IF);
   IFS0bits.AD1IF=0;
 
}
//************************************************************************************
//************************************************************************************



void ConfigADC()
{
  /*
  AD1PCFG=0xFF3F;
  //AD1PCFG=0xFF9F;
  AD1CON1=0x00E0;
  //AD1CSSL=0x0000;
  //AD1CSSL=0x0060;
  AD1CSSL=0x00C0;
  AD1CON3=0x1F02;
  AD1CON2=0x0000;
  AD1CON1bits.ADON=1;				// A/D convertter module is operating

 

  AD1PCFG=0xFF9F;
  //////AD1PCFG=0xFF9F;
  /////AD1CON1=0x00E0;
  AD1CON1bits.SSRC0=1;
  AD1CON1bits.SSRC1=1;
  AD1CON1bits.SSRC2=1;
  AD1CON2bits.VCFG0=0;
  ////AD1CSSL=0x0000;
  ////AD1CSSL=0x0060;
  AD1CON3bits.SAMC0=1;///
  AD1CON3bits.SAMC1=1;///  
  AD1CON3bits.SAMC2=1;///  31TAD
  AD1CON3bits.SAMC3=1;///
  AD1CON3bits.SAMC4=1;///
  AD1CON3bits.ADCS1=1;
  AD1CSSLbits.CSSL6=0;
  AD1CSSLbits.CSSL7=0;
  ////AD1CSSL=0x00C0;
  ///////AD1CON3=0x1F02;
  //AD1CON2=0x0000;
  AD1CON1bits.ADON=1;				// A/D convertter module is operating

 
   */

  IEC0bits.AD1IE=1;
/*
  AD1PCFG=0XFF3F;
  
 
  
  AD1CON1bits.SSRC0=1;       //
  AD1CON1bits.SSRC1=1;		//	AUTO COVERT(INternal Counter)
  AD1CON1bits.SSRC2=1;		//
  AD1CON1bits.ASAM=1;    // Sampling  begins immediatly (SAMP is Auto-set)



  AD1CON2bits.SMPI0=1;//
  AD1CON2bits.SMPI1=1;//  interrupts at 16th sample
  AD1CON2bits.SMPI2=1;//
  AD1CON2bits.SMPI3=1;//

 

  AD1CON3bits.SAMC0=1;///
  AD1CON3bits.SAMC1=1;///  
  AD1CON3bits.SAMC2=1;///  31TAD
  AD1CON3bits.SAMC3=1;///
  AD1CON3bits.SAMC4=1;///

  AD1CON3bits.ADCS0=1;
  AD1CON3bits.ADCS1=1;
  AD1CON3bits.ADCS2=1;
  AD1CON3bits.ADCS3=1;
  AD1CON3bits.ADCS4=1;
  AD1CON3bits.ADCS5=1;
  AD1CON3bits.ADCS6=1;
  AD1CON3bits.ADCS7=1;


  AD1CSSL=0x0000;
 */
 
 
 AD1CON1=0x00E4;
 AD1CON2=0x003C;
 //AD1CON3= 0x0D09;
  AD1CON3=0x1F02;
 AD1PCFG=0xFF3F;
 AD1CSSL=0x00C0;



  AD1CON1bits.ADON=1;
 


}


//************************************************************************************
//************************************************************************************


int ReadADC(int ch)
{
  AD1CHS = ch;						// Select Channel no.
  AD1CON1bits.SAMP = 1;				// Ends sampling and starts conversion
  DelayMs();
 AD1CON1bits.SAMP = 0;				// Ends sampling and starts conversion
 while(!AD1CON1bits.DONE);			// Conversion done ?
 return ADC1BUF0;     				// Read the conversion result
  //if(ch==5) return ADC1BUF5;     				// Read the conversion result
  //else return ADC1BUF6;     				// Read the conversion result
}



//************************************************************************************
//************************************************************************************

void _ISRFAST _ADC1Interrupt(void);


//************************************************************************************
//************************************************************************************

void Tx_UARTS(unsigned char uartno,int Txdata)
{
 switch(uartno){
  
  case 1:
    while(U1STAbits.UTXBF);
	U1TXREG=Txdata;
    break;
  case 2:
    while(U2STAbits.UTXBF);
	U2TXREG=Txdata;
    break;
  case 3:
   while(U3STAbits.UTXBF);
	U3TXREG=Txdata;
    break;
  case 4:
    while(U4STAbits.UTXBF);
	U4TXREG=Txdata;
    break;
 }

}
//************************************************************************************
//************************************************************************************
