 
/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/* ================================================================ */
/*
File:	ConvertUTF.c
Author: Mark E. Davis
Copyright (C) 1994 Taligent, Inc. All rights reserved.

This code is copyrighted. Under the copyright laws, this code may not
be copied, in whole or part, without prior written consent of Taligent. 

Taligent grants the right to use or reprint this code as long as this
ENTIRE copyright notice is reproduced in the code or reproduction.
The code is provided AS-IS, AND TALIGENT DISCLAIMS ALL WARRANTIES,
EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN
NO EVENT WILL TALIGENT BE LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING,
WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS
INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY
LOSS) ARISING OUT OF THE USE OR INABILITY TO USE THIS CODE, EVEN
IF TALIGENT HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
BECAUSE SOME STATES DO NOT ALLOW THE EXCLUSION OR LIMITATION OF
LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.

RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
government is subject to restrictions as set forth in subparagraph
(c)(l)(ii) of the Rights in Technical Data and Computer Software
clause at DFARS 252.227-7013 and FAR 52.227-19.

This code may be protected by one or more U.S. and International
Patents.

TRADEMARKS: Taligent and the Taligent Design Mark are registered
trademarks of Taligent, Inc.

HISTORY:

	22-Jan-1999	Don Brady		Add decomposition to ConvertUTF8toUTF16.
	17-Nov-1998	Don Brady		Add ":" to "/" conversions.
*/
/* ================================================================ */

#include "ConvertUTF.h"

/* ================================================================ */

const int halfShift				= 10;
const UCS4 halfBase				= 0x0010000UL;
const UCS4 halfMask				= 0x3FFUL;
const UCS4 kSurrogateHighStart	= 0xD800UL;
const UCS4 kSurrogateHighEnd	= 0xDBFFUL;
const UCS4 kSurrogateLowStart	= 0xDC00UL;
const UCS4 kSurrogateLowEnd		= 0xDFFFUL;

const UCS4 kReplacementCharacter =	0x0000FFFDUL;
const UCS4 kMaximumUCS2 =			0x0000FFFFUL;
const UCS4 kMaximumUTF16 =			0x0010FFFFUL;
const UCS4 kMaximumUCS4 =			0x7FFFFFFFUL;

/* ================================================================ */

UCS4 offsetsFromUTF8[6] =	{0x00000000UL, 0x00003080UL, 0x000E2080UL, 
					 	 	 0x03C82080UL, 0xFA082080UL, 0x82082080UL};
char bytesFromUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5};

UTF8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

static UTF16 Decomposer(register UTF16 srcChar, UTF16 *cmbChar);

/* ================================================================ */
/*	This code is similar in effect to making successive calls on the
* mbtowc and wctomb routines in FSS-UTF. However, it is considerably
* different in code:
* it is adapted to be consistent with UTF16,
* the interface converts a whole buffer to avoid function-call overhead
* constants have been gathered.
* loops & conditionals have been removed as much as possible for
* efficiency, in favor of drop-through switch statements.
*/

/*
 * Colons vs. Slash
 *
 * The VFS layer uses a "/" as a pathname separator but HFS disks
 * use a ":".  So when converting from UTF-8, ":" characters need
 * to be changed to "/" so that colons don't end up on HFS disks.
 * Likewise when converting into UTF-8, "/" characters need to be
 * changed to ":" so that a "/" in a filename is not returned 
 * through the VFS layer.
 *
 * We do not need to worry about full-width slash or colons since
 * their respective representations outside of Unicode are never
 * the 7-bit versions (0x2f or 0x3a).
 */


