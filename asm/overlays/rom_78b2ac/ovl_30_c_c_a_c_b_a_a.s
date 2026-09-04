	.include "macros.inc"

@ Cutscene: roughly 517 instructions of straight-line script --
@ 21 turns, 16 animation changes, 1 dialogue line, 15 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xfe3, 0xff0, 0xff1, 0xff2.
@ Reads save bits 0x809, 0x814.
@ Sets save bits 0x144, 0x809.
.thumb_func_start OvlFunc_890_2008488
	push	{lr}
	ldr	r0, =0x814
	bl	__GetFlag
	cmp	r0, #0
	beq	.L498
	bl	OvlFunc_890_2008054
.L498:
	ldr     r0, =0x809
	bl	__GetFlag
	cmp	r0, #0
	beq	.L4a4
	b	.L9da
.L4a4:
	bl	__CutsceneStart
	ldr	r0, =0xfe3
	bl	__MessageID
	mov	r0, #0x11
	bl	__PlaySound
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x90
	mov	r2, #0xe8
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r1, #0
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x15
	bl	__PlaySound
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L502
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x10
	bl	__MapActor_SetPos
.L502:
	mov	r0, #0x10
	ldr	r1, =0x16666
	ldr	r2, =0xb333
	bl	__MapActor_SetSpeed
	mov	r1, #0x90
	lsl	r1, #1
	mov	r2, #0xce
	mov	r0, #0x10
	bl	__Func_80921c4
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #4
	mov	r2, #0x3c
	bl	__MapActor_Jump
	mov	r0, #0x10
	mov	r1, #0x14
	bl	OvlFunc_890_200a5fc
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L550
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__MapActor_SetPos
.L550:
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L564
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #5
	bl	__MapActor_SetPos
.L564:
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #8
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #8
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r1, #0x8c
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0xf8
	bl	__Func_809218c
	mov	r1, #0x94
	mov	r2, #0xf8
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r0, #1
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0xd0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r2, #0x1e
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_8092adc
	ldr	r0, =0x9999
	ldr	r1, =0x1333
	bl	__Func_80933d4
	mov	r0, #0x90
	mov	r1, #1
	mov	r2, #0xd5
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	lsl	r2, #16
	bl	__Func_80933f8
	mov	r0, #0x10
	ldr	r1, =0x6666
	ldr	r2, =0x3333
	bl	__MapActor_SetSpeed
	mov	r1, #0x90
	mov	r2, #0xb0
	lsl	r1, #1
	mov	r0, #0x10
	bl	__Func_80921c4
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0x10
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x10
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x80
	mov	r2, #0x3c
	mov	r0, #0x10
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r0, #0x10
	mov	r1, #0x14
	bl	OvlFunc_890_200a5fc
	mov	r2, #0x28
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_8092adc
	mov	r1, #3
	mov	r0, #0x10
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0x10
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #3
	mov	r0, #0x10
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #5
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0x90
	mov	r2, #0xa
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #5
	mov	r1, #0xa
	bl	OvlFunc_890_200a5fc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0xf0
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #5
	bl	__MapActor_Surprise
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0xa0
	mov	r2, #0xa
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_8092adc
	ldr	r0, =0x2005
	mov	r1, #0xa
	bl	OvlFunc_890_200a5fc
	mov	r1, #2
	mov	r0, #0x10
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #0x10
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x10
	bl	__MapActor_Surprise
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xa0
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r2, #0x14
	mov	r0, #1
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r1, #0
	mov	r0, #1
	bl	__ActorMessage
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, #0x10
	bl	__MapActor_DoAnim
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0x10
	mov	r1, #0x14
	bl	OvlFunc_890_200a5fc
	mov	r2, #0x28
	mov	r0, #5
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r0, #5
	mov	r1, #0x3c
	bl	OvlFunc_890_200a5fc
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x10
	mov	r1, #0xa
	bl	OvlFunc_890_200a5fc
	mov	r0, #0
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #1
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r2, #0x3c
	mov	r0, #5
	ldr	r1, =0x105
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #1
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #0xa
	bl	OvlFunc_890_200a5fc
	mov	r1, #3
	mov	r0, #0x10
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #5
	lsl	r1, #1
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r2, #0x50
	mov	r0, #0x10
	ldr	r1, =0x105
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xf0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x90
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x3c
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #0x10
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r1, #3
	mov	r0, #0x10
	bl	__Func_80925cc
	mov	r0, #6
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0x10
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L850
	ldr	r0, =_MSG_ff0
	bl	__MessageID
	b	.L860

	.pool_aligned

.L850:
	ldr	r0, =0xff1
	bl	__MessageID
	mov	r0, #0x10
	ldr	r1, =0x107
	mov	r2, #0x14
	bl	__MapActor_Emote
