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

  AD1PCFGL=0XFF3F;
  AD1PCFGH=0XFFFF;
  AD1CON2bits.VCFG0=1;
  AD1CON2bits.VCFG1=1;
  AD1CON2bits.VCFG2=1;
  AD1CON3bits.ADCS0=1;
  AD1CON3bits.ADCS1=1;
  AD1CON3bits.ADCS2=1;
  AD1CON3bits.ADCS3=1;
  AD1CON3bits.ADCS4=1;
  AD1CON3bits.ADCS5=1;
  AD1CON3bits.ADCS6=1;
  AD1CON3bits.ADCS7=1;
  AD1CON1bits.SSRC0=1;
  AD1CON1bits.SSRC1=1;
  AD1CON1bits.SSRC2=1;
  

  AD1CON3bits.SAMC0=1;///
  AD1CON3bits.SAMC1=1;///  
  AD1CON3bits.SAMC2=1;///  31TAD
  AD1CON3bits.SAMC3=1;///
  AD1CON3bits.SAMC4=1;///

  AD1CON1bits.ADON=1;



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

