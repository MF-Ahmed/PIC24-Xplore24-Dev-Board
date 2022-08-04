/********************************************************************\ *                                                                  * *          Control & Automation Division                           * *                                                                  * *      DR. A. Q. KHAN RESEARCH LABORATORIES                        * *                                                                  * *      Centrifuge Plant Control Software Package                   * *                                                                  * *   This document is the property of Government of Pakistan and    * *   is issued for the information of such persons who  need  to    * *   know its  contents in the course of their official  duties.    * *                                                                  * *   The information in this document is subject to change without  * *   notice and should not be construed as a committment by C & A.  * *                                                                  * *   Version K01/C12_B1	PIC24FJ256GA106                            	* *                                                                  * *   Waseem Akhtar	17-April 2009                  		            * *                                                                  * *   ProcMSlipBox.c - 	Process Inverter Data						* *                  												* *             								                        *
\*******************************************************************/

#include <P24FJ256GA106.h>
#include "KCSDF.h"
#include "Common.h"

/********** *** ** * Process Inverter Data * ** ** **********/void PccSeInvSts(unsigned char InvData) {
  EINVDT = InvData ;
}
/********** ** * End Of Process Inverter Data * ** **********/

/*************** *** ** * End Of File * ** ** ***************/