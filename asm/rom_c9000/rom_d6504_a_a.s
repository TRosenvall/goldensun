	.include "macros.inc"
	.include "gba.inc"

@ StepScreenShake
@ Takes no arguments. Advances the battle screen shake one frame using the
@ amount at [iwram_1eec]+0x77AC and the mode at +0x77B0, adding the offset into
@ the view's y at [iwram_1e80]+0x36.
@ Mode 1 applies the full amount once and then clears the mode -- a single
@ jolt. Any other mode applies half the amount and alternates between 2 and 0,
@ giving the oscillating shake.
.thumb_func_start Task_SpinCamera  @ 0x080d6504
	push	{lr}
	ldr	r3, =iwram_3001eec
	ldr	r2, [r3]
	sub	r3, #0x6c
	ldr	r1, [r3]
	ldr	r3, =0x77b0
	add	r0, r2, r3
	ldr	r3, [r0]
	cmp	r3, #1
	bne	.Ld652a
	ldr	r4, =0x77ac
	add	r3, r2, r4
	ldr	r2, [r3]
	ldrh	r3, [r1, #0x36]
	add	r3, r2
	mov	r2, #0
	strh	r3, [r1, #0x36]
	str	r2, [r0]
	b	.Ld654a
.Ld652a:
	ldr	r4, =0x77ac
	add	r3, r2, r4
	ldr	r2, [r3]
	lsr	r3, r2, #31
	add	r2, r3
	ldrh	r3, [r1, #0x36]
	asr	r2, #1
	add	r3, r2
	strh	r3, [r1, #0x36]
	ldr	r3, [r0]
	cmp	r3, #2
	bne	.Ld6546
	mov	r3, #0
	b	.Ld6548
.Ld6546:
	mov	r3, #2
.Ld6548:
	str	r3, [r0]
.Ld654a:
	pop	{r0}
	bx	r0
.func_end Task_SpinCamera

