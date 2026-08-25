	.include "macros.inc"
	.include "gba.inc"


@ 52 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   WaitFrames, AllocOverlayState, TestSaveBit, OvlFunc_a54
@ reads save bit 0x201.
.thumb_func_start OvlFunc_957_2008b30
	push	{r5, lr}
	ldr	r5, =iwram_3001ebc
	mov	r3, #0xe0
	ldr	r1, [r5]
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x18
	str	r3, [r2]
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x4d
	bl	__Func_808fe38
	ldr	r3, =0x52a
	ldr	r5, [r5, #0x10]
	add	r2, r5, r3
	mov	r3, #5
	strh	r3, [r2]
	ldr	r0, =0x201
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb7e
	ldr	r3, =0x534
	add	r2, r5, r3
	ldr	r3, =0x1d1d
	strh	r3, [r2]
	ldr	r3, =0x536
	add	r2, r5, r3
	mov	r3, #0x3f
	strh	r3, [r2]
	bl	OvlFunc_957_2008a54
	b	.Lb9a
.Lb7e:
	ldr	r3, =0x534
	add	r2, r5, r3
	ldr	r3, =0x3f3f
	strh	r3, [r2]
	ldr	r3, =0x536
	add	r2, r5, r3
	mov	r3, #0x1f
	strh	r3, [r2]
	ldr	r2, =0x3f42
	ldr	r3, =REG_BLDCNT
	strh	r2, [r3]
	ldr	r2, =0xc04
	add	r3, #2
	strh	r2, [r3]
.Lb9a:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_957_2008b30

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSlotPalette x5
.thumb_func_start OvlFunc_957_2008bc8
	push	{lr}
	ldr	r3, =gState
	mov	r0, #0xe0
	lsl	r0, #1
	add	r3, r0
	ldrh	r1, [r3]
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r3, =0x92
	cmp	r2, r3
	bne	.Lbe6
	mov	r2, #0x80
	ldr	r3, =REG_BLDALPHA
	lsl	r2, #5
	strh	r2, [r3]
.Lbe6:
	lsl	r3, r1, #16
	ldr	r2, =0x97
	asr	r3, #16
	cmp	r3, r2
	bne	.Lc18
	mov	r0, #0x10
	mov	r1, #1
	bl	__Func_8092950
	mov	r0, #0x11
	mov	r1, #4
	bl	__Func_8092950
	mov	r0, #0x12
	mov	r1, #0xb
	bl	__Func_8092950
	mov	r0, #0x13
	mov	r1, #2
	bl	__Func_8092950
	mov	r0, #0x14
	mov	r1, #3
	bl	__Func_8092950
.Lc18:
	pop	{r0}
	bx	r0
.func_end OvlFunc_957_2008bc8
