/* 
 * Colin Powell
 * PPU library functions
 */

#include "ppu.h"
#include <stdint.h>

#pragma bss-name(push, "ZEROPAGE")
uint8_t _p_count;           // ppu counter
uintptr_t ppu_addr;			// unsigned 16-bit type
const uint8_t *ppu_data;	// unsigned 8-bit type
uint8_t ppu_data_size;		// unsigned 8-bit type
#pragma bss-name(pop)

/* 
 * WritePPU()
 * NOTE: ppu_addr, ppu_data global (ZP) variables
 * set elsewhere.
 */

void _WritePPU() {
    PPU_ADDRESS = (uint8_t) (ppu_addr >> 8); 	// right shift to write only hi-byte
    PPU_ADDRESS = (uint8_t) (ppu_addr);		// now write lo byte

    for(_p_count = 0; _p_count < ppu_data_size; ++_p_count) {
        PPU_DATA = ppu_data[_p_count];
    }	
}

/*
 * LOADING THE PALETTE: 
 * palette data is stored in mem addresses 0x3f00 to 0x3f1f
 * that is, 4x8 bits = 32 bits of data, starting at 0x3f00
 * we need to write this to PPU_DATA (0x2007), one byte at a time
 */

void __fastcall__ LoadPalette(const uint8_t* palette, const uint8_t pal_size) {    
    ppu_addr = PPU_PALETTE;	// 0x3f00: palette memory
    ppu_data = palette; 
    ppu_data_size = pal_size;
    _WritePPU();
}

/*
 * DrawBackgroundRLE():
 * 
 * BACKGROUND TILES: NAMETABLE SETUP:
 * nametable 0: 0x2000-0x23FF
 * again, we write tiles to PPU_DATA, one at a time
 * set position example:
 * -- 32 (0x20) tiles per row, 30 (0x1d) rows (32 * 30 = 960 tiles)
 * -- 1c6: 454th tile, or row 14, col 6
 */

void __fastcall__ DrawBackgroundRLE(const uint8_t* nametable, const uint8_t table_num) {

    if( table_num == 0 ) {
        // set address of nametable 0
        PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_0 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
        PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_0 + NAMETABLE_OFFSET);		    // now write lo-byte
    }
    else if( table_num == 1 ) {
        // set address of nametable 0
        PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_1 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
        PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_1 + NAMETABLE_OFFSET);		    // now write lo-byte        
    }
    else {
        return;
    }

    // decompress and draw nametable
    UnRLE(nametable);

}

/* 
 * ResetScroll():
 * set address of nametable 0
 * PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_0 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
 * PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_0 + NAMETABLE_OFFSET);		    // now write lo-byte
 */

void ResetScroll() {
    SCROLL = 0x00;	// horizontal
    SCROLL = 0x00;	// vertical
}

/*
 * EnablePPU():
 */

void EnablePPU() {

    /*
     * PPU_CTRL ($2000): PPU Control Register 
     * http://wiki.nesdev.com/w/i.php/PPU_registers#Controller_.28.242000.29_.3E_write
     * In this case, we set flags to:
     * -- generate NMI (non-maskable interrupt) at start of vblank (bit 7)
     * -- set sprite size to 8x8 (bit 5 == 0)
     * -- set background pattern table to 0x1000 (bit 4 == 1)
     * -- set sprite pattern table address to 0x0000 (bit 3 == 0)
     * -- set VRAM to inc. address by 1 per CPU read/write, going across (bit 2 == 0) 
     * -- set base nametable address to 0x2000 (bit 1 && 0 == 0)
     */

    //	turn on screen -- screen is on, NMI on
    PPU_CTRL = 	PPUCTRL_NAMETABLE_0 | 	// use nametable 0
                PPUCTRL_BPATTERN_1 	| 	// background uses pattern table 1
                PPUCTRL_NMI_ON		|	// enable NMI
                PPUCTRL_SPATTERN_0;		// sprites use pattern table 0

    /*
     * PPU_MASK ($2001): PPU Mask Register, controls rendering of sprites / background
     * http://wiki.nesdev.com/w/i.php/PPU_registers#Mask_.28.242001.29_.3E_write
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

