
/**
 * These fonts are from the xtrs emulator, and the CG# references match those.
 * They're identical to the fonts in the sdltrs emulator. They don't include
 * the 2x3 graphical characters; we generate those procedurally. see
 * make_graphical_chars.py.
 *
 * See the original trs_chars.c file for Tim Mann's explanations and historical
 * notes.
 */

// Here is the LICENSE file from the xtrs emulator:
/*

Copyright (C) 1992 Clarendon Hill Software.

Permission is granted to any individual or institution to use, copy,
or redistribute this software, provided this copyright notice is retained. 

This software is provided "as is" without any expressed or implied
warranty.  If this software brings on any sort of damage -- physical,
monetary, emotional, or brain -- too bad.  You've got no one to blame
but yourself. 

The software may be modified for your own purposes, but modified versions
must retain this notice.

***

Copyright (c) 1996-2020, Timothy P. Mann

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "fonts.h"

#if 0
/**
 * Original Model I character set.
 */
const unsigned char GLYPH_CG1[] = {
    0x00,0x1f,0x11,0x11,0x11,0x11,0x11,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x02,0x04,0x08,0x1e,0x04,0x08,0x10,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x11,0x1b,0x15,0x1b,0x11,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x08,0x05,0x03,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x1f,0x0a,0x0a,0x1b,0x00,0x00,0x00,0x00,
    0x00,0x04,0x02,0x0f,0x12,0x14,0x10,0x10,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x08,0x1f,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x00,0x00,0x1f,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x04,0x15,0x0e,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x15,0x0e,0x04,0x15,0x0e,0x04,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x02,0x1f,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x1b,0x15,0x1b,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x15,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x11,0x11,0x1f,0x11,0x11,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x15,0x15,0x1d,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x1d,0x15,0x15,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x17,0x15,0x15,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x15,0x15,0x17,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x14,0x08,0x15,0x03,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x0a,0x0a,0x0a,0x0a,0x0a,0x1b,0x00,0x00,0x00,0x00,
    0x00,0x10,0x10,0x10,0x1f,0x10,0x10,0x10,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x11,0x0a,0x04,0x0a,0x11,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x0e,0x0e,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x01,0x02,0x04,0x00,0x04,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x1f,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x15,0x15,0x17,0x11,0x11,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x11,0x11,0x17,0x15,0x15,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x11,0x11,0x1d,0x15,0x15,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x15,0x15,0x1d,0x11,0x11,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00,0x00,0x00,
    0x00,0x0a,0x0a,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0a,0x0a,0x1f,0x0a,0x1f,0x0a,0x0a,0x00,0x00,0x00,0x00,
    0x00,0x04,0x1e,0x05,0x0e,0x14,0x0f,0x04,0x00,0x00,0x00,0x00,
    0x00,0x03,0x13,0x08,0x04,0x02,0x19,0x18,0x00,0x00,0x00,0x00,
    0x00,0x02,0x05,0x05,0x02,0x15,0x09,0x16,0x00,0x00,0x00,0x00,
    0x00,0x06,0x06,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x08,0x04,0x02,0x02,0x02,0x04,0x08,0x00,0x00,0x00,0x00,
    0x00,0x02,0x04,0x08,0x08,0x08,0x04,0x02,0x00,0x00,0x00,0x00,
    0x00,0x04,0x15,0x0e,0x1f,0x0e,0x15,0x04,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x04,0x1f,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x06,0x06,0x02,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x08,0x04,0x02,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x19,0x15,0x13,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x04,0x06,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x10,0x0e,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x10,0x0c,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x08,0x0c,0x0a,0x09,0x1f,0x08,0x08,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x01,0x0f,0x10,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0c,0x02,0x01,0x0f,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x10,0x08,0x04,0x02,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x0e,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x1e,0x10,0x08,0x06,0x00,0x00,0x00,0x00,
    0x00,0x00,0x06,0x06,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,
    0x00,0x06,0x06,0x00,0x06,0x06,0x02,0x01,0x00,0x00,0x00,0x00,
    0x00,0x08,0x04,0x02,0x01,0x02,0x04,0x08,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1f,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x02,0x04,0x08,0x10,0x08,0x04,0x02,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x10,0x08,0x04,0x00,0x04,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x10,0x16,0x15,0x15,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x04,0x0a,0x11,0x11,0x1f,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x0f,0x12,0x12,0x0e,0x12,0x12,0x0f,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x01,0x01,0x01,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0f,0x12,0x12,0x12,0x12,0x12,0x0f,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x01,0x01,0x07,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x01,0x01,0x07,0x01,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x1e,0x01,0x01,0x19,0x11,0x11,0x1e,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x11,0x1f,0x11,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x10,0x10,0x10,0x10,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x11,0x09,0x05,0x03,0x05,0x09,0x11,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x11,0x1b,0x15,0x15,0x11,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x11,0x13,0x15,0x19,0x11,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0f,0x11,0x11,0x0f,0x01,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x11,0x11,0x15,0x09,0x16,0x00,0x00,0x00,0x00,
    0x00,0x0f,0x11,0x11,0x0f,0x05,0x09,0x11,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x11,0x01,0x0e,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x11,0x0a,0x0a,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x11,0x11,0x15,0x1b,0x11,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x0a,0x04,0x0a,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x0a,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x1f,0x10,0x08,0x04,0x02,0x01,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x04,0x0e,0x15,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x04,0x04,0x15,0x0e,0x04,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x02,0x1f,0x02,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x08,0x1f,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x0c,0x0c,0x04,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0e,0x10,0x1e,0x11,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x0d,0x13,0x11,0x13,0x0d,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0e,0x11,0x01,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x10,0x10,0x16,0x19,0x11,0x19,0x16,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0e,0x11,0x1f,0x01,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x08,0x14,0x04,0x0e,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x16,0x19,0x19,0x16,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x0d,0x13,0x11,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x04,0x00,0x06,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x10,0x00,0x10,0x10,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x09,0x05,0x03,0x05,0x09,0x00,0x00,0x00,0x00,
    0x00,0x06,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0b,0x15,0x15,0x15,0x15,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0d,0x13,0x11,0x11,0x11,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0e,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x0d,0x13,0x11,0x13,0x0d,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x16,0x19,0x11,0x19,0x16,0x10,0x10,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x0d,0x13,0x01,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1e,0x01,0x0e,0x10,0x0f,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x1f,0x04,0x04,0x14,0x08,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x11,0x11,0x19,0x16,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x11,0x11,0x0a,0x04,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x11,0x15,0x15,0x0a,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x11,0x0a,0x04,0x0a,0x11,0x00,0x00,0x00,0x00,
    0x00,0x11,0x11,0x11,0x1e,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1f,0x08,0x04,0x02,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x08,0x04,0x04,0x02,0x04,0x04,0x08,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x04,0x00,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
    0x00,0x02,0x04,0x04,0x08,0x04,0x04,0x02,0x00,0x00,0x00,0x00,
    0x00,0x02,0x15,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x0a,0x15,0x0a,0x15,0x0a,0x15,0x0a,0x00,0x00,0x00,0x00,
};

