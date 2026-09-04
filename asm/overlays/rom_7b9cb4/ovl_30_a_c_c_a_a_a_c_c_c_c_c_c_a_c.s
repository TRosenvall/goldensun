	.include "macros.inc"
	.include "gba.inc"

@ 107 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectIndicesU x3, PlaySound, SetMapTransition, DialogueWait
@   WaitFrames, SetInterruptSource, WaitFrames x2, SetInterruptSource
@   PlaySound, SetMapTransition, DialogueWait, CopyMapRectIndicesU
@   SetSaveBit
@ sets 0x907.
.thumb_func_start OvlFunc_932_20088d4
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	sub	sp, #8
	mov	r5, #1
	mov	r8, r3
	mov	r0, #0x71
	mov	r1, #0x1f
	mov	r2, #0x67
	mov	r3, #0x11
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #3
	str	r3, [sp]
	mov	r6, #2
	mov	r0, #0x6f
	mov	r1, #0x20
	mov	r2, #0x68
	mov	r3, #0x12
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #0x12
	mov	r1, #0x20
	mov	r2, #0x67
	mov	r0, #0x40
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xe6
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #10
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_8012330
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r5, #0xb2
	lsl	r5, #1
	add	r5, r8
	mov	r6, #0x17
.L93c:
	ldr	r3, [r5, #0xc]
	ldr	r2, =0xffff0000
	add	r3, r2
	str	r3, [r5, #0xc]
	mov	r0, #4
	sub	r6, #1
	bl	__WaitFrames
	cmp	r6, #0
	bge	.L93c
	ldr	r2, =OvlFunc_932_20086a0
	mov	r0, #1
	mov	r1, #0
	bl	__SetIntrHandler
	ldr	r2, =.L5238
	ldr	r3, .L994	@ 0
	strh	r3, [r2]
	mov	r5, r2
.L962:
	mov	r0, #1
	bl	__WaitFrames
	ldrh	r3, [r5]
	mov	r2, #0xc8
	add	r3, #1
	strh	r3, [r5]
	lsl	r2, #15
	lsl	r3, #16
	cmp	r3, r2
	bls	.L962
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__SetIntrHandler
	ldr	r0, =0x121
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #1
	b	.L9ac

	.align	2, 0
.L994:
	.word	0
	.pool

.L9ac:
	neg	r1, r1
	ldr	r2, =0xe666
	neg	r0, r0
	bl	__Func_8012330
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r3, #4
	mov	r2, #3
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0xe
	mov	r2, #0x67
	mov	r3, #0x11
	mov	r0, #0x67
	bl	__CopyMapTiles
	ldr	r0, =0x907
	bl	__SetFlag
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_20088d4
