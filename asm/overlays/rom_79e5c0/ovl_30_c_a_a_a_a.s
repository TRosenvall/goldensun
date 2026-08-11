	.include "macros.inc"

@ 26 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, OvlFunc_a0
@ reads save bit 0x845.
.thumb_func_start OvlFunc_911_20081dc
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x24
	cmp	r2, r3
	bne	.L204
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	bne	.L200
	ldr	r0, =.L3098
	bl	OvlFunc_911_20080a0
.L200:
	ldr	r0, =.L3098
	b	.L210
.L204:
	ldr	r3, =0x27
	cmp	r2, r3
	bne	.L20e
	ldr	r0, =.L3368
	b	.L210
.L20e:
	ldr	r0, =.L3080
.L210:
	pop	{r1}
	bx	r1
.func_end OvlFunc_911_20081dc
