	.include "macros.inc"

@ Map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204 and sets save bit 0x144.
@
@ Then the arrival fix-up shared by several overlays in this group: if save bit
@ 0x814 is set the player is still riding, so Func_91ff0 starts looping sound
@ 0x8D, .gcc2_compiled. resets the map transition to 0x10000 on all three axes, and
@ StartEarthquake dismounts -- restoring the walking sprite at the dismount tile.
@ Without this the player would arrive still mounted and the room's collision
@ would be wrong for them.
.thumb_func_start OvlFunc_892_2008054
	push	{lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0xa2
	add	r2, #0x44
	lsl	r0, #1
	str	r2, [r3]
	bl	__SetFlag
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
.func_end OvlFunc_892_2008054

	.section .data
	.global gOvl_020080c8
	.global MapEntrance_ARRAY_892__020080c8
gOvl_020080c8:
MapEntrance_ARRAY_892__020080c8:
	.incbin "overlays/rom_78dc80/orig.bin", 0xc8, (0x110-0xc8)
	.global gOvl_02008110
gOvl_02008110:
	.incbin "overlays/rom_78dc80/orig.bin", 0x110, (0x11c-0x110)
	.global gOvl_0200811c
gOvl_0200811c:
	.incbin "overlays/rom_78dc80/orig.bin", 0x11c, (0x134-0x11c)
	.global gOvl_02008134
gOvl_02008134:
	.incbin "overlays/rom_78dc80/orig.bin", 0x134
