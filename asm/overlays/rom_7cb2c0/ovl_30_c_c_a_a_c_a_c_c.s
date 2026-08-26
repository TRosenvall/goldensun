	.include "macros.inc"

@ Leaf helper, 36 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1ebc
.thumb_func_start OvlFunc_945_2009144
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xc
	mov	r4, #0xc
	ldr	r3, [r3]
	neg	r2, r2
	neg	r4, r4
	add	r4, r1
	add	r2, r0
	mov	r6, r0
	mov	r5, #8
	mov	r14, r2
	add	r6, #0xc
	mov	r12, r4
	add	r1, #0xc
	add	r3, #0x34
.L1164:
	ldmia	r3!, {r0}
	mov	r7, #0xa
	ldrsh	r2, [r0, r7]
	mov	r7, #0x12
	ldrsh	r4, [r0, r7]
	cmp	r14, r2
	bge	.L117e
	cmp	r6, r2
	ble	.L117e
	cmp	r12, r4
	bge	.L117e
	cmp	r1, r4
	bgt	.L1186
.L117e:
	add	r5, #1
	cmp	r5, #0x41
	bls	.L1164
	mov	r0, #0
.L1186:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_945_2009144

@ Cutscene: roughly 98 instructions of straight-line script --
@ 0 turns, 2 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_945_2009190
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #1
	mov	r10, r0
	mov	r1, #2
	mov	r0, r5
	mov	r8, r2
	bl	__Func_8092b08
	mov	r0, r5
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r0]
	mov	r2, r10
	ldrh	r3, [r2, #6]
	mov	r2, #0x80
	lsl	r2, #7
	mov	r7, #0xf0
	add	r3, r2
	lsl	r7, #8
	and	r3, r7
	asr	r6, r3, #12
	mov	r0, r6
	bl	OvlFunc_945_2009280
	cmp	r0, #0
	beq	.L11de
	mov	r3, #0
	mov	r8, r3
.L11de:
	mov	r2, r8
	cmp	r2, #0
	beq	.L1212
	mov	r2, r10
	ldrh	r3, [r2, #6]
	ldr	r2, =0xffffc000
	add	r3, r2
	and	r3, r7
	asr	r6, r3, #12
	mov	r0, r6
	bl	OvlFunc_945_2009280
	cmp	r0, #0
	beq	.L11fe
	mov	r3, #0
	mov	r8, r3
.L11fe:
	mov	r2, r8
	cmp	r2, #0
	beq	.L1212
	mov	r2, r10
	ldrh	r3, [r2, #6]
	mov	r2, #0x80
	lsl	r2, #8
	add	r3, r2
	and	r3, r7
	asr	r6, r3, #12
.L1212:
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1226
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, r5
	bl	__MapActor_SetPos
.L1226:
	mov	r0, r5
	ldr	r2, =0xcccc
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	mov	r0, r5
	mov	r1, #2
	bl	__MapActor_SetAnim
	ldr	r2, =.L6668
	lsl	r3, r6, #2
	ldr	r2, [r2, r3]
	asr	r1, r2, #16
	lsl	r2, #16
	asr	r2, #16
	mov	r0, r5
	bl	__Func_809228c
	mov	r0, r5
	bl	__MapActor_WaitMovement
	mov	r0, r5
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r3, r10
	ldrh	r1, [r3, #6]
	mov	r0, r5
	bl	OvlFunc_945_200c880
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2009190

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_1144, CheckTerrainStep
.thumb_func_start OvlFunc_945_2009280
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r0, #0
	sub	sp, #0xc
	bl	__MapActor_GetActor
	ldr	r3, =.L6668
	lsl	r5, #2
	mov	r6, r0
	ldr	r3, [r3, r5]
	mov	r2, #0xa
	ldrsh	r1, [r6, r2]
	asr	r2, r3, #16
	add	r5, r1, r2
	lsl	r3, #16
	mov	r1, #0x12
	ldrsh	r2, [r6, r1]
	asr	r3, #16
	add	r7, r2, r3
	mov	r0, r5
	mov	r1, r7
	bl	OvlFunc_945_2009144
	cmp	r0, #0
	bne	.L12ca
	mov	r1, sp
	lsl	r3, r5, #16
	str	r3, [r1]
	ldr	r3, [r6, #0xc]
	str	r3, [r1, #4]
	lsl	r3, r7, #16
	str	r3, [r1, #8]
	mov	r0, r6
	bl	__TestCollision
	cmp	r0, #0
	beq	.L12ce
.L12ca:
	mov	r0, #0
	b	.L12d0
.L12ce:
	mov	r0, #1
.L12d0:
	add	sp, #0xc
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_945_2009280
