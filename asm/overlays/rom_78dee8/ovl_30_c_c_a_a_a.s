	.include "macros.inc"

@ Slot 3: read after slot 4, so it must stay in step with OvlFunc_ec below.
@ Area 0x10 splits three ways on the entrance id at ewram_240+0x1C2:
@     entrance 0x0B..0x0D  -> .L2050
@     entrance 0x0E..0x10  -> .L21b8
@     anything else        -> .L1fd8, and ONLY this branch first passes the
@                             table through Func_8b868, which tags the records
@                             whose position falls inside the active bounds.
@                             The other tables are pre-tagged constants.
@ Area 0x13 -> .L22a8. Otherwise -> .L1fc0.
.thumb_func_start OvlFunc_895_200807c
	push	{r5, lr}
	ldr	r1, =gState
	mov	r0, #0xe0
	lsl	r0, #1
	add	r3, r1, r0
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r3, =0x10
	cmp	r2, r3
	bne	.Lba
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r1, r2
	mov	r0, #0
	ldrsh	r3, [r3, r0]
	cmp	r3, #0xb
	blt	.Lae
	cmp	r3, #0xd
	ble	.Laa
	cmp	r3, #0x10
	bgt	.Lae
	ldr	r0, =.L21b8
	b	.Lc6
.Laa:
	ldr	r0, =.L2050
	b	.Lc6
.Lae:
	ldr	r5, =.L1fd8
	mov	r0, r5
	bl	__Func_808b868
	mov	r0, r5
	b	.Lc6
.Lba:
	ldr	r3, =0x13
	cmp	r2, r3
	bne	.Lc4
	ldr	r0, =.L22a8
	b	.Lc6
.Lc4:
	ldr	r0, =.L1fc0
.Lc6:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_895_200807c

@ Slot 4: the map object table, split on the same areas and entrance ranges as
@ slot 3 and in the same order, so the two stay aligned:
@     area 0x13                       -> .L22e4
@     area 0x10, entrance 0x0B..0x0D  -> .L241c
@     area 0x10, entrance 0x0E..0x10  -> .L2524
@     area 0x10, otherwise            -> .L232c
@     any other area                  -> .L22d8
.thumb_func_start OvlFunc_895_20080ec
	push	{lr}
	ldr	r1, =gState
	mov	r0, #0xe0
	lsl	r0, #1
	add	r3, r1, r0
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r3, =0x13
	cmp	r2, r3
	bne	.L104
	ldr	r0, =.L22e4
	b	.L12e
.L104:
	ldr	r3, =0x10
	cmp	r2, r3
	bne	.L12c
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r1, r2
	mov	r0, #0
	ldrsh	r3, [r3, r0]
	cmp	r3, #0xb
	blt	.L128
	cmp	r3, #0xd
	ble	.L124
	cmp	r3, #0x10
	bgt	.L128
	ldr	r0, =.L2524
	b	.L12e
.L124:
	ldr	r0, =.L241c
	b	.L12e
.L128:
	ldr	r0, =.L232c
	b	.L12e
.L12c:
	ldr	r0, =.L22d8
.L12e:
	pop	{r1}
	bx	r1
.func_end OvlFunc_895_20080ec

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