/* ================================================================ */
ConversionResult	ConvertUTF16toUTF8 (
		UTF16** sourceStart, const UTF16* sourceEnd, 
		UTF8** targetStart, const UTF8* targetEnd)
{
	ConversionResult result = ok;
	register UTF16* source = *sourceStart;
	register UTF8* target = *targetStart;
	while (source < sourceEnd) {
		register UCS4 ch;
		register unsigned short bytesToWrite;
		register const UCS4 byteMask = 0xBF;
		register const UCS4 byteMark = 0x80; 
		register const UCS4 slash = '/'; 

		ch = *source++;

		/* optimize for ASCII case... */
		if (ch < 0x80) {
			if (ch == slash)
			ch = ':';	/* VFS doesn't like slash */

			if (target >= targetEnd) {
				result = targetExhausted;
				break;
			}
			if (ch == 0) {
				continue;	/* skip over embedded NULLs */
			}

			*target++ = ch;
			continue;
		} else if (ch >= kSurrogateHighStart && ch <= kSurrogateHighEnd
				&& source < sourceEnd) {
			register UCS4 ch2 = *source;
			if (ch2 >= kSurrogateLowStart && ch2 <= kSurrogateLowEnd) {
				ch = ((ch - kSurrogateHighStart) << halfShift)
					+ (ch2 - kSurrogateLowStart) + halfBase;
				++source;
			};
		};

		if (ch < 0x80) {				bytesToWrite = 1;
		} else if (ch < 0x800) {		bytesToWrite = 2;
		} else if (ch < 0x10000) {		bytesToWrite = 3;
		} else if (ch < 0x200000) {		bytesToWrite = 4;
		} else if (ch < 0x4000000) {	bytesToWrite = 5;
		} else if (ch <= kMaximumUCS4){	bytesToWrite = 6;
		} else {						bytesToWrite = 2;
										ch = kReplacementCharacter;
		}; /* I wish there were a smart way to avoid this conditional */
		
		target += bytesToWrite;
		if (target > targetEnd) {
			target -= bytesToWrite; result = targetExhausted; break;
		};
		switch (bytesToWrite) {	/* note: code falls through cases! */
			case 6:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 5:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 4:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 3:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 2:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 1:	*--target =  ch | firstByteMark[bytesToWrite];
		};
		target += bytesToWrite;
	};
	*sourceStart = source;
	*targetStart = target;
	return result;
};

/* ================================================================ */

ConversionResult	ConvertUTF8toUTF16 (
		UTF8** sourceStart, UTF8* sourceEnd, 
		UTF16** targetStart, const UTF16* targetEnd)
{
	ConversionResult result = ok;
	register UTF8* source = *sourceStart;
	register UTF16* target = *targetStart;

	while (source < sourceEnd) {
		UTF8 byte;
		register UCS4 ch;
		register unsigned short extraBytesToWrite;

		/* optimize for ASCII case...*/
		byte = *source;
		if (byte < 128) {
			if (byte == ':')
				byte = '/';	/* HFS doesn't like colons */
			source++;
			*target++ = byte;
			continue;
		}

		extraBytesToWrite = bytesFromUTF8[*source];
		if (source + extraBytesToWrite > sourceEnd) {
			result = sourceExhausted; break;
		};
		ch = 0;
		switch(extraBytesToWrite) {	/* note: code falls through cases! */
			case 5:	ch += *source++; ch <<= 6;
			case 4:	ch += *source++; ch <<= 6;
			case 3:	ch += *source++; ch <<= 6;
			case 2:	ch += *source++; ch <<= 6;
			case 1:	ch += *source++; ch <<= 6;
			case 0:	ch += *source++;
		};
		ch -= offsetsFromUTF8[extraBytesToWrite];

		if (target >= targetEnd) {
			result = targetExhausted; break;
		};
		if (ch <= kMaximumUCS2) {
			UTF16 combine;

			*target++ = Decomposer(ch, &combine);

			if (combine) {
				if (target >= targetEnd) {
					result = targetExhausted; break;
				};
				*target++ = combine;
			}
		} else if (ch > kMaximumUTF16) {
			*target++ = kReplacementCharacter;
		} else {
			if (target + 1 >= targetEnd) {
				result = targetExhausted; break;
			};
			ch -= halfBase;
			*target++ = (ch >> halfShift) + kSurrogateHighStart;
			*target++ = (ch & halfMask) + kSurrogateLowStart;
		};
	};
	*sourceStart = source;
	*targetStart = target;
	return result;
};

/*
 * Lookup tables for Unicode chars 0x00C0 thru 0x00FF
 * primary_char yields first decomposed char. If this
 * char is an alpha char then get the combining char
 * from the combining_char table and add 0x0300 to it.
 */

static unsigned char primary_char[64] = {
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,

	0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,

	0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7,

	0xD8, 0x55, 0x55, 0x55, 0x55, 0x59, 0xDE, 0xDF,

	0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0xE6, 0x63,

	0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69,

	0xF0, 0x6E, 0x6F, 0x6F, 0x6F, 0x6F, 0x6F, 0xF7,

	0xF8, 0x75, 0x75, 0x75, 0x75, 0x79, 0xFE, 0x79,
};

