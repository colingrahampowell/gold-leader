;
; ppu_background.s
; Un-RLE background decompression
;

.export  _UnRLE
.include "zeropage.inc"

;variables
PPU_DATA = 		$2007


.segment "ZEROPAGE"

; rle decompression: reserve 1 byte each:
RLE_LOW:		.res 1
RLE_HIGH:		.res 1
RLE_TAG:		.res 1
RLE_BYTE:		.res 1

.segment "STARTUP"

; 
; unRLE: decompresses an RLE-compressed background
; by Shiru, 2010
;

_UnRLE:
    tay
    stx <RLE_HIGH
    lda #0
    sta <RLE_LOW

    lda (RLE_LOW),y
    sta <RLE_TAG
    iny
    bne @1
    inc <RLE_HIGH
@1:
    lda (RLE_LOW),y
    iny
    bne @11
    inc <RLE_HIGH
@11:
    cmp <RLE_TAG
    beq @2
    sta PPU_DATA
    sta <RLE_BYTE
    bne @1
@2:
    lda (RLE_LOW),y
    beq @4
    iny
    bne @21
    inc <RLE_HIGH
@21:
    tax
    lda <RLE_BYTE
@3:
    sta PPU_DATA
    dex
    bne @3
    beq @1
@4:
    rts