	.include "macros.inc"

@ Counter: shop 0xa via Func_b0278, opened only from inside the facing arc.
@ Outside it the attendant speaks instead -- lines 0x153e, 0x164b.
@ Gated on save bit 0x881.
.thumb_func_start OvlFunc_921_20087a4
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #6
	ldrsh	r5, [r0, r2]
	ldr	r0, =0x881
	bl	__GetFlag
	cmp	r0, #0
	beq	.L804
	ldr	r2, =0x5fff0000
	lsl	r3, r5, #16
	add	r3, r2
	ldr	r2, =0x3ffe0000
	cmp	r3, r2
	bhi	.L7d0
	mov	r0, #0xa
	mov	r1, #0xc
	bl	__Func_80b0278
	b	.L86c
.L7d0:
	bl	__CutsceneStart
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xc
	bl	__Func_809280c
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x164b
	bl	__MessageID
	mov	r0, #0xc
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0xc
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	bl	__CutsceneEnd
	b	.L86c
.L804:
	ldr	r2, =0x5fff0000
	lsl	r3, r5, #16
	add	r3, r2
	ldr	r2, =0x3ffe0000
	cmp	r3, r2
	bhi	.L86c
	bl	__CutsceneStart
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #11
	lsl	r1, #8
	bl	__Func_80933d4
	mov	r0, #0xd5
	mov	r1, #1
	mov	r2, #0xf6
	lsl	r2, #17
	mov	r3, #1
	neg	r1, r1
	lsl	r0, #17
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r1, =gScript_921__0200a5ec
	mov	r0, #0xc
	bl	__MapActor_RunScript
	ldr	r0, =0x153e
	bl	__MessageID
	mov	r0, #0xc
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0xd5
	mov	r1, #1
	mov	r2, #0x9a
	lsl	r0, #17
	neg	r1, r1
	lsl	r2, #18
	mov	r3, #1
	bl	__Func_80933f8
	bl	__Func_8093530
	bl	__CutsceneEnd
.L86c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_921_20087a4
