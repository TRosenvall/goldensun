	.include "macros.inc"
	.include "gba.inc"

@ PlayBattleAnimation -- the call into rom_c9000
@ r0.. = parameters. Registers the animation task with StartTask, submits the
@ combatants with Func_c1054 / Func_c0f98, runs the animation a frame at a time
@ through WaitFrames, and unregisters with StopTask when it finishes.
@ Exported and called from rom_15000 and rom_c9000 as well as here.
.thumb_func_start Func_80c10e8  @ 0x080c10e8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e74
	sub	sp, #0x1c
	mov	r8, r0
	mov	r10, r1
	ldr	r5, [r3]
	cmp	r1, #0
	bne	.Lc1122
	ldr	r0, =Func_80c1084
	bl	StopTask
	ldr	r3, =REG_BLDY
	mov	r1, r10
	strh	r1, [r3]
	bl	Func_80c1054
	mov	r0, #1
	bl	WaitFrames
	ldr	r0, =REG_BLDCNT
	mov	r1, #0
	bl	SetRegAnimDest
.Lc1122:
	cmp	r5, #0
	beq	.Lc11c4
	mov	r2, r10
	cmp	r2, #0
	beq	.Lc11c4
	mov	r1, #0xca
	lsl	r1, #3
	add	r3, r5, r1
	mov	r1, r10
	strh	r1, [r3]
	ldr	r1, =0x64e
	mov	r2, #0
	add	r3, r5, r1
	strh	r2, [r3]
	ldr	r3, =REG_BLDY
	strh	r2, [r3]
	mov	r2, #0x10
	sub	r3, #2
	strh	r2, [r3]
	mov	r5, sp
	mov	r1, r5
	mov	r0, #3
	bl	Func_80b6c08
	mov	r6, #0
	mov	r7, r0
	cmp	r6, r7
	bcs	.Lc1178
	mov	r2, #1
	mov	r11, r5
	mov	r9, r2
	mov	r5, #0
.Lc1162:
	mov	r3, r11
	ldrsh	r0, [r5, r3]
	mov	r2, r9
	mov	r1, r10
	and	r1, r2
	add	r6, #1
	bl	Func_80c0f98
	add	r5, #2
	cmp	r6, r7
	bcc	.Lc1162
.Lc1178:
	mov	r3, r8
	cmp	r3, #0
	beq	.Lc11ac
	ldrh	r0, [r3]
	mov	r1, #2
	mov	r6, #0
	add	r8, r1
	cmp	r0, #0xff
	beq	.Lc11ac
	mov	r7, #1
	mov	r5, r7
	mov	r2, r10
	and	r5, r2
.Lc1192:
	mov	r1, r5
	eor	r1, r7
	add	r6, #1
	bl	Func_80c0f98
	cmp	r6, #0xd
	bhi	.Lc11ac
	mov	r3, r8
	ldrh	r0, [r3]
	mov	r1, #2
	add	r8, r1
	cmp	r0, #0xff
	bne	.Lc1192
.Lc11ac:
	mov	r0, #1
	bl	WaitFrames
	ldr	r0, =REG_BLDCNT
	mov	r1, #0
	bl	SetRegAnimDest
	mov	r1, #0x90
	ldr	r0, =Func_80c1084
	lsl	r1, #3
	bl	StartTask
.Lc11c4:
	add	sp, #0x1c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80c10e8

