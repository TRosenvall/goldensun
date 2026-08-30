	.include "macros.inc"
	.include "gba.inc"

@ 280 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, CloseMessageWindow, FindEquippedInParty, SetActiveMessageId
@   OpenMessageBoxForSlot, OpenWindow, DrawStringToBuffer, DrawNumberEntry
@   DrawStringToBuffer, DrawNumberEntry, RunSubScreenE, CloseWindow
@   RunMenuModalSimple, SetActiveMessageId
@   ... and 27 more
@ message id 0xe46.
.thumb_func_start OvlFunc_951_2008ac8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r3, #0
	mov	r10, r3
	sub	sp, #4
	bl	__CutsceneStart
	bl	__Func_808e118
	ldr	r3, =0xe43
	mov	r9, r3
	ldr	r3, =gState
	mov	r11, r3
.Laec:
	mov	r3, r11
	ldr	r3, [r3, #0x10]
	mov	r0, #0xe5
	mov	r8, r3
	bl	__Func_8078b60
	mov	r7, r0
	mov	r0, r9
	bl	__MessageID
	mov	r0, #1
	mov	r1, #0
	neg	r0, r0
	bl	__Func_8092c40
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #0
	mov	r2, #0x11
	mov	r3, #4
	mov	r0, #0
	bl	__CreateUIBox
	ldr	r5, =0xe49
	mov	r6, r0
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0
	bl	__Func_801e7c0
	mov	r3, #0
	str	r3, [sp]
	mov	r0, r8
	mov	r1, #6
	mov	r2, r6
	mov	r3, #0x48
	bl	__Func_801ea08
	add	r0, r5, #1
	mov	r1, r6
	mov	r2, #0
	mov	r3, #8
	bl	__Func_801e7c0
	mov	r3, #8
	str	r3, [sp]
	mov	r2, r6
	mov	r3, #0x48
	mov	r1, #6
	mov	r0, r7
	bl	__Func_801ea08
	mov	r0, r10
	bl	__LuckyFountainMenu
	mov	r1, #2
	mov	r10, r0
	mov	r0, r6
	bl	__CloseUIBox
	bl	__Func_8019a54
	mov	r3, #1
	neg	r3, r3
	cmp	r10, r3
	bne	.Lb74
	b	.Ld34
.Lb74:
	mov	r3, r10
	cmp	r3, #0
	bne	.Lb86
	mov	r3, r8
	cmp	r3, #0
	bne	.Lbdc
	mov	r0, r9
	add	r0, #1
	b	.Lb94
.Lb86:
	mov	r3, r10
	cmp	r3, #1
	bne	.Lbdc
	cmp	r7, #0
	bne	.Lbb4
	mov	r0, r9
	add	r0, #2
.Lb94:
	bl	__MessageID
	mov	r0, #1
	neg	r0, r0
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #1
	bl	__WaitFrames
	b	.Laec
.Lbaa:
	mov	r0, #0x70
	bl	__PlaySound
	mov	r5, #0
	b	.Lc3c
.Lbb4:
	bl	__Func_8078550
	cmp	r0, #0
	bne	.Lbdc
	mov	r0, r9
	add	r0, #4
	bl	__MessageID
	mov	r0, #1
	mov	r1, #0
	neg	r0, r0
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.Lbdc
	b	.Ld34
.Lbdc:
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #0xf
	mov	r2, #9
	mov	r3, #4
	mov	r0, #0x14
	bl	__CreateUIBox
	ldr	r5, =0xe4c
	mov	r6, r0
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0
	bl	__Func_801e7c0
	add	r0, r5, #1
	mov	r1, r6
	mov	r2, #0
	mov	r3, #8
	bl	__Func_801e7c0
	mov	r0, #5
	bl	__WaitFrames
	mov	r0, #0x74
	bl	__PlaySound
	ldr	r5, =gKeyPress
	mov	r7, #1
	b	.Lc20
.Lc1a:
	mov	r0, #1
	bl	__WaitFrames
.Lc20:
	ldr	r3, [r5]
	and	r3, r7
	cmp	r3, #0
	bne	.Lbaa
	ldr	r3, [r5]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lc1a
	mov	r0, #0x71
	bl	__PlaySound
	mov	r5, #1
	neg	r5, r5
.Lc3c:
	mov	r0, r6
	mov	r1, #2
	bl	__CloseUIBox
	mov	r3, #1
	neg	r3, r3
	cmp	r5, r3
	beq	.Ld34
	mov	r3, r10
	cmp	r3, #0
	bne	.Lc5c
	mov	r0, #1
	neg	r0, r0
	bl	__AddCoins
	b	.Lc68
.Lc5c:
	mov	r3, r10
	cmp	r3, #1
	bne	.Lc68
	mov	r0, #0xe5
	bl	__Func_80789dc
.Lc68:
	mov	r0, r10
	bl	OvlFunc_951_200973c
	mov	r3, r10
	mov	r5, r0
	cmp	r3, #0
	bne	.Lcb2
	cmp	r5, #4
	beq	.Lca4
	ldr	r6, =.L200c
	lsl	r5, #1
	ldrh	r0, [r6, r5]
	bl	__AddCoins
	mov	r0, #0x5b
	bl	__PlaySound
	mov	r1, #5
	ldrh	r0, [r6, r5]
	bl	__Func_8019908
	ldr	r0, =0xe46
	bl	__MessageID
	mov	r0, #1
	neg	r0, r0
	mov	r1, #0
	bl	__ActorMessage
	b	.Ld34
.Lca4:
	mov	r0, #0x71
	bl	__PlaySound
	mov	r0, #0xa
	bl	__CutsceneWait
	b	.Ld34
.Lcb2:
	lsl	r3, r5, #1
	add	r3, r5
	mov	r6, #0
	add	r0, r3, #3
	mov	r7, #0
	cmp	r6, r0
	bge	.Lcd6
	ldr	r2, =0x11d
	mov	r12, r0
	add	r2, r11
.Lcc6:
	ldrb	r3, [r2]
	lsl	r3, #24
	asr	r3, #24
	add	r6, #1
	add	r2, #1
	add	r7, r3
	cmp	r6, r12
	blt	.Lcc6
.Lcd6:
	bl	__Random
	mov	r3, r7
	mul	r3, r0
	mov	r1, r11
	lsr	r2, r3, #16
	mov	r3, #0x8e
	lsl	r3, #1
	add	r1, #1
	ldrsb	r3, [r1, r3]
	sub	r2, r3
	mov	r6, #0
	cmp	r2, #0
	blt	.Ld08
	ldr	r1, =0x11d
	add	r1, r11
.Lcf6:
	add	r6, #1
	cmp	r6, #0xe
	bgt	.Ld08
	add	r1, #1
	mov	r3, #0
	ldrsb	r3, [r1, r3]
	sub	r2, r3
	cmp	r2, #0
	bge	.Lcf6
.Ld08:
	cmp	r6, #0xf
	bne	.Ld0e
	mov	r6, #0xe
.Ld0e:
	ldr	r2, =gLuckyFountainPrizes
	lsl	r3, r6, #2
	ldr	r0, [r2, r3]
	bl	OvlFunc_951_20084bc
	mov	r3, #0x8e
	lsl	r3, #1
	mov	r0, r11
	add	r1, r6, r3
	add	r0, #1
	ldrb	r3, [r0, r1]
	lsl	r3, #24
	asr	r2, r3, #24
	cmp	r2, #1
	ble	.Ld34
	lsr	r3, #31
	add	r3, r2, r3
	asr	r3, #1
	strb	r3, [r0, r1]
.Ld34:
	bl	__CutsceneEnd
	mov	r0, #0
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_951_2008ac8
