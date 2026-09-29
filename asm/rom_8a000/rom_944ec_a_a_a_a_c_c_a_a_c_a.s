	.include "macros.inc"
	.include "gba.inc"

@ UpdateWaterSurface
@ Takes no arguments. Advances the animated water surface in the state at
@ iwram_1ec8, stepping its phase and rewriting the affected tiles.
.thumb_func_start Task_Thunder  @ 0x080949a8
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ec8
	mov	r1, #0xfc
	ldr	r6, [r3]
	lsl	r1, #5
	add	r5, r6, r1
	ldr	r7, [r3, #8]
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	blt	.L94aa2
	mov	r0, #0xb3
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	beq	.L949ce
	mov	r3, #0x80
	strh	r3, [r5]
.L949ce:
	ldrh	r3, [r5]
	sub	r2, r3, #1
	lsl	r3, #16
	asr	r3, #16
	strh	r2, [r5]
	cmp	r3, #0xb
	bhi	.L94aa2
	ldr	r2, =.L949e4
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L949e4:
	.word	.L94a14
	.word	.L94a86
	.word	.L94aa2
	.word	.L94aa2
	.word	.L94aa2
	.word	.L94a62
	.word	.L94a86
	.word	.L94aa2
	.word	.L94aa2
	.word	.L94aa2
	.word	.L94a62
	.word	.L94a86
.L94a14:
	ldr	r1, =0x1f82
	add	r3, r6, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	beq	.L94a62
	bl	Random
	mov	r5, r0
	bl	Random
	mov	r2, #0x64
	mul	r2, r0
	lsl	r3, r5, #1
	add	r3, r5
	lsl	r3, #3
	add	r3, r5
	lsl	r3, #4
	lsr	r2, #16
	lsr	r3, #16
	mov	r1, #0xfc
	sub	r3, r2
	lsl	r1, #5
	add	r2, r6, r1
	add	r3, #0x96
	strh	r3, [r2]
	ldr	r2, =0x1f84
	add	r3, r6, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #0
	beq	.L94a5c
	mov	r0, #0xac
	bl	_PlaySound
	b	.L94a62
.L94a5c:
	mov	r0, #0xab
	bl	_PlaySound
.L94a62:
	mov	r0, r6
	mov	r1, #1
	bl	Func_8091200
	mov	r2, #0xa8
	lsl	r2, #5
	add	r0, r6, r2
	mov	r2, #0xc4
	lsl	r2, #5
	add	r1, r7, r2
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x840002a0
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =0x2a01
	add	r2, r7, r3
	mov	r3, #0xc
	b	.L94a98
.L94a86:
	mov	r3, #0xa8
	lsl	r3, #4
	add	r0, r6, r3
	mov	r1, #1
	bl	Func_8091200
	ldr	r1, =0x2a01
	mov	r3, #1
	add	r2, r7, r1
.L94a98:
	strb	r3, [r2]
	ldr	r2, =0x2a02
	mov	r1, #0
	add	r3, r7, r2
	strb	r1, [r3]
.L94aa2:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Task_Thunder

@ InitRideState
@ Takes no arguments. Allocates the ride state with galloc_ewram(0x1D, 0x410),
@ clears it and seeds the defaults for entering a vehicle mode.
.thumb_func_start StartRain  @ 0x08094ac8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r1, #0x82
	lsl	r1, #3
	mov	r0, #0x1d
	sub	sp, #8
	bl	galloc_ewram
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	ldr	r3, [r3]
	mov	r5, r0
	mov	r0, #0xaa
	mov	r8, r3
	bl	Func_8091ff0
	mov	r6, #0
	mov	r7, r5
	add	r0, sp, #4
	add	r7, #8
	str	r6, [r0]
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	ldr	r2, =0x85000104
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r1, #0x80
	lsl	r1, #3
	mov	r0, #0xe
	bl	galloc_ewram
	mov	r6, r0
	mov	r1, r6
	ldr	r0, =Data_9ff58
	bl	DecompressLZ1
	bl	AllocSpriteSlot
	mov	r1, #0xc0
	str	r0, [r5]
	lsl	r1, #2
	mov	r2, r6
	bl	UploadSpriteGFX
	str	r0, [r5, #4]
	mov	r0, #0xe
	bl	gfree
	mov	r5, #0
.L94b2c:
	mov	r4, #0
	mov	r6, r7
	stmia	r6!, {r4}
	ldr	r3, =0x40000400
	stmia	r6!, {r3}
	mov	r3, #0xd4
	lsl	r3, #8
	str	r3, [r6]
	mov	r3, r8
	ldr	r1, [r3]
	ldr	r2, [r3, #8]
	mov	r0, #0
	str	r1, [r7, #0xc]
	str	r2, [r7, #0x14]
	asr	r1, #16
	asr	r2, #16
	str	r4, [sp]
	bl	_Func_8011f54
	ldr	r2, .L94b8c	@ 0xf
	mov	r3, r5
	and	r3, r2
	lsl	r0, #16
	add	r3, #1
	add	r5, #1
	str	r0, [r7, #0x10]
	strh	r3, [r7, #0x1c]
	ldr	r4, [sp]
	add	r7, #0x20
	cmp	r5, #0x1f
	bls	.L94b2c
	ldr	r3, =REG_BLDCNT
	mov	r2, #0xfc
	lsl	r2, #6
	strh	r2, [r3]
	ldr	r2, =0x1008
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r4, [r3]
	ldr	r0, =Task_Rain
	mov	r1, #0xc8
	lsl	r1, #4
	bl	StartTask
	add	sp, #8
	b	.L94bb0

	.align	2, 0
.L94b8c:
	.word	0xf
	.pool

.L94bb0:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end StartRain
