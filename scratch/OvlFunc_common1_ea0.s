	.include "macros.inc"
	.include "gba.inc"

.thumb_func_start OvlFunc_common1_ea0
	push	{r5, lr}
	mov	r5, r0
	cmp	r5, #0
	bne	.Leda
	bl	__CutsceneStart
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0x59
	bl	__PlaySound
	mov	r0, #0
	bl	OvlFunc_common1_88c
	mov	r0, #1
	mov	r1, #0
	bl	OvlFunc_common1_e10
	mov	r0, #0x78
	bl	__CutsceneWait
	bl	__CutsceneEnd
	b	.Lf98
.Leda:
	mov	r0, #0xf7
	bl	__PlaySound
	bl	__CutsceneStart
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	lsl	r2, r5, #4
	ldr	r3, =.L11
	sub	r2, r5
	lsl	r2, #2
	strh	r2, [r3, #0x1e]
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r5
	add	r0, #0x5a
	bl	__PlaySound
	mov	r0, r5
	bl	OvlFunc_common1_88c
	mov	r0, #1
	mov	r1, #0
	bl	OvlFunc_common1_e10
	mov	r0, #0x78
	bl	__CutsceneWait
	b	.Lf20
.Lf1a:
	mov	r0, #1
	bl	__WaitFrames
.Lf20:
	bl	__Func_80f954c
	cmp	r0, #0
	bne	.Lf1a
	ldr	r0, =0x121
	bl	__PlaySound
	mov	r0, #5
	bl	OvlFunc_common1_88c
	mov	r1, #0
	mov	r0, #2
	bl	OvlFunc_common1_e10
	mov	r0, #0xec
	bl	__PlaySound
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #2
	bl	OvlFunc_common1_e10
	mov	r0, #0xec
	bl	__PlaySound
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #6
	bl	OvlFunc_common1_88c
	mov	r1, #0
	mov	r0, #2
	bl	OvlFunc_common1_e10
	mov	r0, #0xec
	bl	__PlaySound
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #7
	bl	OvlFunc_common1_88c
	mov	r1, #0
	mov	r0, #4
	bl	OvlFunc_common1_e10
	mov	r0, #0xed
	bl	__PlaySound
	bl	__PlayMapMusic
	bl	__CutsceneEnd
	ldr	r0, =0x123
	bl	__SetFlag
.Lf98:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_common1_ea0
