	.include "macros.inc"
	.include "gba.inc"

@ 123 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, SetSlotAnimation, DialogueWait, PlaySound
@   SetEntityAnimation, SetEntityMoveTarget, DialogueWait, SetSlotAnimation
@   SetSlotEntitySpeed, SetEntityAnimation, SetEntityMoveTarget, WaitForEntityIdle
@   SetEntityAnimation, WaitForEntityIdle
@   ... and 3 more
.thumb_func_start OvlFunc_955_2009898
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =gState
	mov	r10, r2
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r3, [r3]
	mov	r9, r3
	mov	r5, r0
	mov	r0, r9
	mov	r6, r1
	sub	sp, #4
	bl	__MapActor_GetActor
	mov	r8, r0
	mov	r0, r5
	bl	__MapActor_GetActor
	mov	r7, r0
	ldr	r1, [r7, #8]
	lsr	r3, r6, #31
	add	r3, r6, r3
	asr	r2, r1, #20
	asr	r3, #1
	mov	r0, #0
	cmp	r2, r3
	beq	.L18dc
	mov	r0, #1
.L18dc:
	mov	r3, r10
	lsl	r3, #16
	lsl	r6, #16
	mov	r10, r3
	cmp	r0, #0
	beq	.L18f8
	sub	r3, r6, r1
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	mov	r2, #0
	mov	r11, r3
	str	r2, [sp]
	b	.L190a
.L18f8:
	mov	r3, #0
	mov	r11, r3
	ldr	r3, [r7, #0x10]
	mov	r2, r10
	sub	r3, r2, r3
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	str	r3, [sp]
.L190a:
	mov	r1, #8
	mov	r0, r9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__CutsceneWait
	ldr	r5, =0x3333
	mov	r3, #0x80
	lsl	r3, #8
	str	r3, [r7, #0x30]
	str	r5, [r7, #0x34]
	mov	r0, #0xef
	bl	__PlaySound
	mov	r0, r7
	mov	r1, #3
	bl	__Actor_SetAnim
	mov	r3, r10
	mov	r1, r6
	mov	r2, #0
	mov	r0, r7
	bl	__Actor_TravelTo
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, r9
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #0x80
	mov	r2, r5
	mov	r0, r9
	lsl	r1, #8
	bl	__MapActor_SetSpeed
	mov	r0, r8
	mov	r1, #2
	bl	__Actor_SetAnim
	mov	r2, r8
	ldr	r1, [r2, #8]
	ldr	r3, [r2, #0x10]
	ldr	r2, [sp]
	add	r1, r11
	add	r3, r2
	mov	r0, r8
	mov	r2, #0
	bl	__Actor_TravelTo
	mov	r0, r8
	bl	__Actor_WaitMovement
	mov	r0, r8
	mov	r1, #1
	bl	__Actor_SetAnim
	mov	r0, r7
	bl	__Actor_WaitMovement
	mov	r1, #1
	mov	r0, r7
	bl	__Actor_SetAnim
	mov	r0, #0x90
	lsl	r0, #1
	bl	__PlaySound
	mov	r0, #0xd5
	bl	__PlaySound
	mov	r0, #0xf
	bl	__CutsceneWait
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_955_2009898

@ Cutscene: roughly 148 instructions of straight-line script --
@ 1 turn, 0 animation changes, 4 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x20ad, 0x20ae.
.thumb_func_start OvlFunc_955_20099bc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r7, r0
	cmp	r3, #2
	bne	.L19da
	bl	OvlFunc_common1_2c4
	b	.L1b1a
.L19da:
	bl	__CutsceneStart
	mov	r0, r7
	mov	r1, #5
	bl	OvlFunc_common1_4cc
	mov	r8, r0
	cmp	r0, #0
	beq	.L19ee
	b	.L1af8
.L19ee:
	ldr	r0, =0x20ae
	bl	__MessageID
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #10
	lsl	r1, #7
	bl	__Func_80933d4
	mov	r0, #0xa4
	mov	r1, #1
	mov	r2, #0x84
	lsl	r2, #17
	mov	r3, #1
	neg	r1, r1
	lsl	r0, #17
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #9
	lsl	r1, #6
	bl	__Func_80933d4
	mov	r0, #0x9c
	mov	r1, #1
	mov	r2, #0xb0
	mov	r3, #1
	lsl	r2, #16
	lsl	r0, #17
	neg	r1, r1
	mov	r5, #0xcc
	mov	r6, #0x84
	bl	__Func_80933f8
	lsl	r5, #1
	bl	__Func_8093530
	lsl	r6, #1
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, r5
	mov	r2, r6
	mov	r0, #0
	bl	OvlFunc_common1_1078
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, r5
	mov	r2, #0xd8
	mov	r0, #0
	bl	OvlFunc_common1_15b8
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0xb4
	lsl	r1, #1
	mov	r0, #0x10
	mov	r2, #0xd0
	bl	OvlFunc_955_2009898
	mov	r0, #0
	mov	r1, r6
	mov	r2, #0x2d
	bl	__MapActor_Emote
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	sub	r5, #0x20
	bl	__MapActor_SetSpeed
	mov	r1, r5
	mov	r2, #0xd8
	mov	r0, #0
	bl	OvlFunc_common1_15b8
	mov	r1, r5
	mov	r2, #0xf8
	mov	r0, #0
	bl	OvlFunc_common1_15b8
	mov	r1, #0x9c
	mov	r2, #0xf8
	lsl	r1, #1
	mov	r0, #0
	bl	OvlFunc_common1_15b8
	mov	r1, #0
	mov	r0, r7
	bl	__ActorMessage
	mov	r0, #0
	bl	OvlFunc_common1_1254
	mov	r0, #0
	mov	r1, #0
	bl	__SetCameraTarget
	mov	r1, #0xc4
	mov	r2, #0xd0
	mov	r0, #0x10
	lsl	r1, #17
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r0, r7
	mov	r1, #5
	bl	OvlFunc_common1_588
	b	.L1b0c
.L1af8:
	mov	r3, r8
	cmp	r3, #1
	bne	.L1b0c
	ldr	r0, =0x20ad
	bl	__MessageID
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
.L1b0c:
	mov	r1, r7
	mov	r2, #5
	mov	r0, r8
	bl	OvlFunc_common1_5e4
	bl	__CutsceneEnd
.L1b1a:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_955_20099bc

	.section .data
	.global .L40c0

.L40c0:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x40c0, (0x40c4-0x40c0)

	.section .data1
	.global .L4834
	.global .L4838
	.global gOvl_0200c474
	.global gOvl_0200c48c
	.global gOvl_0200c414

gOvl_0200c414:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4414, (0x4474-0x4414)
gOvl_0200c474:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4474, (0x448c-0x4474)
gOvl_0200c48c:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x448c, (0x4834-0x448c)
.L4834:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4834, (0x4838-0x4834)
.L4838:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4838, (0x483c-0x4838)
	.global gOvl_0200c83c
gOvl_0200c83c:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x483c, (0x4a1c-0x483c)
