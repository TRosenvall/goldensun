	.include "macros.inc"

@ 22 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, PlaySound, CopyMapRectIndicesU, SetSaveBit
@ reads save bit 0x271; sets 0x271.
.thumb_func_start OvlFunc_943_2008bb8
	push	{lr}
	ldr	r0, =0x271
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lbe6
	mov	r0, #0x9e
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #3
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0x5e
	mov	r2, #0xd
	mov	r3, #0x5e
	bl	__CopyMapTiles
	ldr	r0, =0x271
	bl	__SetFlag
.Lbe6:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2008bb8

@ 22 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, PlaySound, CopyMapRectIndicesU, SetSaveBit
@ reads save bit 0x272; sets 0x272.
.thumb_func_start OvlFunc_943_2008bf0
	push	{lr}
	ldr	r0, =0x272
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lc1e
	mov	r0, #0x9e
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0x6c
	mov	r2, #0xd
	mov	r3, #0x6c
	bl	__CopyMapTiles
	ldr	r0, =0x272
	bl	__SetFlag
.Lc1e:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2008bf0
