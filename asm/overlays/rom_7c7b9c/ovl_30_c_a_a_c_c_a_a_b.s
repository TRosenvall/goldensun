	.include "macros.inc"

@ 54 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSlotAnimation x3
.thumb_func_start OvlFunc_943_200b558
	push	{r5, lr}
	ldr	r3, =.L5b40
	lsl	r1, #1
	ldr	r4, =0xffff97ff
	ldrh	r2, [r3, r1]
	ldr	r5, =0x7fe0000
	add	r3, r2, r4
	lsl	r3, #16
	ldr	r4, =0x7fe
	cmp	r3, r5
	bhi	.L3576
	ldr	r2, =.L5b30
	ldrh	r3, [r2, r1]
	add	r3, #0x70
	b	.L3588
.L3576:
	ldr	r5, =0x17ff
	add	r3, r2, r5
	lsl	r3, #16
	lsr	r3, #16
	cmp	r3, r4
	bhi	.L3592
	ldr	r2, =.L5b30
	ldrh	r3, [r2, r1]
	add	r3, #0xe0
.L3588:
	strh	r3, [r2, r1]
	mov	r1, #3
	bl	__MapActor_SetAnim
	b	.L35c4
.L3592:
	ldr	r4, =0xffff8fff
	ldr	r5, =0x7ffe0000
	add	r3, r2, r4
	lsl	r3, #16
	cmp	r3, r5
	bhi	.L35b2
	ldr	r2, =.L5b30
	mov	r4, #0xe0
	ldrh	r3, [r2, r1]
	lsl	r4, #1
	add	r3, r4
	strh	r3, [r2, r1]
	mov	r1, #2
	bl	__MapActor_SetAnim
	b	.L35c4
.L35b2:
	ldr	r2, =.L5b30
	mov	r5, #0xc0
	ldrh	r3, [r2, r1]
	lsl	r5, #2
	add	r3, r5
	strh	r3, [r2, r1]
	mov	r1, #1
	bl	__MapActor_SetAnim
.L35c4:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_200b558
