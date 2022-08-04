/*

D0 			input	from FPGA (all_stopped), interrupt upon completion of data acquisition
D1			output	bank select 0 to FPGA
D2			output	bank select 1 to FPGA
D3			output  bank select 2 to FPGA
PORTA[4:0]	output	channel select to FPGA
PORTF[7:4]	input	data from FPGA

D4			input	busy signal from printer
D5			output	strobe to printer
D6			input	print command from FPGA (printo)
PORTE[7:0]	output	data to printer

G12			output	ChipSelect for RAM
G15			output 	reset for RAM
*/

#include <string.h>

#include "p24hj256gp610.h"
//	configuration setup
	_FBS  	 (0xFFCF); 
	_FSS  	 (0xFFCF);
	_FGS  	 (0xFF07);
	_FOSCSEL (0xFF22);
	_FOSC	 (0xFF85);
	_FWDT	 (0xFF5F);
	_FPOR	 (0xFFE1);

#define CH_SEL	LATA	//	channel select - otuput to FPGA

#define CHANNELS 20		// 	total number of channels

#define STOPPED	_RD0
#define	PRINT	_RD6	// 	active-low

#define CR		0x0D
#define LF		0x0A

#define DATA	SPI2BUF
#define CS		_LATG12
#define RST		_LATG15

#define FOSC 		32000000
#define BAUDRATE 	9600
#define BRGVAL (((FOSC/2)/BAUDRATE)/4)-1 // for BRGH = 1


volatile unsigned short count, ready;
volatile unsigned char  ch;
unsigned char dat[80];

void printChar (char);
void delay (int);

void initSPI(void);
void waitTilDone(void);

void writeMemory(void);
void readMemory(void);
void printData(void);

inline void sendByte (unsigned char byte)
{
	ready = 0;
	DATA = byte;
	while (!ready);
}

inline unsigned char getByte (void)
{
	ready = 0;
	DATA = 0x00;
	while (!ready);
	return (ch);
}


int main(void)
{
	char str[40];
	int i, done;

	initSPI();

	TRISA = 0x00;
	PORTA = 0x00;
	TRISF = 0xF0;
	TRISD = 0x51;	// D0, D4 and D6 are inputs
	PORTD = 0x20;	// D5 is printer strobe - an active low signal - therefore it should be initialized to hi
	TRISE = 0x00;
	PORTE = 0x00;
	PORTF = 0x01;
	_TRISG12 = 0;	// RG12 is CS pin for memory module X2
	_TRISG15 = 0;	// RG15 is RST pin for all memory modules
	delay (1000);
	_LATF0 = 0;
	delay (10);
	_LATF0 = 1;

	RST = 1;
	CS = 1;
	ready = 0;
	done = 0;

//	settings for UART1
	U1MODEbits.STSEL = 0; // 1-stop bit
	U1MODEbits.PDSEL = 0; // No Parity, 8-data bits
	U1MODEbits.ABAUD = 0; // Autobaud Disabled
	U1MODEbits.BRGH = 1;  // High Speed mode
	U1BRG = BRGVAL ; 

//	U1STAbits.URXISEL = 0b00;  // interrupt at each character received
//	IFS0bits.U1RXIF = 0;    // reset UART1 interrupt flag
//	IPC2bits.U1RXIP = 0x01; // receive inrerupt priority set to 1 (lowest)
//	IEC0bits.U1RXIE = 1; // enable UART1 Receive interrupt	
	U1MODEbits.UARTEN= 1;  // enable UART1
	U1STAbits.UTXEN = 1;   // enable UART1 TX 

	// initialize printer
	printChar(0x1B);
	printChar(0x40);

	// select font A0
	printChar(0x1B);
	printChar(0x4D);
	printChar(0x00);

	strcpy (str, "----- Ready -----");
	printChar (LF);
	for (i=0; i<8; i++)
		printChar(' ');
	for (i=0; i<strlen(str); i++)
		printChar(str[i]);
	for (i=0; i<5; i++)
		printChar(LF);

	while (1) {
		while (!STOPPED && PRINT);	// wait until the FPGA raises the flag that all data has been received OR print command is received
		if (STOPPED && !done) {	// signal of completion from FPGA
			writeMemory();
			printData();
			done = 1;
//			while (STOPPED); // wait while RD0 stays high...
		}
		else if (!PRINT) {
			printData();
			while (!PRINT);
		}
	} // end while(1)	
} // end main()...


void printChar (char data)
{
	delay(100);
	while (!U1STAbits.TRMT);
	U1TXREG = (unsigned char) data;
}


void delay (int m)
{
	int i, j, k;
	for (i=0; i<m; i++)
		for (j=0; j<m; j++)
			k = i + j;
}


void initSPI(void)
{
	//	setting for SPI2
	// SPI2CON1 Register Settings
	SPI2CON1bits.DISSCK = 0; //Internal Serial Clock is Enabled
	SPI2CON1bits.DISSDO = 0; //SDOx pin is controlled by the module
	SPI2CON1bits.MODE16 = 0; //Communication is byte-wide (8 bits)
	SPI2CON1bits.SMP = 0; //Input Data is sampled at the middle of data output time
	SPI2CON1bits.CKE = 0; //Serial output data changes on transition from	
	//Idle clock state to active clock state
	SPI2CON1bits.CKP = 0; //Idle state for clock is a low level;
	//active state is a high level
	SPI2CON1bits.MSTEN = 1; //Master Mode Enabled
	SPI2CON1bits.PPRE = 0b11;	// pri prescalar of 1:1
	SPI2CON1bits.SPRE = 0b111;	// sec pretscalar of 5:1
	SPI2STATbits.SPIROV = 0;
	SPI2STATbits.SPIEN = 1; //Enable SPI Module
	//Interrupt Controller Settings
	IFS2bits.SPI2IF = 0; //Clear the Interrupt Flag
	IEC2bits.SPI2IE = 1; //Enable the Interrupt
}


