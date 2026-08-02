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

@ Counter: shop type 0x10 via UI_Sanctum, opened only from inside the facing arc.
@ Outside it the attendant speaks instead -- line 0x16b3.
.thumb_func_start OvlFunc_911_2008230
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	ldrh	r5, [r0, #6]
	bl	__CutsceneStart
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L250
	mov	r0, #0x10
	bl	__UI_Sanctum
	b	.L25e
.L250:
	ldr	r0, =0x16b3
	bl	__MessageID
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_8093054
.L25e:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_911_2008230

