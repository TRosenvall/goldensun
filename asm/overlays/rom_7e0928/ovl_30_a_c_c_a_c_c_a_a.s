	.include "macros.inc"
	.include "gba.inc"


@ Countdown loop: waits ten frames, then polls a scratch word once per
@ frame until it reaches a target or the attempt limit runs out.
.thumb_func_start OvlFunc_956_20081c8
	push	{r5, lr}
	mov	r0, #0xa
	bl	__WaitFrames
	ldr	r3, =.L5480
	ldr	r3, [r3]
	mov	r5, #0
	b	.L1e8
.L1d8:
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0x77
	bgt	.L1f4
	ldr	r3, =.L5480
	ldr	r3, [r3]
.L1e8:
	cmp	r3, #3
	bne	.L1d8
	ldr	r3, =.L5484
	ldr	r3, [r3]
	cmp	r3, #1
	bne	.L1d8
.L1f4:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_956_20081c8
