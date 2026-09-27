	.include "macros.inc"

@ 222 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, OvlFunc_758, OvlFunc_8ec, DialogueWait
@   PlaySound, OvlFunc_3860, CopyMapRectIndicesU x6, SetSaveBit
@   CopyMapRectIndicesU x8, OvlFunc_8ec, ClearSaveBit, OvlFunc_8ec
@   TestSaveBit, SetCameraSpeed
@   ... and 11 more
@ reads save bit 0x307; sets 0x302, 0x306, 0x307; clears 0x302, 0x306.
.thumb_func_start OvlFunc_924_20099b8
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	sub	sp, #0x20
	bl	__CutsceneStart
	add	r6, sp, #8
	mov	r0, r6
	bl	OvlFunc_924_2008758
	cmp	r0, #0
	bne	.L19d2
	b	.L1bae
.L19d2:
	ldr	r3, [r6, #4]
	cmp	r3, #8
	beq	.L19da
	b	.L1b1a
.L19da:
	ldr	r4, [r6, #8]
	asr	r3, r4, #20
	cmp	r3, #0xb
	bne	.L1a78
	mov	r2, sp
	add	r3, sp, #0x18
	ldmia	r3!, {r0, r1}
	stmia	r2!, {r0, r1}
	mov	r2, r4
	ldr	r3, [r6, #0xc]
	mov	r1, #8
	ldr	r0, [r6]
	bl	OvlFunc_924_20088ec
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0xd3
	bl	__PlaySound
	bl	OvlFunc_924_200b860
	mov	r3, #3
	str	r3, [sp]
	mov	r5, #1
	mov	r8, r3
	mov	r0, #0x4c
	mov	r1, #0x3c
	mov	r2, #0x4a
	mov	r3, #0x26
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r6, #2
	mov	r0, #0x4d
	mov	r1, #0x3c
	mov	r2, #0x4c
	mov	r3, #0x26
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, r8
	str	r0, [sp, #4]
	mov	r1, #0x3a
	mov	r0, #0x4b
	mov	r2, #0x56
	mov	r3, #0x29
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x4b
	mov	r1, #0x3b
	mov	r2, #0x56
	mov	r3, #0x2b
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x4c
	mov	r1, #0x3b
	mov	r2, #0x50
	mov	r3, #0x31
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x4d
	mov	r1, #0x3b
	mov	r2, #0x52
	mov	r3, #0x31
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	ldr	r0, =0x302
	bl	__SetFlag
	b	.L1bae
.L1a78:
	ldr	r3, =OvlFunc_924_200b948
	mov	r5, #1
	str	r3, [r6, #0x14]
	mov	r0, #0x4b
	mov	r1, #0x39
	mov	r2, #0x56
	mov	r3, #0x29
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x56
	mov	r3, #0x2a
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x56
	mov	r3, #0x2b
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x56
	mov	r3, #0x2c
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x50
	mov	r3, #0x31
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x51
	mov	r3, #0x31
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x52
	mov	r3, #0x31
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x4e
	mov	r1, #0x3a
	mov	r2, #0x53
	mov	r3, #0x31
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r2, sp
	add	r3, sp, #0x18
	ldmia	r3!, {r0, r1}
	stmia	r2!, {r0, r1}
	ldr	r0, [r6]
	ldr	r1, [r6, #4]
	ldr	r2, [r6, #8]
	ldr	r3, [r6, #0xc]
	bl	OvlFunc_924_20088ec
	ldr	r0, =0x302
	bl	__ClearFlag
	b	.L1bae
.L1b1a:
	cmp	r3, #0xa
	bne	.L1bae
	ldr	r3, [r6, #0x10]
	asr	r3, #20
	cmp	r3, #0x28
	bne	.L1b86
	mov	r3, sp
	add	r2, sp, #0x18
	ldmia	r2!, {r0, r1}
	stmia	r3!, {r0, r1}
	mov	r1, #0xa
	ldr	r0, [r6]
	ldr	r2, [r6, #8]
	ldr	r3, [r6, #0xc]
	bl	OvlFunc_924_20088ec
	ldr	r0, =0x307
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1b78
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #9
	lsl	r1, #6
	bl	__Func_80933d4
	mov	r1, #1
	mov	r2, #0x94
	neg	r1, r1
	lsl	r2, #18
	mov	r3, #1
	ldr	r0, =0x2ca0000
	bl	__Func_80933f8
	bl	__Func_8093530
	ldr	r0, =0x307
	bl	__SetFlag
	mov	r0, #5
	bl	OvlFunc_924_20097a8
	mov	r0, #0x32
	bl	__CutsceneWait
	b	.L1b7e
.L1b78:
	mov	r0, #5
	bl	OvlFunc_924_20097a8
.L1b7e:
	ldr	r0, =0x306
	bl	__SetFlag
	b	.L1bae
.L1b86:
	cmp	r3, #0x2a
	bne	.L1bae
	ldr	r3, =OvlFunc_924_20098f8
	str	r3, [r6, #0x14]
	mov	r2, sp
	add	r3, sp, #0x18
	ldmia	r3!, {r0, r1}
	stmia	r2!, {r0, r1}
	mov	r1, #0xa
	ldr	r0, [r6]
	ldr	r2, [r6, #8]
	ldr	r3, [r6, #0xc]
	bl	OvlFunc_924_20088ec
	mov	r0, #5
	bl	OvlFunc_924_20096c4
	ldr	r0, =0x306
	bl	__ClearFlag
.L1bae:
	bl	__CutsceneEnd
	add	sp, #0x20
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_20099b8
