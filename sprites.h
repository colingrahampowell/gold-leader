/*
 * sprites.h: useful sprite-related constants
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

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
const uint8_t rolly_state_1[4] = {0x20,0x21,0x30,0x31};

#define MAX_LASERS 5
#define LASER_SPEED 0x08

#define MAX_ROLLYS 4
#define ROLLY_HOR_TILES 2
#define ROLLY_VERT_TILES 2
#define ROLLY_SPEED 0x02

#endif