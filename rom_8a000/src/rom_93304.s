	.include "macros.inc"

@ ============================================================================
@ Camera control and party-follow.
@
@ The camera state block is fetched with Func_48f4(0x1B, 0xCCC); the camera
@ ENTITY lives at +0x1E0 within it and the scene mode at +0x19E. Pointing
@ [iwram_1e70] at the camera entity's position words (+0x08) is what makes the
@ map scroller in rom_9000 follow it.
@ Map bounds are held in the map state at [iwram_1e70]+0xEC..+0xF8 and every
@ camera move below is clamped against them, inset by a margin so the view never
@ shows past the edge.
@ ============================================================================

@ SetDialoguePortrait
@ r0=speaker slot, or 0x80000000 for "no portrait".
@ Writes the portrait id at iwram_1e8c+0x12F4 and its style byte at +0x12F6.
@ The sentinel clears both; otherwise the id comes from Func_92ba8 /
@ Func_915ac and the style is looked up in .L9fc28 by the text-speed setting at
@ ewram_240+0x20C.
.thumb_func_start Func_93304
	push	{r5, lr}
	ldr	r3, =iwram_1e8c
	mov	r1, #0x80
	lsl	r1, #24
	ldr	r5, [r3]
	cmp	r0, r1
	bne	.L9331e
	ldr	r2, =0x12f4
	ldr	r1, =0x12f6
	add	r3, r5, r2
	mov	r2, #0
	strh	r2, [r3]
	b	.L9333c
.L9331e:
	bl	Func_92ba8
	bl	Func_915ac
	ldr	r3, =ewram_240
	mov	r1, #0x83
	lsl	r1, #2
	add	r3, r1
	ldrb	r3, [r3]
	ldr	r1, =0x12f4
	ldr	r2, =.L9fc28
	ldrb	r2, [r2, r3]
	add	r3, r5, r1
	add	r1, #2
	strh	r0, [r3]
.L9333c:
	add	r3, r5, r1
	strh	r2, [r3]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_93304

