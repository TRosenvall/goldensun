	.include "macros.inc"
	.include "gba.inc"

@ SetSoundFlag
@ r0.. = parameters. Reads and writes the flag word at ewram_3040. No calls out.
.thumb_func_start Func_f9570
	push	{lr}
	mov	r3, #0x80
	and	r3, r0
	mov	r2, #0x7f
	and	r0, r2
	cmp	r3, #0
	beq	.Lf9588
	ldr	r2, =ewram_3040
	ldrb	r3, [r2]
	eor	r0, r3
	strb	r0, [r2]
	b	.Lf958c
.Lf9588:
	ldr	r3, =ewram_3040
	strb	r0, [r3]
.Lf958c:
	pop	{r0}
	bx	r0
.func_end Func_f9570

@ GetCurrentMusicId
@ Takes no arguments. Returns the byte at ewram_303c -- the id Func_f9080 last
@ accepted as music.
.thumb_func_start Func_f9594
	ldr	r3, =ewram_303c
	ldrb	r0, [r3]
	bx	lr
.func_end Func_f9594

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
