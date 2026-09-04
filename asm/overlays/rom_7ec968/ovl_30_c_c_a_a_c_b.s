	.include "macros.inc"

@ CrossThreshold
@ Takes no arguments. Clears the player's flag byte +0x55, plays sound 0x9E, and
@ animates a doorway with two CopyMapTiles metatile copies four frames apart --
@ source rows 0x42 then 0x44 into the 8x2 block at (0x24, 0x47). Then
@ Func_92208 nudges the player by (3, -0x10) and the interaction target
@ halfword at [iwram_1ebc]+0x16C is left pending as the next message.
.thumb_func_start OvlFunc_963_2008288
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xb6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0
	sub	sp, #8
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0x9e
	bl	__PlaySound
	mov	r5, #2
	mov	r1, #0x24
	mov	r2, #0x47
	mov	r3, #8
	mov	r0, #0x42
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #4
	bl	__WaitFrames
	mov	r3, #8
	mov	r1, #0x24
	mov	r2, #0x47
	mov	r0, #0x44
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #4
	bl	__WaitFrames
	mov	r2, #0x10
	mov	r1, #3
	neg	r2, r2
	mov	r0, #0
	bl	__Func_8092208
	mov	r0, r6
	bl	__Func_8091e9c
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_963_2008288
