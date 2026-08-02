	.include "macros.inc"

@ Counter: shop 8, speaker slot 0x11.
@ The before-0x845 branch is the elaborate one -- line 0x13E5, the shopkeeper
@ turns to face the player, a ten-frame beat, a question through Func_93054,
@ then a turn to 0x3000. After the bit is set it collapses to a single line.
.thumb_func_start OvlFunc_908_2008124
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	ldrh	r5, [r0, #6]
	bl	__CutsceneStart
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L146
	mov	r0, #8
	mov	r1, #0x11
	bl	__Func_80b0278
	b	.L18a
.L146:
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	bne	.L17c
	ldr	r0, =0x13e5
	bl	__MessageID
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x11
	bl	__Func_809280c
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0x11
	bl	__Func_8093054
	mov	r1, #0xc0
	mov	r0, #0x11
	lsl	r1, #6
	mov	r2, #0xa
	bl	__Func_8092adc
	b	.L18a
.L17c:
	ldr	r0, =0x16f7
	bl	__MessageID
	mov	r0, #0x11
	mov	r1, #0
	bl	__ActorMessage
.L18a:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_908_2008124

@ Talk: slot 0x15, line 0x13ED. Turns to face the player, speaks, then
@ settles back to angle 0xC000.
.thumb_func_start OvlFunc_908_20081a8
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =0x13ed
	bl	__MessageID
	mov	r2, #0
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_809280c
	mov	r0, #0x15
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_908_20081a8

