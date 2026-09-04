	.include "macros.inc"

@ 269 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntity x2, OvlFunc_4704, Atan2, OvlFunc_4754
@   OvlFunc_4840, OvlFunc_48a4, SetEntityAnimation x2, GetSlotEntity
@   OvlFunc_4704, Random, OvlFunc_4754, OvlFunc_4840
@   OvlFunc_4754, OvlFunc_4840
@   ... and 15 more
.thumb_func_start OvlFunc_899_200c8c8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r0, #0
	sub	sp, #4
	bl	__GetFieldActor
	ldr	r3, =iwram_3001ebc
	ldr	r3, [r3]
	mov	r1, #0
	mov	r8, r0
	mov	r0, #2
	mov	r9, r1
	mov	r11, r3
	bl	__GetFieldActor
	mov	r7, r0
	mov	r5, r7
	add	r5, #8
	mov	r0, r5
	bl	OvlFunc_899_200c704
	mov	r10, r0
	cmp	r0, #0
	beq	.L49b8
	mov	r2, #0x80
	ldr	r3, [r7, #0x38]
	lsl	r2, #24
	cmp	r3, r2
	bne	.L49b8
	mov	r1, r8
	ldr	r2, [r5]
	ldr	r3, [r1, #8]
	sub	r6, r2, r3
	ldr	r2, [r7, #0x10]
	ldr	r3, [r1, #0x10]
	sub	r5, r2, r3
	mov	r2, #6
	ldrsh	r3, [r1, r2]
	mov	r1, #2
	add	r1, sp
	mov	r8, r1
	mov	r2, r8
	mov	r1, r6
	strh	r3, [r2]
	mov	r0, r5
	bl	__atan2
	mov	r3, #0xce
	lsl	r3, #1
	add	r3, r11
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	lsl	r0, #16
	asr	r0, #16
	asr	r6, #16
	asr	r5, #16
	cmp	r3, #0
	ble	.L4976
	mov	r4, r6
	mul	r4, r6
	mov	r1, r5
	mul	r1, r5
	mov	r2, #0xc8
	add	r3, r4, r1
	lsl	r2, #1
	cmp	r3, r2
	bgt	.L497e
	mov	r3, r8
	ldrh	r2, [r3]
	lsl	r3, r0, #16
	lsr	r3, #16
	sub	r2, r3
	lsl	r2, #16
	asr	r0, r2, #16
	ldr	r2, =0xfffff000
	cmp	r0, r2
	ble	.L497e
	mov	r3, #0x80
	lsl	r3, #5
	cmp	r0, r3
	bge	.L497e
	b	.L498c
.L4976:
	mov	r4, r6
	mul	r4, r6
	mov	r1, r5
	mul	r1, r5
.L497e:
	add	r3, r4, r1
	cmp	r3, #0x40
	ble	.L498c
	mov	r1, #6
	ldrsh	r3, [r7, r1]
	mov	r2, r8
	strh	r3, [r2]
.L498c:
	mov	r0, r10
	mov	r1, r8
	bl	OvlFunc_899_200c754
	mov	r5, r0
	bl	OvlFunc_899_200c840
	cmp	r0, #0
	bne	.L49b0
	mov	r0, r7
	mov	r1, r5
	bl	OvlFunc_899_200c8a4
	mov	r0, r7
	mov	r1, #2
	bl	__Actor_SetAnim
	b	.L49b8
.L49b0:
	mov	r0, r7
	mov	r1, #1
	bl	__Actor_SetAnim
.L49b8:
	mov	r0, #0x18
	bl	__GetFieldActor
	mov	r7, r0
	add	r0, #8
	bl	OvlFunc_899_200c704
	mov	r10, r0
	cmp	r0, #0
	beq	.L4a4c
	mov	r1, #0x80
	ldr	r3, [r7, #0x38]
	lsl	r1, #24
	cmp	r3, r1
	bne	.L4a4c
	bl	__Random
	lsl	r0, #1
	lsr	r0, #16
	lsl	r3, r0, #1
	add	r3, r0
	mov	r1, #0xd0
	lsl	r1, #24
	lsl	r3, #29
	ldrh	r2, [r7, #6]
	add	r3, r1
	mov	r6, sp
	lsr	r3, #16
	add	r6, #2
	add	r3, r2
	strh	r3, [r6]
	mov	r0, r10
	mov	r1, r6
	bl	OvlFunc_899_200c754
	mov	r5, r0
	bl	OvlFunc_899_200c840
	cmp	r0, #0
	beq	.L4a3c
	ldrh	r3, [r7, #6]
	mov	r2, #0x80
	lsl	r2, #8
	add	r3, r2
	strh	r3, [r6]
	mov	r0, r10
	mov	r1, r6
	bl	OvlFunc_899_200c754
	mov	r5, r0
	bl	OvlFunc_899_200c840
	cmp	r0, #0
	bne	.L4a2e
	mov	r0, #0x18
	mov	r1, #2
	bl	__MapActor_Surprise
	b	.L4a3c
.L4a2e:
	mov	r0, r7
	mov	r1, #4
	bl	__Actor_SetAnim
	mov	r3, #1
	mov	r9, r3
	b	.L4a4c
.L4a3c:
	mov	r0, r7
	mov	r1, r5
	bl	OvlFunc_899_200c8a4
	mov	r0, r7
	mov	r1, #2
	bl	__Actor_SetAnim
.L4a4c:
	mov	r0, #0x19
	bl	__GetFieldActor
	mov	r7, r0
	add	r0, #8
	bl	OvlFunc_899_200c704
	mov	r10, r0
	cmp	r0, #0
	beq	.L4ae2
	mov	r1, #0x80
	ldr	r3, [r7, #0x38]
	lsl	r1, #24
	cmp	r3, r1
	bne	.L4ae2
	bl	__Random
	lsl	r2, r0, #1
	add	r2, r0
	lsr	r2, #16
	lsl	r3, r2, #1
	add	r3, r2
	mov	r1, #0xd0
	lsl	r1, #24
	lsl	r3, #28
	ldrh	r2, [r7, #6]
	add	r3, r1
	mov	r6, sp
	lsr	r3, #16
	add	r6, #2
	add	r3, r2
	strh	r3, [r6]
	mov	r0, r10
	mov	r1, r6
	bl	OvlFunc_899_200c754
	mov	r5, r0
	bl	OvlFunc_899_200c840
	cmp	r0, #0
	beq	.L4ad2
	ldrh	r3, [r7, #6]
	mov	r2, #0x80
	lsl	r2, #8
	add	r3, r2
	strh	r3, [r6]
	mov	r0, r10
	mov	r1, r6
	bl	OvlFunc_899_200c754
	mov	r5, r0
	bl	OvlFunc_899_200c840
	cmp	r0, #0
	bne	.L4ac4
	mov	r0, #0x19
	mov	r1, #2
	bl	__MapActor_Surprise
	b	.L4ad2
.L4ac4:
	mov	r0, r7
	mov	r1, #4
	bl	__Actor_SetAnim
	mov	r3, #2
	add	r9, r3
	b	.L4ae2
.L4ad2:
	mov	r0, r7
	mov	r1, r5
	bl	OvlFunc_899_200c8a4
	mov	r0, r7
	mov	r1, #2
	bl	__Actor_SetAnim
.L4ae2:
	mov	r1, r9
	cmp	r1, #0
	beq	.L4b08
	ldr	r2, =.L64f8
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r2, #0xe8
	lsl	r3, #16
	lsl	r2, #13
	cmp	r3, r2
	bls	.L4b0e
	mov	r2, #0xc1
	mov	r3, r9
	lsl	r2, #1
	add	r3, #0xc8
	add	r2, r11
	strh	r3, [r2]
	b	.L4b0e
.L4b08:
	ldr	r3, =.L64f8
	mov	r1, r9
	strh	r1, [r3]
.L4b0e:
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_899_200c8c8