@ AttachCameraToSlot
@ r0=slot, r1=non-zero to keep the current camera position.
@ Binds the camera entity to the slot's entity with _Func_c4bc and repoints
@ [iwram_1e70] at the camera's position words so the map scrolls with it.
@ Unless r1 says otherwise the camera is snapped straight onto the target's
@ position, given a frame to settle, and -- outside scene mode 3 -- the map view
@ is refreshed with _Func_fe9c so the new region is drawn immediately.
.thumb_func_start Func_9335c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r10, r1
	bl	Func_8ba1c
	ldr	r1, =0xccc
	mov	r6, r0
	mov	r0, #0x1b
	bl	Func_48f4
	mov	r3, #0xf0
	mov	r8, r0
	lsl	r3, #1
	add	r3, r8
	ldr	r5, [r3]
	ldr	r3, =iwram_1e70
	ldr	r3, [r3]
	cmp	r6, #0
	beq	.L933be
	mov	r7, r5
	add	r7, #8
	str	r7, [r3]
	mov	r0, r5
	mov	r1, r6
	bl	_Func_c4bc
	mov	r2, r10
	cmp	r2, #0
	bne	.L933be
	ldr	r3, [r6, #8]
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #0xc]
	ldr	r3, [r6, #0x10]
	mov	r0, #1
	str	r3, [r5, #0x10]
	bl	Func_30f8
	mov	r3, #0xcf
	lsl	r3, #1
	add	r3, r8
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	beq	.L933be
	bl	_Func_fe9c
.L933be:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_9335c

@ SetCameraSpeed
@ r0=max speed, r1=acceleration. Writes +0x30 and +0x34 of the camera entity,
@ controlling how quickly the camera catches up with whatever it is following.
.thumb_func_start Func_933d4
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, r1
	mov	r0, #0x1b
	ldr	r1, =0xccc
	bl	Func_48f4
	mov	r3, #0xf0
	lsl	r3, #1
	add	r0, r3
	ldr	r3, [r0]
	str	r5, [r3, #0x30]
	str	r6, [r3, #0x34]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_933d4

@ MoveCameraTo
@ r0=x, r1=y, r2=z, r3=flags. Any coordinate passed as -1 keeps its current
@ value, so a caller can move one axis at a time.
@ Detaches the camera from its follow target with _Func_c4ac, then clamps the
@ destination against the map bounds at [iwram_1e70]+0xEC..+0xF8 -- each inset
@ by a margin (0x780000 on x, 0x600000 and 0x780000 on z, 0x400000 on the far
@ edge) so the camera stops short of showing past the map -- and starts the
@ move.
.thumb_func_start Func_933f8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x10
	mov	r6, r0
	mov	r10, r1
	mov	r0, #0x1b
	ldr	r1, =0xccc
	str	r3, [sp, #0xc]
	mov	r7, r2
	bl	Func_48f4
	mov	r1, #0xf0
	str	r0, [sp, #8]
	lsl	r1, #1
	add	r3, r0, r1
	ldr	r5, [r3]
	ldr	r3, =iwram_1e70
	ldr	r1, [r3]
	mov	r3, r1
	add	r3, #0xec
	ldr	r3, [r3]
	mov	r2, #0xf0
	lsl	r2, #15
	add	r2, r3, r2
	str	r2, [sp, #4]
	mov	r3, r1
	add	r3, #0xf0
	ldr	r2, [r5, #0xc]
	ldr	r3, [r3]
	mov	r0, #0xc0
	add	r3, r2
	lsl	r0, #15
	add	r0, r3, r0
	str	r0, [sp]
	mov	r3, r1
	add	r3, #0xf4
	ldr	r3, [r3]
	ldr	r0, =0xff880000
	add	r0, r3
	mov	r3, r1
	add	r3, #0xf8
	ldr	r3, [r3]
	add	r3, r2
	ldr	r2, =0xffc00000
	add	r2, r3
	mov	r3, #8
	add	r3, r5
	str	r3, [r1]
	mov	r11, r0
	mov	r0, r5
	mov	r8, r3
	mov	r9, r2
	bl	_Func_c4ac
	mov	r3, #1
	neg	r3, r3
	cmp	r6, r3
	bne	.L9347a
	mov	r0, r8
	ldr	r6, [r0]
.L9347a:
	cmp	r10, r3
	bne	.L93482
	ldr	r1, [r5, #0xc]
	mov	r10, r1
.L93482:
	cmp	r7, r3
	bne	.L93488
	ldr	r7, [r5, #0x10]
.L93488:
	ldr	r2, [sp, #4]
	cmp	r6, r2
	bge	.L93490
	mov	r6, r2
.L93490:
	ldr	r3, [sp]
	cmp	r7, r3
	bge	.L93498
	mov	r7, r3
.L93498:
	cmp	r6, r11
	ble	.L9349e
	mov	r6, r11
.L9349e:
	cmp	r7, r9
	ble	.L934a4
	mov	r7, r9
.L934a4:
	ldr	r0, [sp, #0xc]
	cmp	r0, #0
	bne	.L934d0
	mov	r1, r8
	mov	r2, r10
	str	r6, [r1]
	mov	r0, #1
	str	r2, [r5, #0xc]
	str	r7, [r5, #0x10]
	bl	Func_30f8
	mov	r1, #0xcf
	ldr	r0, [sp, #8]
	lsl	r1, #1
	add	r3, r0, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	beq	.L934dc
	bl	_Func_fe9c
	b	.L934dc
.L934d0:
	mov	r0, r5
	mov	r1, r6
	mov	r2, r10
	mov	r3, r7
	bl	_Func_d14c
.L934dc:
	add	sp, #0x10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_933f8

@ CenterCameraOnSlot
@ r0=slot, r1=flags. Moves the camera to the slot entity's current position via
@ the same clamped path as Func_933f8, leaving the camera detached rather than
@ bound. No-op if the slot is empty.
.thumb_func_start Func_93500
	push	{r5, r6, lr}
	mov	r6, r1
	bl	Func_8ba1c
	ldr	r1, =0xccc
	mov	r5, r0
	mov	r0, #0x1b
	bl	Func_48f4
	cmp	r5, #0
	beq	.L93524
	mov	r1, #1
	ldr	r0, [r5, #8]
	neg	r1, r1
	ldr	r2, [r5, #0x10]
	mov	r3, r6
	bl	Func_933f8
.L93524:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_93500

@ WaitForCameraArrival
@ Takes no arguments. Blocks on _Func_ca6c until the camera entity finishes its
@ move, then pauses a further 2 frames through Func_9163c so the view settles
@ before the next scripted step.
.thumb_func_start Func_93530
	push	{lr}
	ldr	r1, =0xccc
	mov	r0, #0x1b
	bl	Func_48f4
	mov	r3, #0xf0
	lsl	r3, #1
	add	r0, r3
	ldr	r0, [r0]
	bl	_Func_ca6c
	mov	r0, #2
	bl	Func_9163c
	pop	{r0}
	bx	r0
.func_end Func_93530

@ GetCameraEntity
@ Returns the camera entity pointer from +0x1E0 of the camera state block.
.thumb_func_start Func_93554
	push	{lr}
	ldr	r1, =0xccc
	mov	r0, #0x1b
	bl	Func_48f4
	mov	r3, #0xf0
	lsl	r3, #1
	add	r0, r3
	ldr	r0, [r0]
	pop	{r1}
	bx	r1
.func_end Func_93554

@ SetCameraFollowTarget
@ r0=target entity or 0, r1=flags. Binds the camera to r0 with _Func_c4bc when
@ a target is given, or releases it to free movement when r0 is null.
.thumb_func_start Func_93570
	push	{r5, r6, r7, lr}
	mov	r6, r0
	mov	r7, r1
	mov	r0, #0x1b
	ldr	r1, =0xccc
	bl	Func_48f4
	mov	r3, #0xf0
	lsl	r3, #1
	add	r0, r3
	ldr	r5, [r0]
	cmp	r6, #0
	beq	.L935a4
	mov	r0, r5
	mov	r1, #0
	bl	_Func_c4bc
	str	r6, [r5, #0x68]
	cmp	r7, #0
	bne	.L935a4
	ldr	r3, [r6, #8]
	str	r3, [r5, #8]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #0xc]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #0x10]
.L935a4:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_93570

@ SetMapBounds
@ r0, r1, r2, r3 = minimum x, minimum z, maximum x, maximum z. Writes the four
@ bound words at [iwram_1e70]+0xEC, +0xF0, +0xF4 and +0xF8 that every camera
@ move in this file clamps against. Scripts use this to fence the camera into a
@ sub-region of a larger map.
.thumb_func_start Func_935b0
	ldr	r4, =iwram_1e70
	ldr	r4, [r4]
	mov	r12, r4
	add	r4, #0xec
	str	r0, [r4]
	mov	r0, r12
	add	r0, #0xf0
	str	r1, [r0]
	mov	r1, r12
	add	r1, #0xf4
	str	r2, [r1]
	mov	r2, r12
	add	r2, #0xf8
	str	r3, [r2]
	bx	lr
.func_end Func_935b0

@ MoveCameraToClamped
@ r0..r3 = destination and flags. Like Func_933f8 but reads the live map bounds
@ rather than the cached ones and applies the clamp before issuing the move, so
@ it stays correct after a Func_935b0 change mid-scene.
.thumb_func_start Func_935d4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1e70
	ldr	r1, =0xccc
	mov	r0, #0x1b
	ldr	r7, [r3]
	bl	Func_48f4
	mov	r1, #0xf0
	lsl	r1, #1
	add	r0, r1
	ldr	r3, [r0]
	add	r3, #0x5b
	ldrb	r3, [r3]
	mov	r10, r3
	cmp	r3, #0
	bne	.L9367c
	mov	r2, #0xd6
	lsl	r2, #2
	add	r2, r7
	mov	r0, #0
	ldrsh	r3, [r2, r0]
	mov	r8, r2
	cmp	r3, #0
	beq	.L9367c
	mov	r1, #0xd4
	mov	r2, #0xd5
	lsl	r1, #2
	lsl	r2, #2
	add	r6, r7, r1
	add	r3, r7, r2
	ldr	r2, [r3]
	ldr	r3, [r6]
	sub	r2, r3
	ldr	r3, =0x35a
	add	r5, r7, r3
	ldrh	r3, [r5]
	add	r3, #1
	strh	r3, [r5]
	lsl	r3, #16
	asr	r3, #16
	mov	r0, r3
	mul	r0, r2
	mov	r3, r8
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	bl	Func_af0_from_thumb
	mov	r2, r0
	mov	r0, #0xd2
	ldr	r1, [r6]
	lsl	r0, #2
	add	r3, r7, r0
	ldr	r4, =Func_888
	ldr	r0, [r3]
	add	r1, r2
	.call_via r4
	mov	r1, #0xd3
	lsl	r1, #2
	add	r3, r7, r1
	str	r0, [r3]
	mov	r0, #0x8c
	lsl	r0, #1
	add	r3, r7, r0
	ldrh	r3, [r3]
	ldr	r2, =iwram_1af4
	add	r3, #1
	str	r3, [r2]
	mov	r1, #0
	ldrsh	r2, [r5, r1]
	mov	r1, r8
	mov	r0, #0
	ldrsh	r3, [r1, r0]
	cmp	r2, r3
	bne	.L9367c
	mov	r2, r10
	mov	r3, r8
	strh	r2, [r3]
	ldr	r0, =Func_935d4
	bl	Func_4278
.L9367c:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_935d4

@ ScrollCameraBy
@ r0=x delta, r1=z delta. Offsets the camera from its current position, clamped
@ to the map bounds. Scene mode 3 (+0x19E) takes a different path that keeps the
@ camera locked to its follow target instead.
.thumb_func_start Func_936a0
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e70
	mov	r6, r0
	mov	r7, r1
	mov	r0, #0x1b
	ldr	r1, =0xccc
	ldr	r5, [r3]
	bl	Func_48f4
	mov	r1, #0xcf
	lsl	r1, #1
	add	r3, r0, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L936f4
	mov	r1, #0x80
	ldr	r3, =Func_8ac
	lsl	r1, #9
	mov	r0, r6
	bl	_call_via_r3
	mov	r3, #0xd4
	lsl	r3, #2
	add	r1, r5, r3
	add	r3, #4
	add	r2, r5, r3
	ldr	r3, [r2]
	str	r3, [r1]
	mov	r1, #0xd6
	lsl	r1, #2
	add	r3, r5, r1
	add	r1, #2
	str	r0, [r2]
	strh	r7, [r3]
	mov	r2, #0
	add	r3, r5, r1
	strh	r2, [r3]
	ldr	r0, =Func_935d4
	ldr	r1, =0xc94
	bl	Func_41d8
.L936f4:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_936a0

@ RestoreCameraToPlayer
@ Takes no arguments. Returns the camera to the player after a scripted move:
@ in scene mode 3 it re-binds directly, otherwise it re-centres on the player
@ entity and lets the normal follow behaviour resume.
.thumb_func_start Func_93710
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e70
	ldr	r1, =0xccc
	mov	r0, #0x1b
	ldr	r6, [r3]
	bl	Func_48f4
	mov	r2, #0xcf
	lsl	r2, #1
	add	r3, r0, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L93758
	mov	r2, #0xd6
	lsl	r2, #2
	add	r3, r6, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r5, #0
	cmp	r3, #0
	beq	.L93758
	mov	r3, #0xd6
	lsl	r3, #2
	add	r6, r3
.L93742:
	mov	r0, #1
	bl	Func_30f8
	ldr	r2, =0x12b
	add	r5, #1
	cmp	r5, r2
	bgt	.L93758
	mov	r2, #0
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	bne	.L93742
.L93758:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_93710

@ CameraFollowHook
@ r0=camera entity. Per-frame hook. Copies the followed entity's position
@ (from the script argument at +0x68) into the camera, clearing the movement
@ mode at +0x55 so the camera does not try to path toward it, and mirrors the
@ target's facing into +0x66. A null target makes the hook a no-op.
.thumb_func_start Func_9376c
	push	{r5, r6, lr}
	mov	r5, r0
	ldr	r6, [r5, #0x68]
	cmp	r6, #0
	beq	.L937b0
	mov	r2, r5
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	ldr	r3, [r6, #8]
	str	r3, [r5, #8]
	mov	r3, r5
	add	r3, #0x66
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bl	Func_8d394
	mov	r3, #0
	ldrsh	r0, [r0, r3]
	bl	_Func_185008
	mov	r2, #8
	ldrsb	r2, [r0, r2]
	ldr	r3, [r6, #0xc]
	lsl	r2, #16
	add	r3, r2
	mov	r2, #0x80
	lsl	r2, #12
	add	r3, r2
	str	r3, [r5, #0xc]
	ldr	r3, [r6, #0x14]
	str	r3, [r5, #0x14]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #0x10]
.L937b0:
	mov	r0, #0
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_9376c

@ PlayInteractionEffect
@ r0=slot, r1=effect id in the low byte, r2=parameter. Plays the sound and
@ visual that accompanies a field interaction; effect 6 additionally plays
@ sound 0x6E up front. Dispatches on the low byte to select which of the
@ per-effect routines runs.
.thumb_func_start Func_937b8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r7, r1
	mov	r3, #0xff
	and	r3, r7
	mov	r8, r0
	mov	r10, r2
	cmp	r3, #6
	bne	.L937d4
	mov	r0, #0x6e
	bl	_Func_f9080
.L937d4:
	mov	r0, r8
	bl	Func_8ba1c
	mov	r6, r0
	cmp	r6, #0
	beq	.L93866
	ldr	r1, [r6, #8]
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x10]
	mov	r0, #0x15
	bl	_Func_c150
	mov	r5, r0
	cmp	r5, #0
	beq	.L93860
	ldr	r1, =.L9fc2c
	bl	_Func_c2d8
	mov	r1, #0xf
	and	r1, r7
	mov	r0, r5
	bl	_Func_c300
	mov	r3, r5
	mov	r2, #0
	add	r3, #0x55
	strb	r2, [r3]
	add	r3, #0xf
	strh	r2, [r3]
	add	r3, #2
	mov	r2, r8
	strh	r2, [r3]
	ldr	r3, =Func_9376c
	ldr	r0, [r5, #0x50]
	ldr	r1, .L93840	@ 0
	str	r3, [r5, #0x6c]
	mov	r3, r0
	add	r3, #0x26
	strb	r1, [r3]
	mov	r3, #0x80
	lsl	r3, #1
	and	r3, r7
	str	r6, [r5, #0x68]
	cmp	r3, #0
	beq	.L9384c
	ldrb	r3, [r0, #9]
	mov	r2, #0xd
	neg	r2, r2
	and	r2, r3
	mov	r3, #4
	orr	r2, r3
	strb	r2, [r0, #9]
	b	.L93860

	.align	2, 0
.L93840:
	.word	0
	.pool

.L9384c:
	ldr	r3, [r6, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	ldrb	r1, [r0, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r1
	orr	r3, r2
	strb	r3, [r0, #9]
.L93860:
	mov	r0, r10
	bl	Func_9163c
.L93866:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_937b8

@ RunSlotEffectSequence
@ r0=slot, r1=sequence id. Runs a short scripted effect on the slot entity --
@ animation change, wait, and position nudge -- returning when it completes.
@ The ~120-instruction body is characterised structurally.
.thumb_func_start Func_93874
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r10, r0
	mov	r8, r1
	bl	Func_8ba1c
	mov	r7, r0
	mov	r5, #0
	mov	r6, #0
	cmp	r7, #0
	beq	.L93958
	mov	r3, #3
	mov	r1, r8
	and	r3, r1
	cmp	r3, #0
	beq	.L938b2
	cmp	r3, #2
	beq	.L938a2
	ldr	r3, [r7, #0x68]
	cmp	r3, #0
	bne	.L938c2
.L938a2:
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0xc]
	ldr	r3, [r7, #0x10]
	mov	r0, #0xd1
	bl	_Func_c150
	mov	r5, r0
	b	.L938c2
.L938b2:
	ldr	r5, [r7, #0x68]
	cmp	r5, #0
	beq	.L93958
	mov	r0, r5
	bl	_Func_c0f4
	str	r6, [r7, #0x68]
	b	.L93958
.L938c2:
	cmp	r5, #0
	beq	.L93958
	mov	r6, #3
	mov	r2, r8
	and	r6, r2
	cmp	r6, #1
	beq	.L938d6
	cmp	r6, #2
	beq	.L938e8
	b	.L93900
.L938d6:
	mov	r0, r5
	mov	r1, #1
	bl	_Func_c300
	mov	r3, r5
	add	r3, #0x64
	str	r5, [r7, #0x68]
	strh	r6, [r3]
	b	.L93900
.L938e8:
	mov	r0, r5
	mov	r1, #2
	bl	_Func_c300
	ldr	r1, =.L9fd38
	mov	r0, r5
	bl	_Func_c2d8
	mov	r2, r5
	add	r2, #0x64
	mov	r3, #1
	strh	r3, [r2]
.L93900:
	mov	r3, r5
	ldr	r2, .L93938	@ 0
	add	r3, #0x66
	mov	r1, r10
	strh	r1, [r3]
	sub	r3, #0x11
	strb	r2, [r3]
	ldr	r3, =Func_9376c
	ldr	r6, [r5, #0x50]
	str	r3, [r5, #0x6c]
	mov	r3, r6
	add	r3, #0x26
	strb	r2, [r3]
	mov	r3, #0x80
	lsl	r3, #1
	mov	r2, r8
	and	r3, r2
	str	r7, [r5, #0x68]
	cmp	r3, #0
	beq	.L93944
	ldrb	r3, [r6, #9]
	mov	r2, #0xd
	neg	r2, r2
	and	r2, r3
	mov	r3, #4
	orr	r2, r3
	strb	r2, [r6, #9]
	b	.L93958

	.align	2, 0
.L93938:
	.word	0
	.pool

.L93944:
	ldr	r3, [r7, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	ldrb	r1, [r6, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r1
	orr	r3, r2
	strb	r3, [r6, #9]
.L93958:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_93874

@ ClearSlotEffectFlags
@ r0=entity. Clears the actor option byte through _Func_c528(entity, 0) and
@ zeroes the collision flags at +0x59, undoing what an effect sequence set.
@ Always returns 0, so it doubles as a script opcode handler that ends the VM
@ pass.
.thumb_func_start Func_93964
	push	{r5, lr}
	mov	r1, #0
	mov	r5, r0
	bl	_Func_c528
	add	r5, #0x59
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_93964

@ ChaseTargetHook
@ r0=entity. Per-frame hook for an entity chasing the target held at +0x68.
@ Computes the delta to the target, and once it closes to within the threshold
@ switches behaviour -- the usual approach-then-stop pattern. A null target is a
@ no-op.
.thumb_func_start Func_9397c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	ldr	r1, [r6, #0x68]
	cmp	r1, #0
	beq	.L93a00
	ldr	r2, [r1, #8]
	ldr	r3, [r6, #8]
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L93998
	ldr	r2, =0xffff
	add	r0, r2
.L93998:
	ldr	r2, [r1, #0x10]
	ldr	r3, [r6, #0x10]
	asr	r5, r0, #16
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L939a8
	ldr	r3, =0xffff
	add	r0, r3
.L939a8:
	asr	r0, #16
	mov	r8, r0
	mov	r2, r8
	mov	r3, r8
	mul	r3, r2
	mov	r0, r5
	mul	r0, r5
	add	r0, r3
	ldr	r3, =Func_948
	bl	_call_via_r3
	mov	r3, r6
	add	r3, #0x64
	mov	r2, #0
	ldrsh	r7, [r3, r2]
	cmp	r0, r7
	blt	.L939f8
	lsl	r0, r5, #20
	mov	r1, r7
	bl	Func_af0_from_thumb
	ldr	r5, [r6, #8]
	mov	r3, r8
	add	r5, r0
	mov	r1, r7
	lsl	r0, r3, #20
	bl	Func_af0_from_thumb
	ldr	r3, [r6, #0x10]
	mov	r1, r5
	add	r3, r0
	ldr	r2, [r6, #0xc]
	mov	r0, r6
	bl	_Func_d14c
	mov	r0, r6
	mov	r1, #2
	bl	_Func_c300
	b	.L93a00
.L939f8:
	mov	r0, r6
	mov	r1, #1
	bl	_Func_c300
.L93a00:
	mov	r0, #1
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_9397c

@ FaceTargetHook
@ r0=entity. Per-frame hook that keeps the entity looking at the target at
@ +0x68: clears bit 0 of +0x5A so the mover does not also turn it toward its
@ heading, then sets the facing angle from the atan2 of the delta.
.thumb_func_start Func_93a14
	push	{r5, lr}
	mov	r5, r0
	ldr	r1, [r5, #0x68]
	cmp	r1, #0
	beq	.L93a5e
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #0xfe
	and	r3, r2
	strb	r3, [r0]
	ldr	r0, [r1, #0x10]
	ldr	r3, [r5, #0x10]
	ldr	r1, [r1, #8]
	sub	r0, r3
	ldr	r3, [r5, #8]
	sub	r1, r3
	bl	Func_44d0
	ldrh	r3, [r5, #6]
	lsl	r0, #16
	lsr	r0, #16
	sub	r0, r3
	lsl	r0, #16
	asr	r0, #16
	cmp	r0, #0
	beq	.L93a5e
	mov	r2, #0x80
	lsl	r2, #5
	cmp	r0, r2
	ble	.L93a52
	mov	r0, r2
.L93a52:
	ldr	r2, =0xfffff000
	cmp	r0, r2
	bge	.L93a5a
	mov	r0, r2
.L93a5a:
	add	r3, r0
	strh	r3, [r5, #6]
.L93a5e:
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_93a14

@ SetEntityBehaviour
@ r0=entity, r1=behaviour id 1..7. Installs one of seven canned behaviours by
@ jumping through the table at .L93a80 -- each arm points the entity at a
@ different script or hook (follow, chase, face, wander, and so on). Ids outside
@ 1..7 fall through and leave the entity unchanged.
@ This is the common entry point the slot helpers in rom_91584.s call.
.thumb_func_start Func_93a6c
	push	{r5, lr}
	sub	r3, r1, #1
	mov	r5, r0
	cmp	r3, #6
	bhi	.L93ac6
	ldr	r2, =.L93a80
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L93a80:
	.word	.L93a9c
	.word	.L93aa0
	.word	.L93aa4
	.word	.L93aa8
	.word	.L93aac
	.word	.L93ab0
	.word	.L93ac4
.L93a9c:
	ldr	r1, =.L9fe00
	b	.L93ac6
.L93aa0:
	ldr	r1, =.L9fd44
	b	.L93ac6
.L93aa4:
	ldr	r1, =.L9fe10
	b	.L93ac6
.L93aa8:
	ldr	r1, =.L9fecc
	b	.L93ac6
.L93aac:
	ldr	r1, =.L9ff18
	b	.L93ac6
.L93ab0:
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_92054
	ldr	r1, =.L9ff2c
	str	r0, [r5, #0x68]
	b	.L93ac6
.L93ac4:
	ldr	r1, =.L9fe04
.L93ac6:
	mov	r0, r5
	bl	_Func_c2d8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_93a6c

@ ScanNearbyEntities
@ Walks the 0x40 entities at [iwram_1e64] looking for ones within 0x28 units of
@ the reference position, collecting matches for the caller. The
@ ~130-instruction body is characterised structurally.
.thumb_func_start Func_93af8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r3, #0x28
	mov	r9, r3
	ldr	r3, =iwram_1e64
	mov	r2, #0
	mov	r10, r2
	ldr	r5, [r3]
	mov	r2, #0x3f
	mov	r7, r0
	mov	r11, r1
	mov	r8, r2
.L93b1a:
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.L93bb2
	cmp	r5, r7
	beq	.L93bb2
	mov	r3, r5
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L93bb2
	ldr	r1, [r5, #0xc]
	ldr	r3, [r7, #0xc]
	sub	r2, r1, r3
	cmp	r2, #0
	blt	.L93b40
	ldr	r3, =0x2fffff
	cmp	r2, r3
	ble	.L93b48
	b	.L93bb2
.L93b40:
	ldr	r2, =0x2fffff
	sub	r3, r1
	cmp	r3, r2
	bgt	.L93bb2
.L93b48:
	ldr	r2, [r5, #8]
	ldr	r3, [r7, #8]
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L93b56
	ldr	r3, =0xffff
	add	r0, r3
.L93b56:
	ldr	r2, [r5, #0x10]
	ldr	r3, [r7, #0x10]
	sub	r2, r3
	asr	r0, #16
	cmp	r2, #0
	bge	.L93b66
	ldr	r3, =0xffff
	add	r2, r3
.L93b66:
	asr	r3, r2, #16
	mov	r2, r0
	mul	r2, r0
	mov	r0, r2
	mov	r2, r3
	mul	r2, r3
	mov	r3, r2
	add	r0, r3
	ldr	r3, =Func_948
	bl	_call_via_r3
	mov	r6, r0
	cmp	r6, r9
	bge	.L93bb2
	ldr	r3, [r7, #0x10]
	ldr	r0, [r5, #0x10]
	ldr	r1, [r5, #8]
	sub	r0, r3
	ldr	r3, [r7, #8]
	sub	r1, r3
	bl	Func_44d0
	lsl	r0, #16
	lsr	r0, #16
	cmp	r6, #0x17
	ble	.L93bae
	ldrh	r3, [r7, #6]
	sub	r3, r0, r3
	lsl	r3, #16
	asr	r0, r3, #16
	ldr	r3, =0xffffd001
	cmp	r0, r3
	blt	.L93bb2
	ldr	r2, =0x2fff
	cmp	r0, r2
	bgt	.L93bb2
.L93bae:
	mov	r10, r5
	mov	r9, r6
.L93bb2:
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r2, r8
	add	r5, #0x70
	cmp	r2, #0
	bge	.L93b1a
	mov	r3, r10
	mov	r0, #0
	cmp	r3, #0
	beq	.L93bd8
	mov	r2, r10
	ldr	r3, [r2, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, r11
	bne	.L93bd8
	mov	r0, r10
.L93bd8:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_93af8

@ AdvanceDialogue
@ Takes no arguments. The handler behind message control codes 0xF9 and 0xFE
@ (see Func_8d8f0): resolves the player entity from ewram_240+0x1F4 and steps
@ the conversation to its next line, closing the current box and opening the
@ following one.
.thumb_func_start Func_93c00
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	sub	sp, #0x14
	bl	Func_92054
	mov	r3, #1
	neg	r3, r3
	str	r3, [sp, #4]
	mov	r6, r0
	ldrh	r3, [r6, #6]
	mov	r2, #0x80
	lsl	r2, #6
	add	r2, r3
	mov	r3, #0xc0
	lsl	r3, #8
	and	r2, r3
	mov	r3, #0x55
	add	r3, r6
	mov	r9, r2
	ldrb	r2, [r3]
	mov	r8, r3
	str	r2, [sp]
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r10, r3
	mov	r3, #1
	mov	r11, r3
	add	r7, sp, #8
.L93c4c:
	ldr	r3, [r6, #8]
	ldr	r5, =0xfff00000
	mov	r2, #0x80
	lsl	r2, #12
	and	r3, r5
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	and	r3, r5
	add	r3, r2
	mov	r0, #0x80
	lsl	r0, #13
	mov	r1, r9
	str	r3, [r7, #8]
	mov	r2, r7
	bl	Func_447c
	mov	r0, r6
	mov	r1, r7
	bl	_Func_120dc
	cmp	r0, #1
	bne	.L93c84
	mov	r0, #1
	neg	r0, r0
	b	.L93e00
.L93c84:
	ldr	r3, [r6, #8]
	mov	r2, #0x80
	lsl	r2, #12
	and	r3, r5
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	and	r3, r5
	add	r3, r2
	mov	r0, #0x80
	lsl	r0, #14
	mov	r1, r9
	str	r3, [r7, #8]
	mov	r2, r7
	bl	Func_447c
	mov	r0, r6
	mov	r1, r7
	bl	_Func_120dc
	cmp	r0, #0
	beq	.L93cb6
	b	.L93dfe
.L93cb6:
	mov	r3, r6
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L93cc8
	ldr	r3, [r6, #0x50]
	add	r3, #0x26
	ldrb	r3, [r3]
	mov	r11, r3
.L93cc8:
	bl	Func_916b0
	mov	r1, #6
	mov	r0, r6
	bl	_Func_c300
	mov	r0, #6
	bl	Func_30f8
	mov	r0, #0x98
	bl	_Func_f9080
	mov	r0, r6
	mov	r1, #7
	bl	_Func_c300
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r6, #0x30]
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r6, #0x34]
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r6, #0x28]
	mov	r3, r8
	ldrb	r2, [r3]
	mov	r3, #0x7e
	and	r3, r2
	mov	r2, r8
	strb	r3, [r2]
	mov	r1, #0xfe
	mov	r3, r11
	and	r1, r3
	mov	r0, r6
	bl	_Func_c528
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	mov	r3, #0xa
	ldrsh	r2, [r7, r3]
	mov	r3, #2
	ldrsh	r1, [r7, r3]
	bl	Func_92158
	mov	r0, r6
	mov	r1, #6
	bl	_Func_c300
	mov	r0, r6
	mov	r1, r11
	bl	_Func_c528
	mov	r0, r6
	mov	r1, #0xcf
	bl	Func_93af8
	cmp	r0, #0
	bne	.L93d50
	mov	r0, r6
	mov	r1, #0xcd
	bl	Func_93af8
	cmp	r0, #0
	beq	.L93da0
.L93d50:
	mov	r1, #7
	bl	_Func_c300
	ldr	r5, =0xffff0000
	ldr	r3, [r6, #0xc]
	add	r3, r5
	str	r3, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	add	r3, r5
	str	r3, [r6, #0x14]
	mov	r0, #2
	bl	Func_30f8
	ldr	r3, [r6, #0xc]
	add	r3, r5
	str	r3, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	add	r3, r5
	str	r3, [r6, #0x14]
	mov	r0, #0xa
	bl	Func_30f8
	mov	r5, #0x80
	ldr	r3, [r6, #0xc]
	lsl	r5, #9
	add	r3, r5
	str	r3, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	add	r3, r5
	str	r3, [r6, #0x14]
	mov	r0, #4
	bl	Func_30f8
	ldr	r3, [r6, #0xc]
	add	r3, r5
	str	r3, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	add	r3, r5
	str	r3, [r6, #0x14]
	b	.L93da6
.L93da0:
	mov	r0, #6
	bl	Func_30f8
.L93da6:
	mov	r2, sp
	ldrb	r2, [r2]
	mov	r3, r8
	strb	r2, [r3]
	bl	Func_91750
	mov	r3, r10
	cmp	r3, #0
	beq	.L93dd8
	mov	r3, #0xd8
	lsl	r3, #1
	add	r3, r10
	mov	r1, #0x80
	ldr	r4, =Func_888
	ldr	r0, [r3]
	lsl	r1, #14
	.call_via r4
	mov	r2, #0xda
	lsl	r2, #1
	add	r2, r10
	ldr	r3, [r2]
	add	r3, r0
	str	r3, [r2]
.L93dd8:
	mov	r3, r6
	add	r3, #0x22
	ldrb	r0, [r3]
	ldr	r1, [r7]
	ldr	r2, [r7, #8]
	bl	_Func_12038
	cmp	r0, #0xf9
	bne	.L93dfa
	mov	r0, r6
	mov	r1, #1
	bl	_Func_c300
	mov	r0, #6
	bl	Func_30f8
	b	.L93c4c
.L93dfa:
	mov	r2, #0
	str	r2, [sp, #4]
.L93dfe:
	ldr	r0, [sp, #4]
.L93e00:
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_93c00

@ DialogueChoiceA
@ Takes no arguments. The first branch of the 0xFD two-way prompt -- taken when
@ A is pressed. Reads the player entity from ewram_240+0x1F4 and follows the
@ affirmative path of the conversation.
.thumb_func_start Func_93e28
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r0, =ewram_240
	mov	r1, #0xfa
	mov	r8, r0
	lsl	r1, #1
	add	r1, r8
	ldr	r0, [r1]
	mov	r11, r1
	sub	sp, #0x18
	bl	Func_92054
	mov	r6, r0
	ldr	r3, =0xfff0
	mov	r2, #0xa
	ldrsh	r5, [r6, r2]
	mov	r1, #0x12
	ldrsh	r7, [r6, r1]
	and	r5, r3
	and	r7, r3
	mov	r0, #8
	mov	r2, #8
	add	r0, r5
	add	r2, r7
	mov	r10, r0
	mov	r9, r2
	bl	Func_916b0
	mov	r3, #0xf9
	lsl	r3, #1
	add	r8, r3
	mov	r0, r8
	ldrb	r3, [r0]
	cmp	r3, #0
	bne	.L93f2e
	mov	r3, r10
	cmp	r3, #0
	bge	.L93e82
	mov	r3, r5
	add	r3, #0x17
.L93e82:
	asr	r2, r3, #4
	mov	r3, r9
	cmp	r3, #0
	bge	.L93e8e
	mov	r3, r7
	add	r3, #0x17
.L93e8e:
	asr	r3, #4
	lsl	r3, #7
	add	r3, r2, r3
	ldr	r1, =ewram_10000
	ldr	r0, =ewram_10200
	lsl	r3, #2
	add	r2, r3, r1
	add	r3, r0
	ldrb	r2, [r2, #2]
	ldrb	r3, [r3, #2]
	cmp	r2, r3
	bne	.L93f72
	ldr	r3, [r6, #8]
	mov	r0, sp
	str	r3, [r0]
	ldr	r1, =0xfff00000
	ldr	r3, [r6, #0xc]
	add	r3, r1
	str	r3, [r0, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r0, #8]
	bl	_Func_1219c
	mov	r7, r0
	cmp	r7, #0
	bne	.L93f72
	mov	r2, r11
	ldr	r0, [r2]
	mov	r1, r10
	mov	r2, r9
	bl	Func_92158
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r6, #0x30]
	mov	r1, #0xc0
	mov	r3, r11
	mov	r2, #0
	ldr	r0, [r3]
	lsl	r1, #8
	bl	Func_92adc
	mov	r1, r11
	ldr	r0, [r1]
	bl	Func_920e8
	mov	r3, r6
	add	r3, #0x5a
	mov	r5, #1
	strb	r5, [r3]
	sub	r3, #5
	strb	r7, [r3]
	mov	r0, r6
	mov	r1, #0
	bl	_Func_c528
	mov	r0, r6
	mov	r1, #0xd
	bl	_Func_c300
	mov	r2, r10
	lsl	r1, r2, #16
	ldr	r3, =0xfff00000
	ldr	r2, [r6, #0xc]
	mov	r0, r9
	add	r2, r3
	lsl	r3, r0, #16
	mov	r0, #0x80
	lsl	r0, #13
	add	r3, r0
	mov	r0, r6
	bl	_Func_d14c
	mov	r1, r11
	ldr	r0, [r1]
	bl	Func_923c4
	mov	r2, r8
	strb	r5, [r2]
	b	.L93f6a
.L93f2e:
	mov	r0, r6
	mov	r1, #0xa
	bl	_Func_c300
	mov	r2, r6
	add	r2, #0x55
	mov	r3, #3
	strb	r3, [r2]
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r6, #0x28]
	ldr	r3, [r6, #0xc]
	mov	r0, r6
	str	r3, [r6, #0x14]
	mov	r1, #1
	bl	_Func_c528
	mov	r0, #6
	bl	Func_9163c
	mov	r5, #0
	mov	r3, r8
	mov	r2, r6
	strb	r5, [r3]
	add	r2, #0x5a
	mov	r3, #1
	strb	r3, [r2]
	mov	r3, #0xc0
	lsl	r3, #8
	strh	r3, [r6, #6]
.L93f6a:
	bl	Func_91750
	mov	r0, #0
	b	.L93f7a
.L93f72:
	bl	Func_91750
	mov	r0, #1
	neg	r0, r0
.L93f7a:
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_93e28

@ DialogueChoiceB
@ Takes no arguments. The second branch of the 0xFD prompt -- taken when B is
@ pressed. Unlike Func_93e28 it consults the save block at ewram_434 as well,
@ so the decline path can depend on stored progress.
.thumb_func_start Func_93fa0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r1, =ewram_434
	ldr	r0, =ewram_240
	mov	r9, r0
	ldr	r0, [r1]
	sub	sp, #0x18
	bl	Func_92054
	mov	r7, r0
	mov	r3, #0xa
	ldrsh	r5, [r7, r3]
	mov	r1, #0x12
	ldrsh	r6, [r7, r1]
	ldr	r3, =0xfff0
	mov	r2, #1
	and	r5, r3
	and	r6, r3
	mov	r11, r2
	mov	r0, #8
	mov	r2, #8
	add	r0, r5
	add	r2, r6
	mov	r8, r0
	mov	r10, r2
	bl	Func_916b0
	mov	r3, r7
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L93ff2
	ldr	r3, [r7, #0x50]
	add	r3, #0x26
	ldrb	r3, [r3]
	mov	r11, r3
.L93ff2:
	mov	r3, #0xf9
	lsl	r3, #1
	add	r9, r3
	mov	r0, r9
	ldrb	r3, [r0]
	cmp	r3, #0
	bne	.L940b8
	mov	r3, r8
	cmp	r3, #0
	bge	.L9400a
	mov	r3, r5
	add	r3, #0x17
.L9400a:
	asr	r2, r3, #4
	mov	r3, r10
	cmp	r3, #0
	bge	.L94016
	mov	r3, r6
	add	r3, #0x17
.L94016:
	asr	r3, #4
	lsl	r3, #7
	add	r3, r2, r3
	ldr	r1, =ewram_10000
	ldr	r0, =ewram_fe00
	lsl	r3, #2
	add	r2, r3, r1
	add	r3, r0
	ldrb	r2, [r2, #2]
	ldrb	r3, [r3, #2]
	cmp	r2, r3
	beq	.L94030
	b	.L94138
.L94030:
	ldr	r3, [r7, #8]
	mov	r0, sp
	str	r3, [r0]
	ldr	r3, [r7, #0xc]
	str	r3, [r0, #4]
	ldr	r3, [r7, #0x10]
	str	r3, [r0, #8]
	bl	_Func_1219c
	mov	r5, r0
	cmp	r5, #0
	bne	.L94138
	mov	r6, r7
	add	r6, #0x5a
	ldr	r1, =ewram_434
	strb	r5, [r6]
	mov	r2, r10
	ldr	r0, [r1]
	mov	r1, r8
	bl	Func_92158
	mov	r1, #6
	mov	r0, r7
	bl	_Func_c300
	mov	r0, #4
	bl	Func_30f8
	mov	r1, #7
	mov	r0, r7
	bl	_Func_c300
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r7, #0x28]
	mov	r0, #4
	bl	Func_30f8
	mov	r3, r7
	add	r3, #0x55
	strb	r5, [r3]
	mov	r2, r11
	mov	r3, #0xfe
	and	r2, r3
	mov	r11, r2
	mov	r0, r7
	mov	r1, r11
	bl	_Func_c528
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r7, #0x30]
	mov	r0, r7
	mov	r1, #0xc
	str	r5, [r7, #0x28]
	bl	_Func_c300
	mov	r0, #4
	bl	Func_30f8
	mov	r3, #1
	mov	r0, r9
	strb	r3, [r0]
	strb	r3, [r6]
	mov	r0, #8
	bl	Func_30f8
	b	.L94112
.L940b8:
	mov	r5, r7
	add	r5, #0x55
	mov	r6, #0
	strb	r6, [r5]
	mov	r0, r7
	mov	r1, #0xb
	bl	_Func_c300
	mov	r2, r8
	lsl	r1, r2, #16
	mov	r3, #0x80
	ldr	r2, [r7, #0xc]
	lsl	r3, #12
	mov	r0, r10
	add	r2, r3
	lsl	r3, r0, #16
	ldr	r0, =0xfff00000
	add	r3, r0
	mov	r0, r7
	bl	_Func_d14c
	ldr	r1, =ewram_434
	ldr	r0, [r1]
	bl	Func_923c4
	mov	r3, #3
	strb	r3, [r5]
	ldr	r5, .L9411c	@ 1
	mov	r2, r11
	ldr	r3, [r7, #0xc]
	orr	r2, r5
	mov	r11, r2
	str	r3, [r7, #0x14]
	mov	r0, r7
	mov	r1, r11
	bl	_Func_c528
	mov	r0, #4
	bl	Func_9163c
	mov	r3, r9
	strb	r6, [r3]
	mov	r3, r7
	add	r3, #0x5a
	strb	r5, [r3]
.L94112:
	bl	Func_91750
	mov	r0, #0
	b	.L94140

	.align	2, 0
.L9411c:
	.word	1
	.pool

.L94138:
	bl	Func_91750
	mov	r0, #1
	neg	r0, r0
.L94140:
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_93fa0

@ IsSlotWithinCameraView
@ r0=slot, r1=margin. Returns 0 when the slot entity lies inside the current
@ camera view expanded by r1, and -1 when it does not or the slot is empty.
.thumb_func_start Func_94154
	push	{r5, r6, lr}
	mov	r5, r1
	bl	Func_8ba1c
	mov	r4, r0
	cmp	r4, #0
	bne	.L94168
	mov	r0, #1
	neg	r0, r0
	b	.L941c8
.L94168:
	ldr	r3, =iwram_1e70
	ldr	r3, [r3]
	add	r3, #0xe4
	ldr	r1, =0xffff0000
	ldr	r0, [r3]
	ldr	r2, [r3, #4]
	ldr	r3, [r4, #0x10]
	and	r2, r1
	and	r0, r1
	ldr	r1, [r4, #8]
	sub	r3, r2
	ldr	r2, [r4, #0xc]
	sub	r1, r0
	sub	r6, r3, r2
	mov	r2, r5
	add	r5, #4
	cmp	r1, #0
	bge	.L94190
	ldr	r3, =0xffff
	add	r1, r3
.L94190:
	asr	r3, r1, #16
	str	r3, [r2]
	mov	r3, r6
	cmp	r3, #0
	bge	.L9419e
	ldr	r2, =0xffff
	add	r3, r2
.L9419e:
	asr	r3, #16
	str	r3, [r5]
	mov	r3, r4
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L941c6
	ldr	r3, [r4, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bl	_Func_185008
	ldr	r3, [r5]
	mov	r2, #8
	ldrsb	r2, [r0, r2]
	sub	r3, r2
	str	r3, [r5]
.L941c6:
	mov	r0, #0
.L941c8:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_94154

@ Nop -- empty hook.
.thumb_func_start Func_941dc
	bx	lr
.func_end Func_941dc

@ RunPendingSceneEvent
@ Takes no arguments. Reads the pending event id at ewram_240+0x1EE and, if one
@ is queued, runs it against the scene block at iwram_1ebc. This is how a map
@ transition hands a cutscene to the newly loaded map.
.thumb_func_start Func_941e0
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	mov	r1, #0xf7
	ldr	r6, [r3]
	ldr	r3, =ewram_240
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	sub	sp, #8
	bl	_Func_f9080
	mov	r0, #0x90
	lsl	r0, #1
	bl	_Func_f9080
	mov	r0, #0x93
	bl	_Func_f9080
	mov	r1, #0xcf
	lsl	r1, #1
	add	r3, r6, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L94266
	ldr	r2, =0x7fff
	ldr	r3, =0x50001e6
	strh	r2, [r3]
	ldr	r0, =0x401
	mov	r1, #0x10
	bl	Func_901c0
	mov	r3, #0xe3
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #0
	strh	r3, [r2]
	mov	r0, #0x10
	bl	Func_30f8
	mov	r7, #0xf0
	mov	r1, #0xf0
	mov	r5, #0
	lsl	r1, #2
	lsl	r7, #7
	mov	r6, #0x1e
.L94242:
	mov	r3, r7
	orr	r3, r1
	ldr	r2, =0x50001e6
	orr	r3, r6
	strh	r3, [r2]
	mov	r0, #1
	str	r1, [sp, #4]
	bl	Func_30f8
	ldr	r2, =0xfffff800
	ldr	r1, [sp, #4]
	add	r5, #1
	sub	r1, #0x40
	add	r7, r2
	sub	r6, #2
	cmp	r5, #0xf
	ble	.L94242
	b	.L942b6
.L94266:
	ldr	r3, =0x7fff
	mov	r5, #0xa0
	lsl	r5, #19
	strh	r3, [r5]
	ldr	r0, =0x207
	mov	r1, #0x10
	bl	Func_901c0
	mov	r3, #0xe3
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #0
	strh	r3, [r2]
	mov	r0, #0x10
	bl	Func_30f8
	mov	r7, #0xf0
	mov	r2, #0xf0
	mov	r8, r5
	lsl	r2, #2
	lsl	r7, #7
	mov	r6, #0x1e
	mov	r5, #0xf
.L94294:
	mov	r3, r7
	orr	r3, r2
	orr	r3, r6
	mov	r1, r8
	strh	r3, [r1]
	mov	r0, #1
	str	r2, [sp]
	bl	Func_30f8
	ldr	r3, =0xfffff800
	ldr	r2, [sp]
	sub	r5, #1
	sub	r2, #0x40
	add	r7, r3
	sub	r6, #2
	cmp	r5, #0
	bge	.L94294
.L942b6:
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_941e0

@ PlayPartyEffectAnimation
@ r0 = effect id. Takes the party entity from ewram_240+0x1F4 through
@ Func_8ba1c, starts animation 0x1B on its actor with _Func_b8ac, snaps the
@ entity to a fixed position -- x's low 20 bits cleared and 0x80000 added, z's
@ cleared and 0x100000 added -- zeroes its velocity words at +0x24 and +0x2C,
@ sets +0x38 and +0x40 to 0x80000000, runs the effect through _Func_c300 and
@ waits eighteen frames.
@
@ NOTE: declared `.thumb_func_Start` with a capital S. GAS accepts it and the
@ bytes are identical, but a case-sensitive scan of the sources misses this
@ function -- it and Func_97f80 in rom_97b54.s are the only two in the ROM with
@ the typo, along with Func_a1c6c and Func_a1f74 in rom_a1000.
.thumb_func_Start Func_942e0
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r8, r0
	ldr	r3, =ewram_240
	mov	r0, #0xfa
	lsl	r0, #1
	add	r3, r0
	ldr	r0, [r3]
	bl	Func_8ba1c
	mov	r5, r0
	ldr	r6, [r5, #0x50]
	mov	r1, #0x1b
	mov	r0, r6
	bl	_Func_b8ac
	add	r6, #0x26
	mov	r1, #0
	strb	r1, [r6]
	mov	r3, #0xf
	strb	r3, [r0, #5]
	ldr	r2, =0xfff00000
	ldr	r3, [r5, #8]
	mov	r0, #0x80
	and	r3, r2
	lsl	r0, #12
	add	r3, r0
	str	r3, [r5, #8]
	ldr	r3, [r5, #0x10]
	and	r3, r2
	mov	r2, #0x80
	lsl	r2, #13
	add	r3, r2
	str	r3, [r5, #0x10]
	mov	r3, #0x80
	lsl	r3, #24
	str	r1, [r5, #0x24]
	str	r1, [r5, #0x2c]
	mov	r0, r5
	str	r3, [r5, #0x38]
	str	r3, [r5, #0x40]
	mov	r1, r8
	bl	_Func_c300
	mov	r0, #0x12
	bl	Func_30f8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_942e0

@ SetProgressFlagA
@ Takes no arguments. Runs effect 0x1A through Func_942e0 and sets event flag
@ 0x120. Called from the examine handler in rom_8d5dc.s for one outcome class.
.thumb_func_start Func_94354
	push	{lr}
	mov	r0, #0x1a
	bl	Func_942e0
	mov	r0, #0x90
	lsl	r0, #1
	bl	_Func_79358
	pop	{r0}
	bx	r0
.func_end Func_94354

@ SetProgressFlagB
@ Takes no arguments. Runs effect 0x19 through Func_942e0 and sets event flag
@ 0x121 -- the sibling of Func_94354 for the other outcome class.
.thumb_func_start Func_94368
	push	{lr}
	mov	r0, #0x19
	bl	Func_942e0
	ldr	r0, =0x121
	bl	_Func_79358
	pop	{r0}
	bx	r0
.func_end Func_94368

@ RunPlayerReactionSequence
@ r0=reaction id. Plays a scripted reaction on the player entity (from
@ ewram_240+0x1F4): animation, effect and a short wait. The ~40-instruction
@ body is characterised structurally.
.thumb_func_start Func_94380
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =ewram_240
	mov	r1, #0xfa
	lsl	r1, #1
	add	r3, r1
	mov	r6, r0
	ldr	r0, [r3]
	bl	Func_8ba1c
	mov	r5, r0
	ldr	r2, [r5, #0x50]
	mov	r8, r2
	mov	r0, r8
	mov	r1, #0x1b
	bl	_Func_b8ac
	mov	r3, #0
	mov	r10, r3
	mov	r7, r8
	add	r7, #0x26
	mov	r1, r10
	strb	r1, [r7]
	mov	r3, #0xf
	ldr	r2, =0xfff00000
	strb	r3, [r0, #5]
	mov	r1, #0x80
	ldr	r3, [r5, #8]
	lsl	r1, #12
	and	r3, r2
	mov	r9, r1
	add	r3, r9
	str	r3, [r5, #8]
	ldr	r3, [r5, #0x10]
	and	r3, r2
	mov	r1, r6
	str	r3, [r5, #0x10]
	mov	r0, r5
	bl	_Func_c300
	mov	r0, #0x1e
	bl	Func_30f8
	mov	r3, r8
	add	r3, #0x27
	mov	r6, #1
	strb	r6, [r3]
	mov	r2, r8
	ldr	r0, [r2, #0x2c]
	bl	_Func_bc48
	mov	r3, r10
	mov	r1, r8
	str	r3, [r1, #0x2c]
	strb	r6, [r7]
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r5, #0x34]
	str	r3, [r5, #0x30]
	ldr	r3, [r5, #0x10]
	mov	r0, r5
	ldr	r1, [r5, #8]
	ldr	r2, [r5, #0xc]
	add	r3, r9
	bl	_Func_d14c
	mov	r0, r5
	bl	_Func_ca6c
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_94380

@ CheckProgressFlags
@ Takes no arguments. Tests event flag 0x120 (and the companion 0x121) with
@ _Func_79338 and returns which of the two outcome states the save is in --
@ the read side of Func_94354 / Func_94368.
.thumb_func_start Func_94428
	push	{r5, r6, lr}
	mov	r5, #0x90
	lsl	r5, #1
	mov	r0, r5
	mov	r6, #0
	bl	_Func_79338
	cmp	r0, #0
	beq	.L9444a
	mov	r0, #0x18
	bl	Func_94380
	mov	r0, r5
	bl	_Func_79374
	mov	r6, #1
	b	.L944da
.L9444a:
	ldr	r5, =0x121
	mov	r0, r5
	bl	_Func_79338
	cmp	r0, #0
	beq	.L94466
	mov	r0, #0x17
	bl	Func_94380
	mov	r0, r5
	bl	_Func_79374
	mov	r6, #2
	b	.L944da
.L94466:
	mov	r5, #0x91
	lsl	r5, #1
	mov	r0, r5
	bl	_Func_79338
	cmp	r0, #0
	beq	.L944da
	mov	r0, r5
	bl	_Func_79374
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r6, [r3]
	mov	r0, r6
	bl	Func_8ba1c
	mov	r5, r0
	ldr	r3, [r5, #0xc]
	mov	r2, #0xa0
	lsl	r2, #16
	add	r3, r2
	mov	r2, #1
	neg	r2, r2
	str	r3, [r5, #0xc]
	mov	r0, r2
	mov	r1, r2
	mov	r3, #0
	bl	Func_933f8
	b	.L944ac
.L944a6:
	mov	r0, #1
	bl	Func_30f8
.L944ac:
	ldr	r2, [r5, #0x28]
	ldr	r3, [r5, #0xc]
	add	r3, r2
	ldr	r2, [r5, #0x14]
	cmp	r3, r2
	bgt	.L944a6
	mov	r0, #0x9f
	bl	_Func_f9080
	ldr	r3, [r5, #0x14]
	mov	r1, #0x16
	str	r3, [r5, #0xc]
	mov	r0, r5
	bl	_Func_c300
	mov	r0, #0xf
	bl	Func_9163c
	mov	r0, r6
	mov	r1, #1
	bl	Func_9335c
	mov	r6, #3
.L944da:
	mov	r0, r6
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_94428

	.section .rodata

.L9fc28:
	.incrom 0x9fc28, 0x9fc2c
.L9fc2c:
	.incrom 0x9fc2c, 0x9fd38
.L9fd38:
	.incrom 0x9fd38, 0x9fd44
.L9fd44:
	.incrom 0x9fd44, 0x9fe00
.L9fe00:
	.incrom 0x9fe00, 0x9fe04
.L9fe04:
	.incrom 0x9fe04, 0x9fe10
.L9fe10:
	.incrom 0x9fe10, 0x9fecc
.L9fecc:
	.incrom 0x9fecc, 0x9ff18
.L9ff18:
	.incrom 0x9ff18, 0x9ff2c
.L9ff2c:
	.incrom 0x9ff2c, 0x9ff40
