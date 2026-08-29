	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 64 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_948_2009edc
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldr	r3, [r0, #0xc]
	lsl	r2, #13
	cmp	r3, r2
	ble	.L1f18
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, #2
	add	r0, #0x23
	strb	r5, [r0]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.L1f10
	mov	r0, #0xa
	bl	__MapActor_GetActor
	add	r0, #0x23
	strb	r5, [r0]
.L1f10:
	mov	r0, #0xb
	bl	__MapActor_GetActor
	b	.L1f60
.L1f18:
	mov	r0, #0xa
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.L1f44
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	cmp	r3, #0
	bge	.L1f34
	ldr	r2, =0xfffff
	add	r3, r2
.L1f34:
	asr	r3, #20
	cmp	r3, #0x38
	ble	.L1f44
	mov	r0, #0xa
	mov	r1, #3
	bl	__Func_8092b08
	b	.L1f58
.L1f44:
	mov	r0, #0xa
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r3, #1
	add	r0, #0x23
	strb	r3, [r0]
.L1f58:
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r5, #0
.L1f60:
	add	r0, #0x23
	strb	r5, [r0]
	mov	r0, #0xc
	bl	__MapActor_GetActor
	add	r0, #0x23
	strb	r5, [r0]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_2009edc
