#include "P24HJ256GP610.h"

int main(void)
{
  unsigned long i, j;
TRISD=0x00;				//Configure PORTD for output
  while(1){
    
	PORTD=0xFF;
    for(j=0; j<100000; j++);
	PORTD=0x00;
    for(j=0; j<100000; j++);    
 
   
  }
  
}
