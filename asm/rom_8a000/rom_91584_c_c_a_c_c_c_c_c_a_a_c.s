	.include "macros.inc"

@ PlaceSlotAt
@ r0=slot, r1=x (16.16), r2=z (16.16). Teleports the entity: installs the
@ default behaviour, zeroes all three velocity words, clears the x and y targets
@ to the 0x80000000 sentinel and writes the position directly.
@ When terrain-follow is enabled (bit 0 of +0x55) it also resamples the ground
@ height at the destination with _Func_11f54, using the entity's tile-type byte
@ at +0x22, and shifts y by the difference from the cached height at +0x14 so
@ the entity lands on the surface rather than at the old altitude.
.thumb_func_start MapActor_SetPos  @ 0x080923e4
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	GetFieldActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L9244a
	bl	_Actor_Stop
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x28]
	str	r3, [r5, #0x2c]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x38]
	str	r6, [r5, #8]
	str	r7, [r5, #0x10]
	mov	r3, r5
	add	r3, #0x55
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L9244a
	mov	r3, r5
	add	r3, #0x22
	mov	r1, r6
	ldrb	r0, [r3]
	cmp	r1, #0
	bge	.L9242a
	ldr	r3, =0xffff
	add	r1, r3
.L9242a:
	mov	r2, r7
	asr	r1, #16
	cmp	r2, #0
	bge	.L92436
	ldr	r3, =0xffff
	add	r2, r3
.L92436:
	asr	r2, #16
	bl	_Func_8011f54
	ldr	r3, [r5, #0xc]
	ldr	r2, [r5, #0x14]
	lsl	r0, #16
	sub	r3, r2
	add	r3, r0
	str	r3, [r5, #0xc]
	str	r0, [r5, #0x14]
.L9244a:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end MapActor_SetPos

@ PlaceSlotAt3D
@ r0=slot, r1=x, r2=y, r3=z (all 16.16). The three-axis form of MapActor_SetPos:
@ installs the default behaviour, zeroes the velocity, clears the x and y
@ targets and writes all three position words. When terrain-follow is enabled
@ (bit 0 of +0x55) the y written by the caller is then corrected against the
@ ground height sampled by _Func_11f54 at the destination.
.thumb_func_start MapActor_SetPos3D  @ 0x08092454
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r1
	mov	r6, r2
	mov	r8, r3
	bl	GetFieldActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L924c4
	bl	_Actor_Stop
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x28]
	str	r3, [r5, #0x2c]
	mov	r3, #0x80
	lsl	r3, #24
	mov	r2, r8
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x38]
	str	r7, [r5, #8]
	str	r6, [r5, #0xc]
	str	r2, [r5, #0x10]
	mov	r3, r5
	add	r3, #0x55
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L924c4
	mov	r3, r5
	add	r3, #0x22
	mov	r1, r7
	ldrb	r0, [r3]
	cmp	r1, #0
	bge	.L924a4
	ldr	r3, =0xffff
	add	r1, r3
.L924a4:
	mov	r3, r8
	asr	r1, #16
	cmp	r3, #0
	bge	.L924b0
	ldr	r2, =0xffff
	add	r3, r2
.L924b0:
	asr	r2, r3, #16
	bl	_Func_8011f54
	ldr	r3, [r5, #0xc]
	ldr	r2, [r5, #0x14]
	lsl	r0, #16
	sub	r3, r2
	add	r3, r0
	str	r3, [r5, #0xc]
	str	r0, [r5, #0x14]
.L924c4:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end MapActor_SetPos3D

