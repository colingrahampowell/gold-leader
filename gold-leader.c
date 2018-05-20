/*	
 * simple Hello World, for cc65, for NES
 * adapted from: doug fraker, nesdoug.com
 */	


#define TV_NTSC

#include "ppu.h"
#include "ppu_data.h"
#include "reset.h"
#include "input.h"
#include "sprites.h"
#include "background.h"
#include <stdint.h>

#pragma bss-name(push, "ZEROPAGE")

//	global counters, etc. (place in zero page for speed)
uint8_t i, j;
uint8_t new_laser_pos;
uint8_t row;
uint8_t col;

uint8_t h_scroll;

uint8_t attr_offset;
uint8_t curr_sprite;

uint8_t laser_count;

uintptr_t ppu_addr;			// unsigned 16-bit type
const uint8_t *ppu_data;	// unsigned 8-bit type
uint8_t ppu_data_size;		// unsigned 8-bit type

// nametable-to-sprite mapping tables
uint8_t ship_level[ SHIP_SPRITE_HOR_TILES * SHIP_SPRITE_VERT_TILES ];
uint8_t ship_bank_up[ SHIP_SPRITE_HOR_TILES * SHIP_SPRITE_VERT_TILES ];
uint8_t ship_bank_down[ SHIP_SPRITE_HOR_TILES * SHIP_SPRITE_VERT_TILES ];

// player 1 sprite (3 tiles wide x 2 tiles tall)
metasprite_t player;
sprite_t lasers[MAX_LASERS];

#pragma bss-name(pop)

#pragma bss-name(push, "OAM")
sprite_t oam_sprites[64];
#pragma bss-name(pop)

/*
 * DrawBackgroundRLE():
 */

void DrawBackgroundRLE() {
	
	// set address of nametable 0
	PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_0 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
	PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_0 + NAMETABLE_OFFSET);		// now write lo ebyte

	// decompress and draw nametable 0
	UnRLE(nametable_0);

	// set address of nametable 1
	PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_1 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
	PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_1 + NAMETABLE_OFFSET);		// now write lo ebyte
	UnRLE(nametable_1);
    
}

/*
 * DrawBackground():
 */

