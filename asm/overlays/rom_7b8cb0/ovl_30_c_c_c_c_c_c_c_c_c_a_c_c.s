	.include "macros.inc"

@ 89 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetMapTransition, SetEntityAnimation, PlaySound, SetMapTransition
@   PlaySound, SetEntityAnimation
.thumb_func_start OvlFunc_931_20086f0
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r6, r5
	add	r6, #0x66
	mov	r1, #0
	ldrsh	r3, [r6, r1]
	ldrh	r2, [r6]
	cmp	r3, #0
	beq	.L71e
	sub	r3, r2, #1
	mov	r2, #0x80
	strh	r3, [r6]
	lsl	r2, #9
	lsl	r3, #16
	cmp	r3, r2
	bne	.L71e
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_8012330
.L71e:
	ldr	r7, [r5, #0x28]
	cmp	r7, #0
	bne	.L766
	mov	r1, #1
	mov	r0, r5
	bl	__Actor_SetAnim
	ldr	r3, [r5, #0xc]
	ldr	r1, =0xfffe8000
	ldr	r2, [r5, #0x14]
	add	r3, r1
	str	r3, [r5, #0xc]
	cmp	r3, r2
	bge	.L75e
	ldr	r3, [r5, #0x68]
	cmp	r3, #0
	beq	.L75c
	mov	r0, #0xe5
	bl	__PlaySound
	mov	r3, #4
	mov	r1, #0x80
	mov	r2, #0x80
	str	r7, [r5, #0x68]
	lsl	r2, #9
	strh	r3, [r6]
	mov	r0, #0
	lsl	r1, #9
	bl	__Func_8012330
	ldr	r2, [r5, #0x14]
.L75c:
	str	r2, [r5, #0xc]
.L75e:
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #1
	b	.L76c
.L766:
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
.L76c:
	strb	r3, [r2]
	mov	r6, r5
	add	r6, #0x64
	mov	r1, #0
	ldrsh	r3, [r6, r1]
	ldrh	r2, [r6]
	cmp	r3, #0
	bne	.L796
	mov	r0, #0x98
	bl	__PlaySound
	mov	r3, #1
	mov	r0, r5
	mov	r1, #2
	str	r3, [r5, #0x68]
	bl	__Actor_SetAnim
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r5, #0x28]
	ldrh	r2, [r6]
.L796:
	add	r3, r2, #1
	mov	r2, #0xf0
	strh	r3, [r6]
	lsl	r2, #14
	lsl	r3, #16
	cmp	r3, r2
	bne	.L7a8
	mov	r3, #0
	strh	r3, [r6]
.L7a8:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_20086f0

@ 53 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SetSlotEntitySpeed, MoveSlotToAndWait x2, PlaySound
@   DespawnSlotEntity, SetMapTransition, DialogueWait, SetMapTransition
@   DialogueWait, SetSlotAnimation
.thumb_func_start OvlFunc_931_20087b8
	push	{lr}
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r1, r0
	mov	r3, r1
	mov	r2, #0
	add	r3, #0x64
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	ldr	r3, =0x6666
	str	r3, [r1, #0x48]
	ldr	r3, =OvlFunc_931_20086f0
	mov	r0, #0x12
	str	r3, [r1, #0x6c]
	ldr	r2, =0x9999
	ldr	r1, =0x13333
	bl	__MapActor_SetSpeed
	mov	r2, #0xe6
	mov	r0, #0x12
	mov	r1, #0x1c
	lsl	r2, #1
	bl	__Func_8092158
	mov	r2, #0xe0
	mov	r1, #0x18
	lsl	r2, #1
	mov	r0, #0x12
	bl	__Func_8092158
	mov	r0, #0xe5
	bl	__PlaySound
	mov	r0, #0x12
	bl	__DeleteFieldActor
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #9
	lsl	r2, #9
	mov	r0, #0
	bl	__Func_8012330
	mov	r0, #4
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #1
	neg	r1, r1
	ldr	r2, =0xe666
	neg	r0, r0
	bl	__Func_8012330
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0x12
	mov	r1, #1
	bl	__MapActor_SetAnim
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_20087b8