@ AnimateSceneElement
@ r0.. = parameters. Moves a scene element along a path built from sin /
@ cos (sine and cosine) with Func_4458 variation. 282 lines; traced
@ structurally.
.thumb_func_start Func_80c11ec  @ 0x080c11ec
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r2, =gPtrs
	mov	r12, r2
	mov	r3, r12
	add	r3, #0xa0
	ldr	r3, [r3]
	sub	sp, #0x10
	str	r3, [sp, #0xc]
	mov	r3, r12
	add	r3, #0x9c
	ldr	r3, [r3]
	mov	r2, #0x9e
	mov	r10, r3
	lsl	r2, #5
	add	r2, r10
	mov	r3, #0
	str	r3, [r2]
	mov	r3, r12
	add	r3, #0xb8
	ldr	r3, [r3]
	mov	r7, #0x8e
	mov	r11, r3
	mov	r2, #0xf
	ldr	r3, =Func_8000888
	lsl	r7, #5
	str	r2, [sp, #8]
	mov	r9, r3
	add	r7, r10
.Lc1230:
	ldr	r0, [r7, #0x18]
	cmp	r0, #0
	beq	.Lc12b4
	ldr	r3, [r7]
	asr	r3, #8
	mov	r0, r3
	mul	r0, r3
	ldr	r3, [r7, #4]
	asr	r3, #8
	mov	r2, r3
	mul	r2, r3
	mov	r3, r2
	add	r0, r3
	ldr	r3, [r7, #8]
	asr	r3, #8
	mov	r2, r3
	mul	r2, r3
	mov	r3, r2
	add	r0, r3
	bl	FastIntSqrtFP1616_RAM 
	ldr	r3, =0xfff
	cmp	r0, r3
	bgt	.Lc1266
	mov	r3, #0
	str	r3, [r7, #0x18]
	b	.Lc12ae
.Lc1266:
	mov	r1, #0x80
	ldr	r3, =Func_80008ac
	lsl	r1, #9
	bl	_call_via_r3
	ldr	r3, [r7, #0x18]
	sub	r3, #1
	mov	r2, #2
	str	r3, [r7, #0x18]
	ldr	r6, =Func_8000888
	mov	r8, r0
	mov	r5, r7
	mov	r14, r2
.Lc1280:
	ldr	r4, [r5]
	neg	r0, r4
	asr	r0, #8
	mov	r1, r8
	.call_via r6
	mov	r1, #0x98
	lsl	r1, #9
	.call_via r6
	ldr	r3, [r5, #0xc]
	asr	r2, r3, #7
	sub	r3, r2
	add	r3, r0
	str	r3, [r5, #0xc]
	add	r4, r3
	mov	r3, #1
	neg	r3, r3
	add	r14, r3
	mov	r2, r14
	stmia	r5!, {r4}
	cmp	r2, #0
	bge	.Lc1280
.Lc12ae:
	ldr	r0, [r7, #0x18]
	cmp	r0, #0
	bne	.Lc1348
.Lc12b4:
	ldr	r3, =0x13bc
	add	r3, r10
	ldr	r3, [r3]
	cmp	r3, #0x18
	bgt	.Lc1344
	bl	Random
	mov	r5, r0
	bl	Random
	mov	r3, #0x80
	lsl	r3, #9
	add	r3, r0
	lsr	r6, r3, #1
	mov	r0, r5
	mov	r8, r3
	bl	cos
	mov	r1, r6
	.call_via r9
	str	r0, [r7]
	mov	r0, r5
	bl	sin
	mov	r1, r6
	.call_via r9
	ldr	r2, [r7]
	mov	r1, #1
	mov	r3, r2
	and	r3, r1
	str	r0, [r7, #4]
	cmp	r3, #0
	beq	.Lc1302
	neg	r3, r2
	str	r3, [r7]
.Lc1302:
	ldr	r2, [r7, #4]
	mov	r3, r2
	and	r3, r1
	cmp	r3, #0
	beq	.Lc1310
	neg	r3, r2
	str	r3, [r7, #4]
.Lc1310:
	bl	Random
	mov	r2, #0x80
	lsl	r2, #8
	add	r0, r2
	lsr	r0, #2
	ldr	r3, [r7, #4]
	str	r0, [r7, #8]
	ldr	r0, [r7]
	asr	r2, r3, #8
	neg	r0, r0
	neg	r3, r3
	asr	r1, r0, #7
	asr	r3, #7
	asr	r0, #8
	add	r3, r0
	add	r1, r2
	str	r3, [r7, #0x10]
	mov	r2, r8
	mov	r3, #0
	str	r3, [r7, #0x14]
	lsr	r3, r2, #13
	add	r3, #1
	str	r1, [r7, #0xc]
	str	r3, [r7, #0x18]
	mov	r0, r3
.Lc1344:
	cmp	r0, #0
	beq	.Lc1384
.Lc1348:
	ldr	r3, [r7]
	asr	r3, #10
	mov	r5, r3
	ldr	r3, [r7, #4]
	asr	r3, #10
	mov	r4, r3
	mov	r6, r0
	add	r5, #0x40
	add	r4, #0x40
	cmp	r6, #0
	bge	.Lc1362
	mov	r6, #0
	b	.Lc1368
.Lc1362:
	cmp	r6, #6
	ble	.Lc1368
	mov	r6, #6
.Lc1368:
	ldr	r3, =.Lc3620
	ldr	r2, =.Lc3604
	ldrb	r0, [r3, r6]
	lsl	r3, r6, #2
	ldr	r1, [r2, r3]
	lsr	r3, r0, #1
	sub	r2, r5, r3
	str	r0, [sp]
	str	r0, [sp, #4]
	add	r1, r10
	sub	r3, r4, r3
	ldr	r0, [sp, #0xc]
	bl	_call_via_r11
.Lc1384:
	ldr	r3, [sp, #8]
	sub	r3, #1
	add	r7, #0x1c
	str	r3, [sp, #8]
	cmp	r3, #0
	blt	.Lc1392
	b	.Lc1230
.Lc1392:
	ldr	r3, =gPtrs
	add	r3, #0xbc
	ldr	r3, [r3]
	mov	r5, #0x9c
	lsl	r5, #5
	mov	r11, r3
	add	r5, r10
	mov	r7, #2
.Lc13a2:
	ldr	r1, [r5]
	ldr	r3, [r5, #8]
	ldr	r2, [r5, #4]
	add	r1, r3
	ldr	r3, [r5, #0xc]
	str	r1, [r5]
	asr	r4, r1, #10
	ldr	r1, [r5, #0x10]
	add	r2, r3
	str	r2, [r5, #4]
	asr	r6, r2, #10
	mov	r2, r1
	cmp	r1, #0
	bge	.Lc13c0
	add	r2, r1, #7
.Lc13c0:
	asr	r2, #3
	mov	r3, #3
	sub	r0, r3, r2
	cmp	r0, #0
	blt	.Lc13ea
	add	r3, r1, #1
	str	r3, [r5, #0x10]
	ldr	r2, =.Lc3628
	lsl	r3, r0, #2
	ldr	r1, [r2, r3]
	mov	r0, #0x20
	mov	r2, r4
	mov	r3, r6
	str	r0, [sp]
	str	r0, [sp, #4]
	add	r1, r10
	add	r2, #0x30
	add	r3, #0x30
	ldr	r0, [sp, #0xc]
	bl	_call_via_r11
.Lc13ea:
	sub	r7, #1
	add	r5, #0x14
	cmp	r7, #0
	bge	.Lc13a2
	ldr	r2, =0x13bc
	add	r2, r10
	ldr	r3, [r2]
	add	r3, #1
	str	r3, [r2]
	mov	r2, #0x9e
	lsl	r2, #5
	add	r2, r10
	mov	r3, #1
	str	r3, [r2]
	add	sp, #0x10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80c11ec
