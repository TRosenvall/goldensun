	.include "macros.inc"


@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x209, then stages by
@ entrance id:
@   entrance 5        a 0x43x4 metatile copy from (0, 0x78), and slot 8 has its
@                     flag byte +0x55 cleared and both height words +0x0C and
@                     +0x14 zeroed -- dropping it to ground level.
@   entrances 7, 0x0B spawn object 0xE7 at (0x2380000, 0x100000, 0x2A00000)
@                     through OvlFunc_570, and register OvlFunc_30 as a task at
@                     priority 0xC80 so the player's position is watched.
@ Every other entrance needs nothing.
.thumb_func_start OvlFunc_902_20084e4
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
	sub	sp, #8
	cmp	r3, #5
	bne	.L536
	mov	r3, #4
	mov	r2, #3
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x78
	mov	r2, #8
	mov	r3, #0x43
	mov	r0, #0
	bl	__CopyMapTiles
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, #0
	add	r0, #0x55
	strb	r5, [r0]
	mov	r0, #8
	bl	__MapActor_GetActor
	str	r5, [r0, #0xc]
	mov	r0, #8
	bl	__MapActor_GetActor
	str	r5, [r0, #0x14]
	b	.L55a
.L536:
	cmp	r3, #7
	beq	.L53e
	cmp	r3, #0xb
	bne	.L55a
.L53e:
	mov	r1, #0x8e
	mov	r2, #0x80
	mov	r3, #0xa8
	lsl	r1, #18
	mov	r0, #0xe7
	lsl	r2, #13
	lsl	r3, #18
	bl	OvlFunc_902_2008570
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_902_2008030
	lsl	r1, #4
	bl	__StartTask
.L55a:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_902_20084e4
