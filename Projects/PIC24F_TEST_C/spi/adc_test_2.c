/******************************************************************** *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Apr-2009                             * *                                                                  * *   ADC.c - 														* *                  												* *             								                        *
 ********************************************************************/
#include <P24FJ256GA106.h>
#define USE_AND_OR /* To enable AND_OR mask setting */
#include<uart.h>
#include<PPS.h>
#include<spi.h>

#define master 1
#define slave 2

/*
To perform an A/D conversion:
1. Configure the A/D module:
	a) Configure port pins as analog inputs and/or
		select band gap reference input
		(AD1PCFGL<15:0> and AD1PCFGH<1:0>).
	b) Select voltage reference source to match
		expected range on analog inputs
		(AD1CON2<15:13>).
	c) Select the analog conversion clock to
		match desired data rate with processor
		clock (AD1CON3<7:0>).
	d) Select the appropriate sample/conversion
		sequence (AD1CON1<7:5> and AD1CON3<12:8>).
	e) Select how conversion results are
		presented in the buffer (AD1CON1<9:8>).
	f) Select interrupt rate (AD1CON2<5:2>).
	g) Turn on A/D module (AD1CON1<15>).
2. Configure A/D interrupt (if required):
	a) Clear the AD1IF bit.
	b) Select A/D interrupt priority.
*/

//int CHNO=7;

char Rx_UARTS(unsigned char uno);
void ConfigADC()
{
  AD1PCFG=0xFF3F;
  //AD1PCFG=0xFF9F;
  AD1CON1=0x00E0;
  //AD1CSSL=0x0000;
  //AD1CSSL=0x0060;
  AD1CSSL=0x00C0;
  AD1CON3=0x1F02;
  AD1CON2=0x0000;
  AD1CON1bits.ADON=1;				// A/D convertter module is operating
}

int ReadADC(int ch)
{
  AD1CHS = ch;						// Select Channel no.
  AD1CON1bits.SAMP = 1;				// Ends sampling and starts conversion
  while(!AD1CON1bits.DONE);			// Conversion done ?
  AD1CON1bits.SAMP = 0;				// Ends sampling and starts conversion
  return ADC1BUF0;     				// Read the conversion result
  //if(ch==5) return ADC1BUF5;     				// Read the conversion result
  //else return ADC1BUF6;     				// Read the conversion result
}

void _ISRFAST _ADC1Interrupt(void);
void _ISRFAST _ADC1Interrupt(void)
{
Tx_UARTS4('F');

}
char udata;


void main()
{
  int a;
RemapPins_spi(slave);
 
  unsigned int uart_no;
char umer;
 TRISE=0;							// Set PORTE for output
 TRISB=0;	

  RemapPins_usart4();

//  ConfigADC();
  config_USART4();  // 9600 baud , 8 data, none parity, i stop
  /*UART initialized to 2400 baudrate @BRGH=0, 8bit,no parity and 1 stopbit*/
//OpenUART2(UART_EN, UART_TX_ENABLE, 12); 
//Tx_UARTS4('1');

//Test_SPI();



Tx_UARTS4('2');

DelayMs();DelayMs();

int datard;
datard = My_Read_SPI();
Tx_UARTS4(datard);
Tx_UARTS4(datard>>8);
Tx_UARTS4('3');
int i=0;

while(1)
{
/*master mode sending data*/
/*datard = Master_Write(i);
DelayMs();
DelayMs();
DelayMs();

Tx_UARTS4(datard);
DelayMs();
Tx_UARTS4(datard>>8);
DelayMs();
Tx_UARTS4(datard<<8);


i++;
*/
}
/*
  while(1) {
    a=ReadADC(6);
	//CHNO++; if(CHNO>7) CHNO=6;
    //a=ReadADC(6);
   //udata=Rx_UARTS(4);
    
	Tx_UARTS4(a);
    Tx_UARTS4(a>>8);
    DelayMs(); DelayMs(); DelayMs();
  }
*/
//  return 0;

}

void config_USART4(void)
{							 
  U4BRG=12;	//BR=9600
  U4MODE= 0x8008;  	// 8 bit data, odd parity, one stop bit
  //U4MODEbits.UARTEN=1;	// ENABLE TX,RX
  U4STA=0x0400;


}

