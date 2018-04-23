/*
 * gold-leader.h
 * useful gameplay structs
 */

#include <stdint.h>

typedef struct sprite {
    uint8_t y;          // y pos
    uint8_t tile_idx;   // index into patt table
    uint8_t attr;       // attribute flags
    uint8_t x;          // x pos
} sprite_t;