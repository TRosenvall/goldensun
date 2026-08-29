	.include "macros.inc"
	.include "gba.inc"


@ Countdown loop: waits ten frames, then polls a scratch word once per
@ frame until it reaches a target or the attempt limit runs out.
.thumb_func_start OvlFunc_955_2008970
	push	{r5, lr}
	mov	r0, #0xa
	bl	__WaitFrames
	ldr	r3, =.L4834
	ldr	r3, [r3]
	mov	r5, #0
	b	.L994
.L980:
	mov	r0, #1
	bl	__WaitFrames
	mov	r3, #0x96
	add	r5, #1
	lsl	r3, #2
	cmp	r5, r3
	bge	.L9a0
	ldr	r3, =.L4834
	ldr	r3, [r3]
.L994:
	cmp	r3, #0
	bne	.L980
	ldr	r3, =.L4838
	ldr	r3, [r3]
	cmp	r3, #0x4b
	bne	.L980
.L9a0:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_955_2008970
