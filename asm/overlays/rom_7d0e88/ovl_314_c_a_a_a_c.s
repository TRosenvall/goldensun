	.include "macros.inc"
	.include "gba.inc"

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
