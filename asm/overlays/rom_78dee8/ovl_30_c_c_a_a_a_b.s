	.include "macros.inc"

@ WalkPlayerInThroughDoorway
@ Takes no arguments. A map-entry scene: the player walks in from off-screen
@ while the doorway animates shut behind them.
@
@ Sound 0xB5, then three CopyMapTiles rectangle copies at x = 0x1C, 0x1E, 0x20 --
@ the same 3x0x15 column stepped one tile right each time, ten frames apart --
@ which is the door closing frame by frame. Then draw priority 2, speed
@ 0x9999/0x4CCC, a walk to (0x78, 0x62) via Func_921c4, animation 2, and a
@ nudge of -8 on z. Closes by hiding the overlay, waiting the scene delay, and
@ leaving message id 2 pending for whatever runs next.
.thumb_func_start OvlFunc_895_2008154
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__CutsceneStart
	mov	r0, #0xb5
	bl	__PlaySound
	mov	r5, #3
	mov	r6, #2
	mov	r1, #0x1c
	mov	r2, #0x15
	mov	r3, #3
	mov	r0, #0x10
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__WaitFrames
	mov	r1, #0x1e
	mov	r2, #0x15
	mov	r3, #3
	mov	r0, #0x10
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__WaitFrames
	mov	r3, #3
	mov	r2, #0x15
	mov	r1, #0x20
	mov	r0, #0x10
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #2
	bl	__Func_8092b08
	mov	r0, #0
	ldr	r1, =0x9999
	ldr	r2, =0x4ccc
	bl	__MapActor_SetSpeed
	mov	r2, #0x62
	mov	r0, #0
	mov	r1, #0x78
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r2, #8
	mov	r1, #0
	neg	r2, r2
	mov	r0, #0
	bl	__Func_809228c
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #2
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2008154
