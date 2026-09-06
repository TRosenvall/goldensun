	.include "macros.inc"

@ Cutscene: roughly 266 instructions of straight-line script --
@ 0 turns, 6 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Sets save bit 0x9a7.
.thumb_func_start OvlFunc_969_200b924
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r0, #0
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r10, r0
	bl	__CutsceneStart
	bl	__Func_8093554
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r2, #0x17
	mov	r3, #0x12
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x66
	mov	r1, #4
	mov	r2, #0x4a
	mov	r3, #4
	bl	__CopyMapTiles
	mov	r3, #0x10
	mov	r2, #0x14
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x27
	mov	r1, #0x48
	mov	r2, #0xb
	mov	r3, #0x48
	bl	__CopyMapTiles
	mov	r2, #0x16
	str	r2, [sp]
	mov	r8, r2
	mov	r6, #6
	mov	r0, #0x13
	mov	r1, #6
	mov	r2, #3
	mov	r3, #7
	str	r6, [sp, #4]
	bl	__Func_8010704
	mov	r5, #0xd
	mov	r0, #0x13
	mov	r1, #6
	mov	r2, #3
	mov	r3, #7
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010704
	mov	r3, r8
	str	r3, [sp]
	mov	r0, #0x13
	mov	r1, #6
	mov	r2, #3
	mov	r3, #7
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r1, #6
	mov	r2, #3
	mov	r3, #7
	mov	r0, #0x13
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0xc0
	mov	r2, #0xee
	lsl	r2, #16
	mov	r3, #0
	ldr	r1, =0xffc00000
	lsl	r0, #16
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x14
	bl	__DeleteFieldActor
	mov	r0, #0x13
	bl	__DeleteFieldActor
	mov	r1, #0x13
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r3, #0xad
	mov	r2, r10
	lsl	r3, #17
	str	r3, [r2, #8]
	mov	r3, #0xcd
	lsl	r3, #16
	str	r3, [r2, #0x10]
	mov	r5, #0x80
	mov	r3, #0xc0
	lsl	r3, #7
	lsl	r5, #14
	str	r5, [r2, #0xc]
	strh	r3, [r2, #6]
	mov	r0, r10
	bl	OvlFunc_969_200d688
	mov	r1, #0x12
	mov	r0, #1
	bl	__MapActor_SetAnim
	mov	r0, #1
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #1
	bl	__MapActor_GetActor
	mov	r3, #0xb2
	lsl	r3, #17
	str	r3, [r0, #8]
	mov	r3, #0xc0
	lsl	r3, #16
	str	r3, [r0, #0x10]
	mov	r3, #0xa0
	lsl	r3, #8
	strh	r3, [r0, #6]
	str	r5, [r0, #0xc]
	bl	OvlFunc_969_200d688
	mov	r1, #0x12
	mov	r0, #2
	bl	__MapActor_SetAnim
	mov	r0, #2
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #2
	bl	__MapActor_GetActor
	mov	r3, #0xb4
	lsl	r3, #17
	str	r3, [r0, #8]
	mov	r6, #0xde
	mov	r3, #0x80
	lsl	r3, #6
	lsl	r6, #16
	strh	r3, [r0, #6]
	str	r5, [r0, #0xc]
	str	r6, [r0, #0x10]
	bl	OvlFunc_969_200d688
	mov	r1, #0x12
	mov	r0, #3
	bl	__MapActor_SetAnim
	mov	r0, #3
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #3
	bl	__MapActor_GetActor
	mov	r3, #0xa7
	lsl	r3, #17
	str	r3, [r0, #8]
	mov	r3, #0x80
	lsl	r3, #8
	strh	r3, [r0, #6]
	str	r5, [r0, #0xc]
	str	r6, [r0, #0x10]
	bl	OvlFunc_969_200d688
	mov	r1, #0xc4
	mov	r2, #0xdc
	lsl	r2, #16
	mov	r0, #0x15
	lsl	r1, #16
	bl	__MapActor_SetPos
	mov	r0, #0x15
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r1, #0xbc
	mov	r2, #0x9e
	lsl	r2, #17
	mov	r0, #6
	lsl	r1, #16
	bl	__MapActor_SetPos
	mov	r1, #5
	mov	r0, #6
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r5, =0xfff00000
	ldr	r3, [r0, #8]
	add	r3, r5
	str	r3, [r0, #8]
	bl	OvlFunc_969_200d688
	mov	r0, #9
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	add	r3, r5
	str	r3, [r0, #8]
	bl	OvlFunc_969_200d688
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r5, #0x80
	ldr	r3, [r0, #8]
	lsl	r5, #13
	add	r3, r5
	str	r3, [r0, #8]
	bl	OvlFunc_969_200d688
	mov	r0, #0xb
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	add	r3, r5
	str	r3, [r0, #8]
	bl	OvlFunc_969_200d688
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	lsl	r1, #9
	lsl	r0, #9
	bl	__Func_8012330
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r3, #4
	add	r0, #0x55
	strb	r3, [r0]
	mov	r1, #4
	mov	r0, #0x17
	bl	__Func_8092950
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r3, #0xa0
	lsl	r3, #14
	mov	r1, #0xc8
	str	r3, [r0, #0xc]
	lsl	r1, #4
	ldr	r0, =OvlFunc_969_200da28
	bl	__StartTask
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	add	r3, #0x40
	str	r3, [r2]
	sub	r3, #0x38
	add	r2, r1, r3
	mov	r3, #0x18
	str	r3, [r2]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	bl	OvlFunc_969_200cbec
	ldr	r0, =0x9a7
	bl	__SetFlag
	mov	r0, #2
	bl	__Func_8091e9c
	add	sp, #8
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_969_200b924

@ Cutscene: roughly 289 instructions of straight-line script --
@ 0 turns, 6 animation changes, 1 dialogue line, 10 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2809.
.thumb_func_start OvlFunc_969_200bbc8
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6}
	mov	r6, r8
	push	{r6}
	mov	r0, #0x8d
	sub	sp, #8
	bl	__PlaySound
	mov	r3, #0x11
	mov	r2, #8
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #2
	mov	r1, #0xa
	mov	r2, #4
	mov	r0, #0x11
	bl	__Func_8010704
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, #0xa0
	lsl	r5, #8
	ldr	r2, =0
	strh	r5, [r0, #6]
	mov	r0, #1
	mov	r9, r2
	bl	__MapActor_GetActor
	mov	r1, #0xa4
	mov	r2, #0xa8
	strh	r5, [r0, #6]
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #1
	bl	__MapActor_SetPos
	mov	r0, #2
	bl	__MapActor_GetActor
	mov	r1, #0xaa
	mov	r2, #0xc4
	strh	r5, [r0, #6]
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #2
	bl	__MapActor_SetPos
	mov	r0, #3
	bl	__MapActor_GetActor
	mov	r1, #0xa3
	mov	r2, #0xcc
	b	.L3c3c

	.pool_aligned

.L3c3c:
	strh	r5, [r0, #6]
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #3
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r5, #0xd0
	lsl	r5, #8
	mov	r1, #0xc8
	mov	r2, #0xd8
	strh	r5, [r0, #6]
	lsl	r1, #16
	lsl	r2, #16
	mov	r0, #0x15
	bl	__MapActor_SetPos
	mov	r0, #6
	bl	__MapActor_GetActor
	mov	r1, #0xc8
	mov	r2, #0xd8
	strh	r5, [r0, #6]
	lsl	r1, #16
	lsl	r2, #16
	mov	r0, #6
	bl	__MapActor_SetPos
	mov	r0, #0x14
	bl	__MapActor_GetActor
	mov	r5, #0xc0
	lsl	r5, #6
	mov	r1, #0x9b
	mov	r2, #0x9e
	strh	r5, [r0, #6]
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #0x14
	bl	__MapActor_SetPos
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r1, #0x92
	mov	r2, #0x9e
	lsl	r2, #16
	strh	r5, [r0, #6]
	lsl	r1, #17
	mov	r0, #0x13
	bl	__MapActor_SetPos
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x18
	mov	r1, #7
	bl	__Func_8092950
	mov	r1, #1
	mov	r0, #0x18
	bl	__Func_8092b08
	mov	r0, #0x18
	bl	__MapActor_GetActor
	ldr	r3, =0xffff0000
	ldr	r6, =0x3333
	str	r3, [r0, #0x1c]
	mov	r8, r3
	mov	r3, r0
	mov	r2, r9
	add	r3, #0x55
	str	r6, [r0, #0x18]
	strb	r2, [r3]
	mov	r3, #0x98
	lsl	r3, #17
	mov	r10, r3
	str	r3, [r0, #8]
	mov	r5, #0xc0
	mov	r3, #0x80
	lsl	r3, #10
	lsl	r5, #15
	str	r3, [r0, #0xc]
	str	r5, [r0, #0x10]
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x19
	mov	r1, #7
	bl	__Func_8092950
	mov	r1, #1
	mov	r0, #0x19
	bl	__Func_8092b08
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r2, r8
	mov	r3, r0
	str	r2, [r0, #0x1c]
	add	r3, #0x55
	mov	r2, r9
	str	r6, [r0, #0x18]
	strb	r2, [r3]
	mov	r3, r10
	str	r3, [r0, #8]
	mov	r3, #0x88
	lsl	r3, #14
	mov	r1, #0xc8
	str	r3, [r0, #0xc]
	str	r5, [r0, #0x10]
	lsl	r1, #4
	ldr	r0, =OvlFunc_969_200b6d0
	bl	__StartTask
	bl	__Func_8093554
	mov	r2, r9
	add	r0, #0x55
	strb	r2, [r0]
	mov	r1, #0x80
	mov	r2, #0xb4
	mov	r3, #0
	lsl	r1, #14
	lsl	r2, #16
	mov	r0, r10
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x80
	lsl	r0, #9
	mov	r1, r0
	mov	r2, r1
	bl	__Func_8012330
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #0x14
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x2809
	bl	__MessageID
	mov	r0, #0x14
	bl	OvlFunc_969_2008894
	mov	r1, #3
	mov	r0, #0x13
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0x28
	mov	r1, #0
	mov	r0, #0x13
	bl	__Func_8093040
	mov	r0, #0x11
	bl	__PlaySound
	mov	r0, #0x14
	bl	__MapActor_GetActor
	mov	r3, #0x9a
	mov	r5, r0
	lsl	r3, #17
	str	r3, [r5, #8]
	mov	r6, #0x98
	mov	r3, #0xe0
	lsl	r6, #16
	lsl	r3, #13
	str	r3, [r5, #0xc]
	str	r6, [r5, #0x10]
	mov	r1, #0xa
	mov	r0, #0x14
	mov	r8, r3
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r3, #0x99
	mov	r2, r8
	lsl	r3, #17
	str	r2, [r5, #0xc]
	str	r3, [r5, #8]
	str	r6, [r5, #0x10]
	mov	r1, #0xb
	mov	r0, #0x14
	bl	__MapActor_SetAnim
	mov	r0, #0xc
	bl	__CutsceneWait
	mov	r3, r10
	str	r3, [r5, #8]
	mov	r3, #0xa8
	lsl	r3, #13
	str	r3, [r5, #0xc]
	mov	r1, #0xc
	str	r6, [r5, #0x10]
	mov	r0, #0x14
	bl	__MapActor_SetAnim
	mov	r0, #8
	bl	__CutsceneWait
	mov	r0, #0x14
	bl	__DeleteFieldActor
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0x93
	mov	r5, r0
	mov	r2, r8
	lsl	r3, #17
	str	r2, [r5, #0xc]
	str	r3, [r5, #8]
	mov	r1, #8
	str	r6, [r5, #0x10]
	mov	r0, #0x13
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r3, #0x96
	lsl	r3, #17
	str	r3, [r5, #8]
	mov	r3, #0xd8
	lsl	r3, #13
	str	r3, [r5, #0xc]
	mov	r1, #9
	str	r6, [r5, #0x10]
	mov	r0, #0x13
	bl	__MapActor_SetAnim
	mov	r0, #0xc
	bl	__CutsceneWait
	mov	r3, r10
	str	r3, [r5, #8]
	mov	r3, #0x88
	lsl	r3, #13
	str	r3, [r5, #0xc]
	mov	r1, #0xa
	str	r6, [r5, #0x10]
	mov	r0, #0x13
	bl	__MapActor_SetAnim
	mov	r0, #8
	bl	__CutsceneWait
	mov	r0, #0x13
	bl	__DeleteFieldActor
	mov	r0, #0xa0
	bl	__CutsceneWait
	add	sp, #8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_969_200bbc8
