	.include "macros.inc"
	.include "gba.inc"

@ 87 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, OvlFunc_b8c, PlaySound, OvlFunc_944
@   SetSlotAnimation, TestSaveBit, SetSaveBit, PlaySound x2
@   OvlFunc_944, PlaySound
@ reads save bit 0x207; sets 0x207.
.thumb_func_start OvlFunc_935_20089c0
	push	{r5, r6, r7, lr}
	mov	r0, #0xa
	bl	__MapActor_GetActor
	add	r0, #0x5b
	ldrb	r6, [r0]
	cmp	r6, #0
	bne	.La82
	ldr	r1, =.L2224
	ldr	r3, [r1]
	add	r3, #1
	str	r3, [r1]
	cmp	r3, #0xbe
	ble	.L9de
	str	r6, [r1]
.L9de:
	ldr	r7, =.L2228
	ldr	r0, [r7]
	ldr	r2, =.L2214
	lsl	r3, r0, #2
	ldr	r2, [r2, r3]
	ldr	r3, [r1]
	cmp	r2, r3
	bne	.La06
	add	r0, #0xb
	bl	__MapActor_GetActor
	ldr	r3, =0xa3d
	mov	r5, r0
	str	r3, [r5, #0x48]
	ldr	r3, [r7]
	add	r3, #1
	str	r3, [r7]
	cmp	r3, #3
	ble	.La06
	str	r6, [r7]
.La06:
	mov	r6, #0
	mov	r7, #0
.La0a:
	mov	r0, r6
	add	r0, #0xb
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r3, [r5, #0x28]
	cmp	r3, #0
	blt	.La3c
	ldr	r3, [r5, #0xc]
	ldr	r2, =0xffff
	cmp	r3, r2
	bgt	.La3c
	bl	OvlFunc_935_2008b8c
	mov	r3, #0xff
	lsl	r3, #16
	str	r3, [r5, #0xc]
	mov	r3, r5
	add	r3, #0x5b
	str	r7, [r5, #0x48]
	str	r7, [r5, #0x28]
	mov	r0, #0x6a
	strb	r7, [r3]
	bl	__PlaySound
.La3c:
	add	r6, #1
	cmp	r6, #3
	ble	.La0a
	mov	r0, #0xa
	bl	OvlFunc_935_2008944
	cmp	r0, #0
	beq	.La72
	mov	r0, #0xa
	mov	r1, #1
	bl	__MapActor_SetAnim
	ldr	r0, =0x207
	bl	__GetFlag
	cmp	r0, #0
	bne	.La6c
	ldr	r0, =0x207
	bl	__SetFlag
	mov	r0, #0xcc
	bl	__PlaySound
	b	.La72
.La6c:
	mov	r0, #0x6a
	bl	__PlaySound
.La72:
	mov	r0, #9
	bl	OvlFunc_935_2008944
	cmp	r0, #0
	beq	.La82
	mov	r0, #0x6a
	bl	__PlaySound
.La82:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_935_20089c0

@ 72 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, Random, UnsignedRem, GetSlotEntityChecked x2
@   TestSaveBit, PlaySound x2
.thumb_func_start OvlFunc_935_2008aa0
	push	{r5, r6, r7, lr}
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r3, r5
	add	r3, #0x5b
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.Lb3a
	ldr	r3, =.L222c
	ldr	r2, [r3]
	add	r2, #1
	str	r2, [r3]
	mov	r3, #0x3f
	and	r3, r2
	cmp	r3, #0
	bne	.Lade
	ldr	r5, =.L2230
	bl	__Random
	mov	r1, #6
	bl	_umodsi3_RAM
	str	r0, [r5]
	add	r0, #0xa
	bl	__MapActor_GetActor
	ldr	r3, =0xa3d
	mov	r5, r0
	str	r3, [r5, #0x48]
.Lade:
	mov	r7, #0xff
	mov	r6, #0
	lsl	r7, #16
.Lae4:
	mov	r0, r6
	add	r0, #0xa
	bl	__MapActor_GetActor
	mov	r2, #0x80
	lsl	r2, #2
	mov	r5, r0
	add	r0, r6, r2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb1a
	ldr	r3, [r5, #0x28]
	cmp	r3, #0
	bgt	.Lb0a
	ldr	r3, [r5, #0xc]
	ldr	r2, =0x20ffff
	cmp	r3, r2
	bgt	.Lb34
.Lb0a:
	mov	r3, #0
	str	r7, [r5, #0xc]
	str	r3, [r5, #0x48]
	str	r3, [r5, #0x28]
	mov	r0, #0x6a
	bl	__PlaySound
	b	.Lb34
.Lb1a:
	ldr	r3, [r5, #0x28]
	cmp	r3, #0
	bgt	.Lb28
	ldr	r3, [r5, #0xc]
	ldr	r2, =0xffff
	cmp	r3, r2
	bgt	.Lb34
.Lb28:
	str	r0, [r5, #0x48]
	str	r0, [r5, #0x28]
	str	r7, [r5, #0xc]
	mov	r0, #0x6a
	bl	__PlaySound
.Lb34:
	add	r6, #1
	cmp	r6, #5
	ble	.Lae4
.Lb3a:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_935_2008aa0
