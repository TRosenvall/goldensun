	.include "macros.inc"

@ GetActiveMessagePortrait
@ Returns the portrait id for the current speaker, biased by 0x100, or 0 when
@ there is none. Requires the portrait-enable byte at ewram_240+0x20A to be set
@ and the speaker record from GetSpriteVoiceEntry to carry a portrait at +0x02 other than
@ the 0xFF "none" marker.
.thumb_func_start GetSpriteVoice  @ 0x080915ac
	push	{lr}
	ldr	r3, =gState
	ldr	r2, =0x20a
	add	r3, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L915c4
	bl	GetSpriteVoiceEntry
	ldrb	r0, [r0, #2]
	cmp	r0, #0xff
	bne	.L915c8
.L915c4:
	mov	r0, #0
	b	.L915ce
.L915c8:
	mov	r3, #0x80
	lsl	r3, #1
	add	r0, r3
.L915ce:
	pop	{r1}
	bx	r1
.func_end GetSpriteVoice

