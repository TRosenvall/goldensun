	.include "macros.inc"

@ WarpToTableEntry
@ Takes no arguments. The town's warp points, all sharing one handler.
@
@ First it clears the flag byte +0x55 on every live entity from slot 8 to 0x41
@ -- a blanket reset so nothing is mid-interaction across the warp. Then the
@ interaction target halfword at [iwram_1ebc]+0x16C, minus 0x0E, indexes the
@ eight-byte records at .L1dcc: a word destination followed by two halfword
@ coordinates, handed to Func_10560. Sound 0x9E plays throughout.
@
@ Subtracting 0x0E means trigger ids 0x0E upward map to entries 0, 1, 2 ...,
@ so the table is dense and the triggers are numbered consecutively.
.thumb_func_start OvlFunc_950_20080c0
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r6, [r3]
	mov	r5, #8
	mov	r7, #0
.Lca:
	mov	r0, r5
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lda
	mov	r3, r0
	add	r3, #0x55
	strb	r7, [r3]
.Lda:
	add	r5, #1
	cmp	r5, #0x41
	bls	.Lca
	mov	r3, #0xb6
	lsl	r3, #1
	add	r6, r3
	mov	r3, #0
	ldrsh	r5, [r6, r3]
	mov	r0, #0x9e
	sub	r5, #0xe
	bl	__PlaySound
	lsl	r5, #3
	ldr	r0, =.L1dcc
	add	r3, r5, #4
	ldrh	r1, [r0, r3]
	add	r3, r0
	ldrh	r2, [r3, #2]
	ldr	r0, [r0, r5]
	bl	__Func_8010560
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #7
	lsl	r1, #8
	mov	r0, #0
	bl	__MapActor_SetSpeed
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r1, #2
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r3, #0
	ldrsh	r0, [r6, r3]
	bl	__Func_8091e9c
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_950_20080c0

@ Cutscene: the town's arrival scene, roughly 180 instructions.
@ Actors are created with CreateActor and DeleteActor rather than taken from the
@ object table, walked in at speeds 0x1CCCC/0xB333 and 0x16666/0xE666, and put
@ through a conversation from message base 0x2394 with formation changes and
@ interaction effect 0x101.
.thumb_func_start OvlFunc_950_200813c
	push	{r5, lr}
	bl	__CutsceneStart
	ldr	r0, =0x2394
	bl	__MessageID
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0x8e
	mov	r1, #0x96
	mov	r3, #0xce
	lsl	r3, #18
	mov	r2, #0
	lsl	r1, #18
	lsl	r0, #1
	bl	__CreateActor
	mov	r1, #0
	mov	r5, r0
	bl	__Actor_SetSpriteFlags
	mov	r0, r5
	mov	r1, #6
	bl	__Actor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, r5
	bl	__Actor_SetAnim
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, r5
	bl	__DeleteActor
	mov	r0, #2
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0x19
	lsl	r1, #1
	mov	r2, #0x32
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x19
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x96
	mov	r2, #0xd4
	mov	r0, #0x19
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0xc0
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #0x19
	bl	__Func_8092adc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0x19
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #2
	mov	r0, #0x19
	bl	__Func_80925cc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0x8e
	mov	r2, #0xd4
	mov	r0, #0x19
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #0x19
	bl	__Func_8092adc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0x84
	lsl	r1, #1
	mov	r2, #0x32
	mov	r0, #0x19
	bl	__MapActor_Emote
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0x10
	mov	r1, #0
	neg	r2, r2
	mov	r0, #0
	bl	__Func_8092304
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r2, #0
	lsl	r1, #6
	mov	r0, #0x19
	bl	__Func_8092adc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #0x19
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0x19
	bl	__ActorMessage
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0x32
	ldr	r1, =0x101
	mov	r0, #0
	bl	__MapActor_Emote
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, #0x19
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0x19
	bl	__ActorMessage
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r2, #0x32
	mov	r0, #0x19
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #0x19
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0x19
	ldr	r1, =0x16666
	ldr	r2, =0xb333
	bl	__MapActor_SetSpeed
	mov	r0, #0x19
	mov	r1, #0x10
	mov	r2, #0
	bl	__Func_8092304
	mov	r2, #0x20
	mov	r1, #0
	mov	r0, #0x19
	bl	__Func_8092304
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0x19
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x19
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0
	mov	r1, #0x10
	mov	r2, #0
	bl	__Func_8092304
	mov	r1, #0x80
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x19
	ldr	r1, =0x1cccc
	ldr	r2, =0xe666
	bl	__MapActor_SetSpeed
	mov	r0, #0x19
	mov	r1, #0
	mov	r2, #0x30
	bl	__Func_8092304
	mov	r0, #0x19
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_950_200813c
