	.include "macros.inc"

@ 45 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Cos, Sin
.thumb_func_start OvlFunc_888_200a6f0
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r5, r0
	mov	r2, #0x64
	add	r2, r5
	ldrh	r6, [r2]
	ldr	r1, [r5, #0x68]
	mov	r0, r6
	mov	r8, r2
	mov	r10, r1
	bl	__cos
	mov	r1, r10
	lsl	r2, r0, #3
	ldr	r3, [r1, #8]
	sub	r2, r0
	lsl	r2, #1
	add	r3, r2
	str	r3, [r5, #8]
	mov	r0, r6
	bl	__sin
	mov	r1, r10
	lsl	r3, r0, #2
	ldr	r2, [r1, #0x10]
	add	r3, r0
	lsl	r3, #1
	add	r2, r3
	ldr	r3, [r5, #8]
	str	r2, [r5, #0x10]
	str	r2, [r5, #0x40]
	str	r3, [r5, #0x38]
	mov	r2, r8
	add	r5, #0x66
	ldrh	r3, [r2]
	ldrh	r2, [r5]
	mov	r1, r8
	add	r3, r2
	strh	r3, [r1]
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_888_200a6f0

@ 54 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SpawnEntity, SetEntityScript
.thumb_func_start OvlFunc_888_200a750
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r1
	bl	__MapActor_GetActor
	mov	r7, r0
	cmp	r7, #0
	beq	.L27c8
	ldr	r2, [r7, #0xc]
	mov	r3, #0xb4
	lsl	r3, #14
	add	r2, r3
	ldr	r1, [r7, #8]
	ldr	r3, [r7, #0x10]
	ldr	r0, =0x11d
	bl	__CreateActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L27c8
	ldr	r1, =gScript_888__0200c15c
	ldr	r5, [r6, #0x50]
	bl	__Actor_SetScript
	mov	r3, r6
	mov	r2, #0
	add	r3, #0x55
	strb	r2, [r3]
	add	r3, #0xf
	strh	r2, [r3]
	add	r3, #2
	mov	r2, r8
	strh	r2, [r3]
	ldr	r3, =OvlFunc_888_200a6f0
	ldr	r1, .L27b8	@ 0
	str	r3, [r6, #0x6c]
	mov	r3, r5
	add	r3, #0x26
	strb	r1, [r3]
	ldr	r3, [r7, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	ldrb	r1, [r5, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r1
	orr	r3, r2
	str	r7, [r6, #0x68]
	strb	r3, [r5, #9]
	b	.L27c8

	.align	2, 0
.L27b8:
	.word	0
	.pool

.L27c8:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_888_200a750

@ 125 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaceSlotAt x7, StartFadeOut, WaitForFade, DialogueWait
@   OpenWindow, DrawStringAt, PlaySelectSound, DrawStringAt x2
@   ReserveScreenTiles, ReleaseScreenTiles x2, DialogueWait, .gcc2_compiled.
@   CloseWindow
.thumb_func_start OvlFunc_888_200a7d4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	sub	sp, #0x14
	bl	__MapActor_SetPos
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	bl	__MapActor_SetPos
	mov	r0, #0x80
	mov	r1, #2
	lsl	r0, #9
	bl	__Func_8091200
	mov	r0, #1
	bl	__Func_8091254
	mov	r0, #1
	bl	__CutsceneWait
	mov	r3, #1
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #0x19
	mov	r3, #5
	mov	r0, #2
	bl	__CreateUIBox
	ldr	r5, =0x116e
	mov	r7, r0
	mov	r1, r7
	mov	r0, r5
	mov	r2, #0x10
	mov	r3, #0
	bl	__DrawSmallText
	mov	r0, #1
	bl	__Func_801f730
	cmp	r0, #0
	bne	.L2870
	add	r0, r5, #2
	mov	r1, r7
	mov	r2, #0x10
	mov	r3, #0x10
	bl	__DrawSmallText
	b	.L287c
.L2870:
	add	r0, r5, #1
	mov	r1, r7
	mov	r2, #0x10
	mov	r3, #0x10
	bl	__DrawSmallText
.L287c:
	add	r1, sp, #4
	add	r0, sp, #8
	bl	__Func_801c0dc
	mov	r2, #0x3c
	add	r0, sp, #8
	mov	r1, #0x48
	bl	__Func_801c154
	ldr	r3, =gKeyPress
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	mov	r5, #0
	cmp	r3, #0
	bne	.L28dc
	ldr	r2, =.L411c
	mov	r6, #1
	mov	r8, r2
.L28a2:
	ldr	r3, =gKeyRepeat
	ldr	r3, [r3]
	mov	r2, #0xc0
	and	r3, r2
	cmp	r3, #0
	beq	.L28b0
	eor	r5, r6
.L28b0:
	ldr	r3, =iwram_3001800
	ldr	r3, [r3]
	mov	r2, #0xf
	lsr	r3, #1
	and	r3, r2
	lsl	r3, #2
	mov	r2, r8
	ldr	r1, [r2, r3]
	lsl	r2, r5, #4
	add	r0, sp, #8
	add	r1, #0x18
	add	r2, #0x3c
	bl	__Func_801c154
	mov	r0, #1
	bl	__CutsceneWait
	ldr	r3, =gKeyPress
	ldr	r3, [r3]
	and	r3, r6
	cmp	r3, #0
	beq	.L28a2
.L28dc:
	ldr	r0, [sp, #4]
	bl	__Func_801c17c
	mov	r0, r7
	mov	r1, #1
	bl	__CloseUIBox
	mov	r0, r5
	add	sp, #0x14
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_888_200a7d4
