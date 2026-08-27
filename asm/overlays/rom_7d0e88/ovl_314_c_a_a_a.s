	.include "macros.inc"
	.include "gba.inc"

@ 111 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked
.thumb_func_start OvlFunc_947_2008ddc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r5, r3
	ldr	r3, =iwram_3001e70
	mov	r8, r1
	mov	r6, r2
	ldr	r7, [r3]
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =.L2ce0
	mov	r4, #0
	ldr	r3, [r3, r4]
	cmp	r2, r3
	beq	.Le1e
.Le02:
	mov	r3, #7
	add	r4, #1
	str	r3, [r5]
	cmp	r4, #5
	bhi	.Le20
	ldr	r3, [r0, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	ldr	r2, =.L2ce0
	lsl	r3, r4, #2
	ldr	r3, [r2, r3]
	cmp	r1, r3
	bne	.Le02
.Le1e:
	str	r4, [r5]
.Le20:
	ldr	r2, [r5]
	cmp	r2, #6
	bls	.Le2a
	mov	r0, #0
	b	.Leb2
.Le2a:
	ldr	r3, [r0, #8]
	str	r3, [r5, #8]
	ldr	r3, [r0, #0xc]
	str	r3, [r5, #0xc]
	ldr	r3, [r0, #0x10]
	lsl	r1, r2, #4
	str	r3, [r5, #0x10]
	ldr	r0, =gScript_884__0200acf8
	add	r3, r1, #4
	ldr	r2, [r0, r3]
	cmp	r2, #0
	bge	.Le44
	neg	r2, r2
.Le44:
	mov	r3, r1
	add	r3, #0xc
	ldr	r3, [r0, r3]
	cmp	r3, #0
	bge	.Le50
	neg	r3, r3
.Le50:
	add	r3, r2, r3
	asr	r3, #4
	str	r3, [r6]
	ldr	r3, [r5]
	lsl	r3, #4
	ldr	r2, [r0, r3]
	cmp	r2, #0
	bge	.Le62
	neg	r2, r2
.Le62:
	add	r3, #8
	ldr	r3, [r0, r3]
	cmp	r3, #0
	bge	.Le6c
	neg	r3, r3
.Le6c:
	add	r3, r2, r3
	asr	r3, #4
	mov	r1, r8
	str	r3, [r1]
	ldr	r2, [r5]
	lsl	r2, #4
	ldr	r3, [r0, r2]
	ldr	r1, [r5, #8]
	lsl	r3, #16
	add	r1, r3
	str	r1, [r5, #8]
	add	r2, #4
	ldr	r3, [r0, r2]
	ldr	r2, [r5, #0x10]
	lsl	r3, #16
	add	r2, r3
	asr	r2, #20
	str	r2, [r5, #0x10]
	asr	r1, #20
	mov	r2, #0x9e
	str	r1, [r5, #8]
	lsl	r2, #1
	add	r3, r7, r2
	ldr	r3, [r3]
	ldr	r2, [sp, #0x14]
	asr	r3, #20
	mov	r1, #0xa0
	str	r3, [r2]
	lsl	r1, #1
	add	r3, r7, r1
	ldr	r3, [r3]
	ldr	r2, [sp, #0x18]
	asr	r3, #20
	str	r3, [r2]
	mov	r0, #1
.Leb2:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_947_2008ddc

@ 65 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_ddc, CopyMapRectAttributes, SetEntityAnimation
@   CopyMapRectIndicesU x2
.thumb_func_start OvlFunc_947_2008ec8
	push	{r5, r6, r7, lr}
	sub	sp, #0x30
	mov	r5, r0
	bl	__MapActor_GetActor
	add	r3, sp, #0xc
	add	r6, sp, #0x18
	str	r3, [sp]
	add	r3, sp, #8
	mov	r7, r0
	str	r3, [sp, #4]
	add	r1, sp, #0x14
	add	r2, sp, #0x10
	mov	r0, r5
	mov	r3, r6
	bl	OvlFunc_947_2008ddc
	cmp	r0, #0
	bne	.Lef2
	mov	r0, #0
	b	.Lf4e
.Lef2:
	ldr	r1, [r6, #8]
	ldr	r0, [r6, #0x10]
	ldr	r2, [sp, #0x14]
	ldr	r3, [sp, #0x10]
	str	r1, [sp]
	str	r0, [sp, #4]
	mov	r1, #2
	mov	r0, #2
	bl	__Func_8010704
	mov	r0, r7
	mov	r1, #4
	bl	__Actor_SetAnim
	mov	r1, r7
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r1]
	ldr	r1, [sp, #0x14]
	ldr	r0, [sp, #0x10]
	cmp	r1, r0
	bls	.Lf38
	ldr	r2, [r6, #8]
	ldr	r3, [r6, #0x10]
	add	r2, #0x20
	str	r1, [sp]
	str	r0, [sp, #4]
	add	r3, #2
	mov	r0, #0x46
	mov	r1, #0x28
	bl	__CopyMapTiles
	b	.Lf4c
.Lf38:
	ldr	r2, [r6, #8]
	ldr	r3, [r6, #0x10]
	add	r2, #0x20
	str	r1, [sp]
	str	r0, [sp, #4]
	add	r3, #2
	mov	r0, #0x44
	mov	r1, #0x28
	bl	__CopyMapTiles
.Lf4c:
	mov	r0, #1
.Lf4e:
	add	sp, #0x30
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_947_2008ec8

@ 53 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_ddc, CopyMapRectAttributes, OvlFunc_528
@   SetEntityAnimation
.thumb_func_start OvlFunc_947_2008f58
	push	{r5, r6, r7, lr}
	sub	sp, #0x30
	mov	r5, r0
	bl	__MapActor_GetActor
	add	r3, sp, #0xc
	add	r6, sp, #0x18
	str	r3, [sp]
	add	r3, sp, #8
	mov	r7, r0
	str	r3, [sp, #4]
	add	r1, sp, #0x14
	add	r2, sp, #0x10
	mov	r0, r5
	mov	r3, r6
	bl	OvlFunc_947_2008ddc
	cmp	r0, #0
	bne	.Lf82
	mov	r0, #0
	b	.Lfc4
.Lf82:
	ldr	r4, [r6, #0x10]
	ldr	r0, [sp, #0xc]
	ldr	r5, [r6, #8]
	ldr	r1, [sp, #8]
	ldr	r2, [sp, #0x14]
	add	r1, r4
	ldr	r3, [sp, #0x10]
	add	r0, r5
	str	r5, [sp]
	str	r4, [sp, #4]
	bl	__Func_8010704
	ldr	r0, [sp, #0x10]
	ldr	r2, [r6, #0x10]
	ldr	r1, [r6, #8]
	str	r0, [sp]
	mov	r0, #0xff
	ldr	r3, [sp, #0x14]
	str	r0, [sp, #4]
	mov	r0, #0
	bl	OvlFunc_947_2008528
	mov	r0, r7
	mov	r1, #1
	bl	__Actor_SetAnim
	mov	r1, r7
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #0xfd
	and	r3, r2
	strb	r3, [r1]
	mov	r0, #1
.Lfc4:
	add	sp, #0x30
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_947_2008f58

@ Leaf helper, 33 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: REG_DMA3SAD, iwram_1e70
@ Reads offsets +0x8.
.thumb_func_start OvlFunc_947_2008fcc
	push	{r5, lr}
	mov	r5, r3
	ldr	r3, =iwram_3001e70
	mov	r4, r2
	ldr	r2, [r3]
	cmp	r2, #0
	beq	.L1008
	lsl	r3, r0, #1
	add	r3, r0
	mov	r0, #0x98
	lsl	r0, #1
	lsl	r3, #4
	add	r3, r0
	lsl	r0, r4, #7
	ldr	r2, [r2, r3]
	add	r0, r1, r0
	lsl	r0, #2
	add	r0, r2, r0
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	ldr	r2, =0x84000001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r2, #0x80
	ldr	r1, =REG_DMA3SAD
	lsl	r2, #24
.L1000:
	ldr	r3, [r1, #8]
	and	r3, r2
	cmp	r3, #0
	bne	.L1000
.L1008:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_947_2008fcc
