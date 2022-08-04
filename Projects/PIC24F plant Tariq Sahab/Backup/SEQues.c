/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Tariq Mahmood Akhtar	01-Oct-2008                             * *                                                                  * *   SEQues.c - 													* *                  												* *             								                        *
\*******************************************************************/
#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

#define		t	10

typedef  struct {
  int f,r,te;
  unsigned char d[t][2];
} cqs;

typedef  struct {
  int f,r,te;
  unsigned char d[t][2];
} hqs;

typedef  struct {
  int f,r,te;
  unsigned char d[t][33];
} pqs;

cqs  cq; 
hqs  hq; 
pqs  pq;


void initqs(void)
{
  cq.f=cq.r=cq.te=0;
  hq.f=hq.r=hq.te=0;
  pq.f=pq.r=pq.te=0;
}

int putcq(unsigned char x[])
{
  if(cq.te==t) return(1);
  cq.d[cq.r][0]=x[0]; cq.d[cq.r][1]=x[1];
  cq.r++; cq.te++;
  if(cq.r==t) cq.r=0;
  return(0);
}
int getcq(unsigned char x[])
{
  if(cq.te==0) return(1);
  x[0]=cq.d[cq.f][0]; x[1]=cq.d[cq.f][1];
  cq.f++; cq.te--;
  if(cq.f==t) cq.f=0;
  return(0);
}

int puthq(unsigned char x[])
{
  if(hq.te==t) return(1);
  hq.d[hq.r][0]=x[0]; hq.d[hq.r][1]=x[1];
  hq.r++; hq.te++;
  if(hq.r==t) hq.r=0;
  return(0);
}
int gethq(unsigned char x[])
{
  if(hq.te==0) return(1);
  x[0]=hq.d[hq.f][0]; x[1]=hq.d[hq.f][1];
  hq.f++; hq.te--;
  if(hq.f==t) hq.f=0;
  return(0);
}

int putpq(unsigned char x[])
{
  int I;

  if(pq.te==t) return(1);
  for(I=0; I<33; I++)
    pq.d[pq.r][I]=x[I];
  pq.r++; pq.te++;
  if(pq.r==t) pq.r=0;
  return(0);
}
int getpq(unsigned char x[])
{  
  int I;

  if(pq.te==0) return(1);
  for(I=0; I<33; I++)
    x[I]=pq.d[pq.f][I];
  pq.f++; pq.te--;
  if(pq.f==t) pq.f=0;
  return(0);
}

/********************************* END OF FILE *******************************************/
