	.include "macros.inc"

@ 88 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, GetSlotEntityChecked, PlaySound x2, PlayMapRectAnimation
@   SetSlotEntitySpeed x2, SetSlotAnimation, SetSlotDrawPriority, MoveSlotBy
@   GetSlotEntityChecked, WalkSlotThroughDoorway, DialogueWait, SetPendingMessageId
@   EndCutscene
.thumb_func_start OvlFunc_921_20086c0
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r7, [r3]
	bl	__CutsceneStart
	mov	r5, #8
	mov	r6, #0
.L6ce:
	mov	r0, r5
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L6de
	mov	r3, r0
	add	r3, #0x55
	strb	r6, [r3]
.L6de:
	add	r5, #1
	cmp	r5, #0x41
	bls	.L6ce
	mov	r2, #0xb6
	lsl	r2, #1
	add	r3, r7, r2
	ldrh	r3, [r3]
	sub	r3, #0x32
	lsl	r3, #16
	asr	r5, r3, #16
	cmp	r5, #6
	bne	.L6fe
	mov	r0, #0xbc
	bl	__PlaySound
	b	.L704
.L6fe:
	mov	r0, #0x9e
	bl	__PlaySound
.L704:
	ldr	r2, =.L31a8
	lsl	r3, r5, #2
	sub	r0, r3, #4
	sub	r3, #2
	ldrsh	r1, [r2, r0]
	ldrsh	r2, [r2, r3]
	ldr	r3, =.L3190
	ldr	r0, [r3, r0]
	bl	__Func_8010560
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #7
	mov	r0, #0
	lsl	r1, #8
	bl	__MapActor_SetSpeed
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	cmp	r5, #6
	bne	.L760
	ldr	r2, =0x1999
	mov	r0, #0
	ldr	r1, =0x3333
	bl	__MapActor_SetSpeed
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	mov	r1, #3
	bl	__Func_8092b08
	mov	r2, #8
	mov	r0, #0
	mov	r1, #0
	neg	r2, r2
	bl	__Func_809228c
	b	.L778
.L760:
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	mov	r2, #0x10
	strb	r3, [r0]
	mov	r1, #3
	mov	r0, #0
	neg	r2, r2
	bl	__Func_8092208
.L778:
	mov	r0, #0x10
	bl	__CutsceneWait
	mov	r0, r5
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_921_20086c0

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

@ Counter: shop 0xb via Func_b0278, opened only from inside the facing arc.
@ Outside it the attendant speaks instead -- lines 0x1543, 0x164d.
@ Gated on save bit 0x881.
.thumb_func_start OvlFunc_921_200888c
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #6
	ldrsh	r5, [r0, r2]
	ldr	r0, =0x881
	bl	__GetFlag
	cmp	r0, #0
	beq	.L8ec
	ldr	r2, =0x5fff0000
	lsl	r3, r5, #16
	add	r3, r2
	ldr	r2, =0x3ffe0000
	cmp	r3, r2
	bhi	.L8b8
	mov	r0, #0xb
	mov	r1, #0xd
	bl	__Func_80b0278
	b	.L954
.L8b8:
	bl	__CutsceneStart
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xd
	bl	__Func_809280c
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x164d
	bl	__MessageID
	mov	r0, #0xd
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0xd
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	bl	__CutsceneEnd
	b	.L954
.L8ec:
	ldr	r2, =0x5fff0000
	lsl	r3, r5, #16
	add	r3, r2
	ldr	r2, =0x3ffe0000
	cmp	r3, r2
	bhi	.L954
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
	mov	r0, #0xd
	bl	__MapActor_RunScript
	ldr	r0, =0x1543
	bl	__MessageID
	mov	r0, #0xd
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
.L954:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_921_200888c

@ Counter: shop 0xc via Func_b0278, opened only from inside the facing arc.
@ Outside it the attendant speaks instead -- lines 0x1546, 0x1547, 0x164f.
@ Gated on save bit 0x881.
.thumb_func_start OvlFunc_921_2008974
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #6
	ldrsh	r5, [r0, r2]
	ldr	r0, =0x881
	bl	__GetFlag
	cmp	r0, #0
	beq	.L9ce
	ldr	r2, =0x5fff0000
	lsl	r3, r5, #16
	add	r3, r2
	ldr	r2, =0x3ffe0000
	cmp	r3, r2
	bhi	.L9a0
	mov	r0, #0xc
	mov	r1, #0xf
	bl	__Func_80b0278
	b	.La1e
.L9a0:
	bl	__CutsceneStart
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xf
	bl	__Func_809280c
	ldr	r0, =0x164f
	bl	__MessageID
	mov	r0, #0xf
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0xf
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	bl	__CutsceneEnd
	b	.La1e
.L9ce:
	ldr	r2, =0x5fff0000
	lsl	r3, r5, #16
	add	r3, r2
	ldr	r2, =0x3ffe0000
	cmp	r3, r2
	bhi	.L9fa
	bl	__CutsceneStart
	ldr	r0, =0x1546
	bl	__MessageID
	mov	r0, #0xe
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0xe
	mov	r0, #0xc
	bl	__Func_80b0278
	bl	__CutsceneEnd
	b	.La1e
.L9fa:
	mov	r2, #0xa
	mov	r1, #0
	mov	r0, #0xe
	bl	__Func_809280c
	ldr	r0, =0x1547
	bl	__MessageID
	mov	r0, #0xe
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0xa0
	mov	r0, #0xe
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
.La1e:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_921_2008974
