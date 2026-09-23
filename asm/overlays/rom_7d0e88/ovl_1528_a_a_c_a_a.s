	.include "macros.inc"
	.include "gba.inc"

@ 143 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, OvlFunc_cc0 x6, RegisterTask, WaitFrames
@   SetInterruptSource, PlaySound, WaitFrames, PlaySound
@   TestSaveBit, CopyMapRectIndicesU x4, WaitFrames, SetInterruptSource
@   WaitFrames, UnregisterTask
@   ... and 2 more
@ reads save bit 0x200.
.thumb_func_start OvlFunc_947_20095fc
	push	{r5, r6, lr}
	mov	r0, #0x80
	lsl	r0, #2
	sub	sp, #0xc
	bl	__GetFlag
	cmp	r0, #0
	bne	.L164c
	str	r0, [sp]
	mov	r6, #0xa
	mov	r5, #0x1f
	mov	r0, #0xa
	mov	r1, #0x13
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_947_2008cc0
	mov	r3, #1
	str	r3, [sp]
	mov	r0, #0xa
	mov	r1, #0x33
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_947_2008cc0
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x2a
	mov	r1, #0x33
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_947_2008cc0
	b	.L168c
.L164c:
	mov	r3, #0
	str	r3, [sp]
	mov	r6, #0xa
	mov	r5, #0x1f
	mov	r0, #0xa
	mov	r1, #0x13
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_947_2008cc0
	mov	r3, #1
	str	r3, [sp]
	mov	r0, #0xa
	mov	r1, #0x53
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_947_2008cc0
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x2a
	mov	r1, #0x53
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_947_2008cc0
.L168c:
	ldr	r5, =.L3738
	mov	r6, #0
	mov	r1, #0xc8
	lsl	r1, #4
	str	r6, [r5]
	ldr	r0, =OvlFunc_947_20095cc
	bl	__StartTask
	mov	r0, #1
	bl	__WaitFrames
	ldr	r2, =OvlFunc_947_2009578
	mov	r0, #1
	mov	r1, #0
	bl	__SetIntrHandler
	mov	r0, #0xe7
	bl	__PlaySound
	str	r6, [r5]
.L16b4:
	mov	r0, #1
	bl	__WaitFrames
	ldr	r3, [r5]
	add	r3, #1
	str	r3, [r5]
	cmp	r3, #0x64
	ble	.L16b4
	ldr	r0, =0x121
	bl	__PlaySound
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L16fa
	mov	r5, #0x20
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x20
	mov	r3, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x20
	mov	r1, #0x20
	mov	r2, #0x40
	mov	r3, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	b	.L171c
.L16fa:
	mov	r5, #0x20
	mov	r0, #0
	mov	r1, #0x40
	mov	r2, #0x20
	mov	r3, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x20
	mov	r1, #0x40
	mov	r2, #0x40
	mov	r3, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
.L171c:
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__SetIntrHandler
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =OvlFunc_947_20095cc
	bl	__StopTask
	bl	__Func_800fe9c
	mov	r0, #0x1e
	bl	__WaitFrames
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_947_20095fc
