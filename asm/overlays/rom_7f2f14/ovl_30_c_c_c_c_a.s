	.include "macros.inc"
	.include "gba.inc"

@ 179 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, GetSlotEntityChecked, SetEntityActorOptions, SetSlotPalette
@   StartLoopingSound, ShowScreenOverlay, WaitSceneDelay, DialogueWait
@   PlaySound, SetCameraSpeed, MoveCameraTo, Random x4
@   OvlFunc_118, WaitFrames
@   ... and 8 more
@ sets 0x306.
.thumb_func_start OvlFunc_968_200c610
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x42
	str	r2, [r3]
	sub	sp, #0x38
	bl	__CutsceneStart
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r1, #0xf
	mov	r0, #0
	bl	__Func_8092950
	mov	r0, #0xaa
	bl	__Func_8091ff0
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xa2
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #8
	lsl	r1, #5
	bl	__Func_80933d4
	mov	r0, #0xdc
	mov	r1, #1
	mov	r2, #0xb4
	lsl	r2, #17
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	bl	__Func_80933f8
	mov	r2, #0x10
	mov	r3, #0
	add	r2, sp
	mov	r10, r3
	mov	r8, r3
	mov	r11, r2
	mov	r9, r3
.L468c:
	bl	__Random
	ldr	r2, =0x4ccc
	lsl	r0, #1
	lsr	r0, #16
	mov	r3, r0
	mul	r3, r2
	ldr	r2, =0x17ffc
	add	r3, r2
	str	r3, [sp, #0x18]
	bl	__Random
	ldr	r2, =0x4ccc
	lsl	r0, #1
	lsr	r0, #16
	mov	r3, r0
	mul	r3, r2
	ldr	r2, =0x17ffc
	add	r3, r2
	str	r3, [sp, #0x1c]
	bl	__Random
	mov	r3, #0xf8
	lsl	r0, #12
	lsr	r0, #16
	lsl	r3, #8
	mov	r2, #0x32
	add	r0, r3
	add	r2, sp
	strh	r0, [r2]
.L46c8:
	mov	r5, #0xc0
	lsl	r5, #16
	mov	r6, #0
	mov	r7, #0
	add	r5, r9
.L46d2:
	bl	__Random
	mov	r3, r0
	lsl	r0, r3, #3
	sub	r0, r3
	lsr	r0, #16
	mov	r3, #0xd0
	lsl	r3, #17
	lsl	r0, #19
	add	r0, r3
	mov	r3, #0x88
	lsl	r3, #16
	mov	r2, r11
	str	r3, [sp, #8]
	str	r2, [sp, #0xc]
	mov	r3, #0
	mov	r2, r5
	mov	r1, #0
	str	r7, [sp]
	str	r7, [sp, #4]
	bl	OvlFunc_968_2008118
	mov	r3, #0x80
	lsl	r3, #11
	add	r6, #1
	add	r5, r3
	cmp	r6, #3
	bls	.L46d2
	mov	r0, #3
	bl	__WaitFrames
	mov	r2, r8
	cmp	r2, #3
	bne	.L4732
	mov	r3, r10
	cmp	r3, #2
	bhi	.L4722
	mov	r2, #1
	add	r10, r2
	b	.L46c8
.L4722:
	mov	r3, r10
	cmp	r3, #3
	bne	.L4732
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_968_200c5f0
	lsl	r1, #4
	bl	__StartTask
.L4732:
	mov	r3, r8
	add	r3, #0xc
	mov	r2, #3
	mov	r1, #1
	str	r2, [sp]
	str	r1, [sp, #4]
	mov	r2, #0x1a
	mov	r1, r3
	mov	r0, #0x35
	bl	__CopyMapTiles
	mov	r2, #0x80
	mov	r3, #1
	lsl	r2, #13
	add	r8, r3
	add	r9, r2
	mov	r2, r8
	cmp	r2, #0xc
	bls	.L468c
	mov	r3, #9
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x51
	mov	r1, #0x29
	mov	r2, #0x59
	mov	r3, #0xe
	bl	__CopyMapTiles
	bl	__Func_8093530
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r1, r1
	neg	r2, r2
	mov	r3, #0
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r0, =0x306
	bl	__SetFlag
	mov	r0, #0x13
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	add	sp, #0x38
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_968_200c610

@ 176 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, GetSlotEntityChecked, SetEntityActorOptions, SetSlotPalette
@   StartLoopingSound, ShowScreenOverlay, WaitSceneDelay, DialogueWait
@   PlaySound, SetCameraSpeed, MoveCameraTo, Random x4
@   OvlFunc_118, WaitFrames
@   ... and 8 more
@ sets 0x307.
.thumb_func_start OvlFunc_968_200c7c0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x42
	str	r2, [r3]
	sub	sp, #0x38
	bl	__CutsceneStart
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r1, #0xf
	mov	r0, #0
	bl	__Func_8092950
	mov	r0, #0xaa
	bl	__Func_8091ff0
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xa2
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #8
	lsl	r1, #5
	bl	__Func_80933d4
	mov	r0, #0x8e
	mov	r1, #1
	mov	r2, #0xb4
	lsl	r2, #17
	mov	r3, #1
	lsl	r0, #18
	neg	r1, r1
	bl	__Func_80933f8
	mov	r2, #0x10
	mov	r3, #0
	add	r2, sp
	mov	r10, r3
	mov	r8, r3
	mov	r11, r2
	mov	r9, r3
.L483c:
	bl	__Random
	ldr	r2, =0x4ccc
	lsl	r0, #1
	lsr	r0, #16
	mov	r3, r0
	mul	r3, r2
	ldr	r2, =0x17ffc
	add	r3, r2
	str	r3, [sp, #0x18]
	bl	__Random
	ldr	r2, =0x4ccc
	lsl	r0, #1
	lsr	r0, #16
	mov	r3, r0
	mul	r3, r2
	ldr	r2, =0x17ffc
	add	r3, r2
	str	r3, [sp, #0x1c]
	bl	__Random
	mov	r3, #0xf8
	lsl	r0, #12
	lsr	r0, #16
	lsl	r3, #8
	mov	r2, #0x32
	add	r0, r3
	add	r2, sp
	strh	r0, [r2]
.L4878:
	mov	r5, #0xc0
	lsl	r5, #16
	mov	r6, #0
	mov	r7, #0
	add	r5, r9
.L4882:
	bl	__Random
	mov	r3, r0
	lsl	r0, r3, #3
	sub	r0, r3
	lsr	r0, #16
	mov	r3, #0x88
	lsl	r3, #18
	lsl	r0, #19
	add	r0, r3
	mov	r3, #0x88
	lsl	r3, #16
	mov	r2, r11
	str	r3, [sp, #8]
	str	r2, [sp, #0xc]
	mov	r3, #0
	mov	r2, r5
	mov	r1, #0
	str	r7, [sp]
	str	r7, [sp, #4]
	bl	OvlFunc_968_2008118
	mov	r3, #0x80
	lsl	r3, #11
	add	r6, #1
	add	r5, r3
	cmp	r6, #3
	bls	.L4882
	mov	r0, #3
	bl	__WaitFrames
	mov	r2, r8
	cmp	r2, #3
	bne	.L48d2
	mov	r3, r10
	cmp	r3, #2
	bhi	.L48d2
	mov	r2, #1
	add	r10, r2
	b	.L4878
.L48d2:
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_968_200c600
	bl	__StartTask
	mov	r3, r8
	add	r3, #0xc
	mov	r2, #3
	mov	r1, #1
	str	r2, [sp]
	str	r1, [sp, #4]
	mov	r2, #0x22
	mov	r1, r3
	mov	r0, #0x3a
	bl	__CopyMapTiles
	mov	r3, #0x80
	mov	r2, #1
	lsl	r3, #13
	add	r8, r2
	add	r9, r3
	mov	r3, r8
	cmp	r3, #0xc
	bls	.L483c
	mov	r3, #5
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x56
	mov	r1, #0x29
	mov	r2, #0x61
	mov	r3, #0xe
	bl	__CopyMapTiles
	bl	__Func_8093530
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r1, r1
	neg	r2, r2
	mov	r3, #0
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r0, =0x307
	bl	__SetFlag
	mov	r0, #0x14
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	add	sp, #0x38
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_968_200c7c0

@ 88 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, Random x4, OvlFunc_118
.thumb_func_start OvlFunc_968_200c968
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	sub	sp, #0x38
	add	r2, sp, #0x10
	mov	r3, #1
	str	r3, [r2]
	mov	r3, #5
	str	r3, [r2, #4]
	mov	r3, #0x8f
	lsl	r3, #1
	strh	r3, [r2, #0x18]
	ldr	r3, =.L52cc
	mov	r10, r2
	str	r3, [r2, #0x1c]
	ldr	r2, =iwram_3001e40
	ldr	r7, [r2]
	mov	r3, #3
	and	r7, r3
	mov	r5, r0
	cmp	r7, #0
	bne	.L4a14
	ldr	r3, [r2]
	mov	r2, #7
	and	r3, r2
	cmp	r3, #0
	bne	.L49a6
	mov	r0, #0xf6
	bl	__PlaySound
.L49a6:
	bl	__Random
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #4
	add	r3, r0
	ldr	r2, [r5, #8]
	lsr	r3, #16
	sub	r3, #0x18
	mov	r8, r2
	lsl	r3, #16
	add	r8, r3
	bl	__Random
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #4
	add	r3, r0
	lsr	r3, #16
	ldr	r6, [r5, #0xc]
	sub	r3, #0x18
	lsl	r3, #16
	add	r6, r3
	bl	__Random
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #4
	add	r3, r0
	lsr	r3, #16
	ldr	r5, [r5, #0x10]
	sub	r3, #0x18
	lsl	r3, #16
	add	r5, r3
	bl	__Random
	lsl	r0, #2
	lsr	r0, #16
	mov	r3, #0x80
	lsl	r3, #8
	lsl	r0, #15
	add	r0, r3
	mov	r3, #0xcc
	lsl	r3, #14
	mov	r2, r10
	str	r0, [sp]
	str	r3, [sp, #8]
	str	r2, [sp, #0xc]
	mov	r0, r8
	mov	r1, r6
	mov	r2, r5
	mov	r3, #0
	str	r7, [sp, #4]
	bl	OvlFunc_968_2008118
.L4a14:
	mov	r0, #0
	add	sp, #0x38
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_968_200c968
