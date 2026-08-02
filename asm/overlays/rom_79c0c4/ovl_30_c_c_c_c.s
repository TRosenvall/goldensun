	.include "macros.inc"

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x209, then fixes up slot
@ 0x1B's presentation: the entity's +0x23 is cleared, and byte +0x09 of its
@ actor has bits 0 and 1 cleared before bit 3 is set -- the read-modify-write
@ is spelled `sub r3, #0xd` on a register still holding 0, giving the mask
@ 0xFFFFFFF3. That selects the OAM priority this one object needs.
.thumb_func_start OvlFunc_908_20084c8
	push	{lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	mov	r0, #0x1b
	bl	__MapActor_GetActor
	mov	r2, r0
	add	r2, #0x23
	mov	r3, #0
	strb	r3, [r2]
	ldr	r1, [r0, #0x50]
	ldrb	r2, [r1, #9]
	sub	r3, #0xd
	and	r3, r2
	mov	r2, #8
	orr	r3, r2
	strb	r3, [r1, #9]
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_908_20084c8

	.section .data
	.global .L6b0
	.global gOvl_02008598
	.global MapEntrance_ARRAY_908__02008598
gOvl_02008598:
MapEntrance_ARRAY_908__02008598:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x598, (0x688-0x598)
	.global gOvl_02008688
gOvl_02008688:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x688, (0x6b0-0x688)
.L6b0:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x6b0, (0x8f0-0x6b0)
	.global gOvl_020088f0
gOvl_020088f0:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x8f0
