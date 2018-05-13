/*
 * background.h: useful background-related constants
 */

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdint.h>

// defined in reset.s
void __fastcall__ UnRLE(uint8_t *data);

// demo background tiles
#define BLANK_TILE 0x00
#define CORNER_TL 0x04
#define CORNER_TR 0x01
#define CORNER_BL 0x10
#define CORNER_BR 0x14
#define EDGE_TOP 0x13
#define EDGE_BOTTOM 0x03
#define EDGE_LEFT 0x12
#define EDGE_RIGHT 0x02

uint8_t nametable_0[255] = { 0 };



#endif