	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 517 instructions of straight-line script --
@ 0 turns, 6 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x330, 0x331, 0x332.
@ Sets save bit 0x144.
.thumb_func_start OvlFunc_955_2008bd4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001ebc
	mov	r1, #0xe0
	ldr	r3, [r3]
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0
	mov	r0, #0xa2
	str	r2, [r3]
	lsl	r0, #1
	sub	sp, #0xc
	bl	__SetFlag
	mov	r3, #0x64
	mov	r2, #0xb
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0xb
	mov	r2, #0xc
	mov	r3, #4
	bl	__Func_8010704
	mov	r3, #0x78
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x30
	mov	r2, #5
	mov	r1, #0xa
	mov	r3, #6
	bl	__Func_8010704
	mov	r2, #2
	mov	r6, #0x1a
	mov	r7, #0
	mov	r8, r2
.Lc24:
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r1, #4
	mov	r5, r0
	bl	__Actor_SetAnim
	mov	r3, r5
	add	r3, #0x55
	strb	r7, [r3]
	mov	r1, r8
	sub	r3, #0x32
	add	r6, #1
	str	r7, [r5, #0xc]
	strb	r1, [r3]
	cmp	r6, #0x1e
	ble	.Lc24
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r3, #2
	add	r0, #0x23
	strb	r3, [r0]
	mov	r0, #0xcc
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lca0
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	mov	r3, #0xa8
	mov	r5, r0
	lsl	r3, #17
	str	r3, [r5, #8]
	ldr	r3, =0xfff80000
	str	r3, [r5, #0xc]
	mov	r3, #0x84
	lsl	r3, #17
	str	r3, [r5, #0x10]
	mov	r2, #0x10
	mov	r3, #0x14
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x13
	mov	r1, #0x10
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r3, #0x15
	mov	r2, #0x50
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x14
	mov	r1, #0x50
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	b	.Lcb4
.Lca0:
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	mov	r1, #3
	mov	r5, r0
	bl	__Actor_SetAnim
	mov	r3, #0x80
	lsl	r3, #13
	str	r3, [r5, #0xc]
.Lcb4:
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r3, #2
	mov	r8, r3
	add	r0, #0x23
	mov	r1, r8
	mov	r2, #0
	strb	r1, [r0]
	ldr	r0, =0x335
	mov	r10, r2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lce6
	mov	r3, #0x23
	mov	r2, #0x4d
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x23
	mov	r1, #0x4e
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Lce6:
	ldr	r0, =0x333
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ld0c
	mov	r0, #0x13
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r3, #0x20
	mov	r2, #0x4d
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x20
	mov	r1, #0x25
	mov	r2, #1
	mov	r3, #4
	bl	__Func_8010704
.Ld0c:
	ldr	r0, =0x331
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ld4a
	mov	r0, #0x14
	bl	__MapActor_GetActor
	mov	r2, r10
	add	r0, #0x55
	strb	r2, [r0]
	mov	r0, #0x14
	bl	__MapActor_GetActor
	mov	r3, r8
	add	r0, #0x23
	strb	r3, [r0]
	mov	r1, #5
	mov	r0, #0x14
	bl	__MapActor_SetAnim
	mov	r3, #0x2c
	mov	r2, #0x11
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x11
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Ld4a:
	ldr	r0, =0x332
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ld88
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r1, r10
	add	r0, #0x55
	strb	r1, [r0]
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r2, r8
	add	r0, #0x23
	strb	r2, [r0]
	mov	r1, #5
	mov	r0, #0x15
	bl	__MapActor_SetAnim
	mov	r3, #0x32
	mov	r2, #0x11
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x11
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Ld88:
	mov	r0, #0x20
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r3, [r5, #8]
	asr	r2, r3, #20
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r10
	strb	r1, [r3]
	sub	r3, #0x32
	mov	r1, r8
	strb	r1, [r3]
	mov	r3, #0xa
	str	r2, [sp]
	str	r3, [sp, #4]
	mov	r1, #0x1c
	mov	r2, #1
	mov	r3, #3
	mov	r0, #0x34
	bl	__Func_8010704
	mov	r0, #0x21
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r3, [r5, #8]
	asr	r2, r3, #20
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r10
	strb	r1, [r3]
	sub	r3, #0x32
	mov	r1, r8
	strb	r1, [r3]
	mov	r3, #0xd
	str	r2, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x34
	mov	r1, #0x1c
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
	mov	r0, #0xd0
	lsl	r0, #2
	bl	__GetFlagByte
	mov	r6, r0
	cmp	r6, #0
	bne	.Ldf0
	mov	r6, #0x49
.Ldf0:
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r2, #0x80
	lsl	r3, r6, #20
	lsl	r2, #12
	mov	r5, r0
	add	r3, r2
	str	r3, [r5, #8]
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r10
	strb	r1, [r3]
	mov	r2, r8
	sub	r3, #0x32
	strb	r2, [r3]
	mov	r0, #0x47
	mov	r1, #0x10
	mov	r2, #1
	mov	r3, #1
	mov	r7, #0x10
	str	r6, [sp]
	str	r7, [sp, #4]
	bl	__Func_8010704
	mov	r0, #0xd2
	lsl	r0, #2
	bl	__GetFlagByte
	mov	r6, r0
	cmp	r6, #0
	bne	.Le32
	mov	r6, #0x4c
.Le32:
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r1, #0x80
	lsl	r3, r6, #20
	lsl	r1, #12
	mov	r5, r0
	add	r3, r1
	str	r3, [r5, #8]
	mov	r3, r5
	add	r3, #0x55
	mov	r2, r10
	strb	r2, [r3]
	mov	r1, r8
	sub	r3, #0x32
	strb	r1, [r3]
	mov	r0, #0x47
	mov	r1, #0x10
	mov	r2, #1
	mov	r3, #1
	str	r6, [sp]
	str	r7, [sp, #4]
	bl	__Func_8010704
	mov	r0, #0xd4
	lsl	r0, #2
	bl	__GetFlagByte
	mov	r6, r0
	cmp	r6, #0
	bne	.Le72
	mov	r6, #0x4f
.Le72:
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r2, #0x80
	lsl	r2, #12
	lsl	r3, r6, #20
	mov	r5, r0
	add	r3, r2
	str	r3, [r5, #8]
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r10
	strb	r1, [r3]
	mov	r2, r8
	sub	r3, #0x32
	strb	r2, [r3]
	mov	r0, #0x47
	mov	r1, #0x10
	mov	r2, #1
	mov	r3, #1
	str	r6, [sp]
	str	r7, [sp, #4]
	bl	__Func_8010704
	bl	OvlFunc_955_200862c
	mov	r0, #0x1f
	mov	r1, #0xa
	bl	__MapActor_SetAnim
	mov	r0, #0xcd
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf02
	mov	r3, #0xd
	mov	r6, #0x16
	mov	r8, r3
	mov	r7, #0x3a
.Lec2:
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r2, r5
	add	r2, #0x23
	mov	r3, #2
	strb	r3, [r2]
	mov	r1, #4
	bl	__Actor_SetAnim
	mov	r1, r8
	str	r1, [sp, #4]
	mov	r0, #0x38
	mov	r1, #0xd
	mov	r2, #1
	mov	r3, #1
	add	r6, #1
	str	r7, [sp]
	bl	__Func_8010704
	add	r7, #2
	cmp	r6, #0x19
	ble	.Lec2
	mov	r0, #0x1f
	mov	r1, #0xa
	bl	__MapActor_SetAnim
	mov	r0, #0x1f
	bl	__Func_809ad90
	b	.Lf40
.Lf02:
	mov	r2, #2
	mov	r7, #0x80
	mov	r6, #0x16
	mov	r8, r2
	lsl	r7, #8
.Lf0c:
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r3, r5
	add	r3, #0x23
	mov	r1, r8
	strb	r1, [r3]
	mov	r1, #4
	bl	__Actor_SetAnim
	ldr	r3, =0x3333
	add	r6, #1
	str	r7, [r5, #0x30]
	str	r3, [r5, #0x34]
	cmp	r6, #0x19
	ble	.Lf0c
	ldr	r5, =OvlFunc_955_2008714
	ldr	r1, =0xc85
	mov	r0, r5
	bl	__StartTask
	mov	r0, r5
	mov	r1, #1
	bl	__Func_8004358
.Lf40:
	mov	r0, #8
	mov	r1, #9
	bl	__MapActor_SetAnim
	ldr	r5, =gState
	mov	r3, #0xf9
	lsl	r3, #1
	add	r2, r5, r3
	mov	r3, #0
	strb	r3, [r2]
	mov	r1, #0x59
	mov	r0, #0x29
	bl	OvlFunc_common1_1608
	mov	r1, #0x4d
	mov	r0, #0x28
	bl	OvlFunc_common1_1608
	mov	r1, #1
	mov	r0, #8
	bl	__Func_8092950
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r5, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #1
	cmp	r3, #4
	bls	.Lf7e
	b	.L10c0
.Lf7e:
	ldr	r2, =.Lf88
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.Lf88:
	.word	.Lfc8
	.word	.L105c
	.word	.L108e
	.word	.L10a4
	.word	.L10b2

	.pool_aligned

.Lfc8:
	mov	r2, #0x80
	lsl	r2, #17
	str	r2, [sp]
	mov	r2, #0x28
	str	r2, [sp, #4]
	mov	r3, #0xd0
	mov	r2, #0x29
	str	r2, [sp, #8]
	lsl	r3, #15
	mov	r1, #8
	mov	r2, #5
	mov	r0, #0
	bl	OvlFunc_common1_1ecc
	mov	r3, #0x4f
	mov	r2, #6
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x7f
	bl	__Func_8010788
	mov	r0, #0x22
	bl	__DeleteFieldActor
	mov	r0, #0x23
	bl	__DeleteFieldActor
	mov	r0, #0x24
	bl	__DeleteFieldActor
	mov	r0, #0x25
	bl	__DeleteFieldActor
	mov	r0, #0x26
	bl	__DeleteFieldActor
	mov	r0, #0x27
	bl	__DeleteFieldActor
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L103c
	mov	r0, #0x11
	bl	__PlaySound
	mov	r0, #0
	bl	OvlFunc_common1_78
	bl	OvlFunc_common1_0
	mov	r0, #2
	bl	OvlFunc_common1_ea0
.L103c:
	mov	r0, #1
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r0, #2
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r0, #3
	mov	r1, #0
	bl	__MapActor_SetExtra
	ldr	r0, =0xe5
	bl	OvlFunc_common1_1fb4
	b	.L10c0
.L105c:
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_common1_148
	bl	__StartTask
	mov	r0, #0x28
	bl	__DeleteFieldActor
	mov	r0, #0x29
	bl	__DeleteFieldActor
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L10c0
	bl	OvlFunc_common1_0
	mov	r0, #1
	bl	OvlFunc_common1_78
	mov	r0, #0
	bl	OvlFunc_common1_ea0
	b	.L10c0
.L108e:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L10c0
	mov	r0, #0x22
	bl	OvlFunc_955_20090dc
	bl	OvlFunc_common1_488
	b	.L10c0
.L10a4:
	mov	r0, #2
	bl	OvlFunc_955_2008b38
	mov	r0, #4
	bl	__Func_8091e9c
	b	.L10c0
.L10b2:
	mov	r0, #2
	neg	r0, r0
	bl	OvlFunc_955_2008b38
	mov	r0, #5
	bl	__Func_8091e9c
.L10c0:
	mov	r0, #0
	add	sp, #0xc
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_955_2008bd4

@ Cutscene: roughly 207 instructions of straight-line script --
@ 1 turn, 8 animation changes, 4 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0x20e9.
.thumb_func_start OvlFunc_955_20090dc
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r0
	bl	__MapActor_GetActor
	mov	r3, #0xa
	ldrsh	r2, [r0, r3]
	mov	r9, r2
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r10, r2
	bl	__CutsceneStart
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, r7
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #2
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #3
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r3, r10
	lsl	r5, r3, #16
	mov	r2, r9
	ldr	r3, =0xffd00000
	lsl	r6, r2, #16
	mov	r1, r6
	add	r2, r5, r3
	mov	r0, #0
	bl	__MapActor_SetPos
	ldr	r3, =0xffd80000
	ldr	r2, =0xfff00000
	add	r3, r5
	mov	r8, r3
	add	r1, r6, r2
	mov	r0, #1
	mov	r2, r8
	bl	__MapActor_SetPos
	mov	r2, #0x80
	lsl	r2, #13
	add	r1, r6, r2
	mov	r0, #2
	mov	r2, r8
	bl	__MapActor_SetPos
	ldr	r3, =0xffe00000
	mov	r1, r6
	add	r2, r5, r3
	mov	r0, #3
	bl	__MapActor_SetPos
	ldr	r2, =0xffb00000
	add	r5, r2
	mov	r2, r5
	mov	r1, r6
	mov	r0, r7
	bl	__MapActor_SetPos
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r6, #0xc0
	lsl	r6, #8
	mov	r1, #0
	strh	r6, [r0, #6]
	mov	r0, #0
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	ldr	r0, =0x20e9
	bl	__MessageID
	mov	r0, r7
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #3
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1238
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.L1238:
	mov	r0, #2
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1258
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #2
	bl	__MapActor_TravelTo
.L1258:
	mov	r0, #3
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1278
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #3
	bl	__MapActor_TravelTo
.L1278:
	mov	r5, r9
	sub	r5, #0x10
	mov	r2, r10
	mov	r0, r7
	mov	r1, r5
	sub	r2, #0x40
	bl	__Func_80921c4
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #2
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #3
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, r10
	mov	r0, r7
	mov	r1, r5
	sub	r2, #0x10
	bl	__Func_80921c4
	mov	r0, r7
	mov	r1, r9
	mov	r2, r10
	bl	__Func_80921c4
	mov	r0, r7
	mov	r1, r6
	mov	r2, #0xa
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_955_20090dc
