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

// Documentation placed on the library collection history page. This file only
// include comments for Doxygen; it does not contain any
// code.
/**
\page history History and release notes for this library collection.

Next release:
- No changes at this time.


5-Mar-2012:
- Updated .gld and include/devices to support new additions to the PIC24/dsPIC family.

2-Mar-2012 (r38 / TBD):
- Removed unnecessary whitespace in pic24_uart.c-template
- Made a few strings const, to store them in ROM
- Removed scary warning for an unknown revision of a chip
- volatile qualifier added to specifyVar
- pic24_configbits.c \#warnings reworded to remove unnecessary (and now wrong)
  file names
- New revisions added for two chips
- Updated runscons.bat to produce no errors / warnings on a successful compile

27-Feb-2012 (r37 / 220dc4d778e9):
- Updated chap10/mcp24lc515_i2c_eeprom.c to use BLKSIZE everywhere
- Put the link in textbook_examples_dox.h to rtcc.c in the correct area
- Fixed bug in outUint8Decimal when x > 100
- Fixed to doc to keep Doxygen from complaining; updated ignores, moved to
  current version of Doxygen.
- Changed naming of Microchip include files to capital letters, to build on Linux.
- CR/LF -> CR cleanup plus a bit more via AStyle.
- SCons build updated to produce Unix-style line endings in the generated
  template files.
- Astyle now runs on the template files
- Link to Mercurial repo added on pic24 lib page

24-Aug-2011 (r28 / rd3252bde49e7):
- Fixed configPinsForLowPower to look at the LPOSCEN / SOSCEN bit to avoid
  disrupting an attached crystal oscillator.
- Moved repo hosting to Bitbucket
- Fixed compile errors in ESOS
- Update docs generation to include ESOS. Tidyed up doxygen a bit.
- lib/common/pic24_stdio_uart.c, chap10/stdio_test.* fixed, updated so
  it compiles with UART 3, 4
- scons build fixes / improvements

12-Apr-2011 (r21 / r880971ce93b6):
- Simplified the method for selecting which hardware platform
  to build for by introducing \ref HARDWARE_PLATFORM and
  appropriately refactoring the code.
- Added support for the The Microchip 16-bit 28-pin Starter Board,
  Part number DM300027. Thanks to Istv?n for contributing this!
  See \ref STARTER_BOARD_28P.
- Merged in support for the Dangerous Prototypes web platform.
  See \ref DANGEROUS_WEB.
- Updated the build process to build reset for all supported platforms.
- Updated doxygen to not include files from lib/includes/devices.
- Updated runastyle.bat; now uses Unix line endings.
- Cleaned up comments in pic24_libconfig.h.
- Ran astyle, moving the files to unix-style line ends.
- Added this history file.

Previous to this, history resided in SVN / HG. Sigh.
 */
