/*	
 * simple Hello World, for cc65, for NES
 * adapted from: doug fraker, nesdoug.com
 */	


#define TV_NTSC

#include "gold-leader.h"
#include "ppu.h"
#include "ppu_data.h"
#include "reset.h"
#include "input.h"
#include <stdint.h>

#pragma bss-name(push, "ZEROPAGE")
//	Globals
uint8_t index, inner;
uint8_t attr_offset;

uintptr_t ppu_addr;				// unsigned 16-bit type
const uint8_t *ppu_data;		// unsigned 8-bit type
uint8_t ppu_data_size;		// unsigned 8-bit type
#pragma bss-name(pop)

#pragma bss-name(push, "OAM")
// player 1 sprite
sprite_t player;
#pragma bss-name(pop)

/*
 * DrawBackground():
 */

void DrawBackground() {

	// write background data, one tile at a time
	PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_0 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
	PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_0 + NAMETABLE_OFFSET);		// now write lo byte

	// draw top row
	PPU_DATA = CORNER_TL;
	for(index = 0; index < NUM_COLS - 2; ++index) {	
		PPU_DATA = EDGE_TOP;
	}
	PPU_DATA = CORNER_TR;

	// draw middle rows
	for(index = 0; index < NUM_ROWS - 2; ++index) {
		PPU_DATA = EDGE_LEFT;
		for(inner = 0; inner < NUM_COLS - 2; ++inner ) {
			PPU_DATA = BLANK_TILE;
		}
		PPU_DATA = EDGE_RIGHT;
	}

	// draw bottom row
	PPU_DATA = CORNER_BL;
	for(index = 0; index < NUM_COLS - 2; ++index) {	
		PPU_DATA = EDGE_BOTTOM;
	}
	PPU_DATA = CORNER_BR;

}

/* 
 * WritePPU():
 */

void WritePPU() {

	PPU_ADDRESS = (uint8_t) (ppu_addr >> 8); 	// right shift to write only hi-byte
	PPU_ADDRESS = (uint8_t) (ppu_addr);		// now write lo byte
	
	for(index = 0; index < ppu_data_size; ++index){
		PPU_DATA = ppu_data[index];
	}	

}

/* 
 * ResetScroll():
 */

void ResetScroll() {

	/*
	 * first write horizontal offset, then vertical offset to PPUSCROLL
	 */

	SCROLL = 0x00;	// horizontal
	SCROLL = 0x00;	// vertical
}

/*
 * EnablePPU():
 */
void EnablePPU() {

	/*
	 * PPU_CTRL ($2000): PPU Control Register 
	 * http://wiki.nesdev.com/w/index.php/PPU_registers#Controller_.28.242000.29_.3E_write
	 * In this case, we set flags to:
	 * -- generate NMI (non-maskable interrupt) at start of vblank (bit 7)
	 * -- set sprite size to 8x8 (bit 5 == 0)
	 * -- set background pattern table to 0x1000 (bit 4 == 1)
	 * -- set sprite pattern table address to 0x1000 (bit 3 == 1)
	 * -- set VRAM to inc. address by 1 per CPU read/write, going across (bit 2 == 0) 
	 * -- set base nametable address to 0x2000 (bit 1 && 0 == 0)
	 */

	//	turn on screen -- screen is on, NMI on
	PPU_CTRL = 	PPUCTRL_NAMETABLE_0 | 	// use nametable 0
				PPUCTRL_BPATTERN_1 	| 	// background uses pattern table 1
				PPUCTRL_NMI_ON		|	// enable NMI
				PPUCTRL_SPATTERN_1;		// sprites use pattern table 1

	/*
	 * PPU_MASK ($2001): PPU Mask Register, controls rendering of sprites / background
	 * http://wiki.nesdev.com/w/index.php/PPU_registers#Mask_.28.242001.29_.3E_write
	 * In this case: we set flags to:
	 * -- show sprites (bit 4)
	 * -- show background (bit 3)
	 * -- show sprites in leftmost 8 pixels of screen (bit 2)
	 * -- show background in leftmost 8 pixels of screen (bit 1)
	 */

	PPU_MASK = 	PPUMASK_COLOR 		| 	// show color
				PPUMASK_BSHOW		|	// show background
				PPUMASK_L8_BSHOW	|	// show bkgrd tiles in left 8 pixels
				PPUMASK_SSHOW		|	// show sprites
				PPUMASK_L8_SSHOW;		// show sprites in left 8 pixels

}				

/*
 * main()
 */

void main (void) {

	//	turn off the screen
//	PPU_CTRL = 0;	// zero-out PPU_CTRL (see below)
//	PPU_MASK = 0;	// zero-out PPU_MASK (see below)
	
	/*
	 * LOADING THE PALETTE: 
	 * palette data is stored in mem addresses 0x3f00 to 0x3f1f
	 * that is, 4x8 bits = 32 bits of data, starting at 0x3f00
	 * we need to write this to PPU_DATA (0x2007), one byte at a time
	 */

	ppu_addr = PPU_PALETTE;	// 0x3f00: palette memory
	ppu_data = PALETTES; 
	ppu_data_size = sizeof(PALETTES);
	WritePPU();

	/* 
	 * BACKGROUND TILES: NAMETABLE SETUP:
	 * nametable 0: 0x2000-0x23FF
	 * again, we write tiles to PPU_DATA, one at a time
	 * set position:
	 * -- 32 (0x20) tiles per row, 30 (0x1d) rows (32 * 30 = 960 tiles)
	 * -- 1c6: 454th tile, or row 14, col 6
	*/

	DrawBackground();

	player.x = MIN_X + SPRITE_WIDTH * 2;
	player.y = MAX_Y / 2 - SPRITE_HEIGHT / 2;
	player.tile_idx = SPRITE_SHIP;

	ResetScroll();
	EnablePPU();

	while(1) {
		UpdateInput();
		WaitFrame();

		// move up
		if( (JoyPad1 & BUTTON_UP) && (player.y > (MIN_Y + SPRITE_HEIGHT )) ) {
			player.y -= 2;
		}

		// move down
		else if( (JoyPad1 & BUTTON_DOWN) && (player.y < (MAX_Y - 2 * SPRITE_HEIGHT) ) ){
			player.y += 2;
		}

	}

}
	
	