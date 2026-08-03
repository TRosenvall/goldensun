	.include "macros.inc"

@ CheckPlateePuzzle
@ Takes no arguments. The puzzle logic, re-run whenever a block moves.
@
@ Slots 0x0B and 0x0C are the blocks. Each is tested against the single target
@ tile (0x23, 0x17) -- both coordinates at whole-tile resolution -- and its own
@ save bit is set or CLEARED accordingly: 0x303 for slot 0x0B, 0x304 for slot
@ 0x0C. Clearing on failure is what lets the player undo a wrong move.
@
@ The gate then follows the OR of the two: if either bit is set and the gate
@ bit 0x302 is not, sound 0xD2 plays, slot 0x11 runs animation 6 and two
@ attribute cells are repainted from column 0 -- the gate opening. If neither
@ is set and 0x302 IS, sound 0xDC plays, slot 0x11 returns to animation 2 and
@ the same two cells are repainted from column 1 -- the gate closing again.
@ 0x302 tracks which state the gate is currently in, so the transition fires
@ once per change rather than every frame.
.thumb_func_start OvlFunc_920_2008304
	push	{r5, r6, lr}
	mov	r0, #0xb
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0xc
	bl	__MapActor_GetActor
	ldr	r3, [r5, #8]
	asr	r3, #20
	mov	r6, r0
	cmp	r3, #0x23
	bne	.L330
	ldr	r3, [r5, #0x10]
	asr	r3, #20
	cmp	r3, #0x17
	bne	.L330
	ldr	r0, =0x303
	bl	__SetFlag
	b	.L336
.L330:
	ldr	r0, =0x303
	bl	__ClearFlag
.L336:
	ldr	r3, [r6, #8]
	asr	r3, #20
	cmp	r3, #0x23
	bne	.L350
	ldr	r3, [r6, #0x10]
	asr	r3, #20
	cmp	r3, #0x17
	bne	.L350
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__SetFlag
	b	.L358
.L350:
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__ClearFlag
.L358:
	ldr	r0, =0x303
	bl	__GetFlag
	cmp	r0, #0
	bne	.L36e
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3c2
.L36e:
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3ba
	bl	__CutsceneStart
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xd2
	bl	__PlaySound
	mov	r0, #0x11
	mov	r1, #6
	bl	__MapActor_DoAnim
	mov	r3, #0x16
	str	r3, [sp, #4]
	mov	r5, #0x24
	mov	r0, #0
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #2
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	bl	__CutsceneEnd
.L3ba:
	ldr	r0, =0x302
	bl	__SetFlag
	b	.L414
.L3c2:
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	beq	.L40e
	bl	__CutsceneStart
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xdc
	bl	__PlaySound
	mov	r0, #0x11
	mov	r1, #2
	bl	__MapActor_DoAnim
	mov	r3, #0x16
	str	r3, [sp, #4]
	mov	r5, #0x24
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #1
	mov	r1, #2
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	bl	__CutsceneEnd
.L40e:
	ldr	r0, =0x302
	bl	__ClearFlag
.L414:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_920_2008304
