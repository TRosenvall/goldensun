	.include "macros.inc"

@ TryPushBlock
@ Takes no arguments. This overlay's block-push interaction, the same shape as
@ OvlFunc_c4 in overlays/rom_780898/ovl_30.s but simpler: one tile, no
@ footprint table, and no map-collision restamp.
@
@ Steps one tile along the player's facing through the table at .L265c, finds
@ the block with OvlFunc_79c, sets its tile-type byte +0x22 to 2 so TestCollision
@ samples the right layer, and rejects the push if TestCollision returns > 0.
@ Otherwise: animation 8, fifteen frames, sound 0xB9, both block and player
@ given speed 0x3333 and sent one tile with Actor_TravelTo, Func_ca6c waits it out.
@
@ It then re-runs the position trackers for whichever arrangement this entrance
@ uses -- slots 9 and 0xA for entrance 0x0B..0x0D, slots 9..0xE for 0x0E..0x10 --
@ so the save bits stay current after every single push.
.thumb_func_start OvlFunc_895_20087d0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0
	sub	sp, #0xc
	bl	__MapActor_GetActor
	ldrh	r3, [r0, #6]
	ldr	r2, =.L265c
	lsr	r3, #12
	lsl	r5, r3, #2
	ldr	r3, [r2, r5]
	mov	r8, r0
	mov	r1, #0xa
	ldrsh	r0, [r0, r1]
	mov	r10, r2
	asr	r2, r3, #16
	add	r0, r2
	mov	r2, r8
	mov	r4, #0x12
	ldrsh	r1, [r2, r4]
	lsl	r3, #16
	asr	r3, #16
	add	r1, r3
	asr	r0, #4
	asr	r1, #4
	bl	OvlFunc_895_200879c
	mov	r7, r0
	cmp	r7, #0
	beq	.L8d4
	mov	r3, #0
	mov	r2, r7
	add	r2, #0x22
	mov	r9, r3
	mov	r3, #2
	strb	r3, [r2]
	mov	r4, r10
	ldr	r1, [r4, r5]
	ldr	r2, =0xffff0000
	ldr	r3, [r7, #8]
	and	r2, r1
	mov	r6, sp
	add	r3, r2
	str	r3, [r6]
	ldr	r3, [r7, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r7, #0x10]
	lsl	r1, #16
	add	r3, r1
	mov	r1, r6
	str	r3, [r6, #8]
	bl	__TestCollision
	cmp	r0, #0
	bgt	.L8d4
	mov	r1, #8
	mov	r0, r8
	bl	__Actor_SetAnim
	ldr	r5, =0x3333
	mov	r0, #0xf
	bl	__WaitFrames
	mov	r0, #0xb9
	bl	__PlaySound
	str	r5, [r7, #0x30]
	str	r5, [r7, #0x34]
	mov	r0, r7
	ldr	r1, [r6]
	ldr	r2, [r6, #4]
	ldr	r3, [r6, #8]
	bl	__Actor_TravelTo
	mov	r1, r8
	str	r5, [r1, #0x30]
	str	r5, [r1, #0x34]
	mov	r0, r8
	ldr	r1, [r6]
	ldr	r2, [r6, #4]
	ldr	r3, [r6, #8]
	bl	__Actor_TravelTo
	mov	r0, r7
	bl	__Actor_WaitMovement
	ldr	r3, [r6]
	str	r3, [r7, #8]
	ldr	r3, [r6, #8]
	mov	r2, r9
	str	r3, [r7, #0x10]
	mov	r1, #1
	str	r2, [r7, #0x24]
	str	r2, [r7, #0x2c]
	mov	r0, r8
	bl	__Actor_SetAnim
	ldr	r3, =gState
	mov	r4, #0xe1
	lsl	r4, #1
	add	r3, r4
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #0xb
	blt	.L8d4
	cmp	r3, #0xd
	ble	.L8b2
	cmp	r3, #0x10
	bgt	.L8d4
	b	.L8bc
.L8b2:
	bl	OvlFunc_895_200856c
	bl	OvlFunc_895_20085ac
	b	.L8d4
.L8bc:
	bl	OvlFunc_895_20085ec
	bl	OvlFunc_895_2008634
	bl	OvlFunc_895_200867c
	bl	OvlFunc_895_20086c4
	bl	OvlFunc_895_200870c
	bl	OvlFunc_895_2008754
.L8d4:
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_20087d0
