	.include "macros.inc"
	.include "gba.inc"

@ WaitForJingle
@ Takes no arguments. Spins on Func_30f8(1) until the countdown byte at
@ ewram_3000 reaches zero, so the caller can block until a fanfare has finished.
.thumb_func_start Func_f95a0
	push	{r5, r6, lr}
	ldr	r6, =ewram_3000
	mov	r5, #0
.Lf95a6:
	ldrb	r3, [r6]
	cmp	r3, #0
	beq	.Lf95ba
	mov	r0, #1
	bl	Func_30f8
	ldr	r3, =0x12b
	add	r5, #1
	cmp	r5, r3
	ble	.Lf95a6
.Lf95ba:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_f95a0

@ ReadSoundState
@ r0.. = parameters. A small accessor over the state block; no calls out.
.thumb_func_start Func_f95c8
	push	{lr}
	cmp	r0, #0x46
	beq	.Lf95d6
	cmp	r0, #0x4b
	beq	.Lf95d6
	cmp	r0, #0x43
	bne	.Lf95da
.Lf95d6:
	mov	r0, #3
	b	.Lf95dc
.Lf95da:
	mov	r0, #2
.Lf95dc:
	pop	{r1}
	bx	r1
.func_end Func_f95c8

	.section .rodata

@ PROMOTED: referenced from rom_f9080.s across the split.
	.global	Lfb794
Lfb794:
.Lfb794:
	.incrom 0xfb794, 0xfb7a0
