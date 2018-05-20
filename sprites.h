/*
 * sprites.h: useful sprite-related constants
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

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
} metasprite_t;

#define SPRITE_HEIGHT    8
#define SPRITE_WIDTH     8

#define MAX_SPRITES 64

// sprites
#define SHIP_SPRITE_LEVEL 0x00
#define SHIP_SPRITE_BANK_UP 0x03
#define SHIP_SPRITE_BANK_DOWN 0x06
#define SHIP_SPRITE_HOR_TILES 0x03
#define SHIP_SPRITE_VERT_TILES 0x02
#define LASER_SPRITE 0x09

/*
 * sprite maps: proceed top-left to bottom-right
 */

const uint8_t ship_level[6] = {0x00, 0x01, 0x02, 0x10, 0x11, 0x12};   
const uint8_t ship_bank_up[6] = {0x03,0x04,0x05,0x13,0x14,0x15};
const uint8_t ship_bank_down[6] = {0x06,0x07,0x08,0x16,0x17,0x18};

#define MAX_LASERS 5
#define LASER_SPEED 0x08

#endif