/**
 * Model I character set with official Radio Shack upgrade.
 */
const unsigned char GLYPH_CG2[] = {
    0x0e,0x11,0x10,0x16,0x15,0x15,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x04,0x0a,0x11,0x11,0x1f,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x12,0x12,0x0e,0x12,0x12,0x0f,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x01,0x01,0x01,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x12,0x12,0x12,0x12,0x12,0x0f,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x01,0x01,0x07,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x01,0x01,0x07,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x1e,0x01,0x01,0x19,0x11,0x11,0x1e,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x1f,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x10,0x10,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x11,0x09,0x05,0x03,0x05,0x09,0x11,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x11,0x1b,0x15,0x15,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x11,0x13,0x15,0x19,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x11,0x11,0x0f,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x11,0x11,0x15,0x09,0x16,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x11,0x11,0x0f,0x05,0x09,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x01,0x0e,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x0a,0x0a,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x11,0x15,0x1b,0x11,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x0a,0x04,0x0a,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x0a,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x10,0x08,0x04,0x02,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x04,0x0e,0x15,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x04,0x04,0x15,0x0e,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x02,0x1f,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x08,0x1f,0x08,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
    0x0a,0x0a,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x0a,0x0a,0x1f,0x0a,0x1f,0x0a,0x0a,0x00,0x00,0x00,0x00,0x00,
    0x04,0x1e,0x05,0x0e,0x14,0x0f,0x04,0x00,0x00,0x00,0x00,0x00,
    0x03,0x13,0x08,0x04,0x02,0x19,0x18,0x00,0x00,0x00,0x00,0x00,
    0x02,0x05,0x05,0x02,0x15,0x09,0x16,0x00,0x00,0x00,0x00,0x00,
    0x06,0x06,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x04,0x02,0x02,0x02,0x04,0x08,0x00,0x00,0x00,0x00,0x00,
    0x02,0x04,0x08,0x08,0x08,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x04,0x15,0x0e,0x1f,0x0e,0x15,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x1f,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x06,0x06,0x02,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,
    0x00,0x10,0x08,0x04,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x19,0x15,0x13,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x04,0x06,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x10,0x0e,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x10,0x0c,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x08,0x0c,0x0a,0x09,0x1f,0x08,0x08,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x01,0x0f,0x10,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0c,0x02,0x01,0x0f,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x10,0x08,0x04,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x11,0x0e,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x11,0x1e,0x10,0x08,0x06,0x00,0x00,0x00,0x00,0x00,
    0x00,0x06,0x06,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x06,0x06,0x00,0x06,0x06,0x02,0x01,0x00,0x00,0x00,0x00,
    0x08,0x04,0x02,0x01,0x02,0x04,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1f,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x02,0x04,0x08,0x10,0x08,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x10,0x08,0x04,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x10,0x16,0x15,0x15,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x04,0x0a,0x11,0x11,0x1f,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x12,0x12,0x0e,0x12,0x12,0x0f,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x01,0x01,0x01,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x12,0x12,0x12,0x12,0x12,0x0f,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x01,0x01,0x07,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x01,0x01,0x07,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x1e,0x01,0x01,0x19,0x11,0x11,0x1e,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x1f,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x10,0x10,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x11,0x09,0x05,0x03,0x05,0x09,0x11,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0x01,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x11,0x1b,0x15,0x15,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x11,0x13,0x15,0x19,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x11,0x11,0x0f,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x11,0x11,0x15,0x09,0x16,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x11,0x11,0x0f,0x05,0x09,0x11,0x00,0x00,0x00,0x00,0x00,
    0x0e,0x11,0x01,0x0e,0x10,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x0a,0x0a,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x11,0x11,0x15,0x1b,0x11,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x0a,0x04,0x0a,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x11,0x11,0x0a,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x1f,0x10,0x08,0x04,0x02,0x01,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x04,0x0e,0x15,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x04,0x04,0x15,0x0e,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x02,0x1f,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x08,0x1f,0x08,0x04,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,
    0x04,0x0a,0x02,0x07,0x02,0x12,0x0f,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0e,0x10,0x1e,0x11,0x1e,0x00,0x00,0x00,0x00,0x00,
    0x01,0x01,0x0d,0x13,0x11,0x13,0x0d,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0e,0x11,0x01,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x16,0x19,0x11,0x19,0x16,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0e,0x11,0x1f,0x01,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x08,0x14,0x04,0x0e,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0e,0x11,0x11,0x1e,0x10,0x0e,0x00,0x00,0x00,0x00,
    0x01,0x01,0x0d,0x13,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x04,0x00,0x06,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x10,0x00,0x18,0x10,0x10,0x10,0x12,0x0c,0x00,0x00,0x00,0x00,
    0x02,0x02,0x12,0x0a,0x06,0x0a,0x12,0x00,0x00,0x00,0x00,0x00,
    0x06,0x04,0x04,0x04,0x04,0x04,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0b,0x15,0x15,0x15,0x15,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0d,0x13,0x11,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0e,0x11,0x11,0x11,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0d,0x13,0x13,0x0d,0x01,0x01,0x00,0x00,0x00,0x00,
    0x00,0x00,0x16,0x19,0x19,0x16,0x10,0x10,0x00,0x00,0x00,0x00,
    0x00,0x00,0x0d,0x13,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1e,0x01,0x0e,0x10,0x0f,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x0e,0x04,0x04,0x14,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x11,0x11,0x11,0x19,0x16,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x11,0x11,0x11,0x0a,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x11,0x11,0x15,0x15,0x0a,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x11,0x0a,0x04,0x0a,0x11,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x11,0x11,0x11,0x1e,0x10,0x0e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1f,0x08,0x04,0x02,0x1f,0x00,0x00,0x00,0x00,0x00,
    0x08,0x04,0x04,0x02,0x04,0x04,0x08,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x04,0x00,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x02,0x04,0x04,0x08,0x04,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x11,0x0a,0x04,0x1f,0x04,0x1f,0x04,0x00,0x00,0x00,0x00,0x00,
    0x15,0x0a,0x15,0x0a,0x15,0x0a,0x15,0x0a,0x00,0x00,0x00,0x00,
};
#endif

