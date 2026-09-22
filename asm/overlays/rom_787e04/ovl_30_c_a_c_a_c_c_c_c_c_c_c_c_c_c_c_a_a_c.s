	.include "macros.inc"
	.include "gba.inc"

@ Counter: INN 0x0 via Func_b3284, opened only from inside the facing arc.
@ Outside it the attendant speaks instead -- lines 0xf58, 0x11a9, 0x1c14, 0x1c15.
@ Gated on save bits 0x300, 0x815, 0x87a.
.thumb_func_start OvlFunc_887_2008e34
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r2, =0xffffe000
	ldrh	r3, [r0, #6]
	mov	r5, #0x90
	add	r3, r2
	lsl	r5, #8
	cmp	r3, r5
	bls	.Le54
	mov	r0, #0
	mov	r1, #0xd
	bl	__Func_80b3284
	b	.Led4
.Le54:
	bl	__CutsceneStart
	ldr	r0, =0x87a
	bl	__GetFlag
	cmp	r0, #0
	beq	.Leb0
	mov	r0, #0xd
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_809280c
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.Le96
	ldr	r0, =0x1c14
	bl	__MessageID
	mov	r0, #0xd
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__SetFlag
.Le96:
	ldr	r0, =0x1c15
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0xd
	bl	__Func_8093054
	mov	r0, #0xd
	mov	r1, r5
	mov	r2, #0xa
	bl	__Func_8092adc
	b	.Led0
.Leb0:
	ldr	r0, =0x815
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lec2
	ldr	r0, =0x11a9
	bl	__MessageID
	b	.Lec8
.Lec2:
	ldr	r0, =0xf58
	bl	__MessageID
.Lec8:
	mov	r0, #0xd
	mov	r1, #0
	bl	__ActorMessage
.Led0:
	bl	__CutsceneEnd
.Led4:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_887_2008e34
