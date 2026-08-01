	.include "macros.inc"

@ ============================================================================
@ Overlay 0x7fc618 -- a minimal overlay whose only code is four SCENE-FLAG
@ SETTERS reached from its tables, not from the export slots.
@
@ Slot 0  OvlFunc_b4  map-load entry -- does nothing but return 0
@ Slot 1  OvlFunc_30  edge transitions   -> .L1a8
@ Slot 2  OvlFunc_3c  map event list     -> .L1d8
@ Slot 3  OvlFunc_44  read after slot 4  -> .L1ec
@ Slot 4  OvlFunc_ac  map objects        -> .L264
@ Slot 5  OvlFunc_38  interactions       -> none (returns 0)
@
@ OvlFunc_4c, _64, _7c and _94 are NOT exported. They are reached through one
@ of the tables above, one per trigger, and form a regular series:
@
@     handler        Func_955b0(scene, index)   clears save bit
@     OvlFunc_4c     8, 0                       0x30
@     OvlFunc_64     9, 1                       0x44
@     OvlFunc_7c     10, 2                      0x58
@     OvlFunc_94     11, 3                      0x6C
@
@ Scene ids run 8..11 against indices 0..3, and the save bits step by 0x14.
@ Each sets the scene flag through Func_955b0 and clears the bit that would
@ re-arm the trigger, so stepping on one advances the room by exactly one state
@ and cannot be repeated.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.L1a8
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L1d8
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: read after slot 4.
.thumb_func_start OvlFunc_44
	ldr	r0, =.L1ec
	bx	lr
.func_end OvlFunc_44

@ Trigger 0: scene 8, index 0. Clears save bit 0x30. See the header.
.thumb_func_start OvlFunc_4c
	push	{lr}
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	bl	__Func_955b0
	mov	r0, #0x30
	bl	__Func_79374
	pop	{r0}
	bx	r0
.func_end OvlFunc_4c

@ Trigger 1: scene 9, index 1. Clears save bit 0x44.
.thumb_func_start OvlFunc_64
	push	{lr}
	mov	r0, #9
	mov	r1, #1
	mov	r2, #0
	bl	__Func_955b0
	mov	r0, #0x44
	bl	__Func_79374
	pop	{r0}
	bx	r0
.func_end OvlFunc_64

@ Trigger 2: scene 10, index 2. Clears save bit 0x58.
.thumb_func_start OvlFunc_7c
	push	{lr}
	mov	r0, #0xa
	mov	r1, #2
	mov	r2, #0
	bl	__Func_955b0
	mov	r0, #0x58
	bl	__Func_79374
	pop	{r0}
	bx	r0
.func_end OvlFunc_7c

@ Trigger 3: scene 11, index 3. Clears save bit 0x6C.
.thumb_func_start OvlFunc_94
	push	{lr}
	mov	r0, #0xb
	mov	r1, #3
	mov	r2, #0
	bl	__Func_955b0
	mov	r0, #0x6c
	bl	__Func_79374
	pop	{r0}
	bx	r0
.func_end OvlFunc_94

@ Slot 4: map object table.
.thumb_func_start OvlFunc_ac
	ldr	r0, =.L264
	bx	lr
.func_end OvlFunc_ac

@ Slot 0: map-load entry. This room needs no setup.
.thumb_func_start OvlFunc_b4
	mov	r0, #0
	bx	lr
.func_end OvlFunc_b4

	.section .data

	.incbin "overlays/rom_7fc618/orig.bin", 0xc8, (0x1a8-0xc8)
.L1a8:
	.incbin "overlays/rom_7fc618/orig.bin", 0x1a8, (0x1d8-0x1a8)
.L1d8:
	.incbin "overlays/rom_7fc618/orig.bin", 0x1d8, (0x1ec-0x1d8)
.L1ec:
	.incbin "overlays/rom_7fc618/orig.bin", 0x1ec, (0x264-0x1ec)
.L264:
	.incbin "overlays/rom_7fc618/orig.bin", 0x264