static unsigned char combining_char[64] = {
	0x00, 0x01, 0x02, 0x03, 0x08, 0x0A, 0xFF, 0x27,

	0x00, 0x01, 0x02, 0x08, 0x00, 0x01, 0x02, 0x08,

	0xFF, 0x03, 0x00, 0x01, 0x02, 0x03, 0x08, 0xFF,

	0xFF, 0x00, 0x01, 0x02, 0x08, 0x01, 0xFF, 0xFF,

	0x00, 0x01, 0x02, 0x03, 0x08, 0x0A, 0xFF, 0x27,

	0x00, 0x01, 0x02, 0x08, 0x00, 0x01, 0x02, 0x08,

	0xFF, 0x03, 0x00, 0x01, 0x02, 0x03, 0x08, 0xFF,

	0xFF, 0x00, 0x01, 0x02, 0x08, 0x01, 0xFF, 0x08
};


static const unsigned long __CJKDecompBitmap[] = {	// 0x3000 ~ 0x30FF
    0x00000000, 0x00000000, 0x000AAAAA, 0xA540DB6C,	// 0x3000
    0x00000802, 0x000AAAAA, 0xA540DB6C, 0x000009E2,	// 0x3080
};

#define IS_DECOMPOSABLE(table,unicodeVal) (table[(unicodeVal) / 32] & (1 << (31 - ((unicodeVal) % 32))))


/*
 * Decomposer
 *
 * Composed Unicode characters are forbidden on
 * HFS Plus volumes. Decomposer will convert a
 * composed character into its correct decomposed
 * sequence.
 *
 * Currently only MacRoman and MacJapanese chars
 * are handled.  Other composed characters are
 * passed unchanged.
 */
static UTF16
Decomposer(register UTF16 srcChar, UTF16 *cmbChar)
{
	UTF16 dstChar;
	
	*cmbChar = 0;

	if ((srcChar <= 0x00FF) && (srcChar >= 0x00C0)) {
		srcChar -= 0x00C0;
		
		dstChar = (UTF16) primary_char[srcChar];

		if (dstChar <= 'z') {
			*cmbChar = (UTF16) 0x0300 + (UTF16) combining_char[srcChar];
		}
	} else if ((srcChar > 0x3000) && (srcChar < 0x3100) &&
				IS_DECOMPOSABLE(__CJKDecompBitmap, srcChar - 0x3000)) {
		switch(srcChar) {
		case 0x3071: dstChar = 0x306F; *cmbChar = 0x309A; break;	// HIRAGANA LETTER PA
		case 0x3074: dstChar = 0x3072; *cmbChar = 0x309A; break; 	// HIRAGANA LETTER PI
		case 0x3077: dstChar = 0x3075; *cmbChar = 0x309A; break;	// HIRAGANA LETTER PU
		case 0x307A: dstChar = 0x3078; *cmbChar = 0x309A; break;	// HIRAGANA LETTER PE

		case 0x307D: dstChar = 0x307B; *cmbChar = 0x309A; break;	// HIRAGANA LETTER PO
		case 0x3094: dstChar = 0x3046; *cmbChar = 0x3099; break;	// HIRAGANA LETTER VU
		case 0x30D1: dstChar = 0x30CF; *cmbChar = 0x309A; break;	// KATAKANA LETTER PA
		case 0x30D4: dstChar = 0x30D2; *cmbChar = 0x309A; break;	// KATAKANA LETTER PI

		case 0x30D7: dstChar = 0x30D5; *cmbChar = 0x309A; break;	// KATAKANA LETTER PU
		case 0x30DA: dstChar = 0x30D8; *cmbChar = 0x309A; break;	// KATAKANA LETTER PE
		case 0x30DD: dstChar = 0x30DB; *cmbChar = 0x309A; break;	// KATAKANA LETTER PO
		case 0x30F4: dstChar = 0x30A6; *cmbChar = 0x3099; break;	// KATAKANA LETTER VU

		case 0x30F7: dstChar = 0x30EF; *cmbChar = 0x3099; break;	// KATAKANA LETTER VA
		case 0x30F8: dstChar = 0x30F0; *cmbChar = 0x3099; break;	// KATAKANA LETTER VI
		case 0x30F9: dstChar = 0x30F1; *cmbChar = 0x3099; break;	// KATAKANA LETTER VE
		case 0x30FA: dstChar = 0x30F2; *cmbChar = 0x3099; break;	// KATAKANA LETTER VO
		
		default:
			/* the rest (41 of them) have a simple conversion */
			dstChar = srcChar - 1;
			*cmbChar = 0x3099;
		};
	} else {
		dstChar = srcChar;
	}
	
	return dstChar;
}
