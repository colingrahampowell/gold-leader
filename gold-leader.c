/*	
 * gold leader - primary game loop
 */	

#define TV_NTSC

#include "sprites.h"
#include "background.h"
#include "colors.h"
#include "ppu.h"

#include "reset.h"
#include "input.h"

#include <stdint.h>
#include <stdlib.h>

#pragma bss-name(push, "ZEROPAGE")

//	global counters, etc. (place in zero page for speed)
uint8_t i, j;
uint8_t new_laser_pos;
uint8_t row;
uint8_t col;

// game clock
uint16_t game_clock;

uint8_t h_scroll;

uint8_t attr_offset;
uint8_t curr_sprite;

// player 1 sprite (3 tiles wide x 2 tiles tall)
metasprite_t player;

sprite_t lasers[MAX_LASERS];
uint8_t laser_count;

// enemies
metasprite_t rollys[MAX_ROLLYS];
uint8_t rolly_count;
uint8_t new_rolly_pos;
uint8_t offscreen_rollys;

#pragma bss-name(pop)

#pragma bss-name(push, "OAM")
sprite_t oam_sprites[64];
#pragma bss-name(pop)

/* 
 * WriteMetaSpriteToOAM()
 */

void __fastcall__ WriteMetaSpriteToOAM(metasprite_t* mspr) {

    uint8_t col;
    uint8_t row;
    uint8_t sprite_base_offset = 0;
    uint8_t height_offset = SPRITE_HEIGHT;

    // for each row
    for(row = 0; row < mspr->num_v_sprites; ++row ) {
        // get offset into sprite_offsets array
        sprite_base_offset = row * mspr->num_h_sprites;
        height_offset = SPRITE_HEIGHT * row;

        // for each column
        for(col = 0; col < mspr->num_h_sprites; ++col ) {

            // place a sprite into OAM
            oam_sprites[curr_sprite].y = mspr->top_y + height_offset;
            oam_sprites[curr_sprite].tile_idx = mspr->sprite_offsets[col + sprite_base_offset];
            oam_sprites[curr_sprite].x = mspr->left_x + (SPRITE_WIDTH * col);
            oam_sprites[curr_sprite].attr = 0x00;

            ++curr_sprite;
        }

    }

}

/*
 * WriteSpriteToOAM()
 */

void WriteSpriteToOAM(sprite_t* spr) {

    oam_sprites[curr_sprite].y = spr->y;
    oam_sprites[curr_sprite].tile_idx = spr->tile_idx;
    oam_sprites[curr_sprite].x = spr->x;
    oam_sprites[curr_sprite].attr = 0x00;
    ++curr_sprite;

}

/*
 * update laser positions: 
 */

