	.include "macros.inc"

@ 47 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, GetCameraEntity
.thumb_func_start OvlFunc_959_2009980
	push	{r5, lr}
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__Func_8093554
	ldr	r3, [r5, #8]
	mov	r2, r0
	cmp	r3, #0
	bge	.L1998
	ldr	r0, =0xfffff
	add	r3, r0
.L1998:
	ldr	r0, [r5, #0x10]
	asr	r4, r3, #20
	cmp	r0, #0
	bge	.L19a4
	ldr	r1, =0xfffff
	add	r0, r1
.L19a4:
	ldr	r3, [r2, #8]
	asr	r1, r0, #20
	cmp	r3, #0
	bge	.L19b0
	ldr	r0, =0xfffff
	add	r3, r0
.L19b0:
	ldr	r0, [r2, #0x10]
	asr	r3, #20
	cmp	r0, #0
	bge	.L19bc
	ldr	r2, =0xfffff
	add	r0, r2
.L19bc:
	sub	r3, r4, r3
	asr	r0, #20
	cmp	r3, #0
	bge	.L19c6
	neg	r3, r3
.L19c6:
	sub	r0, r1, r0
	cmp	r0, #0
	bge	.L19ce
	neg	r0, r0
.L19ce:
	cmp	r3, #7
	bgt	.L19d6
	cmp	r0, #5
	ble	.L19da
.L19d6:
	mov	r0, #0
	b	.L19dc
.L19da:
	mov	r0, #1
.L19dc:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_2009980

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_f30
@ reads save bit 0x35b.
.thumb_func_start OvlFunc_959_20099e8
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r0, =0x35b
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1a32
	ldr	r3, [r5, #8]
	cmp	r3, #0
	bge	.L1a06
	ldr	r2, =0xfffff
	add	r3, r2
.L1a06:
	ldr	r0, [r5, #0x10]
	asr	r3, #20
	cmp	r0, #0
	bge	.L1a12
	ldr	r2, =0xfffff
	add	r0, r2
.L1a12:
	asr	r0, #20
	cmp	r3, #0x2b
	bne	.L1a32
	cmp	r0, #0x1c
	ble	.L1a32
	cmp	r0, #0x1f
	bgt	.L1a32
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xb6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0x29
	strh	r2, [r3]
	bl	OvlFunc_959_2008f30
.L1a32:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_20099e8

@ 47 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_e80
@ reads save bit 0x358.
.thumb_func_start OvlFunc_959_2009a44
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0xd6
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1aa2
	ldr	r3, [r5, #8]
	cmp	r3, #0
	bge	.L1a64
	ldr	r1, =0xfffff
	add	r3, r1
.L1a64:
	asr	r2, r3, #20
	ldr	r3, [r5, #0x10]
	cmp	r3, #0
	bge	.L1a70
	ldr	r1, =0xfffff
	add	r3, r1
.L1a70:
	asr	r3, #20
	cmp	r2, #0x10
	bne	.L1aa2
	cmp	r3, #0x37
	ble	.L1aa2
	cmp	r3, #0x3a
	bgt	.L1aa2
	mov	r2, #0xc0
	ldrh	r0, [r5, #6]
	lsl	r2, #8
	cmp	r0, r2
	beq	.L1a90
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bne	.L1aa2
.L1a90:
	ldr	r3, =iwram_3001ebc
	mov	r1, #0xb6
	ldr	r3, [r3]
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0x28
	strh	r2, [r3]
	bl	OvlFunc_959_2008e80
.L1aa2:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2009a44