void DrawBackground() {

	// write background data, one tile at a time
	PPU_ADDRESS = (uint8_t) ((PPU_NAMETABLE_0 + NAMETABLE_OFFSET) >> 8); 	// right shift to write only hi-byte
	PPU_ADDRESS = (uint8_t) (PPU_NAMETABLE_0 + NAMETABLE_OFFSET);		// now write lo byte

	// draw top row
	PPU_DATA = CORNER_TL;
	for(i = 0; i < NUM_COLS - 2; ++i) {	
		PPU_DATA = EDGE_TOP;
	}
	PPU_DATA = CORNER_TR;

	// draw middle rows
	for(i = 0; i < NUM_ROWS - 2; ++i) {
		PPU_DATA = EDGE_LEFT;
		for(j = 0; j < NUM_COLS - 2; ++j ) {
			PPU_DATA = BLANK_TILE;
		}
		PPU_DATA = EDGE_RIGHT;
	}

	// draw bottom row
	PPU_DATA = CORNER_BL;
	for(i = 0; i < NUM_COLS - 2; ++i) {	
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
	
	for(i = 0; i < ppu_data_size; ++i){
		PPU_DATA = ppu_data[i];
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

/* 
 * WriteMetaSpriteToOAM()
 */

void WriteMetaSpriteToOAM(metasprite_t* mspr) {

	for( i = 0; i < (player.num_v_sprites * player.num_h_sprites); ++i ) {

		row = i / player.num_h_sprites;
		col = i % player.num_h_sprites;

		oam_sprites[curr_sprite].y = mspr->top_y + (SPRITE_HEIGHT * row);
		oam_sprites[curr_sprite].tile_idx = mspr->sprite_offsets[i];
		oam_sprites[curr_sprite].x = mspr->left_x + (SPRITE_WIDTH * col);
		oam_sprites[curr_sprite].attr = 0x00;

		++curr_sprite;
	}

}

/*
 * WriteSpriteToOAM()
 */

void WriteSpriteToOAM(sprite_t* spr) {
//	if (spr->y < MAX_Y) {
		oam_sprites[curr_sprite].y = spr->y;
		oam_sprites[curr_sprite].tile_idx = spr->tile_idx;
		oam_sprites[curr_sprite].x = spr->x;
		oam_sprites[curr_sprite].attr = 0x00;
		++curr_sprite;
//	}
}

/*
 * update laser positions: 
 */

int CheckOffscreenLasers() {

	int offscreen_lasers = 0;

	for(i = 0; i < MAX_LASERS; ++i ) {
		if( lasers[i].y < MAX_Y) {
			new_laser_pos = lasers[i].x + LASER_SPEED;
			// if overflow, new pos < old pos
			// also possible that x > offscreen position (0xf9-0xff)
			if( lasers[i].x > new_laser_pos || lasers[i].x >= OFFSCREEN_X ) {
				lasers[i].y = MAX_Y; // offscreen
				++offscreen_lasers;
	//				player.sprite offsets = ship_bank_down;
			}
			else {
				lasers[i].x = new_laser_pos;
			}
		}
	}

	return offscreen_lasers;
}

/*
 * Adds a laser to the screen
 * laser fires from nose of ship:
 * xx(x) <-- this tile
 * xxx
 */

void AddLaser() {
	// track max number of lasers on screen
	if(laser_count < MAX_LASERS) {
		i = 0;
		// find a spot for new laser, update pos
		while( i < MAX_LASERS ) {
			if(lasers[i].y >= MAX_Y) {
				lasers[i].x = player.left_x + player.num_h_sprites + SPRITE_WIDTH;
				lasers[i].y = player.top_y;
				++laser_count;
				break;
			}					
			++i;
		}
	}
}



/*
 * main()
 */

void main (void) {

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
	 * set position example:
	 * -- 32 (0x20) tiles per row, 30 (0x1d) rows (32 * 30 = 960 tiles)
	 * -- 1c6: 454th tile, or row 14, col 6
	*/

	DrawBackgroundRLE();
	h_scroll = 0;

	/*
	 * SPRITE SETUP
	 */

	curr_sprite = 0;

	// this is very goofy...find a way to init this data in an array somewhere

	ship_level[0] = 0x00;
	ship_level[1] = 0x01;
	ship_level[2] = 0x02;
	ship_level[3] = 0x10;
	ship_level[4] = 0x11;
	ship_level[5] = 0x12;

	ship_bank_up[0] = 0x03;
	ship_bank_up[1] = 0x04;
	ship_bank_up[2] = 0x05;
	ship_bank_up[3] = 0x13;
	ship_bank_up[4] = 0x14;
	ship_bank_up[5] = 0x15;

	ship_bank_down[0] = 0x06;
	ship_bank_down[1] = 0x07;
	ship_bank_down[2] = 0x08;
	ship_bank_down[3] = 0x16;
	ship_bank_down[4] = 0x17;
	ship_bank_down[5] = 0x18;

	/* 
	 * init player sprite
	 */

	player.left_x =  (MIN_X + SPRITE_WIDTH * 2);
	player.top_y = (MAX_Y / 2 - SPRITE_HEIGHT / 2);
	player.num_h_sprites = SHIP_SPRITE_HOR_TILES;
	player.num_v_sprites = SHIP_SPRITE_VERT_TILES;
	player.sprite_offsets = ship_level;

	WriteMetaSpriteToOAM(&player);	// write player sprite to OAM

	/*
	 * init laser sprites
	 */

	laser_count = 0;
	for(i = 0; i < MAX_LASERS; ++i) {
		lasers[i].y = 0xff;	// offscreen
		lasers[i].tile_idx = LASER_SPRITE;
	}

	ResetScroll();
	EnablePPU();

	while(1) {

		curr_sprite = 0;

		// wait for graphics data to be updated in nmi
		WaitFrame();
	
		// wait for vblank to update display
		curr_sprite = 0;
		h_scroll += 1;

		SCROLL = h_scroll;	// horizontal
		SCROLL = 0x0;

		UpdateInput();

		// update sprite based on input (change sprite to simulate left/right bank manuever)

		if( (JoyPad1 & BUTTON_UP) && player.sprite_offsets != ship_bank_up) {
			player.sprite_offsets = ship_bank_up;
		}
		else if((JoyPad1 & BUTTON_DOWN) && 
				(player.sprite_offsets != ship_bank_down) ) {	
			player.sprite_offsets = ship_bank_down;
		}
		else if( !(JoyPad1 & BUTTON_UP) && 
				 !(JoyPad1 & BUTTON_DOWN) && 
				 (player.sprite_offsets != ship_level ) ) {
			player.sprite_offsets = ship_level;
		}

		/* 
		 * move up: player.top_y == top left sprite of player ship metasprite 
		 * (make sure hasn't reached top of screen)
		 */

		if( (JoyPad1 & BUTTON_UP) && 
			(player.top_y > (MIN_Y + SPRITE_HEIGHT )) ) {
			player.top_y -= 2;
		}

		/*
		 * move down: player.top_y + player.num_v_sprites
		 * == bottom left sprite of player ship metasprite
		 * make sure hasn't reached bottom of screen
		 */

		if( (JoyPad1 & BUTTON_DOWN) && 
			( (player.top_y + player.num_v_sprites + SPRITE_HEIGHT) < (MAX_Y - 2 * SPRITE_HEIGHT) ) ){	
			player.top_y += 2;
		}

		/* 
		 * move forward:
		 */

		if( (JoyPad1 & BUTTON_RIGHT) && 
			( (player.left_x + player.num_h_sprites + SPRITE_WIDTH) < (MAX_X - 2 * SPRITE_WIDTH ) ) ) {
			player.left_x += 2;	
		} 

		/*
		 * move back:
		 */ 

		if( (JoyPad1 & BUTTON_LEFT) && 
			( (player.left_x ) > (MIN_X + SPRITE_WIDTH) ) ) {
			player.left_x -= 1;
		} 

		/* 
		 * fire lasers: only fire on discrete button presses 
		 */

		if( JoyPad1 & BUTTON_A && !(PrevJoyPad1 & BUTTON_A) ) {
			AddLaser();
		}
		laser_count -= CheckOffscreenLasers();

		/*
		 * write updated data to OAM
		 */

		WriteMetaSpriteToOAM(&player);
		for(i = 0; i < MAX_LASERS; ++i) {
			WriteSpriteToOAM( &(lasers[i]) );
		}

	}
}
	
	
