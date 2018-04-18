/*	
 * simple Hello World, for cc65, for NES
 * adapted from: doug fraker, nesdoug.com
 */	

#include "nes.h"
#include "data.h"
#include "reset.h"
#include <stdint.h>

#pragma bss-name(push, "ZEROPAGE")

//	Globals
uint8_t index;
uint8_t attr_offset;

uintptr_t ppu_addr;				// unsigned 16-bit type
const uint8_t *ppu_data;		// unsigned 8-bit type
uint8_t ppu_data_size;		// unsigned 8-bit type

#pragma bss-name(pop)

// conveniently, tile locations in pattern table correspond to ASCII vales
// (so, this works)

const uint8_t TEXT[] = { "Hello World!" };

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

	SCROLL = 0;	// horizontal
	SCROLL = 0;	// vertical
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
	 * -- set sprite pattern table address to 0x0000 (bit 3 == 0)
	 * -- set VRAM to inc. address by 1 per CPU read/write, going across (bit 2 == 0) 
	 * -- set base nametable address to 0x2000 (bit 1 && 0 == 0)
	 */

	//	turn on screen -- screen is on, NMI on
	PPU_CTRL = 0x90; 	//	0x90 --> 1001 0000

	/*
	 * PPU_MASK ($2001): PPU Mask Register, controls rendering of sprites / background
	 * http://wiki.nesdev.com/w/index.php/PPU_registers#Mask_.28.242001.29_.3E_write
	 * In this case: we set flags to:
	 * -- show sprites (bit 4)
	 * -- show background (bit 3)
	 * -- show sprites in leftmost 8 pixels of screen (bit 2)
	 * -- show background in leftmost 8 pixels of screen (bit 1)
	 */

	PPU_MASK = 0x1e;	// 0x1e = 0001 1110 

}

/*
 * main()
 */

void main (void) {

	//	turn off the screen
	PPU_CTRL = 0;	// zero-out PPU_CTRL (see below)
	PPU_MASK = 0;	// zero-out PPU_MASK (see below)
	
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

	ppu_addr = PPU_NAMETABLE_0 + TEXT_OFFSET;
	ppu_data = TEXT;
	ppu_data_size = sizeof(TEXT);
	WritePPU();

	/*
	 * ATTRIBUTE TABLE SETUP:
	 */ 

	ppu_addr = PPU_ATTRIB_TABLE_0 + TEXT_ATTR_OFFSET;
	ppu_data = ATTRIBUTES;
	ppu_data_size = NUM_ATTRS;
	WritePPU();

	/* 
	 * DISPLAY:
	 * now, we've set a nametable (sprite positions), but we haven't
	 * displayed it: need to tell PPU where to start reading from, 
	 * by setting scroll position
	 */
		
	ResetScroll();
	EnablePPU();

	attr_offset = NUM_ATTRS;
	
	//	infinite loop
	while (1) {

		if( FrameCount == FRAMES_PER_SEC * 4 ) {
			ppu_data = ATTRIBUTES + attr_offset;
			WritePPU();

			attr_offset += NUM_ATTRS;
			if( attr_offset == sizeof(ATTRIBUTES)) {
				attr_offset = 0;
			}

			ResetScroll();
			FrameCount = 0;

		}

	}
};
	
	