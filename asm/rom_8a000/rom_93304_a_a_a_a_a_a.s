	.include "macros.inc"

@ SetDialoguePortrait
@ r0=speaker slot, or 0x80000000 for "no portrait".
@ Writes the portrait id at iwram_1e8c+0x12F4 and its style byte at +0x12F6.
@ The sentinel clears both; otherwise the id comes from Func_92ba8 /
@ GetSpriteVoice and the style is looked up in .L9fc28 by the text-speed setting at
@ ewram_240+0x20C.
.thumb_func_start Func_8093304  @ 0x08093304
	push	{r5, lr}
	ldr	r3, =iwram_3001e8c
	mov	r1, #0x80
	lsl	r1, #24
	ldr	r5, [r3]
	cmp	r0, r1
	bne	.L9331e
	ldr	r2, =0x12f4
	ldr	r1, =0x12f6
	add	r3, r5, r2
	mov	r2, #0
	strh	r2, [r3]
	b	.L9333c
.L9331e:
	bl	Func_8092ba8
	bl	GetSpriteVoice
	ldr	r3, =gState
	mov	r1, #0x83
	lsl	r1, #2
	add	r3, r1
	ldrb	r3, [r3]
	ldr	r1, =0x12f4
	ldr	r2, =.L9fc28
	ldrb	r2, [r2, r3]
	add	r3, r5, r1
	add	r1, #2
	strh	r0, [r3]
.L9333c:
	add	r3, r5, r1
	strh	r2, [r3]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_8093304
