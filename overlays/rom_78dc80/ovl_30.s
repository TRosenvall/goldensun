	.include "macros.inc"

@ ============================================================================
@ Overlay 0x78dc80 -- one of the corpus's minimal overlays: four constant table
@ slots, an empty interaction slot, and a short map-load entry.
@
@ Slot 0  OvlFunc_54  map-load entry
@ Slot 1  OvlFunc_30  edge transitions   -> .Lc8
@ Slot 2  OvlFunc_3c  map event list     -> .L110
@ Slot 3  OvlFunc_44  read after slot 4  -> .L11c
@ Slot 4  OvlFunc_4c  map objects        -> .L134
@ Slot 5  OvlFunc_38  interactions       -> none (returns 0)
@
@ Overlays this small have no scripted content at all: the room is entirely
@ described by its four tables, and the only code is the arrival fix-up below.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.Lc8
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L110
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: read after slot 4.
.thumb_func_start OvlFunc_44
	ldr	r0, =.L11c
	bx	lr
.func_end OvlFunc_44

@ Slot 4: map object table.
.thumb_func_start OvlFunc_4c
	ldr	r0, =.L134
	bx	lr
.func_end OvlFunc_4c

@ Map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204 and sets save bit 0x144.
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
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0xa2
	add	r2, #0x44
	lsl	r0, #1
	str	r2, [r3]
	bl	__Func_79358
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

.Lc8:
	.incbin "overlays/rom_78dc80/orig.bin", 0xc8, (0x110-0xc8)
.L110:
	.incbin "overlays/rom_78dc80/orig.bin", 0x110, (0x11c-0x110)
.L11c:
	.incbin "overlays/rom_78dc80/orig.bin", 0x11c, (0x134-0x11c)
.L134:
	.incbin "overlays/rom_78dc80/orig.bin", 0x134