.L860:
	mov	r0, #0x10
	mov	r1, #4
	mov	r2, #0x14
	bl	__MapActor_Jump
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r2, #0
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #6
	mov	r0, #0x10
	bl	OvlFunc_890_200a5fc
	ldr	r0, =0xff2
	bl	__MessageID
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #5
	mov	r1, #4
	bl	__MapActor_DoAnim
	ldr	r0, =0x2005
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #1
	mov	r1, #0x14
	bl	OvlFunc_890_200a5fc
	mov	r0, #0x10
	mov	r1, #6
	mov	r2, #0x14
	bl	__MapActor_Jump
	mov	r1, #0x82
	mov	r2, #0x14
	mov	r0, #0x10
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #0x1e
	bl	OvlFunc_890_200a5fc
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #5
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x10
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x10
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0x10
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L958
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x10
	bl	__MapActor_TravelTo
.L958:
	mov	r0, #0x10
	bl	__MapActor_WaitMovement
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L988
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.L988:
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #5
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L9b8
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #5
	bl	__MapActor_TravelTo
.L9b8:
	mov	r0, #5
	bl	__MapActor_WaitMovement
	mov	r1, #0
	mov	r2, #0
	mov	r0, #5
	bl	__MapActor_SetPos
	mov	r0, #0xa2
	lsl	r0, #1
	bl	__SetFlag
	ldr	r0, =0x809
	bl	__SetFlag
	bl	__CutsceneEnd
.L9da:
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2008488

@ Cutscene: roughly 189 instructions of straight-line script --
@ 9 turns, 3 animation changes, 0 dialogue lines, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xff6, 0xffa, 0xffb, 0xffc.
@ Sets save bits 0x80a, 0x896.
.thumb_func_start OvlFunc_890_20089f4
	push	{r5, r6, lr}
	ldr	r0, =0xff6
	bl	__MessageID
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xf4
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0xb0
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #0
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.La32
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x10
	bl	__MapActor_SetPos
.La32:
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x10
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xec
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0xa8
	bl	__Func_80921c4
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_8092adc
	mov	r2, #0x28
	mov	r0, #0x10
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r0, #0x10
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	ldr	r0, =0x26666
	ldr	r1, =0x4ccc
	bl	__Func_80933d4
	mov	r1, #1
	mov	r2, #0xb5
	lsl	r2, #16
	mov	r3, #1
	neg	r1, r1
	ldr	r0, =0x23f0000
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x78
	bl	__CutsceneWait
	ldr	r0, =0x1010
	mov	r1, #0x50
	bl	OvlFunc_890_200a5fc
	mov	r0, #0xf6
	mov	r1, #1
	mov	r2, #0xa8
	mov	r3, #1
	neg	r1, r1
	lsl	r2, #16
	lsl	r0, #17
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r5, =0x4010
	mov	r1, #0xc0
	mov	r2, #0x14
	mov	r0, #0x10
	lsl	r1, #6
	bl	__Func_8092adc
	mov	r0, r5
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r2, #0x3c
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_8092adc
	mov	r0, #0x10
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0xc0
	mov	r0, #0x10
	lsl	r1, #6
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r1, #0
	mov	r0, r5
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.Lb08
	ldr	r0, =0xffa
	bl	__MessageID
	b	.Lb0e
.Lb08:
	ldr	r0, =0xffb
	bl	__MessageID
.Lb0e:
	ldr	r5, =0x4010
	mov	r1, #0xa0
	mov	r2, #0xa
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #0xa
	mov	r0, r5
	bl	OvlFunc_890_200a5fc
	ldr	r6, =0xffc
	mov	r0, r6
	bl	__MessageID
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r2, #0x28
	mov	r0, #0x10
	ldr	r1, =0x105
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #4
	bl	__MapActor_DoAnim
	mov	r1, #0xc0
	mov	r2, #0xa
	mov	r0, #0x10
	lsl	r1, #6
	bl	__Func_8092adc
	mov	r0, #0x10
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r1, #0
	mov	r0, r5
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.Lb7e
	add	r0, r6, #1
	bl	__MessageID
	ldr	r0, =0x896
	bl	__SetFlag
	b	.Lb84
.Lb7e:
	add	r0, r6, #2
	bl	__MessageID
.Lb84:
	ldr	r0, =0x4010
	mov	r1, #4
	bl	OvlFunc_890_200a5fc
	mov	r0, #0x10
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r1, #0xf3
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0x83
	bl	__Func_80921c4
	mov	r1, #0x90
	mov	r0, #0x10
	lsl	r1, #2
	mov	r2, #0x78
	bl	__Func_80921c4
	mov	r1, #0xc0
	mov	r2, #2
	mov	r0, #0x10
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #11
	lsl	r1, #8
	bl	__Func_80933d4
	ldr	r0, =0x80a
	bl	__SetFlag
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_20089f4
