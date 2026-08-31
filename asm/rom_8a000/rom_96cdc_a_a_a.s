	.include "macros.inc"
	.include "gba.inc"

@ SearchFieldTargets
@ r0=origin, r1=range, r2=kind. Scans the candidate table at ewram_48A for
@ objects the field ability can act on, returning the best match. Used to decide
@ what a cast will affect before any animation plays.
.thumb_func_start Func_8096cdc  @ 0x08096cdc
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r7, r1
	ldr	r1, =ewram_200048a
	mov	r8, r0
	mov	r6, r2
	mov	r5, #0
	mov	r10, r1
.L96cf0:
	mov	r0, r5
	bl	GetFieldActor
	mov	r1, r10
	mov	r2, #0
	ldrsh	r3, [r1, r2]
	cmp	r5, r3
	beq	.L96d14
	cmp	r0, #0
	beq	.L96d14
	cmp	r0, r8
	beq	.L96d14
	mov	r3, r0
	add	r3, #0x5b
	strb	r7, [r3]
	mov	r1, r6
	bl	_Actor_SetAnimSpeed
.L96d14:
	add	r5, #1
	cmp	r5, #0x42
	ble	.L96cf0
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8096cdc
