	.include "macros.inc"

@ ============================================================================
@ Overlay 0x78de18 -- one of the corpus's minimal overlays: four constant table
@ slots, an empty interaction slot, and a short map-load entry.
@
@ Slot 0  OvlFunc_54  map-load entry
@ Slot 1  OvlFunc_30  edge transitions   -> .Lc0
@ Slot 2  OvlFunc_3c  map event list     -> .L120
@ Slot 3  OvlFunc_44  read after slot 4  -> .L130
@ Slot 4  OvlFunc_4c  map objects        -> .L148
@ Slot 5  OvlFunc_38  interactions       -> none (returns 0)
@
@ Overlays this small have no scripted content at all: the room is entirely
@ described by its four tables, and the only code is the arrival fix-up below.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.Lc0
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L120
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: read after slot 4.
.thumb_func_start OvlFunc_44
	ldr	r0, =.L130
	bx	lr
.func_end OvlFunc_44

@ Slot 4: map object table.
.thumb_func_start OvlFunc_4c
	ldr	r0, =.L148
	bx	lr
.func_end OvlFunc_4c

@ Map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204 and the message delay at +0x1C8 to 0x10.
@
@ Then the arrival fix-up shared by several overlays in this group: if save bit
@ 0x814 is set the player is still riding, so Func_91ff0 starts looping sound
@ 0x8D, Func_12330 resets the map transition to 0x10000 on all three axes, and
@ Func_9509c dismounts -- restoring the walking sprite at the dismount tile.
@ Without this the player would arrive still mounted and the room's collision
@ would be wrong for them.

.thumb_func_start OvlFunc_54
	push	{lr}
	ldr	r3, =iwram_1ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	add	r3, #0x44
	str	r3, [r2]
	sub	r3, #0x3c
	add	r2, r1, r3
	mov	r3, #0x10
	str	r3, [r2]
	ldr	r0, =0x814
	bl	__Func_79338
	cmp	r0, #0
	beq	.L90
	mov	r0, #0x8d
	bl	__Func_91ff0
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #9
	lsl	r2, #9
	bl	__Func_12330
	bl	__Func_9509c
.L90:
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_54

	.section .data

.Lc0:
	.incbin "overlays/rom_78de18/orig.bin", 0xc0, (0x120-0xc0)
.L120:
	.incbin "overlays/rom_78de18/orig.bin", 0x120, (0x130-0x120)
.L130:
	.incbin "overlays/rom_78de18/orig.bin", 0x130, (0x148-0x130)
.L148:
	.incbin "overlays/rom_78de18/orig.bin", 0x148
