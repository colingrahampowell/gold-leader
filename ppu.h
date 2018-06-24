/*
 * Colin Powell
 * PPU base: base PPU header file
 */

#ifndef PPU_H
#define PPU_H
#include <stdint.h>

/*
 * text display constants
 */

#define ALPHA_START 0x11

/* 
 * PPU Constants:
 */

#define PPU_CTRL		*((uint8_t*)0x2000)
#define PPU_MASK		*((uint8_t*)0x2001)
#define PPU_STATUS		*((uint8_t*)0x2002)
#define SCROLL			*((uint8_t*)0x2005)
#define PPU_ADDRESS		*((uint8_t*)0x2006)
#define PPU_DATA		*((uint8_t*)0x2007)

// PPU_CTRL flags
// see http://wiki.nesdev.com/w/index.php/PPU_registers#Controller_.28.242000.29_.3E_write

#define PPUCTRL_NAMETABLE_0 0x00 // use nametable 0
#define PPUCTRL_NAMETABLE_1 0x01 // use nametable 1
#define PPUCTRL_NAMETABLE_2 0x02 // use nametable 2
#define PPUCTRL_NAMETABLE_3 0x03 // use nametable 3

#define PPUCTRL_INC_1_HORIZ 0x00 // PPU_DATA increments 1 horizontally
#define PPUCTRL_INC_32_VERT 0x04 // PPU_DATA increments 32 vertically

#define PPUCTRL_SPATTERN_0  0x00 // sprite pattern table 0
#define PPUCTRL_SPATTERN_1  0x08 // sprite pattern table 1

#define PPUCTRL_BPATTERN_0  0x00 // background pattern table 0
#define PPUCTRL_BPATTERN_1  0x10 // background pattern table 1

#define PPUCTRL_SSIZE_8x8   0x00 // 8x8 sprite size
#define PPUCTRL_SSIZE_16x16 0x00 // 16x16 sprite size

#define PPUCTRL_NMI_OFF     0x00 // disable NMIs
#define PPUCTRL_NMI_ON      0x80 // enable NMIs

// PPU_MASK flags
// see http://wiki.nesdev.com/w/index.php/PPU_registers#Mask_.28.242001.29_.3E_write

#define PPUMASK_COLOR    0x00
#define PPUMASK_GRAY     0x01

#define PPUMASK_L8_BHIDE 0x00
#define PPUMASK_L8_BSHOW 0x02
#define PPUMASK_L8_SHIDE 0x00
#define PPUMASK_L8_SSHOW 0x04

#define PPUMASK_BHIDE    0x00
#define PPUMASK_BSHOW    0x08
#define PPUMASK_SHIDE    0x00
#define PPUMASK_SSHOW    0x10

// NTSC Only
#define PPUMASK_EM_RED   0x20
#define PPUMASK_EM_GREEN 0x40

#define PPUMASK_EM_BLUE  0x80

// PPU memory addresses
// see http://wiki.nesdev.com/w/index.php/PPU_memory_map
// and http://wiki.nesdev.com/w/index.php/PPU_nametables
// and http://wiki.nesdev.com/w/index.php/PPU_attribute_tables
// and http://wiki.nesdev.com/w/index.php/PPU_palettes#Memory_Map

#define PPU_PATTERN_TABLE_0 0x0000 // pattern table 0
#define PPU_PATTERN_TABLE_1 0x1000 // pattern table 1

#define PPU_NAMETABLE_0     0x2000 // nametable 0
#define PPU_NAMETABLE_1     0x2400 // nametable 1
#define PPU_NAMETABLE_2     0x2800 // nametable 2
#define PPU_NAMETABLE_3     0x2c00 // nametable 3

/*
 * attribute table mem locations
 */

#define PPU_ATTRIB_TABLE_0  0x23c0 // attribute table for nametable 0
#define PPU_ATTRIB_TABLE_1  0x27c0 // attribute table for nametable 1
#define PPU_ATTRIB_TABLE_2  0x2bc0 // attribute table for nametable 2
#define PPU_ATTRIB_TABLE_3  0x2fc0 // attribute table for nametable 3

/* 
 * palette mem locations
 */

#define PPU_PALETTE         0x3f00 // palette memory
#define PPU_PALLETE_BGC     0x3f00 // universal background color
#define PPU_PALETTE_BG_0    0x3f01 // background palette 0
#define PPU_PALETTE_BG_1    0x3f05 // background palette 1
#define PPU_PALETTE_BG_2    0x3f09 // background palette 2
#define PPU_PALETTE_BG_3    0x3f0d // background palette 3
#define PPU_PALETTE_SP_0    0x3f11 // sprite palette 0
#define PPU_PALETTE_SP_1    0x3f15 // sprite palette 1
#define PPU_PALETTE_SP_2    0x3f19 // sprite palette 2
#define PPU_PALETTE_SP_3    0x3f1d // sprite palette 3

// PPU resolution
// see http://wiki.nesdev.com/w/index.php/PPU_nametables
#define MIN_X        0
#define MAX_X      256
#define OFFSCREEN_X 249
#define NUM_COLS    32

// TV_NTSC
#define MIN_Y      8
#define MAX_Y    231
#define NUM_ROWS  28
#define FIRST_ROW  1
#define LAST_ROW  27

#define NAMETABLE_OFFSET (NUM_COLS * FIRST_ROW)

/*
 * sprite-related constants
 */

#define SPRITE_HEIGHT    8
#define SPRITE_WIDTH     8
#define MAX_SPRITES 64

// defined in reset.s
void __fastcall__ UnRLE(const uint8_t *data);
void __fastcall__ DrawBackgroundRLE(const uint8_t* nametable, const uint8_t table_num);
void __fastcall__ LoadPalette(const uint8_t* palette, const uint8_t size);
void _WritePPU(void);
void ResetScroll(void);
void EnablePPU(void);

/*
 * sprite: 4 bytes representing a single sprite in the game
 * (order is important here - maps to sprite representation in memory)
 */ 

typedef struct sprite {
    uint8_t y;          // y pos
    uint8_t tile_idx;   // index into patt table
    uint8_t attr;       // attribute flags
    uint8_t x;          // x pos
} sprite_t;


/*
 * metasprite: defines an entity (player, enemy, etc.) in the
 * game that is comprised of a series of sprites.
 */ 

typedef struct metasprite {
    uint8_t left_x;
    uint8_t top_y;
    uint8_t num_h_sprites;
    uint8_t num_v_sprites;
    const uint8_t* sprite_offsets;
    uint8_t ticks;  // time onscreen
} metasprite_t;

#endif