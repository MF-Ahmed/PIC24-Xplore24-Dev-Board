/******************************************************************** *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Apr-2009                             * *                                                                  * *   ADC.c - 														* *                  												* *             								                        *
 ********************************************************************/
#include <P24FJ256GA106.h>

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
}
/*
int main()
{
  int a;

  TRISE=0;							// Set PORTE for output
  while(1) {
    a=ReadADC(CHNO);
	//CHNO++; if(CHNO>7) CHNO=6;
    //a=ReadADC(6);
	PORTE=a;
  }
  return 0;
}
*/
