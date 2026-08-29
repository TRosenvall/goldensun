	.include "macros.inc"

@ 67 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, SetSaveBit, ClearSaveBit
@   TestSaveBit, SetSaveBit, ClearSaveBit
@ reads save bits 0x200, 0x201; sets 0x200, 0x201; clears 0x200, 0x201.
.thumb_func_start OvlFunc_921_20098c4
	push	{lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r4, [r0, #8]
	asr	r2, r4, #19
	mov	r3, r2
	sub	r3, #0x18
	cmp	r3, #7
	bls	.L18ea
	ldr	r1, [r0, #0x10]
	asr	r3, r1, #19
	sub	r3, #0x24
	cmp	r3, #9
	bhi	.L1902
	mov	r3, r2
	sub	r3, #0x16
	cmp	r3, #9
	bhi	.L1902
.L18ea:
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1954
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	strb	r0, [r3, #0x17]
	mov	r0, #0x80
	lsl	r0, #2
	b	.L1928
.L1902:
	mov	r2, #0xe8
	lsl	r2, #16
	cmp	r4, r2
	ble	.L1934
	mov	r2, #0xf0
	ldr	r3, [r0, #0xc]
	lsl	r2, #13
	cmp	r3, r2
	ble	.L1934
	mov	r3, #0xd4
	lsl	r3, #16
	cmp	r1, r3
	ble	.L1934
	ldr	r3, =iwram_3001e70
	ldr	r2, [r3]
	mov	r0, #0x80
	mov	r3, #0
	lsl	r0, #2
	strb	r3, [r2, #0x17]
.L1928:
	bl	__SetFlag
	ldr	r0, =0x201
	bl	__ClearFlag
	b	.L1954
.L1934:
	ldr	r0, =0x201
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1954
	ldr	r3, =iwram_3001e70
	ldr	r2, [r3]
	mov	r3, #1
	ldr	r0, =0x201
	strb	r3, [r2, #0x17]
	bl	__SetFlag
	mov	r0, #0x80
	lsl	r0, #2
	bl	__ClearFlag
.L1954:
	pop	{r0}
	bx	r0
.func_end OvlFunc_921_20098c4
