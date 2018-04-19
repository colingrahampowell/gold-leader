/*
 * input.h: useful #defines, functions for reading controller input 
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define BUTTON_RIGHT 0x01
#define BUTTON_LEFT 0x02
#define BUTTON_DOWN 0x04
#define BUTTON_UP 0x08
#define BUTTON_START 0x10
#define BUTTON_SELECT 0x20
#define BUTTON_A 0x40
#define BUTTON_B 0x80

// these are defined in reset.s

extern uint8_t JoyPad1;
extern uint8_t PrevJoyPad1;

// place on zero page (faster reads)

#pragma zpsym("JoyPad1");
#pragma zpsym("PrevJoyPad1");

#endif