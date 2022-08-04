//Pormram for serial communication on PIC24 Using UART1

#include  <P24HJ256GP610.h>

void main (void)
{
  TRISD=0x00;
  U1BRG=22;						// BR=9600
  U1MODE=0x8000;				//
  U1STA=0xA400;  
  while(1){
    Tx_Rx_data1();
    //U2BRG=22;					// BR=9600	
    //U2MODE=0x8000;			//
    //U2STA=0xA400;
    //IEC0bits.U1RXIE=1;	
  }
}

void Tx_Rx_data1(void)
{
  unsigned char data[4];

  while(!U1STAbits.URXDA);
  data[0]=U1RXREG;
  PORTD = data[0]-'0';

  while(U1STAbits.UTXBF);		// || U1STAbits.OERR);//U1STAbits.URXDA); //|| );
  
  U1TXREG=data[0];
  delay();
}

void delay (void)
{
  unsigned int j;
  for(j=0;j<10000;j++);
}



/*
void Tx_data2(void)
{
  unsigned char w[]={'W','E','L','C','O','M','E'};
  unsigned int j;

  while(U2STAbits.UTXBF);

  {for(j=0; j<7; j++) 
		  {
   			 while(U2STAbits.UTXBF);
    		U2TXREG=w[j];
}
}
}
*/					