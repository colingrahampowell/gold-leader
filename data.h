/*
 * data.h: program data
 * adapted from Tim Cheeseman's header file:
 * https://github.com/cirla/nesdev/blob/color_in_motion/data.h
 */

#ifndef DATA_H
#define DATA_H

#include "nes.h"
#include <stdint.h>

/*
 * COLOR PALETTE:
 * -- "Each 16px by 16px area associated with a bkgrd palette, and can make use of three colors
 * --  plus bkgrd color"
 * -- We ignore all but last color in each palette for now
 */

const uint8_t PALETTES[]={
	COLOR_BLUE,			// background color
	0, 0, COLOR_RED,	// background palette 0
	0,					// ignored
	0, 0, COLOR_YELLOW,	// background palette 1
	0,					// ignored
	0, 0, COLOR_GREEN,	// background palette 2
	0,					// ignored
	0, 0, COLOR_PURPLE	// background palette 3
}; 

/*
 * ATTRIBUTE TABLE SETUP:
 * -- three sets of four bytes each, one
 * -- for each color scheme that we rotate through
 * -- palettes assigned: 
 *      top-left:   bits 0-1
 *      top-right:  bits 2-3
 *      btm-left:   bits 4-5
 *      btm-right:  bits 6-7
 * *NOTE*: In this case, 4 palettes, because "Hello World" covers 4 16x16 tiles
 */

#define NUM_ATTRS 4 // number of attributes 
#define TEXT_X 10   // 10th col
#define TEXT_Y 14   // 14th row

// desired offset of text area in nametable 
#define TEXT_OFFSET ((TEXT_Y * NUM_COLS) + TEXT_X)    // skip 30 * 14 + 10 places into nametable 

/* 
 * offset of attribute table in nametable:
 * -- each nametable is 1kB chunk 
 * -- attribute table occupies final 64 bytes of nametable 
 * -- defines which palette used for 16px by 16px areas
 * -- (think of a map, similar to nametable)
 * --> so, offset from beginning of attribute table:
 * --> divide by 4 to get 16x16 (2 tile x 2 tile) chunks 
 * --> instead of 8x8 (1 tile x 1 tile) chunks
 */

#define TEXT_ATTR_OFFSET ((TEXT_Y / 4)  * (NUM_COLS / 4) + (TEXT_X / 4))

const unsigned char ATTRIBUTES[] = {
    // layout 1 - 0120123
    0x00, // 00 00 00 00 or 0 0
          //                0 0
    0x90, // 10 01 00 00 or 0 0
          //                1 2
    0x40, // 01 00 00 00 or 0 0
          //                0 1
    0xe0, // 11 10 00 00 or 0 0
          //                2 3

    // layout 2 - 2012013
    0x80, // 10 00 00 00 or 0 0
          //                0 2
    0x40, // 01 00 00 00 or 0 0
          //                0 1
    0x20, // 00 10 00 00 or 0 0
          //                2 0
    0xd0, // 11 01 00 00 or 0 0
          //                1 3

    // layout 3 - 1201203
    0x40, // 01 00 00 00 or 0 0
          //                0 1
    0x20, // 00 10 00 00 or 0 0
          //                2 0
    0x90, // 10 01 00 00 or 0 0
          //                1 2
    0xc0, // 11 00 00 00 or 0 0
          //                0 3
};

#endif