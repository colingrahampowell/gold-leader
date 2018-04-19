/*
 * reset.h: 
 * Adapted from: http://timcheeseman.com/nesdev/
 */

#ifndef reset_h
#define reset_h

extern unsigned char FrameCount;    // we're identifying FrameCount as unsigned int 
#pragma zpsym ("FrameCount");       // identify FrameCount as zeropage symbol 

void WaitFrame(void);                   // implemented in reset.s
void UpdateInput(void);

#endif

