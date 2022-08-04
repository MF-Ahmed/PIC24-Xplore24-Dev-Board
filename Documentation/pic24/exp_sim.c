#include "p24hj256gp610.h"

	_FBS 	(0xFFCF); 
	_FSS 	(0xFFCF);
	_FGS 	(0xFF07);
	_FOSCSEL (0xFF22);
	_FOSC	(0xFF85);
	_FWDT	(0xFF5F);
	_FPOR	(0xFFE1);

#define FOSC 		48000000
#define BAUDRATE 	9600
#define BRGVAL (((FOSC/2)/BAUDRATE)/4)-1 // for BRGH = 1

volatile unsigned long tVal;
volatile unsigned int  ready, count, tCount, state;
volatile unsigned char ch;
volatile unsigned char data[6] = {2, 4, 6, 8, 10, 12};


void initUART(void);
void initTimer1(void);



//##########################################################################################


int main(void)
{
	TRISB = 0x00;
	AD1PCFGL = 0xFF;	
	AD2PCFGL = 0xFF;	

	initUART();
	initTimer1();

	state = 0;

	while (1) {

		ready = 0;
		tVal = 0;
		count = 0;
		tCount = 0;

		while (!ready);
		LATB = 0x01;
		T1CONbits.TON = 1; // Start Timer1

//		while (!U1STAbits.TRMT);
//		U1TXREG = 't';

//		while (!U1STAbits.TRMT);
//		U1TXREG = tVal>>8;
//		while (!U1STAbits.TRMT);
//		U1TXREG = tVal;		

//		while (!U1STAbits.TRMT);
//		U1TXREG = data;		

//		ready = 0;
//		T1CONbits.TON = 0; // stop Timer1

		while (ready);
		while (!U1STAbits.TRMT);
		U1TXREG = 'e';
	}
}// End of main()...


//##########################################################################################


void initUART(void)
{
	//	settings for UART1
	U1MODEbits.STSEL = 0; // 1-stop bit
	U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
	U1MODEbits.ABAUD = 0; // Autobaud Disabled
	U1MODEbits.BRGH = 1;  // Low Speed mode
	U1BRG = BRGVAL ; 

	U1STAbits.URXISEL = 0b00;  // interrupt at each character received
	IFS0bits.U1RXIF = 0;    // reset UART1 interrupt flag
	IPC2bits.U1RXIP = 0x01; // receive inrerupt priority set to 1 (lowest)
	IEC0bits.U1RXIE = 1;    // enable UART1 Receive interrupt	
	U1MODEbits.UARTEN= 1;   // enable UART1
	U1STAbits.UTXEN = 1;    // enable UART1 TX 
}


void initTimer1(void)
{
	T1CONbits.TCS = 0; // Select internal instruction cycle clock
	T1CONbits.TGATE = 0; // Disable Gated Timer mode
	T1CONbits.TCKPS = 0b01; // Select 1:8 Prescaler
	TMR1 = 0x00; // Clear Timer1 register
	PR1 = 29;  
	IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
	IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
	IEC0bits.T1IE = 1; // Enable Timer1 interrupt
}



//##########################################################################################

// ISR for UART1 
void _ISR _U1RXInterrupt(void)
{	
	IEC0bits.U1RXIE = 0; 	// disable UART1 Receive interrupt
	ch = U1RXREG;

/*
	if (state == 0) {
		if (ch == 'd') {
			state = 1;
			tCount = 0;
			while (!U1STAbits.TRMT);
			U1TXREG = ch;		
		}
		else if (ch == 's') {
			while (!U1STAbits.TRMT);
			U1TXREG = ch;		
			ready = 1;
		}
	}

	else if (state == 1) {
		data[tCount++] = ch;
		if (tCount == 6) {
			state = 0;
			tCount = 0;
			while (!U1STAbits.TRMT);
			U1TXREG = 'f';		
			ready = 1;
		}
	}
*/

	if (ch == 's') {
		while (!U1STAbits.TRMT);
		U1TXREG = ch;		
		ready = 1;
	}

	IFS0bits.U1RXIF = 0;  	// reset UART1 interrupt flag
	IEC0bits.U1RXIE = 1; 	// enable UART1 Receive interrupt
}	


// ISR for Timer1
void _ISR _T1Interrupt(void)
{	
	IEC0bits.T1IE = 0; 	// disable Timer1 interrupt

/*
	if (data[count] == ++tVal) {
		LATB = PORTB << 1;
		if (++count == 6) {
			T1CONbits.TON = 0; // stop Timer1
			ready = 0;
		}
	}
*/

	if (PORTB == 0x80) {
		T1CONbits.TON = 0; // stop Timer1
		ready = 0;
	}
	else
		LATB = PORTB << 1;	

	IFS0bits.T1IF = 0;  // reset Timer1 interrupt flag
	IEC0bits.T1IE = 1; 	// enable Timer1 interrupt
}	

