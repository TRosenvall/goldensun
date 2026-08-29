	.include "macros.inc"

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x209, then does per-
@ entrance staging keyed on ewram_240+0x1C2:
@   entrances 4 and 7  one OvlFunc_common0_70 prop at x 0xF80000, z 0x1A10000,
@   entrance 6         two props, at x 0x1CC0000 and 0x1E40000, both z 0x2380000,
@   entrance 8         clears save bit 0x12F and puts slot 0x0A in animation 6.
@ The props are background scenery the object table cannot place, spawned
@ through the shared helper in overlays/common/.
.thumb_func_start OvlFunc_929_2008598
	push	{r5, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	ldr	r3, =gState
	sub	r2, #0x47
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #4
	beq	.L5ba
	cmp	r3, #7
	bne	.L5c4
.L5ba:
	mov	r0, #0xf8
	lsl	r0, #16
	ldr	r2, =0x1a10000
	mov	r1, #0
	b	.L5e2
.L5c4:
	cmp	r3, #6
	bne	.L5ea
	mov	r5, #0x8e
	lsl	r5, #18
	mov	r0, #0xe6
	mov	r1, #0
	mov	r2, r5
	mov	r3, #0x14
	lsl	r0, #17
	bl	OvlFunc_common0_70
	mov	r0, #0xf2
	lsl	r0, #17
	mov	r1, #0
	mov	r2, r5
.L5e2:
	mov	r3, #0x14
	bl	OvlFunc_common0_70
	b	.L5fc
.L5ea:
	cmp	r3, #8
	bne	.L5fc
	ldr	r0, =0x12f
	bl	__ClearFlag
	mov	r0, #0xa
	mov	r1, #6
	bl	__MapActor_SetAnim
.L5fc:
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_929_2008598

	.section .data
	.global .La28
	.global .Ld4c
	.global .L890
	.global .L9c8
	.global gOvl_02008778

gOvl_02008778:
	.incbin "overlays/rom_7b7790/orig.bin", 0x778, (0x868-0x778)
	.global gOvl_02008868
gOvl_02008868:
	.incbin "overlays/rom_7b7790/orig.bin", 0x868, (0x890-0x868)
.L890:
	.incbin "overlays/rom_7b7790/orig.bin", 0x890, (0x9c8-0x890)
.L9c8:
	.incbin "overlays/rom_7b7790/orig.bin", 0x9c8, (0xa28-0x9c8)
.La28:
	.incbin "overlays/rom_7b7790/orig.bin", 0xa28, (0xd4c-0xa28)
.Ld4c:
	.incbin "overlays/rom_7b7790/orig.bin", 0xd4c