void waitTilDone(void)
{
	CS = 0;
	sendByte (0xD7);
	while (! (getByte() & 0x80));
	CS = 1;
}


void writeMemory(void)
{
	int i, j;
	unsigned long data;

	CS = 0;
	sendByte (0x84);
	sendByte (0x00);
	sendByte (0x00);
	sendByte (0x00);

	for (i=0; i<CHANNELS; i++) {
		CH_SEL = i & 0x1F;
		delay (100);
		data = 0;
		for (j=0; j<8; j++) {	// for each channel the 32-bit data is read one nibble at a time
			_LATD1 = j ;		// bank select 0
			_LATD2 = j >> 1; 	// bank select 1
			_LATD3 = j >> 2;	// bank select 2
			delay (100);		// some delay required so that the FPGA puts the data on data lines
			data |= ((PORTF>>4) & 0x000F);		// read PORTF and store the byte in LSByte of data;
			delay(100);
			if (j<7)
				data <<= 4;		// shift data left by 4 bits to make place for next nibble except for last one
		}
		
		sendByte (data >> 24);
		sendByte (data >> 16);
		sendByte (data >> 8);
		sendByte (data);
	}

	CS = 1;

	delay (10);

	CS = 0;
	sendByte (0x83);
	sendByte (0x00);	
	sendByte (0x00);	
	sendByte (0x00);
	CS = 1;
	waitTilDone();
}


void readMemory()
{
	int i, j;

	CS = 0; 
	sendByte (0xD2);
	sendByte (0x00);
	sendByte (0x00);
	sendByte (0x00);
	sendByte (0x00);
	sendByte (0x00);
	sendByte (0x00);
	sendByte (0x00);

	for (i=0; i<CHANNELS; i++) {	// for all channels...
		for (j=0; j<4; j++) {
			dat[i*4+j] = getByte();		// read the counter value from memory
		}
	}
	
	CS = 1;
}



void printData(void)
{
	int i, j, k;
	unsigned long data;
	unsigned char bcd[11];

	char str[]  = "Channel";
	char str2[] = "Time (us)";

	readMemory();

	printChar (LF);
	
	for (i=0; i<8; i++)
		printChar(' ');
	for (i=0; i<strlen(str); i++)
		printChar(str[i]);
	for (i=0; i<10; i++)
		printChar(' ');
	for (i=0; i<strlen(str2); i++)
		printChar(str2[i]);
	
	printChar (LF);
	
	for (i=0; i<8; i++)
		printChar(' ');
	for (i=0; i<27; i++)
		printChar('-');
	
	printChar (LF); // linefeed
	
	for (i=0; i<CHANNELS; i++) {	// read all channels one by one
		for (k=0; k<2; bcd[k++]=0);
		k = 1;
		data = i+1;
		while (data > 0){
			bcd[k--] = data % 10;
			data /= 10;
		}
		for (j=0; j<10; j++)
			printChar (' ');	// print some spaces
	
		if (i < 9) {		// for channels 1 - 9 skip the leading 0
			printChar (bcd[1]+'0');
			printChar (' ');
		}
		else {
			printChar (bcd[0]+'0');
			printChar (bcd[1]+'0');
		}
		
		for (j=0; j<8; j++)
			printChar (' ');	// print some spaces
	
		delay (10);

/*
CH_SEL = i & 0x1F;
delay (10);
data = 0;
for (j=0; j<8; j++) {	// for each channel the 32-bit data is read one nibble at a time
	_LATD1 = j ;		// bank select 0
	_LATD2 = j >> 1; 	// bank select 1
	_LATD3 = j >> 2;	// bank select 2
	delay (10);		// some delay required so that the FPGA puts the data on data lines
	data |= ((PORTF>>4) & 0x000F);		// read PORTF and store the byte in LSByte of data;
	delay(10);
	if (j<7)
		data <<= 4;		// shift data left by 4 bits to make place for next nibble except for last one
}
*/

		data = dat[i*4];
		data <<= 8;
		data |= dat[i*4+1];
		data <<= 8;
		data |= dat[i*4+2];
		data <<= 8;
		data |= dat[i*4+3];
	
		for (k=0; k<11; bcd[k++]=0);	// initialize the bcd array which will be used to put BCD digits to be printed
		k = 10;						// we're assuming that there will be maximum 10 BCD digits

		if (data == 0) {
			for (k=0; k<11; k++)     	// print data for the current channel (only the integral part)
				printChar (' ');
		}
		else {
			while (data > 0) {
				bcd[k--] = data % 10;
				data /= 10;
			}
			
			k = 0;
			while (bcd[k++] == 0)	// skip leading 0's+
				printChar (' ');	// print spaces instead
			for (--k; k<9; k++)     	// print data for the current channel (only the integral part)
				printChar (bcd[k]+'0');
			
			printChar ('.');		// the decimal point
			for (k=9; k<11; k++)    // print data for the current channel (fractional part)
				printChar (bcd[k]+'0');
			
		}
		printChar (LF);
	}

	// print and feed 10 lines
	printChar (0x1B);
	printChar (0x64);
	printChar (0x0A);
	
	// cut paper
	printChar (0x1D);
	printChar (0x56);
	printChar (0x01);
}


//##########################################################################################

// ISR for SPI
void _ISR _SPI2Interrupt(void)
{	
	IEC2bits.SPI2IE = 0; //Disable the Interrupt
	
	ch = DATA;
	ready = 1;

	IFS2bits.SPI2IF = 0; //Clear the Interrupt Flag
	IEC2bits.SPI2IE = 1; //Enable the Interrupt
}	

