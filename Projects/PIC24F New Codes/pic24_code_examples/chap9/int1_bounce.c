/*
 * "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
 * All rights reserved.
 * (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
 * (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
 * (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */

#include "pic24_all.h"

/** \file
 *  Demonstrates the use of int1 interrupt
 *  to detect switch bounce.
 *
 *
 */

//Interrupt Service Routine for INT1
volatile uint8_t u8_bcnt;
void _ISRFAST _INT1Interrupt (void) {
  _INT1IF = 0;    //clear the interrupt bit
  u8_bcnt++;       //increment the bounce count
}

#define SW1             _RB13           //switch state
#define SW1_PRESSED()   (SW1==0)        //switch test
#define SW1_RELEASED()  (SW1==1)        //switch test
/// Switch1 configuration
inline void CONFIG_SW1()  {
  CONFIG_RB13_AS_DIG_INPUT();     //use RB13 for switch input
  ENABLE_RB13_PULLUP();           //enable the pullup
}


int main (void) {
  uint8_t u8_cnt;
  configBasic(HELLO_MSG);
  /** Configure the switch ***********/
  CONFIG_SW1();
  /** Configure INT1 interrupt  */
  CONFIG_INT1_TO_RP(13);   //RP13 shares RB13 pin
  DELAY_US(1);
  _INT1IF = 0;   //Clear the interrupt flag
  _INT1IP = 2;   //Choose a priority
  _INT1EP = 1;   //negative edge triggerred
  _INT1IE = 1;   //enable INT1 interrupt
  while (1) {
    u8_bcnt = 0;
    outString("Press & release switch... ");
    while (SW1_RELEASED());
    DELAY_MS(DEBOUNCE_DLY);
    while (SW1_PRESSED());
    DELAY_MS(DEBOUNCE_DLY);
    u8_cnt = u8_bcnt;  //copy variable so will not change
    if (u8_cnt != 1) outString("..bounced: ");
    else outString("..no bounce: ");
    outUint8(u8_cnt);
    outString("\n");
  }
}
