	.include "macros.inc"

@ Map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204 and the message delay at +0x1C8 to 0x10.
@
@ Then the arrival fix-up shared by several overlays in this group: if save bit
@ 0x814 is set the player is still riding, so Func_91ff0 starts looping sound
@ 0x8D, .gcc2_compiled. resets the map transition to 0x10000 on all three axes, and
@ StartEarthquake dismounts -- restoring the walking sprite at the dismount tile.
@ Without this the player would arrive still mounted and the room's collision
@ would be wrong for them.
.thumb_func_start OvlFunc_893_2008054
	push	{lr}
	ldr	r3, =iwram_3001ebc
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
	bl	__GetFlag
	cmp	r0, #0
	beq	.L90
	mov	r0, #0x8d
	bl	__Func_8091ff0
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #9
	lsl	r2, #9
	bl	__Func_8012330
	bl	__StartEarthquake
.L90:
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_893_2008054

	.section .data
	.global gOvl_020080c0
	.global MapEntrance_ARRAY_893__020080c0
	.global MapEntrance_ARRAY_894__020080c0
gOvl_020080c0:
MapEntrance_ARRAY_893__020080c0:
MapEntrance_ARRAY_894__020080c0:
	.incbin "overlays/rom_78dd40/orig.bin", 0xc0, (0x120-0xc0)
	.global gOvl_02008120
gOvl_02008120:
	.incbin "overlays/rom_78dd40/orig.bin", 0x120, (0x130-0x120)
	.global gOvl_02008130
gOvl_02008130:
	.incbin "overlays/rom_78dd40/orig.bin", 0x130, (0x148-0x130)
	.global gOvl_02008148
gOvl_02008148:
	.incbin "overlays/rom_78dd40/orig.bin", 0x148
