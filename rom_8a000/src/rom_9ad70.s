	.include "macros.inc"

@ ============================================================================
@ Field ability (Psynergy) effects.
@
@ iwram_1f30 points at the field-effect state: +0x10 the caster entity, +0x14
@ the target position, +0x18/+0x1A the target slot and parameter, and an array
@ of effect instances from +0x58 (0x48 bytes each, updated by Func_9b804 and
@ friends in rom_9b698.s).
@ Each ability is a blocking routine: it opens with Func_916b0, animates the
@ caster and its particles, applies the world change, and closes with
@ Func_91750.
@ ============================================================================

@ IdleFlickerHook
@ r0=entity. Per-frame hook that gives a resting entity a subtle palette
@ flicker: draws a random index from Func_4458, uses it to pick a signed byte
@ from .L9f160 and applies it as the palette through _Func_c598.
.thumb_func_start Func_9ad70
	push	{r5, r6, lr}
	mov	r6, r0
	ldr	r5, =L9f160
	bl	Func_4458
	lsl	r0, #3
	lsr	r0, #16
	ldrsb	r1, [r5, r0]
	mov	r0, r6
	bl	_Func_c598
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_9ad70

@ SaveAndClearEntityHook
@ r0=slot. Saves the slot entity's current per-frame hook (+0x6C) to
@ ewram_240+0x250 and clears it, so a scripted sequence can take over the
@ entity's behaviour and restore it afterwards.
.thumb_func_start Func_9ad90
	push	{lr}
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L9add6
	ldr	r1, =ewram_240
	mov	r3, #0x94
	lsl	r3, #2
	add	r2, r1, r3
	ldr	r3, [r0, #0x6c]
	str	r3, [r2]
	ldr	r3, =0x249
	add	r1, r3
	mov	r3, #0
	strb	r3, [r1]
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L9adc4
	ldr	r3, [r0, #0x50]
	ldr	r3, [r3, #0x28]
	cmp	r3, #0
	beq	.L9adc4
	ldrb	r3, [r3, #5]
	strb	r3, [r1]
.L9adc4:
	ldr	r3, =Func_9ad70
	mov	r2, r0
	str	r3, [r0, #0x6c]
	add	r2, #0x5b
	mov	r3, #1
	strb	r3, [r2]
	mov	r1, #0
	bl	_Func_c344
.L9add6:
	pop	{r0}
	bx	r0
.func_end Func_9ad90

@ RestoreEntityHook
@ r0=slot. Undoes Func_9ad90: puts the saved hook from ewram_240+0x250 back at
@ +0x6C. If the current hook is Func_9ad70 it is cleared first, so the idle
@ flicker does not survive the restore.
.thumb_func_start Func_9ade8
	push	{r5, lr}
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L9ae28
	ldr	r2, [r5, #0x6c]
	ldr	r3, =Func_9ad70
	cmp	r2, r3
	bne	.L9ae18
	ldr	r2, =ewram_240
	mov	r3, #0x94
	lsl	r3, #2
	add	r1, r2, r3
	ldr	r3, [r1]
	str	r3, [r5, #0x6c]
	mov	r3, #0
	str	r3, [r1]
	ldr	r3, =0x249
	add	r2, r3
	mov	r1, #0
	ldrsb	r1, [r2, r1]
	bl	_Func_c598
.L9ae18:
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	mov	r0, r5
	mov	r1, #0x10
	bl	_Func_c344
.L9ae28:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_9ade8

@ GetSlotVoiceOrNone
@ r0=slot. Returns the slot if its speaker record carries a voice id
@ (Func_915dc returns something other than 0xFF), and -1 otherwise.
.thumb_func_start Func_9ae3c
	push	{r5, lr}
	mov	r5, r0
	bl	Func_915dc
	cmp	r0, #0xff
	bne	.L9ae4e
	mov	r0, #1
	neg	r0, r0
	b	.L9ae50
.L9ae4e:
	mov	r0, r5
.L9ae50:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_9ae3c
