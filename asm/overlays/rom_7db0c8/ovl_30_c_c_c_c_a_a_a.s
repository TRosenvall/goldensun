	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 348 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x301, 0x302, 0x303.
@ Sets save bit 0x144.
.thumb_func_start OvlFunc_954_2008a3c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
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
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r1, [r5, #8]
	ldr	r2, [r5, #0x10]
	mov	r0, #0
	bl	__Func_8011f54
	ldr	r3, [r5, #0xc]
	cmp	r3, #0
	bne	.Laa0
	cmp	r0, #0
	bne	.Laa0
	mov	r2, r5
	add	r2, #0x23
	mov	r3, #2
	strb	r3, [r2]
	mov	r3, r5
	add	r3, #0x55
	strb	r0, [r3]
	ldr	r2, [r5, #8]
	ldr	r3, [r5, #0x10]
	asr	r2, #20
	asr	r3, #20
	str	r2, [sp]
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0xd
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Laa0:
	mov	r0, #0xc4
	lsl	r0, #2
	bl	__GetFlagByte
	mov	r6, r0
	cmp	r6, #0
	bne	.Lab0
	mov	r6, #0x19
.Lab0:
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r2, #0x80
	lsl	r2, #12
	mov	r9, r2
	lsl	r3, r6, #20
	mov	r5, r0
	add	r3, r9
	mov	r1, #0
	str	r3, [r5, #8]
	mov	r8, r1
	mov	r3, r5
	add	r3, #0x55
	mov	r2, r8
	strb	r2, [r3]
	mov	r7, #2
	sub	r3, #0x32
	strb	r7, [r3]
	mov	r3, #0xc
	str	r3, [sp, #4]
	mov	r2, #1
	mov	r0, #0xe
	mov	r1, #0xd
	mov	r10, r3
	mov	r3, #1
	str	r6, [sp]
	bl	__Func_8010704
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_954_200804c
	bl	__StartTask
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r2, r5
	add	r2, #0x22
	mov	r3, #1
	strb	r3, [r2]
	ldr	r0, =0x303
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb40
	mov	r0, r5
	mov	r1, #4
	bl	__Actor_SetAnim
	mov	r0, r5
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r3, r5
	add	r3, #0x59
	mov	r1, r8
	strb	r1, [r3]
	mov	r2, #3
	sub	r3, #0x36
	strb	r2, [r3]
	mov	r3, #0x2f
	mov	r2, r10
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Lb40:
	mov	r0, #0x11
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r3, [r5, #0x10]
	asr	r2, r3, #20
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r8
	strb	r1, [r3]
	sub	r3, #0x32
	strb	r7, [r3]
	mov	r3, #0x40
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x18
	mov	r2, #3
	mov	r3, #1
	mov	r0, #0x40
	bl	__Func_8010704
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r3, [r5, #8]
	asr	r2, r3, #20
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r8
	strb	r1, [r3]
	sub	r3, #0x32
	strb	r7, [r3]
	mov	r3, #9
	str	r2, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x3f
	mov	r1, #0x19
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lbd6
	mov	r6, #0x22
	mov	r5, #7
	mov	r0, #0x25
	mov	r1, #7
	mov	r2, #1
	mov	r3, #4
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x25
	str	r3, [sp]
	mov	r0, #0x24
	mov	r1, #7
	mov	r2, #1
	mov	r3, #4
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x26
	str	r3, [sp, #4]
	mov	r0, #0x64
	mov	r1, #0x1d
	mov	r2, #1
	mov	r3, #3
	str	r6, [sp]
	bl	__Func_80105d4
.Lbd6:
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r0, =0x301
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc1a
	mov	r3, #0x29
	mov	r2, r10
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2b
	mov	r1, #0xc
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r3, r5
	add	r3, #0x55
	mov	r1, r8
	strb	r1, [r3]
	ldr	r3, =0x6666
	str	r3, [r5, #0x34]
	ldr	r3, =0xcccc
	mov	r2, r9
	str	r3, [r5, #0x30]
	str	r2, [r5, #0xc]
	mov	r0, r5
	mov	r1, #3
	bl	__Actor_SetAnim
	b	.Lc22
.Lc1a:
	mov	r0, r5
	mov	r1, #2
	bl	__Actor_SetAnim
.Lc22:
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, #2
	add	r0, #0x23
	strb	r3, [r0]
	mov	r1, #0x78
	mov	r0, #0x18
	bl	OvlFunc_common1_1608
	mov	r1, #0x7f
	mov	r0, #0x19
	bl	OvlFunc_common1_1608
	ldr	r3, =gState
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #1
	cmp	r3, #4
	bls	.Lc52
	b	.Ld74
.Lc52:
	ldr	r2, =.Lc5c
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.Lc5c:
	.word	.Lc70
	.word	.Ld10
	.word	.Ld42
	.word	.Ld58
	.word	.Ld66
.Lc70:
	mov	r2, #0xc0
	lsl	r2, #16
	str	r2, [sp]
	mov	r2, #0x18
	str	r2, [sp, #4]
	mov	r3, #0xa3
	mov	r2, #0x19
	str	r2, [sp, #8]
	lsl	r3, #19
	mov	r1, #8
	mov	r2, #4
	mov	r0, #0
	bl	OvlFunc_common1_1ecc
	mov	r3, #0x13
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x7f
	bl	__Func_8010788
	mov	r0, #0x13
	bl	__DeleteFieldActor
	mov	r0, #0x14
	bl	__DeleteFieldActor
	mov	r0, #0x15
	bl	__DeleteFieldActor
	mov	r0, #0x16
	bl	__DeleteFieldActor
	mov	r0, #0x17
	bl	__DeleteFieldActor
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lcf0
	mov	r0, #0x11
	bl	__PlaySound
	mov	r0, #0
	bl	OvlFunc_common1_78
	bl	OvlFunc_common1_0
	mov	r0, #1
	bl	OvlFunc_954_2008a10
	mov	r0, #2
	bl	OvlFunc_954_2008a10
	mov	r0, #3
	bl	OvlFunc_954_2008a10
	mov	r0, #1
	bl	OvlFunc_common1_ea0
.Lcf0:
	mov	r0, #1
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r0, #2
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r0, #3
	mov	r1, #0
	bl	__MapActor_SetExtra
	ldr	r0, =0xe4
	bl	OvlFunc_common1_1fb4
	b	.Ld74
.Ld10:
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_common1_148
	bl	__StartTask
	mov	r0, #0x18
	bl	__DeleteFieldActor
	mov	r0, #0x19
	bl	__DeleteFieldActor
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.Ld74
	bl	OvlFunc_common1_0
	mov	r0, #1
	bl	OvlFunc_common1_78
	mov	r0, #0
	bl	OvlFunc_common1_ea0
	b	.Ld74
.Ld42:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.Ld74
	mov	r0, #0x13
	bl	OvlFunc_954_2008db8
	bl	OvlFunc_common1_488
	b	.Ld74
.Ld58:
	mov	r0, #1
	bl	OvlFunc_954_2008974
	mov	r0, #4
	bl	__Func_8091e9c
	b	.Ld74
.Ld66:
	mov	r0, #1
	neg	r0, r0
	bl	OvlFunc_954_2008974
	mov	r0, #5
	bl	__Func_8091e9c
.Ld74:
	mov	r0, #0
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_954_2008a3c