int CheckOffscreenLasers() {

    uint8_t offscreen_lasers = 0;
    uint8_t i = 0;

    for(i = 0; i < MAX_LASERS; ++i ) {
        if( lasers[i].y < MAX_Y) {
            new_laser_pos = lasers[i].x + LASER_SPEED;
            // if overflow, new pos < old pos
            // also possible that x > offscreen position (0xf9-0xff)
            if( lasers[i].x > new_laser_pos || lasers[i].x >= OFFSCREEN_X ) {
                lasers[i].y = MAX_Y; // offscreen
                ++offscreen_lasers;
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

    uint8_t i = 0;

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
 * UpdatePlayerSprite: updates player sprite based on controller input
 */

void UpdatePlayerSprite() {

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
}

/*
 * MovePlayer: moves the player based on controller input
 */

void MovePlayer() {

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
        ( (player.top_y + player.num_v_sprites + SPRITE_HEIGHT) < ( MAX_Y - (SPRITE_HEIGHT << 1) ) ) ){	
        player.top_y += 2;
    }

    /* 
     * move forward:
     */

    if( (JoyPad1 & BUTTON_RIGHT) && 
        ( (player.left_x + player.num_h_sprites + SPRITE_WIDTH) < (MAX_X - (SPRITE_WIDTH << 1) ) ) ) {
        player.left_x += 2;	
    } 

    /*
     * move back:
     */ 

    if( (JoyPad1 & BUTTON_LEFT) && 
        ( (player.left_x ) > (MIN_X) ) ) {
        player.left_x -= 1;
    }

}

/*
 * AddEnemies: add enemy sprites based on clock state
 */

void AddEnemies() {

    if( ((game_clock % 120) == 0) && (rolly_count < MAX_ROLLYS)) {

        rollys[rolly_count].left_x = MAX_X - 0x01;
        rollys[rolly_count].top_y = MAX_Y >> 0x02;

        ++rolly_count;
    }

}


/*
 * main()
 */

void main (void) {

    game_clock = 0;
    h_scroll = 0;

    LoadPalette(PALETTES, sizeof(PALETTES));
    DrawBackgroundRLE(nametable_0, 0);
    DrawBackgroundRLE(nametable_1, 1);
   
    /*
     * SPRITE SETUP
     */

    curr_sprite = 0;

    /* 
     * init player sprite
     */

    player.left_x = ( MIN_X + (SPRITE_WIDTH << 1) );
    player.top_y = ( (MAX_Y >> 1) - (SPRITE_HEIGHT >> 1) );
    player.num_h_sprites = SHIP_SPRITE_HOR_TILES;
    player.num_v_sprites = SHIP_SPRITE_VERT_TILES;
    player.sprite_offsets = ship_level;

    //WriteMetaSpriteToOAM(&player);	// write player sprite to OAM

    /*
     * init laser sprites
     */

    laser_count = 0;
    for(i = 0; i < MAX_LASERS; ++i) {
        lasers[i].y = 0xff;	// offscreen
        lasers[i].tile_idx = LASER_SPRITE;
    }

    /*
     * init rolly sprites
     */

    rolly_count = 0;
    offscreen_rollys = 0;
    for(i = 0; i < MAX_ROLLYS; ++i) {
        rollys[i].num_h_sprites = ROLLY_HOR_TILES;
        rollys[i].num_v_sprites = ROLLY_VERT_TILES;
        rollys[i].sprite_offsets = rolly_state_1;
        rollys[i].ticks = 0;
        rollys[i].top_y = MAX_Y + (SPRITE_HEIGHT);    // offscreen
        rollys[i].left_x = MAX_X;   // offscreen   
    }


    ResetScroll();
    EnablePPU();


    while(1) {

        // wait for graphics data to be updated in nmi
        // wait for vblank to update display

        WaitFrame();
        curr_sprite = 0;

        /*
         * now, go!
         */

        // add enemies
        if( ((game_clock % 30) == 0) && (rolly_count < MAX_ROLLYS)) {

            // search for empty slot
            for(i = 0; i < MAX_ROLLYS; ++i) {

                if( rollys[i].top_y > MAX_Y ) {
                    rollys[i].left_x = (MAX_X - SPRITE_WIDTH << 0x01);
                    rollys[i].top_y = SPRITE_HEIGHT + (rand() % (MAX_Y - SPRITE_HEIGHT) );
                    ++rolly_count;
                    break;

                }
            }

        }

        offscreen_rollys = 0;

        // move rollys
        for(i = 0; i < MAX_ROLLYS; ++i) {

            // if rolly is 'active' --> not offscreen
            if( rollys[i].top_y <= MAX_Y) {

                new_rolly_pos = rollys[i].left_x -= ROLLY_SPEED;
                // if overflow, new pos < old pos
                // also possible that x > offscreen position (0xf9-0xff)
                if( rollys[i].left_x < new_rolly_pos || rollys[i].left_x == 0 ) {
                    rollys[i].top_y = MAX_Y + (SPRITE_HEIGHT); // offscreen
                    ++offscreen_rollys;
                }
                else {
                    rollys[i].left_x = new_rolly_pos;
                }

            }

        }

        rolly_count -= offscreen_rollys;
    
        SCROLL = ++h_scroll;	// horizontal
        SCROLL = 0x0;

        UpdateInput();
        UpdatePlayerSprite();

        MovePlayer();

        // add new enemies
        // AddEnemies();
        // rolly_count -= MoveEnemies();

         // fire lasers: only fire on discrete button presses 
        if( JoyPad1 & BUTTON_A && !(PrevJoyPad1 & BUTTON_A) ) {
            AddLaser();
        }
        laser_count -= CheckOffscreenLasers();

        // check enemy collisions...

        // write updated data to OAM
        WriteMetaSpriteToOAM(&player);

        for(i = 0; i < MAX_LASERS; ++i) {
            WriteSpriteToOAM( &(lasers[i]) );
        }

        for(i = 0; i < MAX_ROLLYS; ++i) {
            if(rollys[i].top_y <= MAX_Y) {
                WriteMetaSpriteToOAM( &(rollys[i]) );
            }
        }

        ++game_clock;

    }

}
    
    
