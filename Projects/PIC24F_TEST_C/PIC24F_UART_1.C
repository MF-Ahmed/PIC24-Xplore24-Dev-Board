//*************************************************************************************
#include <p24FJ256GA106.h>
#include "adc_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char sys[]={"\r\nSystem Power fail occured \r\n X"};
unsigned char stby[]={"\r\nStandbye Power fail occured\r\n X"};
volatile float adc_num;

unsigned char *sysp,*stbyp;
int count,j	,adc_val;
unsigned char rx_1,rx_2,rx_3,rx_4;
volatile unsigned char adc_buff[5],i;

//*************************************************************************************
//*************************************************************************************
//********************* FUNCTION PROTOTPYES *******************************************

char Rx_UARTS(unsigned char);
void USART2_TX(unsigned char);
void RemapPins(void);
void config_Ports(void);
void Tx_UARTS(volatile unsigned char,volatile int);
void config_USARTs(void);
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
#define  ERM   1  /// ERM
#define  CL    2  /// CURRENT LOOP
#define  ELE   4  /// ELECTRONICS
#define  MAS   3  /// MASTER
volatile unsigned int adcval;
volatile unsigned char adcvalL,adcvalH,chno;

int main(void)
 {
	
   
    RemapPins();
	//config_Ports();
	config_USARTs();
    ConfigADC();
	sysp=&sys;
    stbyp=&stby;
   
    //adcval=0;
  // DelayMs();
    //adcval=ReadADC(7);
            //adcvalL=adcval;
    		//adcvalH=adcval>>8;
    		//Tx_UARTS(ELE,adcvalH);
			//Tx_UARTS(ELE,adcvalL);
  // bintoascii_c(0X21);
//j=sprintf(adc_buff,"%f",adc_num);
//itoa(50,adc_buff,10);
//for(i=0;i<4;i++){
//Tx_UARTS(ELE,adc_buff[i]);

	adcval=ReadADC(6);
   	while(1){


           
     		//adc_num = adc_val * 0.00322;
     	
    		//Tx_UARTS(CL,adcvalL);
			//Tx_UARTS(CL,adcvalH);


/*
           rx_3=Rx_UARTS(CL);
			//Tx_UARTS(ELE,rx_3);
            
			if(rx_3=='A' || rx_3 =='a'){
				while(*sysp!='X'){ 
				 Tx_UARTS(CL,*sysp++);  
				}
				sysp=&sys;stbyp=&stby;
				rx_3=0;
			}  			
	        if(rx_3=='B' || rx_3 == 'b'){
				while(*stbyp!='X'){  
				Tx_UARTS(CL,*stbyp++);
				}
                sysp=&sys;stbyp=&stby;
				rx_3=0;
			
              }
            */ 
		      //DelayMs(); DelayMs(); DelayMs();   
              	//adcval=ReadADC(7);
     //adc_num = adc_val * 0.00322;
     //adcvalL=adcval;
     //adcvalH=adcval>>8;
    //Tx_UARTS(CL,adcvalL);
	//Tx_UARTS(CL,adcvalH);
    //DelayMs(); DelayMs(); DelayMs(); 
    			
  } 
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

  U1BRG=103;			//BR=9600				 /* ERM */
  U1MODE=0x8004;		// UART1 Enable, 8 bit data, odd parity, one stop bit
  U1STA=0x8400;			// UART1 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
  
  U3BRG=103;	//BR=9600
  U3MODEbits.PDSEL1=1;  	// 8 bit data, odd parity, one stop bit    /*  MASTER */
  U3MODEbits.UARTEN=1;	// ENABLE TX,RX
  U3STAbits.UTXEN=1;

  //U3STAbits.UTXISEL1=1;
  //U3BRG=103;			//BR=9600	
  // U3MODE=0x8004;		// UART4 Enable, 8 bit data, odd parity, one stop bit
  //U3STA=0x8400;			// UART4 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full


  U2BRG=103;			//BR=9600					 /*  CURRENT LOOP */
  U2MODEbits.PDSEL1=1;  	// 8 bit data, odd parity, one stop bit
  U2MODEbits.UARTEN=1;	// ENABLE TX,RX
  U2STAbits.UTXEN=1;

  U4BRG=103;	//BR=9600
  U4MODEbits.PDSEL1=1;  	// 8 bit data, odd parity, one stop bit
  U4MODEbits.UARTEN=1;	// ENABLE TX,RX
  U4STAbits.UTXEN=1;





  //U4BRG=103;			//BR=9600	
  //U4MODE=0x8004;		// UART4 Enable, 8 bit data, odd parity, one stop bit
  //U4STA=0x8400;			// UART4 TxIntr on Tx buffer empty, Transmit enable, RxIntr on Rx buffer full
   
  U4BRG=103;	//BR=9600						 /*  ELECTRONICS */
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

char Rx_UARTS(unsigned char uno)
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
/*
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
*/
void _ISRFAST _ADC1Interrupt(void)
{
    adcvalL=adcval;
    adcvalH=adcval>>8;
    Tx_UARTS(CL,adcvalH);
	Tx_UARTS(CL,adcvalL);



}


//void bintoascii_c(unsigned int adc_val)
//{ 
//unsigned char i;



//}


	
