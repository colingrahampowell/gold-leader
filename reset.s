; Startup code for cc65/ca65
; from doug fraker: nesdoug.com

.import _main
.export __STARTUP__:absolute=1
.export _WaitFrame, _UpdateInput
.exportzp _FrameCount, _JoyPad1, _PrevJoyPad1

; Linker generated symbols
.import __STACK_START__, __STACK_SIZE__
.import __OAM_LOAD__
.include "zeropage.inc"

;variables
PPU_CTRL =		$2000
PPU_MASK = 		$2001
PPU_STATUS = 	$2002
OAM_ADDRESS = 	$2003
PPU_DATA = 		$2007
APU_DMC = 		$4010
OAM_DMA = 		$4014
APU_STATUS = 	$4015
INPUT_1 = 		$4016
APU_FRAME_CTR = $4017

.segment "ZEROPAGE"

; frame handling: reserve 1 byte:
_FrameCount: 	.res 1
frame_done:		.res 1

; input handling: reserve 1 byte each:
_JoyPad1:		.res 1
_PrevJoyPad1:	.res 1
tmp:			.res 1		; temp var in button reading routing

; rle decompression: reserve 1 byte each:
RLE_LOW:		.res 1
RLE_HIGH:		.res 1
RLE_TAG:		.res 1
RLE_BYTE:		.res 1

.segment "HEADER"

    .byte $4e,$45,$53,$1a	; 'NES' - start of every .nes file
	.byte 01				; size of PRG ROM x 16 KiB 		
	.byte 01				; size of CHR ROM x 8 KiB
	.byte 00				; horizontal mirroring, mapper 000 
	.byte 00				; mapper 000 (NROM)
	.byte 00				; size of PRG RAM x 8 KiB
	.byte 00				; NTSC
	.byte 00				; unused
	.res 5, $00


.segment "STARTUP"

start:
	sei				; ignore IRQ
	cld				; disable decimal mode

	; disable APU frame IRQs
	ldx #$40
	stx APU_FRAME_CTR

	; setup the stack
	ldx #$ff
	txs

	inx					; x = $00
	stx PPU_CTRL		; disable NMI
	stx PPU_MASK		; disable rendering
	stx APU_DMC			; disable DMC IRQs

	; wait for vblank, ensure that PPU has stabilized
	; clear vblank flag before checking, make sure that 
	; loop sees an actual vblank

	bit PPU_STATUS

@vblank_wait_1:	 
	bit PPU_STATUS
	bpl @vblank_wait_1
	
	; disable music channels
	stx APU_STATUS

	; put zeroes in all CPU RAM
	lda #$00
@blankram:			
	sta $00, x
	sta $0100, x
	sta $0200, x
	sta $0300, x
	sta $0400, x
	sta $0500, x
	sta $0600, x
	sta $0700, x
	inx
	bne @blankram

	; init OAM data to have all y-coords off screen
	; set every 4th byte (y coord) to 'ef'

	lda #$ef
@clear_oam:
	sta __OAM_LOAD__, x
	inx 
	inx 
	inx 
	inx
	bne @clear_oam
	
	; second wait for vblank

@vblank_wait_2:
	bit PPU_STATUS
	bpl @vblank_wait_2

	; init PPU OAM
	stx OAM_ADDRESS	; $00
	lda #>(__OAM_LOAD__)
	sta OAM_DMA

	; set C stack pointer
	lda #<(__STACK_START__+__STACK_SIZE__)
    sta	sp
    lda	#>(__STACK_START__+__STACK_SIZE__)
    sta	sp+1      	

	lda PPU_STATUS
	jmp _main

; 
; Proc: WaitFrame (exported / visible to C)
; Description: Waits for frame to finish: this hangs on a loop 
; until NMI generated by vblank, at which point frame_done is set
; to zero.
;

_WaitFrame:
	inc frame_done
@loop:
	lda frame_done		; load frame_done - set to zero in NMI
	bne @loop			; will branch if frame_done not 0
	rts

; 
; Proc: UpdateInput (external / visible in C)
; Description: checks button state for transition
;

_UpdateInput:
	; store prev input state
	lda _JoyPad1
	sta _PrevJoyPad1
	 
	jsr ReadJoy

; read twice: DMC DMA interference 
@mismatch:
	; load joypad1 state, store in tmp
	lda _JoyPad1
	sta tmp

	; read input again, then load JoyPad1
	; then, compare to prev-read value
	; continue until both are identical

	jsr ReadJoy
	lda _JoyPad1
	cmp tmp

	bne @mismatch

	rts

;
; Proc: ReadJoy (internal)
; Description: Read JoyPad for player 1
;

ReadJoy:
	; strobe input (write 1, then 0 to sync input)
	ldx #$01
	stx INPUT_1
	dex		
	stx INPUT_1

	ldy #$08		; put 8 in y register

loop:
	lda INPUT_1
	and #$01		; ignore other than controller (button info stored in bit 0)

	cmp #$01		; set carry iff nonzero
	rol _JoyPad1	; put carry into bit 0, put bit 7 into carry

	dey				; decrement y
	bne loop		; branch back to start if not finished reading
	
	rts				; return from subroutine


; nmi: non-maskable interrupt

nmi:
	; push registers to stack
	pha 	; push accumulator
	txa		; transfer x to a
	pha		; push x (stored in a)
	tya		; transfer y to a
	pha		; push y (stored in a)

	inc _FrameCount	; increment frame counter	

	; write OAM DMA buffer to OAM, every frame
	lda #$0
	sta OAM_ADDRESS
	lda #>(__OAM_LOAD__)
	sta OAM_DMA

	; free _WaitFrame
	lda #$0
	sta frame_done

	; pop registers stored on stack back into place
	pla	
	tay		; y stored last, popped first
	pla
	tax		; x next
	pla		; finally, pop a

irq:
    rti

.segment "RODATA"

;none yet

.segment "VECTORS"

    .word nmi	;$fffa vblank nmi
    .word start	;$fffc reset
   	.word irq	;$fffe irq / brk

.segment "CHARS"

	.incbin "tiles.chr"