/**
 * Original Model III character set.
 */
const unsigned char Trs80FontBits[] = {
    // 0-127
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x30,0x48,0x08,0x3e,0x08,0x48,0x3e,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x20,0x10,0x3c,0x42,0x7e,0x02,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x24,0x00,0x42,0x42,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x10,0x28,0x10,0x28,0x44,0x7c,0x44,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x7e,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
    0x28,0x00,0x38,0x44,0x44,0x44,0x38,0x00,0x00,0x00,0x00,0x00,
    0xb8,0x44,0x64,0x54,0x4c,0x44,0x3a,0x00,0x00,0x00,0x00,0x00,
    0x08,0x10,0x42,0x42,0x42,0x62,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x4c,0x32,0x00,0x34,0x4c,0x44,0x44,0x00,0x00,0x00,0x00,0x00,
    0x10,0x20,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x1c,0x00,0x1c,0x20,0x3c,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x5e,0x22,0x22,0x1e,0x12,0x22,0x00,0x00,0x00,0x00,0x00,
    0x28,0x00,0x10,0x28,0x44,0x7c,0x44,0x00,0x00,0x00,0x00,0x00,
    0x4c,0x32,0x10,0x28,0x44,0x7c,0x44,0x00,0x00,0x00,0x00,0x00,
    0x4c,0x32,0x44,0x4c,0x54,0x64,0x44,0x00,0x00,0x00,0x00,0x00,
    0x00,0x28,0x38,0x44,0x44,0x44,0x38,0x00,0x00,0x00,0x00,0x00,
    0x90,0x68,0x64,0x54,0x4c,0x2c,0x12,0x00,0x00,0x00,0x00,0x00,
    0x4c,0x32,0x00,0x3c,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x44,0x44,0x3c,0x44,0x44,0x3e,0x00,0x00,0x00,0x00,0x00,
    0x24,0x00,0x42,0x42,0x42,0x62,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x4c,0x32,0x00,0x18,0x24,0x24,0x18,0x00,0x00,0x00,0x00,0x00,
    0x38,0x54,0x50,0x38,0x14,0x54,0x38,0x00,0x00,0x00,0x00,0x00,
    0x14,0x00,0x1c,0x20,0x3c,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x04,0x08,0x1c,0x20,0x3c,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x08,0x00,0x1c,0x20,0x3c,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x02,0x3e,0x42,0x7c,0x40,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x20,0x10,0x7c,0x04,0x7c,0x04,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x10,0x78,0x24,0x64,0x3c,0x24,0x64,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x04,0x04,0x44,0x38,0x10,0x08,0x00,0x00,0x00,0x00,
    0x00,0x00,0x4c,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x10,0x10,0x10,0x00,0x10,0x00,0x00,0x00,0x00,0x00,
    0x24,0x24,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x24,0x24,0x7e,0x24,0x7e,0x24,0x24,0x00,0x00,0x00,0x00,0x00,
    0x10,0x78,0x14,0x38,0x50,0x3c,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x46,0x26,0x10,0x08,0x64,0x62,0x00,0x00,0x00,0x00,0x00,
    0x0c,0x12,0x12,0x0c,0x52,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x20,0x10,0x08,0x08,0x08,0x10,0x20,0x00,0x00,0x00,0x00,0x00,
    0x04,0x08,0x10,0x10,0x10,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x54,0x38,0x7c,0x38,0x54,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x10,0x10,0x7c,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x08,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x62,0x5a,0x46,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x10,0x18,0x14,0x10,0x10,0x10,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x40,0x30,0x0c,0x02,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x40,0x38,0x40,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x20,0x30,0x28,0x24,0x7e,0x20,0x20,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x02,0x1e,0x20,0x40,0x22,0x1c,0x00,0x00,0x00,0x00,0x00,
    0x38,0x04,0x02,0x3e,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x42,0x20,0x10,0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x42,0x3c,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x42,0x7c,0x40,0x20,0x1c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x00,0x00,0x10,0x10,0x08,0x00,0x00,0x00,0x00,
    0x60,0x30,0x18,0x0c,0x18,0x30,0x60,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7e,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x06,0x0c,0x18,0x30,0x18,0x0c,0x06,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x40,0x30,0x08,0x00,0x08,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x52,0x6a,0x32,0x04,0x78,0x00,0x00,0x00,0x00,0x00,
    0x18,0x24,0x42,0x7e,0x42,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x3e,0x44,0x44,0x3c,0x44,0x44,0x3e,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x02,0x02,0x02,0x44,0x38,0x00,0x00,0x00,0x00,0x00,
    0x1e,0x24,0x44,0x44,0x44,0x24,0x1e,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x02,0x02,0x1e,0x02,0x02,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x02,0x02,0x1e,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x02,0x72,0x42,0x44,0x38,0x00,0x00,0x00,0x00,0x00,
    0x42,0x42,0x42,0x7e,0x42,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x38,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00,0x00,0x00,0x00,
    0x70,0x20,0x20,0x20,0x20,0x22,0x1c,0x00,0x00,0x00,0x00,0x00,
    0x42,0x22,0x12,0x0e,0x12,0x22,0x42,0x00,0x00,0x00,0x00,0x00,
    0x02,0x02,0x02,0x02,0x02,0x02,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x42,0x66,0x5a,0x5a,0x42,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x42,0x46,0x4a,0x52,0x62,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x3e,0x42,0x42,0x3e,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x42,0x42,0x52,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x3e,0x42,0x42,0x3e,0x12,0x22,0x42,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0x02,0x3c,0x40,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x42,0x42,0x42,0x42,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x42,0x42,0x42,0x24,0x24,0x18,0x18,0x00,0x00,0x00,0x00,0x00,
    0x42,0x42,0x42,0x5a,0x5a,0x66,0x42,0x00,0x00,0x00,0x00,0x00,
    0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x44,0x44,0x44,0x38,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x20,0x18,0x04,0x02,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x04,0x04,0x04,0x04,0x04,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x20,0x20,0x20,0x20,0x20,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x10,0x28,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,
    0x08,0x10,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x1c,0x20,0x3c,0x22,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x02,0x02,0x3a,0x46,0x42,0x46,0x3a,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3c,0x42,0x02,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x40,0x40,0x5c,0x62,0x42,0x62,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3c,0x42,0x7e,0x02,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x30,0x48,0x08,0x3e,0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x5c,0x62,0x62,0x5c,0x40,0x3c,0x00,0x00,0x00,0x00,
    0x02,0x02,0x3a,0x46,0x42,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x10,0x00,0x18,0x10,0x10,0x10,0x38,0x00,0x00,0x00,0x00,0x00,
    0x20,0x00,0x30,0x20,0x20,0x20,0x22,0x1c,0x00,0x00,0x00,0x00,
    0x02,0x02,0x22,0x12,0x0a,0x16,0x22,0x00,0x00,0x00,0x00,0x00,
    0x18,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x6e,0x92,0x92,0x92,0x92,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3a,0x46,0x42,0x42,0x42,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3c,0x42,0x42,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3a,0x46,0x46,0x3a,0x02,0x02,0x00,0x00,0x00,0x00,
    0x00,0x00,0x5c,0x62,0x62,0x5c,0x40,0x40,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3a,0x46,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7c,0x02,0x3c,0x40,0x3e,0x00,0x00,0x00,0x00,0x00,
    0x08,0x08,0x3e,0x08,0x08,0x48,0x30,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x42,0x42,0x42,0x62,0x5c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x42,0x42,0x42,0x24,0x18,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x82,0x92,0x92,0x92,0x6c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x42,0x24,0x18,0x24,0x42,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x42,0x42,0x62,0x5c,0x40,0x3c,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7e,0x20,0x18,0x04,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x30,0x08,0x08,0x04,0x08,0x08,0x30,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x10,0x00,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x0c,0x10,0x10,0x20,0x10,0x10,0x0c,0x00,0x00,0x00,0x00,0x00,
    0x0c,0x92,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x7c,0x10,0x10,0x00,0x7c,0x00,0x00,0x00,0x00,0x00,

    // 128-191, graphical characters, see make_graphical_chars.py.
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
    0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
    0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
    0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,
    0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,
    0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,
    0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,
    0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,
    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,
    0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
    0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
    0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
    0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,
    0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,
    0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,
    0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,
    0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,
    0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,
    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,
    0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,
    0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,
    0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,
    0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,
    0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
    0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
    0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
    0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,
    0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,
    0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
    0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
    0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,
    0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,
    0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,
    0x0f,0x0f,0x0f,0x0f,0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,
    0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xf0,0xf0,0xf0,0xf0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

    // 192-255
    0x10,0x38,0x7c,0xfe,0xfe,0x7c,0x10,0x10,0x00,0x00,0x00,0x00,
    0x00,0x6c,0xfe,0xfe,0x7c,0x38,0x10,0x00,0x00,0x00,0x00,0x00,
    0x10,0x38,0x7c,0xfe,0x7c,0x38,0x10,0x00,0x00,0x00,0x00,0x00,
    0x38,0x38,0x10,0xd6,0xfe,0xd6,0x10,0x38,0x00,0x00,0x00,0x00,
    0x3c,0x42,0xa5,0x81,0xa5,0x99,0x42,0x3c,0x00,0x00,0x00,0x00,
    0x3c,0x42,0xa5,0x81,0x99,0xa5,0x42,0x3c,0x00,0x00,0x00,0x00,
    0x20,0x10,0x08,0x04,0x08,0x10,0x20,0x3c,0x00,0x00,0x00,0x00,
    0x04,0x08,0x10,0x20,0x10,0x08,0x04,0x3c,0x00,0x00,0x00,0x00,
    0x00,0x00,0x9c,0x62,0x62,0x9c,0x00,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x44,0x3c,0x44,0x44,0x3c,0x04,0x02,0x00,0x00,0x00,0x00,
    0x86,0x48,0x28,0x18,0x08,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,
    0x30,0x48,0x08,0x30,0x50,0x48,0x30,0x00,0x00,0x00,0x00,0x00,
    0x60,0x10,0x08,0x7c,0x08,0x10,0x60,0x00,0x00,0x00,0x00,0x00,
    0x68,0x60,0x10,0x08,0x38,0x40,0x30,0x00,0x00,0x00,0x00,0x00,
    0x34,0x4a,0x48,0x48,0x40,0x40,0x40,0x00,0x00,0x00,0x00,0x00,
    0x10,0x28,0x44,0x7c,0x44,0x28,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x04,0x04,0x04,0x44,0x44,0x38,0x00,0x00,0x00,0x00,0x00,
    0x02,0x12,0x0a,0x06,0x0a,0x52,0x22,0x00,0x00,0x00,0x00,0x00,
    0x04,0x08,0x08,0x08,0x18,0x24,0x42,0x00,0x00,0x00,0x00,0x00,
    0x24,0x24,0x24,0x24,0x5c,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x4c,0x48,0x28,0x18,0x08,0x00,0x00,0x00,0x00,0x00,
    0x10,0x38,0x04,0x18,0x04,0x38,0x40,0x30,0x00,0x00,0x00,0x00,
    0x00,0x18,0x24,0x42,0x42,0x24,0x18,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7c,0x2a,0x28,0x28,0x28,0x00,0x00,0x00,0x00,0x00,
    0x00,0x18,0x24,0x24,0x1c,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7c,0x12,0x12,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7c,0x12,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x00,
    0x00,0x40,0x26,0x24,0x24,0x24,0x18,0x00,0x00,0x00,0x00,0x00,
    0x10,0x38,0x54,0x54,0x54,0x38,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x46,0x28,0x10,0x28,0xc4,0x00,0x00,0x00,0x00,0x00,
    0x92,0x54,0x54,0x38,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x44,0x82,0x92,0x92,0x6c,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x82,0x82,0xc6,0x44,0xc6,0x00,0x00,0x00,0x00,0x00,
    0x78,0x08,0x08,0x08,0x0a,0x0c,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x10,0x00,0x7c,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x04,0x08,0x30,0x08,0x04,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x00,0x4c,0x32,0x00,0x4c,0x32,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x28,0x44,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,
    0x20,0x10,0x08,0x08,0x10,0x10,0x08,0x04,0x00,0x00,0x00,0x00,
    0x80,0x40,0xfe,0x10,0xfe,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x08,0x10,0x20,0x7c,0x08,0x10,0x20,0x00,0x00,0x00,0x00,0x00,
    0xfc,0x4a,0x24,0x10,0x48,0xa4,0x42,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x82,0x82,0xfe,0x44,0x44,0xc6,0x00,0x00,0x00,0x00,
    0x00,0x00,0x6c,0x92,0x92,0x6c,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x40,0x20,0x12,0x0a,0x06,0x02,0x00,0x00,0x00,0x00,0x00,
    0x78,0x04,0x38,0x44,0x38,0x40,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x44,0xaa,0x54,0x28,0x54,0xaa,0x44,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x42,0xb9,0x85,0x85,0xb9,0x42,0x3c,0x00,0x00,0x00,0x00,
    0x42,0x24,0x18,0x24,0x18,0x24,0x42,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x52,0x52,0x5c,0x50,0x50,0x50,0x50,0x00,0x00,0x00,0x00,
    0x10,0x38,0x54,0x14,0x54,0x38,0x10,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x5e,0xa5,0xa5,0x9d,0x95,0x66,0x3c,0x00,0x00,0x00,0x00,
    0xfa,0x06,0xc6,0x46,0x26,0xde,0x06,0xfa,0x00,0x00,0x00,0x00,
    0xff,0x20,0xc0,0x3f,0x40,0x3f,0x20,0x1f,0x00,0x00,0x00,0x00,
    0x3f,0x40,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x1e,0x22,0x22,0x1e,0x52,0x22,0xd2,0x00,0x00,0x00,0x00,0x00,
    0x86,0x41,0x21,0x16,0x68,0x94,0x92,0x61,0x00,0x00,0x00,0x00,
    0x70,0x60,0x50,0x0e,0x09,0x09,0x06,0x00,0x00,0x00,0x00,0x00,
    0x38,0x44,0x44,0x44,0x38,0x10,0x38,0x10,0x00,0x00,0x00,0x00,
    0x70,0x10,0x10,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xff,0xc7,0xbb,0xcf,0xef,0xff,0xef,0xff,0x00,0x00,0x00,0x00,
    0x10,0x28,0x10,0x38,0x54,0x10,0x28,0x44,0x00,0x00,0x00,0x00,
    0x10,0x28,0x10,0x38,0x54,0x28,0x7c,0x28,0x00,0x00,0x00,0x00,
    0x10,0x28,0x44,0x44,0x44,0x54,0x6c,0x44,0x00,0x00,0x00,0x00,

    // Alt character set (192-255).
#if 0
    0x44,0x28,0x10,0x7c,0x10,0x7c,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x04,0x0a,0x04,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x20,0x20,0x20,0x20,0x3e,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3c,0x20,0x3c,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7c,0x40,0x30,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x20,0x10,0x18,0x14,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x7c,0x44,0x40,0x20,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x38,0x10,0x10,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x10,0x3c,0x18,0x14,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x7c,0x48,0x08,0x08,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x38,0x20,0x20,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x7c,0x40,0x78,0x40,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x54,0x54,0x44,0x20,0x10,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x28,0x18,0x08,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x40,0x20,0x10,0x18,0x14,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x10,0x7c,0x44,0x44,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x10,0x10,0x10,0x10,0x10,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x10,0x7e,0x10,0x18,0x14,0x12,0x10,0x00,0x00,0x00,0x00,0x00,
    0x08,0x7e,0x48,0x48,0x48,0x44,0x72,0x00,0x00,0x00,0x00,0x00,
    0x10,0x38,0x10,0x7c,0x10,0x10,0x10,0x00,0x00,0x00,0x00,0x00,
    0x7c,0x44,0x44,0x42,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x7c,0x14,0x12,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x40,0x40,0x40,0x40,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x24,0x7e,0x24,0x24,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x1c,0x40,0x4e,0x40,0x40,0x24,0x18,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x20,0x10,0x18,0x24,0x42,0x00,0x00,0x00,0x00,0x00,
    0x08,0x7e,0x48,0x28,0x08,0x48,0x38,0x00,0x00,0x00,0x00,0x00,
    0x42,0x44,0x48,0x20,0x10,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x42,0x42,0x50,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x50,0x3e,0x10,0x7c,0x10,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x00,0x7e,0x40,0x20,0x10,0x0c,0x00,0x00,0x00,0x00,0x00,
    0x38,0x00,0x7c,0x10,0x10,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x1c,0x24,0x44,0x04,0x04,0x00,0x00,0x00,0x00,0x00,
    0x10,0x7c,0x10,0x10,0x10,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x38,0x00,0x00,0x00,0x00,0x00,0x7c,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x40,0x28,0x10,0x28,0x44,0x00,0x00,0x00,0x00,0x00,
    0x10,0x7e,0x40,0x20,0x30,0x58,0x14,0x00,0x00,0x00,0x00,0x00,
    0x60,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x10,0x20,0x50,0x50,0x50,0x48,0x44,0x00,0x00,0x00,0x00,0x00,
    0x02,0x02,0x7e,0x02,0x02,0x42,0x3c,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x40,0x20,0x10,0x08,0x04,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x14,0x22,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
    0x10,0x7c,0x10,0x54,0x54,0x54,0x10,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x40,0x28,0x10,0x20,0x40,0x00,0x00,0x00,0x00,0x00,
    0x02,0x3c,0x42,0x3c,0x42,0x3c,0x40,0x00,0x00,0x00,0x00,0x00,
    0x20,0x10,0x08,0x04,0x12,0x22,0x5e,0x00,0x00,0x00,0x00,0x00,
    0x40,0x44,0x28,0x10,0x28,0x04,0x02,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x08,0x3c,0x08,0x08,0x48,0x30,0x00,0x00,0x00,0x00,0x00,
    0x08,0x7e,0x48,0x28,0x08,0x08,0x08,0x00,0x00,0x00,0x00,0x00,
    0x3c,0x20,0x20,0x20,0x10,0x08,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x40,0x40,0x7c,0x40,0x40,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x54,0x54,0x44,0x40,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x42,0x42,0x42,0x42,0x22,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x0a,0x0a,0x0a,0x4a,0x4a,0x2a,0x1a,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x04,0x44,0x44,0x24,0x1c,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x42,0x42,0x42,0x42,0x42,0x7e,0x00,0x00,0x00,0x00,0x00,
    0x7e,0x42,0x42,0x40,0x20,0x10,0x08,0x00,0x00,0x00,0x00,0x00,
    0x4e,0x40,0x40,0x40,0x20,0x12,0x0e,0x00,0x00,0x00,0x00,0x00,
    0x08,0x12,0x24,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x04,0x0a,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
#endif
};