void DelayMs(void)
{
int f, j;
for(f=0;f<500;f++)
for(j=0;j<10;j++);

}

void Tx_UARTS4(unsigned char Txdata)
{
     while(U4STAbits.UTXBF);
     U4TXREG=Txdata;
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


void RemapPins_usart4(void)
{
  // Unlock Registers
 /* asm volatile ("MOV 	#OSCCON, w1  \n"
  				"MOV 	#0x46, 	w2 	 \n"
  				"MOV 	#0x57, 	w3 	 \n"
  				"MOV.b 	w2, 	[w1] \n"
  				"MOV.b 	w3, 	[w1] \n"
				"BCLR 	OSCCON,	#6"
				);
//**************************************************************

  

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
*/
  // iPPSInput(IN_FN_PPS_U4RX,IN_PIN_PPS_RP27);


// iPPSOutput(OUT_PIN_PPS_RP26,OUT_FN_PPS_U4TX);
//iPPSInput(IN_FN_PPS_U4RX,IN_PIN_PPS_RP10);
iPPSInput(IN_FN_PPS_U4RX,IN_PIN_PPS_RP27);
 // iPPSInput(IN_FN_PPS_U2CTS,IN_PIN_PPS_RPI32);
  iPPSOutput(OUT_PIN_PPS_RP19,OUT_FN_PPS_U4TX);
 // iPPSOutput(OUT_PIN_PPS_RP31,OUT_FN_PPS_U2RTS);



}
//#define USE_AND_OR /* To enable AND_OR mask setting */

void RemapPins_spi(int m)
{

if (m==1)
{
/*For SPI MAster*/
iPPSInput(IN_FN_PPS_SDI2,IN_PIN_PPS_RP12);
iPPSOutput(OUT_PIN_PPS_RP2,OUT_FN_PPS_SDO2);
iPPSOutput(OUT_PIN_PPS_RP3,OUT_FN_PPS_SS2OUT);
iPPSOutput(OUT_PIN_PPS_RP4,OUT_FN_PPS_SCK2OUT);
}

else if(m==2)

/*For SPI Slave*/


{
iPPSInput(IN_FN_PPS_SDI2,IN_PIN_PPS_RP12);
//CONFIG_RP6_AS_DIG_PIN();
iPPSInput(IN_FN_PPS_SCK2IN,IN_PIN_PPS_RP4);
//iPPSInput(IN_FN_PPS_SS2IN,IN_PIN_PPS_RP3);
iPPSOutput(OUT_PIN_PPS_RP2,OUT_FN_PPS_SDO2);

}

 /*asm volatile ("MOV 	#OSCCON, w1  \n"
  				"MOV 	#0x46, 	w2 	 \n"
  				"MOV 	#0x57, 	w3 	 \n"
  				"MOV.b 	w2, 	[w1] \n"
  				"MOV.b 	w3, 	[w1] \n"
				"BCLR 	OSCCON,	#6"
				);
//**************************************************************

  


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

*/

}


//void __attribute__((__interrupt__)) _SPI1Interrupt(void)
//{
//   SPI1_Clear_Intr_Status_Bit;
//}

void __attribute__((__interrupt__)) _SPI2Interrupt(void)
{
   SPI2_Clear_Intr_Status_Bit;
Tx_UARTS4('9');
   SPI1_Clear_Recv_OV; /* Clear SPI1 receive overflow flag if set */
}

int Test_SPI(void)
{
   unsigned int SPICON1Value; /* Holds the information about SPI configuartion */
   unsigned int SPICON2Value; /* Holds the information about SPI configuartion */
   unsigned int SPISTATValue; /* Holds the information about SPI Enable/Disable */
   unsigned int datard; /* Data received at SPI2 */

  // CloseSPI1(); /* Turn off SPI modules */
   CloseSPI2();

  // ConfigIntSPI2(SPI_INT_EN | SPI_INT_PRI_6); /* Configure SPI2 interrupt */

   /* Configure SPI1 module to transmit 16 bit timer1 value in master mode */
   SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE16_ON |SPI_SMP_ON | SPI_CKE_OFF
                  | SLAVE_ENABLE_OFF |CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON |SEC_PRESCAL_8_1
                  | PRI_PRESCAL_64_1;
   SPICON2Value = FRAME_ENABLE_OFF;
   SPISTATValue = SPI_ENABLE | SPI_IDLE_CON |SPI_RX_OVFLOW_CLR;
   OpenSPI2(SPICON1Value,SPICON2Value,SPISTATValue );

  /* Configure SPI2 module to receive 16 bit timer value in slave mode */
 /*  SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE16_ON |SPI_SMP_OFF | SPI_CKE_OFF
                  | SLAVE_ENABLE_OFF |CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_OFF | SEC_PRESCAL_7_1
                  | PRI_PRESCAL_64_1;
   SPICON2Value = FRAME_ENABLE_OFF;
   SPISTATValue = SPI_ENABLE | SPI_IDLE_CON |SPI_RX_OVFLOW_CLR;
   OpenSPI2(SPICON1Value,SPICON2Value,SPISTATValue );
*/
  // WriteSPI2(0x5678);
 
//SPI2BUF = 0x02;
//while(SPI2STATbits.SPITBF);
//SPI2BUF = 0x03;
//while(SPI2STATbits.SPITBF);
  // while(!DataRdySPI2());
  // datard = ReadSPI2();
  // CloseSPI1();
  // CloseSPI2();

}


int Master_Write (int data)
{
SPI2BUF = data;
   while(!SPI2STATbits.SPIRBF);
return SPI2BUF;

}

int My_Read_SPI(void)
{
   unsigned int SPICON1Value; /* Holds the information about SPI configuartion */
   unsigned int SPICON2Value; /* Holds the information about SPI configuartion */
   unsigned int SPISTATValue; /* Holds the information about SPI Enable/Disable */
   unsigned int datard; /* Data received at SPI2 */

  // CloseSPI1(); /* Turn off SPI modules */
   CloseSPI2();

   ConfigIntSPI2(SPI_INT_EN | SPI_INT_PRI_6); /* Configure SPI2 interrupt */

   /* Configure SPI1 module to transmit 16 bit timer1 value in master mode */
 /*  SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE16_ON |SPI_SMP_ON | SPI_CKE_OFF
                  | SLAVE_ENABLE_OFF |CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_ON |SEC_PRESCAL_7_1
                  | PRI_PRESCAL_64_1;
   SPICON2Value = FRAME_ENABLE_OFF;
   SPISTATValue = SPI_ENABLE | SPI_IDLE_CON |SPI_RX_OVFLOW_CLR;
   OpenSPI1(SPICON1Value,SPICON2Value,SPISTATValue );
*/
  /* Configure SPI2 module to receive 16 bit timer value in slave mode */
   SPICON1Value = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON |SPI_SMP_OFF | SPI_CKE_OFF
                  | SLAVE_ENABLE_OFF |CLK_POL_ACTIVE_HIGH | MASTER_ENABLE_OFF | SEC_PRESCAL_8_1
                  | PRI_PRESCAL_64_1;
   SPICON2Value = FRAME_ENABLE_OFF;
   SPISTATValue = SPI_ENABLE | SPI_IDLE_CON |SPI_RX_OVFLOW_CLR;
SPI2STATbits.SPIROV=0;
   OpenSPI2(SPICON1Value,SPICON2Value,SPISTATValue );
//SPI2CON1 = 0x0480;
//SPI2CON2=0x0000;
//SPI2STAT=0x8000;

   //WriteSPI(0x1234);
Tx_UARTS4('3');
Tx_UARTS4(SPI2STATbits.SPIRBF+0x30);
//SPI2STATbits.SPIRBF = 1;
//SPI2BUF=10;

Tx_UARTS4(SPI2STATbits.SPIRBF+0x30);
Tx_UARTS4(SPI2STATbits.SPITBF+0x30);
   while(!SPI2STATbits.SPIRBF)
{
//Tx_UARTS4(SPI2STATbits.SPIRBF+0x30);
//Tx_UARTS4(SPI2STATbits.SPITBF+0x30);
//Tx_UARTS4(SPI2BUF>>8);
//Tx_UARTS4(SPI2BUF);
//Tx_UARTS4(SPI2BUF<<8);
};

  // while(!DataRdySPI2());
//while(!SPI2STATbits.SPIRBF);
Tx_UARTS4('4');
   datard = ReadSPI2();
Tx_UARTS4('5');


   CloseSPI2();
return datard;
}