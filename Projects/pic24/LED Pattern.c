//Dated 9th May 2008
#include <p24HJ256GP610.h>

// This Program outputs various LED Patterns in POrt D

//main function

void main(void)
{	
	TRISD=0xf000;
	PORTD=0;
	unsigned int c;		
	unsigned int d;	
	while(1)
	{
		
		PORTD=0;
			for (c=0;c<10;c++)DelayMs();
		
			PORTD=0xFFFF;	
			
			for (c=0;c<10;c++)DelayMs();			
		
//		for(c=0; c<5; c++)
//		{
//		   PORTDbits.RD0=1;	
//			for(d=0;d<8;d++)
//						{
//							PORTD<<=1;
//							DelayMs();
//										}
//			PORTDbits.RD7=1;
//			for(d=0;d<8;d++)
//						{
//						PORTD>>=1;
//							DelayMs();
//										}
//
//			}
//		
//	//Sequence 2
//		PORTD= 0x00;
//		PORTD= 0b10101010;	
//		for(c=0; c<10; c++)
//					
//					{
//							PORTD=~PORTD;
//							DelayMs2();
//										}
//				
//		//Sequence 3
//		PORTD = 0b11110000;
//		for(c=0; c<10; c++)
//					{
//							PORTD=~PORTD;
//							DelayMs2();
//											}	
//
//
////Sequence 4
//		PORTD = 0b11001100;
//		for(c=0; c<10; c++)
//					{
//							PORTD=~PORTD;
//							DelayMs2();
//											}
//		for(c=0; c<10; c++)
//		{
//			PORTD = 0b10000001;
//			DelayMs2();
//			PORTD = 0b01000010;
//			DelayMs2();
//			PORTD = 0b00100100;
//			DelayMs2();
//			PORTD = 0b00011000;
//			DelayMs2();
//			PORTD = 0b00100100;
//			DelayMs2();
//			PORTD = 0b01000010;
//			DelayMs2();
//		}
//
	}
}

void DelayMs(void)
{
unsigned long f;
for(f=0;f<50000;f++);

}
void DelayMs2(void)
{
unsigned long g;
for(g=0;g<100000;g++);

}		