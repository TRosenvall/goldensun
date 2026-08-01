	.include "macros.inc"
	.include "gba.inc"

@ LoadAndDecompressBattleGraphic
@ r0, r1 = parameters, r2 = asset id. Fetches the asset with Func_2f40, then
@ ALLOCATES 0x230 BYTES UNDER TAG 0x31 AND DMA3-COPIES Func_b5138 INTO IT,
@ calling the decompressor there rather than in place -- which is why
@ Func_b5138 relocates its own jump table. Func_2dd8 frees the scratch after.
@ Exported; rom_c9000's class handlers use it to load their graphics.
.thumb_func_start Func_c08ec
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r2
	ldr	r2, =iwram_1f00
	ldr	r2, [r2]
	mov	r9, r0
	mov	r0, r1
	mov	r10, r2
	bl	Func_2f40
	ldr	r3, =iwram_1f00
	sub	r3, #0x8c
	ldr	r6, [r3]
	mov	r8, r0
	ldr	r5, =0x230
	mov	r0, #0x31
	mov	r1, r5
	bl	Func_48b0
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_b5138
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, #0x80
	ldr	r2, =iwram_1f00
	lsl	r0, #1
	ldr	r3, [r2, #0x14]
	ldr	r1, =0x6008000
	add	r0, r8
	bl	_call_via_r3
	mov	r0, #0x31
	bl	Func_2dd8
	ldr	r3, =0x544
	add	r4, r6, r3
	mov	r0, r8
	ldr	r3, =REG_DMA3SAD
	mov	r1, r4
	ldr	r2, =0x84000040
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	cmp	r7, #0
	blt	.Lc0974
	lsl	r3, r7, #4
	ldr	r2, =0x644
	add	r3, r7
	lsl	r3, #4
	add	r0, r6, r2
	add	r3, r7
	mov	r2, #0x80
	lsl	r3, #2
	lsl	r2, #9
	sub	r2, r3
	str	r2, [r0]
	ldr	r1, =0x50000c0
	mov	r0, r4
	mov	r3, #0x80
	bl	Func_c1724
.Lc0974:
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x5000200
	ldr	r1, =0x50000a0
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =0x50001e8
	ldr	r2, =0x50000bc
	ldrh	r3, [r3]
	ldr	r0, =0x6003800
	strh	r3, [r2]
	bl	Func_c0098
	ldr	r0, =0x600f800
	bl	Func_c00d8
	ldr	r3, =Func_8d4
	ldr	r0, =0x600ffc0
	mov	r1, #0x40
	bl	_call_via_r3
	mov	r2, r10
	ldr	r3, [r2, #8]
	cmp	r3, #0
	bne	.Lc09ae
	ldr	r0, =Func_c0130
	ldr	r1, =0x4ff
	bl	Func_41d8
.Lc09ae:
	mov	r3, r9
	mov	r2, r10
	str	r3, [r2, #8]
	cmp	r3, #1
	bne	.Lc09be
	ldr	r2, =REG_BG1CNT
	ldr	r3, .Lc09cc	@ 0x1f83
	strh	r3, [r2]
.Lc09be:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0

	.align	2, 0
.Lc09cc:
	.word	0x1f83
.func_end Func_c08ec

@ UploadViewMatrix
@ r0.. = parameters. Pushes the battle view matrix to the affine registers,
@ gated on a save bit through _Func_79338. 219 lines; traced structurally.
.thumb_func_start Func_c0a24
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x10
	str	r3, [sp, #4]
	ldr	r3, =iwram_1f00
	str	r2, [sp, #8]
	str	r1, [sp, #0xc]
	mov	r2, r3
	sub	r2, #0x88
	ldr	r2, [r2]
	mov	r10, r0
	mov	r0, #0x80
	ldr	r1, [r3]
	lsl	r0, #4
	mov	r9, r2
	sub	r3, #0x80
	mov	r2, #0
	ldr	r3, [r3]
	mov	r11, r0
	str	r2, [sp]
	ldr	r0, [sp, #0x30]
	mov	r2, #0x80
	lsl	r2, #9
	cmp	r0, r2
	blt	.Lc0a78
	mov	r0, #0x80
	lsl	r0, #6
	str	r0, [sp]
	mov	r0, #0x36
	ldrsh	r2, [r3, r0]
	neg	r2, r2
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0xd0
	lsl	r2, #7
	add	r2, r3
	mov	r11, r2
.Lc0a78:
	mov	r3, r9
	cmp	r3, #0
	bne	.Lc0a80
	b	.Lc0bd2
.Lc0a80:
	ldr	r0, [r1, #8]
	cmp	r0, #1
	beq	.Lc0a8c
	ldr	r3, [r1, #0xc]
	cmp	r3, #1
	bne	.Lc0a9a
.Lc0a8c:
	ldr	r3, [r1, #0x10]
	cmp	r3, #0
	bne	.Lc0a9a
	ldr	r2, =iwram_1ad0
	mov	r1, r11
	asr	r3, r1, #8
	strh	r3, [r2, #4]
.Lc0a9a:
	cmp	r0, #2
	beq	.Lc0aa0
	b	.Lc0bd2
.Lc0aa0:
	mov	r3, r9
	ldr	r2, [r3]
	mov	r3, #1
	eor	r2, r3
	lsl	r3, r2, #2
	add	r3, r2
	lsl	r3, #6
	mov	r1, #0x80
	add	r3, r9
	ldr	r0, [sp, #0x30]
	lsl	r1, #9
	ldr	r2, =Func_8ac
	mov	r7, r3
	bl	_call_via_r2
	mov	r6, r9
	add	r6, #0x10
	asr	r5, r0, #8
	mov	r3, #0
	strh	r5, [r6]
	strh	r3, [r6, #2]
	strh	r3, [r6, #4]
	strh	r5, [r6, #6]
	mov	r14, r0
	ldr	r3, [sp, #0x30]
	ldr	r0, =0xffff0000
	add	r3, r0
	mov	r8, r3
	add	r7, #0x20
	ldr	r4, =Func_888
	mov	r0, r10
	mov	r1, r8
	.call_via r4
	mov	r1, r0
	mov	r0, r14
	.call_via r4
	mov	r3, r0
	mov	r1, r8
	ldr	r0, [sp, #0xc]
	.call_via r4
	mov	r1, r0
	mov	r0, r14
	.call_via r4
	ldr	r2, =0x7fff
	ldr	r1, [sp, #8]
	add	r3, r2
	asr	r3, #8
	add	r3, r1
	add	r3, r11
	str	r3, [r6, #8]
	add	r0, r2
	ldr	r2, [sp, #4]
	ldr	r3, =0xfffff000
	asr	r0, #8
	add	r0, r2
	lsl	r5, #16
	mov	r1, #0x80
	add	r0, r3
	asr	r5, #16
	lsl	r1, #7
	str	r0, [r6, #0xc]
	sub	r1, r0
	ldr	r2, =Func_8ac
	mov	r0, r5
	bl	_call_via_r2
	asr	r0, #16
	add	r6, r0, #1
	ldr	r0, =0x16b
	mov	r5, #0
	bl	_Func_79338
	cmp	r0, #0
	bne	.Lc0b4a
	ldr	r3, .Lc0b6c	@ 0x3f8e
.Lc0b40:
	add	r5, #1
	strh	r3, [r7]
	add	r7, #2
	cmp	r5, #0xf
	bls	.Lc0b40
.Lc0b4a:
	cmp	r6, #0x88
	bls	.Lc0b50
	mov	r6, #0x88
.Lc0b50:
	cmp	r5, r6
	bcs	.Lc0b94
	ldr	r3, [sp]
	lsl	r2, r3, #16
	ldr	r3, .Lc0b70	@ 0x478a
	lsr	r2, #16
	orr	r2, r3
.Lc0b5e:
	add	r5, #1
	strh	r2, [r7]
	add	r7, #2
	cmp	r5, r6
	bcc	.Lc0b5e
	b	.Lc0b94

	.align	2, 0
.Lc0b6c:
	.word	0x3f8e
.Lc0b70:
	.word	0x478a
	.pool

.Lc0b94:
	cmp	r5, #0x87
	bhi	.Lc0bac
	ldr	r0, [sp]
	ldr	r3, =0x478e
	lsl	r2, r0, #16
	lsr	r2, #16
	orr	r2, r3
.Lc0ba2:
	add	r5, #1
	strh	r2, [r7]
	add	r7, #2
	cmp	r5, #0x87
	bls	.Lc0ba2
.Lc0bac:
	cmp	r5, #0x9f
	bhi	.Lc0bc8
	ldr	r3, =0x3f8e
.Lc0bb2:
	add	r5, #1
	strh	r3, [r7]
	add	r7, #2
	cmp	r5, #0x9f
	bls	.Lc0bb2
	b	.Lc0bc8

	.pool_aligned

.Lc0bc8:
	mov	r1, r9
	ldr	r3, [r1]
	mov	r2, #1
	eor	r3, r2
	str	r3, [r1]
.Lc0bd2:
	add	sp, #0x10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0a24

@ BuildSceneMatrix
@ r0.. = parameters. Composes the scene transform from Func_49ac, Func_4bd4,
@ Func_4c1c, Func_4cb4, Func_51d8, Func_5258 and Func_5268.
.thumb_func_start Func_c0be4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1e80
	ldr	r3, [r3]
	mov	r11, r2
	mov	r8, r3
	mov	r2, #0xc
	add	r2, r8
	mov	r5, r0
	mov	r9, r1
	lsl	r0, r4, #16
	mov	r1, #0x64
	sub	sp, #0x28
	mov	r10, r2
	bl	Func_af0_from_thumb
	mov	r3, r10
	mov	r2, r9
	str	r2, [r3, #4]
	mov	r2, r11
	str	r5, [r3]
	str	r2, [r3, #8]
	mov	r6, #0xff
	ldr	r2, =Func_8ac
	add	r3, sp, #4
	mov	r5, #0
	lsl	r6, #17
	mov	r1, #0xc0
	str	r5, [r3]
	str	r5, [r3, #4]
	str	r5, [r3, #8]
	mov	r7, r0
	mov	r11, r2
	mov	r0, r6
	lsl	r1, #8
	mov	r9, r3
	bl	_call_via_r11
	lsl	r2, r6, #1
	mov	r1, r0
	mov	r0, r6
	bl	Func_5258
	bl	Func_49ac
	mov	r0, r10
	bl	Func_4cb4
	mov	r2, r8
	mov	r3, #0x36
	ldrsh	r0, [r2, r3]
	bl	Func_4c1c
	mov	r2, r8
	mov	r3, #0x34
	ldrsh	r0, [r2, r3]
	bl	Func_4bd4
	add	r0, sp, #0x1c
	str	r5, [r0]
	str	r5, [r0, #4]
	mov	r2, r8
	ldr	r3, [r2, #0x20]
	mov	r1, r8
	str	r3, [r0, #8]
	ldr	r3, =Func_9c0
	bl	_call_via_r3
	ldr	r3, =iwram_1ce0
	mov	r5, #0x78
	str	r5, [r3, #0xc]
	str	r5, [r3, #0x10]
	bl	Func_49ac
	mov	r0, r8
	mov	r1, r10
	bl	Func_51d8
	add	r6, sp, #0x10
	mov	r1, r6
	mov	r0, r9
	bl	Func_5268
	ldr	r3, [r6, #4]
	ldr	r2, [r6]
	sub	r2, r5, r2
	sub	r5, r3
	lsl	r5, #8
	mov	r1, #0xf0
	mov	r3, r5
	lsl	r1, #15
	lsl	r5, r7, #8
	lsl	r2, #8
	mov	r0, r1
	sub	r5, r7
	str	r7, [sp]
	lsl	r6, r5, #1
	bl	Func_c0a24
	mov	r1, #0xc0
	mov	r0, r6
	lsl	r1, #8
	bl	_call_via_r11
	lsl	r5, #2
	mov	r1, r0
	mov	r2, r5
	mov	r0, r6
	bl	Func_5258
	add	sp, #0x28
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0be4

@ BuildViewMatrix
@ r0.. = parameters. The camera counterpart to Func_c0be4, same helper set.
@ Exported.
.thumb_func_start Func_c0cec
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r11, r2
	ldr	r2, =iwram_1e80
	ldr	r2, [r2]
	lsl	r3, #16
	mov	r8, r2
	mov	r2, #0xc
	add	r2, r8
	mov	r5, r0
	mov	r9, r1
	mov	r0, r3
	mov	r1, #0x64
	sub	sp, #0x28
	mov	r10, r2
	bl	Func_af0_from_thumb
	mov	r3, r10
	mov	r2, r9
	str	r2, [r3, #4]
	mov	r2, r11
	str	r5, [r3]
	str	r2, [r3, #8]
	mov	r6, #0xff
	ldr	r2, =Func_8ac
	add	r3, sp, #4
	mov	r5, #0
	lsl	r6, #17
	mov	r1, #0xc0
	str	r5, [r3]
	str	r5, [r3, #4]
	str	r5, [r3, #8]
	mov	r7, r0
	mov	r11, r2
	mov	r0, r6
	lsl	r1, #8
	mov	r9, r3
	bl	_call_via_r11
	lsl	r2, r6, #1
	mov	r1, r0
	mov	r0, r6
	bl	Func_5258
	bl	Func_49ac
	mov	r0, r10
	bl	Func_4cb4
	mov	r2, r8
	mov	r3, #0x36
	ldrsh	r0, [r2, r3]
	bl	Func_4c1c
	mov	r2, r8
	mov	r3, #0x34
	ldrsh	r0, [r2, r3]
	bl	Func_4bd4
	add	r0, sp, #0x1c
	mov	r1, r8
	str	r5, [r0]
	str	r5, [r0, #4]
	str	r6, [r0, #8]
	ldr	r3, =Func_9c0
	bl	_call_via_r3
	ldr	r3, =iwram_1ce0
	mov	r5, #0x78
	str	r5, [r3, #0xc]
	str	r5, [r3, #0x10]
	bl	Func_49ac
	mov	r0, r8
	mov	r1, r10
	bl	Func_51d8
	add	r6, sp, #0x10
	mov	r1, r6
	mov	r0, r9
	bl	Func_5268
	ldr	r3, [r6, #4]
	ldr	r2, [r6]
	sub	r2, r5, r2
	sub	r5, r3
	lsl	r5, #8
	mov	r1, #0xf0
	mov	r3, r5
	lsl	r1, #15
	lsl	r5, r7, #8
	lsl	r2, #8
	mov	r0, r1
	sub	r5, r7
	str	r7, [sp]
	lsl	r6, r5, #1
	bl	Func_c0a24
	mov	r1, #0xc0
	mov	r0, r6
	lsl	r1, #8
	bl	_call_via_r11
	lsl	r5, #2
	mov	r1, r0
	mov	r2, r5
	mov	r0, r6
	bl	Func_5258
	add	sp, #0x28
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0cec

@ AimCameraAtCombatant
@ r0 = combatant id. Points the camera at a combatant, resolving it with
@ Func_b7dd0 and rebuilding with Func_c0cec. Exported.
.thumb_func_start Func_c0df4
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r6, r1
	mov	r8, r2
	bl	Func_b7dd0
	ldr	r5, [r0]
	mov	r0, r6
	bl	Func_b7dd0
	ldr	r3, [r0]
	ldr	r1, [r5, #8]
	ldr	r0, [r3, #8]
	ldr	r4, [r5, #0x10]
	ldr	r2, [r3, #0x10]
	add	r0, r1
	add	r2, r4
	lsr	r3, r0, #31
	add	r0, r3
	lsr	r3, r2, #31
	add	r2, r3
	asr	r0, #1
	asr	r2, #1
	mov	r1, #0
	mov	r3, r8
	bl	Func_c0cec
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c0df4

@ WaitFramesA
@ r0 = count. Spins on Func_30f8(1). Exported.
.thumb_func_start Func_c0e38
	push	{r5, r6, r7, lr}
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lc0e48	@ 0x2044
	ldr	r7, =REG_BLDALPHA
	strh	r3, [r2]
	ldr	r6, .Lc0e4c	@ 0x1010
	mov	r5, #1
	b	.Lc0e58

	.align	2, 0
.Lc0e48:
	.word	0x2044
.Lc0e4c:
	.word	0x1010
	.pool

.Lc0e58:
	sub	r3, r6, r5
	strh	r3, [r7]
	mov	r0, #1
	add	r5, #2
	bl	Func_30f8
	cmp	r5, #0x10
	ble	.Lc0e58
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0e38

@ WaitFramesB
@ r0 = count. A second Func_30f8 spin with different bookkeeping. Exported.
.thumb_func_start Func_c0e70
	push	{r5, r6, r7, lr}
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lc0e80	@ 0x2044
	ldr	r7, =REG_BLDALPHA
	strh	r3, [r2]
	ldr	r6, .Lc0e84	@ 0x1000
	mov	r5, #1
	b	.Lc0e90

	.align	2, 0
.Lc0e80:
	.word	0x2044
.Lc0e84:
	.word	0x1000
	.pool

.Lc0e90:
	add	r3, r5, r6
	strh	r3, [r7]
	mov	r0, #1
	add	r5, #2
	bl	Func_30f8
	cmp	r5, #0x10
	ble	.Lc0e90
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0e70

@ GetSceneFlag
@ r0 = index. Returns a scene flag; no calls out.
.thumb_func_start Func_c0ea8
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lc0eb0	@ 0xbf
	strh	r3, [r2]
	bx	lr

	.align	2, 0
.Lc0eb0:
	.word	0xbf
.func_end Func_c0ea8

@ SetSceneFlag
@ r0 = index, r1 = value. Writes a scene flag.
.thumb_func_start Func_c0eb8
	push	{r5, r6, lr}
	mov	r5, r0
	ldr	r6, [r5]
	mov	r0, r5
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	ldr	r3, [r5]
	add	r6, r3
	str	r6, [r5, #4]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c0eb8

@ GetSceneCounter
@ Takes no arguments. Returns the scene frame counter.
.thumb_func_start Func_c0edc
	push	{lr}
	cmp	r0, #0
	bge	.Lc0ee4
	add	r0, #0xf
.Lc0ee4:
	asr	r0, #4
	pop	{r1}
	bx	r1
.func_end Func_c0edc

@ RunSceneFade
@ r0.. = parameters. Fades the scene a frame at a time through Func_30f8.
.thumb_func_start Func_c0eec
	push	{r5, lr}
	ldr	r1, =iwram_1ae8
	ldr	r3, [r1]
	mov	r2, #8
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f6e
	ldr	r5, =iwram_1b04
.Lc0efc:
	ldr	r3, =iwram_1e74
	ldr	r1, [r3]
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f14
	ldr	r3, =0x828
	add	r2, r1, r3
	ldr	r3, [r2]
	sub	r3, #1
	str	r3, [r2]
.Lc0f14:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f28
	ldr	r3, =0x828
	add	r2, r1, r3
	ldr	r3, [r2]
	add	r3, #1
	str	r3, [r2]
.Lc0f28:
	ldr	r3, [r5]
	mov	r2, #0x40
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f3c
	ldr	r3, =0x828
	add	r2, r1, r3
	ldr	r3, [r2]
	sub	r3, #0x64
	str	r3, [r2]
.Lc0f3c:
	ldr	r3, [r5]
	mov	r2, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f50
	ldr	r3, =0x828
	add	r2, r1, r3
	ldr	r3, [r2]
	add	r3, #0x64
	str	r3, [r2]
.Lc0f50:
	ldr	r3, =iwram_1c94
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f66
	ldr	r2, =0x828
	add	r3, r1, r2
	ldr	r0, [r3]
	ldr	r1, =iwram_1ae8
	b	.Lc0f6e
.Lc0f66:
	mov	r0, #1
	bl	Func_30f8
	b	.Lc0efc
.Lc0f6e:
	ldr	r3, [r1]
	mov	r2, #4
	and	r3, r2
	cmp	r3, #0
	beq	.Lc0f7a
	ldr	r0, =0x18f
.Lc0f7a:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_c0eec

@ SubmitCombatantToScene
@ r0 = combatant id. Adds the combatant's sprite to the scene draw list, via
@ Func_b7dd0.
.thumb_func_start Func_c0f98
	push	{r5, r6, lr}
	mov	r5, r1
	bl	Func_b7dd0
	cmp	r0, #0
	beq	.Lc100c
	ldr	r0, [r0]
	cmp	r0, #0
	beq	.Lc100c
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	mov	r2, #0xf
	and	r2, r3
	cmp	r2, #1
	beq	.Lc0fbe
	cmp	r2, #2
	beq	.Lc0fde
	b	.Lc100c
.Lc0fbe:
	ldr	r4, [r0, #0x50]
	mov	r2, #0xd
	mov	r3, #3
	ldrb	r1, [r4, #5]
	neg	r2, r2
	and	r5, r3
	mov	r3, r2
	lsl	r0, r5, #2
	and	r3, r1
	orr	r3, r0
	strb	r3, [r4, #5]
	ldrb	r3, [r4, #0x11]
	and	r2, r3
	orr	r2, r0
	strb	r2, [r4, #0x11]
	b	.Lc100c
.Lc0fde:
	mov	r3, #3
	and	r5, r3
	ldr	r1, [r0, #0x50]
	lsl	r0, r5, #2
	mov	r5, #0xd
	mov	r6, #0
	neg	r5, r5
.Lc0fec:
	ldmia	r1!, {r4}
	cmp	r4, #0
	beq	.Lc100c
	ldrb	r2, [r4, #5]
	mov	r3, r5
	and	r3, r2
	orr	r3, r0
	ldrb	r2, [r4, #0x11]
	strb	r3, [r4, #5]
	mov	r3, r5
	and	r3, r2
	orr	r3, r0
	add	r6, #1
	strb	r3, [r4, #0x11]
	cmp	r6, #3
	ble	.Lc0fec
.Lc100c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c0f98

@ SubmitQueuedCombatants
@ Takes no arguments. Walks the action queue with Func_b6c08 and submits each
@ combatant through Func_c0f98.
.thumb_func_start Func_c1014
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x1c
	mov	r5, sp
	mov	r7, r0
	mov	r1, r5
	mov	r0, #3
	bl	Func_b6c08
	cmp	r0, #0
	ble	.Lc1048
	mov	r8, r5
	mov	r6, #0
	mov	r5, r0
.Lc1032:
	mov	r2, r8
	ldrsh	r0, [r6, r2]
	cmp	r0, r7
	beq	.Lc1040
	mov	r1, #1
	bl	Func_c0f98
.Lc1040:
	sub	r5, #1
	add	r6, #2
	cmp	r5, #0
	bne	.Lc1032
.Lc1048:
	add	sp, #0x1c
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c1014

@ SubmitQueuedCombatantsAlt
@ Takes no arguments. As Func_c1014 over the other queue group.
.thumb_func_start Func_c1054
	push	{r5, r6, r7, lr}
	sub	sp, #0x1c
	mov	r5, sp
	mov	r0, #3
	mov	r1, r5
	bl	Func_b6c08
	cmp	r0, #0
	ble	.Lc107c
	mov	r7, r5
	mov	r6, #0
	mov	r5, r0
.Lc106c:
	ldrsh	r0, [r6, r7]
	mov	r1, #0
	sub	r5, #1
	bl	Func_c0f98
	add	r6, #2
	cmp	r5, #0
	bne	.Lc106c
.Lc107c:
	add	sp, #0x1c
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c1054

@ SortSceneDrawList
@ r0.. = parameters. Orders the scene draw list by depth; no calls out.
.thumb_func_start Func_c1084
	push	{lr}
	ldr	r3, =iwram_1e74
	ldr	r0, [r3]
	cmp	r0, #0
	beq	.Lc10e4
	mov	r2, #0xca
	lsl	r2, #3
	add	r3, r0, r2
	ldrh	r3, [r3]
	cmp	r3, #0
	beq	.Lc10e4
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lc10c8	@ 0x3f90
	strh	r3, [r2]
	ldr	r3, .Lc10cc	@ 0x10
	add	r2, #2
	strh	r3, [r2]
	ldr	r3, =0x64e
	add	r0, r3
	ldr	r2, =.Lc5c10
	ldrh	r3, [r0]
	ldr	r1, =REG_BLDY
	ldrsb	r3, [r2, r3]
	strh	r3, [r1]
	ldrh	r2, [r0]
	mov	r3, #0xf
	add	r1, r2, #1
	and	r1, r3
	cmp	r2, #0xe
	bls	.Lc10c4
	mov	r3, #0x10
	orr	r1, r3
.Lc10c4:
	strh	r1, [r0]
	b	.Lc10e4

	.align	2, 0
.Lc10c8:
	.word	0x3f90
.Lc10cc:
	.word	0x10
	.pool

.Lc10e4:
	pop	{r0}
	bx	r0
.func_end Func_c1084

	.section .rodata

@ PROMOTED: referenced from rom_bffb8.s across the split
	.global	Lc5a30
Lc5a30:
.Lc5a30:
	.incrom 0xc5a30, 0xc5b30
@ PROMOTED: referenced from rom_bffb8.s across the split
	.global	Lc5b30
Lc5b30:
.Lc5b30:
	.incrom 0xc5b30, 0xc5c10
.Lc5c10:
	.incrom 0xc5c10, 0xc5c38
