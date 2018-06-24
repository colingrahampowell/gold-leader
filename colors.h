/*
 * ppu_colors.h: palette - and attribute table-related data
 * adapted from Tim Cheeseman's header file:
 * https://github.com/cirla/nesdev/blob/color_in_motion/data.h
 */

#ifndef PPU_COLORS_H
#define PPU_COLORS_H
#include <stdint.h>

/*
 * PPU palette colors
 * see http://wiki.nesdev.com/w/index.php/PPU_palettes
 */

#define COLOR_AQUA    0x1c
#define COLOR_BLACK   0x0f
#define COLOR_BLUE    0x12
#define COLOR_BROWN   0x17
#define COLOR_DGRAY   0x00
#define COLOR_GREEN   0x1a
#define COLOR_LIME    0x2a
#define COLOR_LGRAY   0x10
#define COLOR_MAGENTA 0x24
#define COLOR_MAROON  0x06
#define COLOR_NAVY    0x02
#define COLOR_OLIVE   0x18
#define COLOR_PURPLE  0x14
#define COLOR_RED     0x16
#define COLOR_TEAL    0x2c
#define COLOR_WHITE   0x20
#define COLOR_YELLOW  0x28

/*
 * COLOR PALETTE:
 * -- "Each 16px by 16px area associated with a bkgrd palette, and can make use of three colors
 * --  plus bkgrd color"
 */

uint8_t const PALETTES[] = {
    COLOR_BLACK,                           // background color

    COLOR_BLUE, COLOR_RED, COLOR_PURPLE,    // background palette 0
    0,                                     // ignored
    0, 0, 0,                               // background palette 1
    0,                                     // ignored
    0, 0, 0,                               // background palette 2
    0,                                     // ignored
    0, 0, 0,                               // background palette 3

    COLOR_BLACK,                           // background color (mirror)

    COLOR_WHITE, COLOR_BLUE, COLOR_LGRAY,   // sprite palette 0
    0,                                     // ignored
    0, 0, 0,                               // sprite palette 1
    0,                                     // ignored
    0, 0, 0,                               // sprite palette 2
    0,                                     // ignored
    0, 0, 0,                               // sprite palette 3
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

/*
#define TEXT_ATTR_OFFSET ((TEXT_Y / 4)  * (NUM_COLS / 4) + (TEXT_X / 4))

const uint8_t ATTRIBUTES[] = {
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

*/

#endif