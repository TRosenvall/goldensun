	.include "macros.inc"

@ GetSlotSpriteId
@ r0=slot. Returns the sprite resource id of the slot's actor -- via the entity
@ (Func_8ba1c), its actor at +0x50 and that actor's first part at +0x28 -- or 0
@ if the slot is empty, not draw kind 1, or has no parts.
.thumb_func_start Func_91584
	push	{lr}
	bl	Func_8ba1c
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L915a0
	ldr	r0, [r0, #0x50]
	cmp	r0, #0
	beq	.L915a0
	ldr	r0, [r0, #0x28]
	cmp	r0, #0
	bne	.L915a4
.L915a0:
	mov	r0, #0
	b	.L915a8
.L915a4:
	mov	r3, #0
	ldrsh	r0, [r0, r3]
.L915a8:
	pop	{r1}
	bx	r1
.func_end Func_91584

@ GetActiveMessagePortrait
@ Returns the portrait id for the current speaker, biased by 0x100, or 0 when
@ there is none. Requires the portrait-enable byte at ewram_240+0x20A to be set
@ and the speaker record from Func_91560 to carry a portrait at +0x02 other than
@ the 0xFF "none" marker.
.thumb_func_start Func_915ac
	push	{lr}
	ldr	r3, =ewram_240
	ldr	r2, =0x20a
	add	r3, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L915c4
	bl	Func_91560
	ldrb	r0, [r0, #2]
	cmp	r0, #0xff
	bne	.L915c8
.L915c4:
	mov	r0, #0
	b	.L915ce
.L915c8:
	mov	r3, #0x80
	lsl	r3, #1
	add	r0, r3
.L915ce:
	pop	{r1}
	bx	r1
.func_end Func_915ac

@ GetActiveMessageVoice
@ Returns byte +0x03 of the current speaker record -- the voice/beep id used
@ while text scrolls. Calls Func_91584 first for its side effect of resolving
@ the speaker; the returned sprite id is discarded.
.thumb_func_start Func_915dc
	push	{lr}
	bl	Func_91584
	bl	Func_91560
	ldrb	r0, [r0, #3]
	pop	{r1}
	bx	r1
.func_end Func_915dc

@ DialogueFastForwardTask
@ Per-frame task registered by Func_916b0 at priority 0xC80. Takes no arguments.
@ Only acts while the input-enable flag iwram_1f54 is set. Reads iwram_1c94 and
@ writes the fast-forward flag at iwram_1ebc+0x1CC: L (0x200) clears it, R
@ (0x100) sets it to -1. Func_9163c consults that flag, so holding R collapses
@ every scripted wait in the scene to nothing.
.thumb_func_start Func_915ec
	push	{lr}
	ldr	r3, =iwram_1ebc
	ldr	r1, [r3]
	ldr	r3, =iwram_1f54
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L9162a
	ldr	r0, =iwram_1c94
	mov	r2, #0x80
	ldr	r3, [r0]
	lsl	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L91612
	mov	r3, #0xe6
	lsl	r3, #1
	add	r2, r1, r3
	mov	r3, #0
	str	r3, [r2]
.L91612:
	ldr	r3, [r0]
	mov	r2, #0x80
	lsl	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L9162a
	mov	r3, #0xe6
	lsl	r3, #1
	add	r2, r1, r3
	mov	r3, #1
	neg	r3, r3
	str	r3, [r2]
.L9162a:
	pop	{r0}
	bx	r0
.func_end Func_915ec

@ DialogueWait
@ r0=frames. Blocks for r0 frames with Func_30f8, unless the fast-forward flag
@ at iwram_1ebc+0x1CC is set or the count is zero. Every scripted pause in a
@ cutscene routes through here so the player's R-button skip is honoured
@ uniformly.
.thumb_func_start Func_9163c
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldr	r3, [r3]
	cmp	r3, #0
	bne	.L91656
	cmp	r0, #0
	beq	.L91656
	bl	Func_30f8
.L91656:
	pop	{r0}
	bx	r0
.func_end Func_9163c

@ ResetPlayerMovement
@ Takes no arguments. Puts the player entity (id at ewram_240+0x1F4) into a
@ clean standing state for a cutscene: max speed 0x10000 into +0x30,
@ acceleration 0x8000 into +0x34, both x and z targets set to the 0x80000000
@ "none" sentinel and their velocities zeroed.
@ Selects animation 0x0C when the transition-tile flag at ewram_240+0x1F2 is 1,
@ otherwise the normal idle animation 1.
.thumb_func_start Func_91660
	push	{r5, lr}
	ldr	r5, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r5, r2
	ldr	r0, [r3]
	bl	Func_8ba1c
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r0, #0x30]
	mov	r3, #0x80
	lsl	r3, #8
	str	r3, [r0, #0x34]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r0, #0x38]
	str	r3, [r0, #0x40]
	mov	r2, #0xf9
	mov	r3, #0
	str	r3, [r0, #0x24]
	str	r3, [r0, #0x2c]
	lsl	r2, #1
	add	r3, r5, r2
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L9169e
	mov	r1, #0xc
	bl	_Func_c300
	b	.L916a4
.L9169e:
	mov	r1, #1
	bl	_Func_c300
.L916a4:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_91660

@ BeginCutscene
@ Takes no arguments. Opens a scripted scene.
@ Calls _Func_1c428 to take over input, resets the player with Func_91660, and
@ tears down any live message box (Func_8e118) if one is flagged at +0xCB6.
@ Then initialises the dialogue state: clears +0xCC2 and +0xCC4, sets the
@ message delay at +0x1C8 to 0x10, clears the fast-forward flag at +0x1CC,
@ and sets the three message id slots at +0x1DA/+0x1DC/+0x1DE to their "none"
@ values (0xFFFF, -1, -1).
@ Registers Func_915ec at priority 0xC80, clears event flag 0x132, and captures
@ the current player id from ewram_240+0x1F4 into +0x1F4 while clearing +0x1F8.
@ Func_91750 is the matching close.
.thumb_func_start Func_916b0
	push	{r5, r6, lr}
	ldr	r3, =iwram_1ebc
	ldr	r6, [r3]
	bl	_Func_1c428
	bl	Func_91660
	ldr	r2, =0xcb6
	add	r3, r6, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	beq	.L916ce
	bl	Func_8e118
.L916ce:
	ldr	r2, =0xcc2
	mov	r5, #0
	add	r3, r6, r2
	add	r2, #2
	strh	r5, [r3]
	add	r3, r6, r2
	strh	r5, [r3]
	mov	r3, #0xe4
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #0x10
	str	r3, [r2]
	mov	r2, #0xe6
	lsl	r2, #1
	add	r3, r6, r2
	str	r5, [r3]
	mov	r3, #0xed
	lsl	r3, #1
	add	r2, r6, r3
	ldr	r3, =0xffff
	strh	r3, [r2]
	mov	r3, #0xee
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #1
	neg	r3, r3
	strh	r3, [r2]
	mov	r3, #0xef
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #1
	neg	r3, r3
	mov	r1, #0xc8
	strh	r3, [r2]
	lsl	r1, #4
	ldr	r0, =Func_915ec
	bl	Func_41d8
	mov	r0, #0x99
	lsl	r0, #1
	bl	_Func_79374
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	ldr	r3, [r3, r2]
	str	r3, [r6, r2]
	add	r2, #4
	add	r3, r6, r2
	str	r5, [r3]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_916b0

@ EndCutscene
@ Takes no arguments. Closes what Func_916b0 opened: unregisters the
@ fast-forward task with Func_4278, restores the player's normal control state
@ through Func_9335c(player, 1) and hands input back with _Func_77c10.
.thumb_func_start Func_91750
	push	{lr}
	ldr	r0, =Func_915ec
	bl	Func_4278
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	mov	r1, #1
	bl	Func_9335c
	bl	_Func_77c10
	pop	{r0}
	bx	r0
.func_end Func_91750

@ Nop1 -- empty hook, distinct address from Func_9177c.
.thumb_func_start Func_91778
	bx	lr
.func_end Func_91778

@ Nop2 -- empty hook called by Func_91780 before the map is entered; a place
@ to attach per-map setup without changing the caller.
.thumb_func_start Func_9177c
	bx	lr
.func_end Func_9177c

@ EnterMapAndGetPlayer
@ r0=scene descriptor. Calls the Func_9177c hook, spawns the player and camera
@ with Func_8b674, lets one frame pass so the spawn settles, and returns the
@ player entity looked up from ewram_240+0x1F4.
.thumb_func_start Func_91780
	push	{r5, lr}
	mov	r5, r0
	bl	Func_9177c
	mov	r0, r5
	bl	Func_8b674
	mov	r0, #1
	bl	Func_30f8
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_8ba1c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_91780

@ AddSceneRecord
@ r0=record. Allocates a scene entity slot with Func_8b824 and binds the record
@ to it with Func_8b3ec. Returns whatever Func_8b3ec returns; a full table
@ yields the -1 from Func_8b824.
.thumb_func_start Func_917ac
	push	{r5, lr}
	mov	r5, r0
	bl	Func_8b824
	mov	r1, r0
	mov	r0, r5
	bl	Func_8b3ec
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_917ac

@ SetSceneUserWord
@ r0=value. Stores r0 at iwram_1ebc+0x10, a general-purpose scene word that map
@ scripts read back later.
.thumb_func_start Func_917c4
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	str	r0, [r3, #0x10]
	bx	lr
.func_end Func_917c4

@ RefreshPartyMemberStats
@ r0=party member id, r1=optional equipment argument. Fetches the member record
@ with _Func_77394, recomputes its derived stats with _Func_7961c, and when r1
@ is non-zero applies the equipment change through _Func_21390.
.thumb_func_start Func_917d0
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, r1
	bl	_Func_77394
	mov	r0, r5
	bl	_Func_7961c
	cmp	r6, #0
	beq	.L917ec
	mov	r0, r5
	mov	r1, r6
	bl	_Func_21390
.L917ec:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_917d0

@ SwapPartyMembers
@ r0, r1 = party member ids. Recomputes both members' derived stats with
@ _Func_7961c before handing the pair to _Func_21488, so the swap operates on
@ up-to-date values.
.thumb_func_start Func_917f4
	push	{r5, r6, lr}
	mov	r6, r1
	mov	r5, r0
	bl	_Func_7961c
	mov	r0, r6
	bl	_Func_7961c
	mov	r0, r5
	mov	r1, r6
	bl	_Func_21488
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_917f4

@ CheckFieldAbilityAvailable
@ r0=packed request: party member index in bits 10-13, ability id in bits 0-9.
@ Returns 0 if the ability can be used now, or a negative reason code:
@    -1  the member index is above 7
@    -2  that member is not in the party (_Func_79338 on the member flag)
@    -3  the member cannot currently use the ability (_Func_78bc0)
.thumb_func_start Func_91814
	push	{r5, r6, lr}
	lsr	r5, r0, #10
	mov	r3, #0xf
	ldr	r6, =0x3ff
	and	r5, r3
	and	r6, r0
	cmp	r5, #7
	ble	.L9182a
	mov	r0, #1
	neg	r0, r0
	b	.L9184e
.L9182a:
	mov	r0, r5
	bl	_Func_79338
	cmp	r0, #0
	bne	.L9183a
	mov	r0, #2
	neg	r0, r0
	b	.L9184e
.L9183a:
	mov	r0, r5
	mov	r1, r6
	bl	_Func_78bc0
	cmp	r0, #0
	bne	.L9184c
	mov	r0, #3
	neg	r0, r0
	b	.L9184e
.L9184c:
	mov	r0, #0
.L9184e:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_91814

@ ValidateFieldAbilityShortcuts
@ Takes no arguments. Re-checks the two field-ability shortcut slots at
@ ewram_240+0x220 and +0x222 with Func_91814 and clears either one that no
@ longer passes -- so a shortcut survives only while its owner is in the party
@ and still able to use it.
.thumb_func_start Func_91858
	push	{r5, r6, lr}
	ldr	r6, =ewram_240
	mov	r3, #0x88
	lsl	r3, #2
	add	r5, r6, r3
	ldrh	r0, [r5]
	bl	Func_91814
	cmp	r0, #0
	beq	.L91870
	mov	r3, #0
	strh	r3, [r5]
.L91870:
	ldr	r3, =0x222
	add	r5, r6, r3
	ldrh	r0, [r5]
	bl	Func_91814
	cmp	r0, #0
	beq	.L91882
	mov	r3, #0
	strh	r3, [r5]
.L91882:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_91858

@ RefreshPartyVitals
@ r0=party member id. Recomputes the HP and PP bar fractions for the member
@ record from _Func_77394. Current HP/PP at +0x34/+0x36 are copied to +0x38/+0x3A
@ and the fractions at +0x14/+0x16 become (current << 14) / max, clamped to
@ 0..0x4000; a non-zero value that rounds to 0 is floored at 1 so a barely-alive
@ member never shows an empty bar.
@ It then counts party members with non-zero HP. If none are left, the player
@ (ewram_240+0x1F4) is given 1 HP and their fractions recomputed, so the party
@ can never end up entirely dead on the field.
.thumb_func_start Func_91890
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r5, r0
	bl	_Func_79664
	bl	Func_91858
	mov	r0, r5
	bl	_Func_77394
	mov	r6, r0
	ldrh	r1, [r6, #0x34]
	ldrh	r3, [r6, #0x36]
	strh	r1, [r6, #0x38]
	strh	r3, [r6, #0x3a]
	lsl	r1, #16
	asr	r1, #16
	lsl	r0, r1, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L918ca
	mov	r3, #0
	cmp	r0, #0
	blt	.L918ca
	mov	r3, r0
.L918ca:
	strh	r3, [r6, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L918de
	mov	r2, #0x38
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	beq	.L918de
	mov	r3, #1
	strh	r3, [r6, #0x14]
.L918de:
	mov	r3, #0x3a
	ldrsh	r0, [r6, r3]
	mov	r2, #0x36
	ldrsh	r1, [r6, r2]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L918fc
	mov	r3, #0
	cmp	r0, #0
	blt	.L918fc
	mov	r3, r0
.L918fc:
	strh	r3, [r6, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L91910
	mov	r2, #0x3a
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	beq	.L91910
	mov	r3, #1
	strh	r3, [r6, #0x16]
.L91910:
	ldr	r3, =0x131
	add	r2, r6, r3
	mov	r3, #0
	strb	r3, [r2]
	mov	r8, r3
	bl	_Func_795fc
	cmp	r8, r0
	bge	.L91948
	ldr	r3, =ewram_240
	mov	r2, #0xfc
	lsl	r2, #1
	add	r7, r3, r2
	mov	r5, r0
.L9192c:
	ldrb	r0, [r7]
	bl	_Func_77394
	mov	r6, r0
	mov	r2, #0x38
	ldrsh	r3, [r6, r2]
	add	r7, #1
	cmp	r3, #0
	beq	.L91942
	mov	r3, #1
	add	r8, r3
.L91942:
	sub	r5, #1
	cmp	r5, #0
	bne	.L9192c
.L91948:
	mov	r2, r8
	cmp	r2, #0
	bne	.L919c4
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	_Func_77394
	mov	r5, #1
	mov	r6, r0
	strh	r5, [r6, #0x38]
	lsl	r5, #14
	mov	r3, #0x34
	ldrsh	r1, [r6, r3]
	mov	r0, r5
	bl	Func_af0_from_thumb
	mov	r2, #0x80
	lsl	r2, #7
	cmp	r0, r2
	bgt	.L9197e
	mov	r5, #0
	cmp	r0, #0
	blt	.L9197e
	mov	r5, r0
.L9197e:
	lsl	r3, r5, #16
	strh	r5, [r6, #0x14]
	cmp	r3, #0
	bne	.L91992
	mov	r2, #0x38
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	beq	.L91992
	mov	r3, #1
	strh	r3, [r6, #0x14]
.L91992:
	mov	r3, #0x3a
	ldrsh	r0, [r6, r3]
	mov	r2, #0x36
	ldrsh	r1, [r6, r2]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L919b0
	mov	r3, #0
	cmp	r0, #0
	blt	.L919b0
	mov	r3, r0
.L919b0:
	strh	r3, [r6, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L919c4
	mov	r2, #0x3a
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	beq	.L919c4
	mov	r3, #1
	strh	r3, [r6, #0x16]
.L919c4:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_91890

@ CheckPartyFatigueThreshold
@ r0=context id. Sums _Func_78af8 across every party member (ids at
@ ewram_240+0x1F8). If the total reaches count * 30, it raises the condition:
@ _Func_19908(r0, 2) followed by message 0x97D, then the same pair again with
@ message 0x97E, and returns -1. Returns 0 when the party is below threshold.
.thumb_func_start Func_919d8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r2, #0
	mov	r8, r0
	mov	r10, r2
	bl	_Func_795fc
	mov	r6, r0
	cmp	r10, r6
	bge	.L91a0c
	ldr	r3, =ewram_240
	mov	r2, #0xfc
	lsl	r2, #1
	add	r7, r3, r2
	mov	r5, r6
.L919fa:
	ldrb	r0, [r7]
	mov	r1, r8
	bl	_Func_78af8
	sub	r5, #1
	add	r7, #1
	add	r10, r0
	cmp	r5, #0
	bne	.L919fa
.L91a0c:
	lsl	r3, r6, #4
	sub	r3, r6
	lsl	r3, #1
	cmp	r10, r3
	blt	.L91a40
	mov	r0, r8
	mov	r1, #2
	bl	_Func_19908
	ldr	r5, =0x97d
	mov	r1, #1
	mov	r0, r5
	bl	_Func_1776c
	add	r5, #1
	mov	r0, r8
	mov	r1, #2
	bl	_Func_19908
	mov	r0, r5
	mov	r1, #1
	bl	_Func_1776c
	mov	r0, #1
	neg	r0, r0
	b	.L91a42
.L91a40:
	mov	r0, #0
.L91a42:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_919d8

@ ApplyFieldItemOrAbility
@ r0=target/context id. Returns the affected party member id, or -1.
@ Resolves the effect through _Func_78618 and reports it with the standard
@ message pipeline: _Func_19908 pushes a substitution slot, _Func_1776c shows
@ the line. Which line depends on the outcome -- bit 3 of record byte +0x03
@ selects the "no effect" branch, a successful use plays sound 0x53 and shows
@ 0x96A when the affected member is the player (ewram_240+0x1F4) or 0x96B plus
@ a second substitution when it is somebody else. Failure paths route through
@ Func_91d84 for a confirmation prompt and _Func_19a54 to close it.
@ Status stacks are consumed by looping _Func_78948 as many times as
@ _Func_784b0 reports. The resulting message id is cached at iwram_1ebc+0x1D8.
@ The ~230-instruction body is characterised structurally; the message ids,
@ sound and outcome branches are verified.
.thumb_func_start Func_91a58
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r7, #0xec
	mov	r11, r3
	lsl	r7, #1
	add	r7, r11
	mov	r3, #0
	ldrsh	r2, [r7, r3]
	sub	sp, #0xc
	str	r2, [sp]
	mov	r6, r0
	bl	_Func_78618
	mov	r2, #1
	mov	r8, r0
	neg	r2, r2
	cmp	r8, r2
	beq	.L91a8c
	b	.L91baa
.L91a8c:
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	ldr	r0, =0x96a
	mov	r1, #1
	bl	_Func_1776c
	ldr	r0, =0x977
	mov	r1, #1
	bl	_Func_1776c
	mov	r3, #8
	mov	r2, #4
	add	r3, sp
	add	r2, sp
	mov	r9, r3
	mov	r10, r2
.L91ab0:
	ldr	r7, =0x978
	mov	r1, #1
	mov	r0, r7
	bl	_Func_1776c
	mov	r0, r9
	mov	r1, r10
	bl	_Func_b3444
	mov	r3, #1
	mov	r5, r0
	neg	r3, r3
	cmp	r5, r3
	bne	.L91b34
	mov	r0, r6
	bl	_Func_78414
	ldrb	r2, [r0, #3]
	mov	r3, #8
	and	r3, r2
	cmp	r3, #0
	beq	.L91aee
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	add	r0, r7, #4
	mov	r1, #1
	bl	_Func_1776c
	b	.L91ab0
.L91aee:
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	add	r0, r7, #1
	mov	r1, #5
	bl	_Func_1776c
	mov	r0, #1
	bl	Func_91d84
	mov	r5, r0
	bl	_Func_19a54
	cmp	r5, #0
	bne	.L91ab0
	mov	r1, #1
	mov	r0, r6
	bl	_Func_78ad0
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	add	r0, r7, #2
	mov	r1, #1
	bl	_Func_1776c
	mov	r3, #0xec
	mov	r2, sp
	lsl	r3, #1
	ldrh	r2, [r2]
	add	r3, r11
	strh	r2, [r3]
	b	.L91bee
.L91b34:
	ldr	r0, [sp, #8]
	bl	_Func_77394
	ldr	r1, [sp, #4]
	ldr	r0, [sp, #8]
	bl	_Func_784b0
	cmp	r0, #0
	ble	.L91b56
	mov	r5, r0
.L91b48:
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #4]
	sub	r5, #1
	bl	_Func_78948
	cmp	r5, #0
	bne	.L91b48
.L91b56:
	mov	r0, r6
	bl	_Func_78618
	mov	r8, r0
	mov	r0, #0x53
	bl	_Func_f9080
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r3, [r3]
	cmp	r8, r3
	bne	.L91b84
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	ldr	r0, =0x96a
	mov	r1, #3
	bl	_Func_1776c
	b	.L91b9c
.L91b84:
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	mov	r0, r8
	mov	r1, #1
	bl	_Func_19908
	ldr	r0, =0x96b
	mov	r1, #3
	bl	_Func_1776c
.L91b9c:
	mov	r3, #0xec
	mov	r2, sp
	lsl	r3, #1
	ldrh	r2, [r2]
	add	r3, r11
	strh	r2, [r3]
	b	.L91bee
.L91baa:
	mov	r0, #0x53
	bl	_Func_f9080
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	ldr	r5, =0x96a
	mov	r1, #3
	mov	r0, r5
	bl	_Func_1776c
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r3, [r3]
	cmp	r8, r3
	beq	.L91be8
	mov	r0, r6
	mov	r1, #2
	bl	_Func_19908
	mov	r0, r8
	mov	r1, #1
	bl	_Func_19908
	add	r0, r5, #1
	mov	r1, #3
	bl	_Func_1776c
.L91be8:
	mov	r3, sp
	ldrh	r3, [r3]
	strh	r3, [r7]
.L91bee:
	mov	r0, r8
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_91a58

@ TryGiveItem
@ r0=item id, r1 unused, r2=party member id. Offers the item to the member with
@ _Func_78588 and returns the member id on success or -1 when the inventory
@ rejects it (a negative result, i.e. no free slot).
.thumb_func_start Func_91c1c
	push	{r5, lr}
	mov	r5, r2
	mov	r1, r0
	mov	r0, r5
	bl	_Func_78588
	cmp	r0, #0
	blt	.L91c30
	mov	r0, r5
	b	.L91c34
.L91c30:
	mov	r0, #1
	neg	r0, r0
.L91c34:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_91c1c

@ Nop3 -- empty hook.
.thumb_func_start Func_91c3c
	bx	lr
.func_end Func_91c3c

@ Nop4 -- empty hook, distinct address from Func_91c3c.
.thumb_func_start Func_91c40
	bx	lr
.func_end Func_91c40

@ WaitForAnimationEnd
@ r0=slot, r1=animation index. Blocks while the slot's actor is still playing
@ animation r1, checking the actor's current-animation byte at +0x24 once per
@ frame. Gives up after 0x5A (90) frames, and returns immediately if the slot is
@ empty or not draw kind 1.
.thumb_func_start Func_91c44
	push	{r5, r6, r7, lr}
	mov	r7, r1
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L91c76
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L91c76
	ldr	r3, [r0, #0x50]
	mov	r6, r3
	mov	r5, #0
	add	r6, #0x24
	b	.L91c66
.L91c64:
	add	r5, #1
.L91c66:
	cmp	r5, #0x59
	bgt	.L91c76
	mov	r0, #1
	bl	Func_30f8
	ldrb	r3, [r6]
	cmp	r7, r3
	beq	.L91c64
.L91c76:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_91c44

@ RunFieldAbilityPrompt
@ r0=slot, r1=force-simple flag. Returns the prompt result.
@ First drains any held buttons (spins on iwram_1c94 until released) and waits
@ for _Func_17364 to report the message window ready, then pauses 3 frames.
@ When r1 is 0 it compares the combined stat at +0x0E plus +0x0A for the two
@ party records at +0x1F8 and +0x1FC and picks the lower; a value above 15
@ downgrades the prompt style passed to _Func_28df4.
@ The prompt itself is _Func_28df4 with the two counters at iwram_1ebc+0xCC2
@ and +0xCC4. A non-zero result selects animation 4 on the slot, a zero result
@ animation 3, each via Func_924d4 then _Func_19e48 / _Func_19a54 and a
@ Func_91c44 wait for that animation to finish.
.thumb_func_start Func_91c7c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	mov	r8, r0
	ldr	r6, [r3]
	mov	r0, #0xfa
	lsl	r0, #1
	add	r3, r6, r0
	ldr	r0, [r3]
	sub	sp, #4
	mov	r9, r1
	bl	Func_8d394
	mov	r2, #0
	ldrsh	r1, [r0, r2]
	mov	r0, #0xfc
	lsl	r0, #1
	add	r3, r6, r0
	ldr	r3, [r3]
	mov	r11, r1
	mov	r1, #0xfe
	lsl	r1, #1
	mov	r2, #1
	mov	r10, r3
	add	r3, r6, r1
	ldr	r5, [r3]
	str	r2, [sp]
	ldr	r2, =iwram_1c94
	ldr	r3, [r2]
	cmp	r3, #0
	beq	.L91cda
	mov	r7, r2
.L91cc6:
	mov	r0, #1
	bl	Func_30f8
	ldr	r3, [r7]
	cmp	r3, #0
	bne	.L91cc6
	b	.L91cda
.L91cd4:
	mov	r0, #1
	bl	Func_30f8
.L91cda:
	bl	_Func_17364
	cmp	r0, #0
	beq	.L91cd4
	mov	r0, #3
	bl	Func_30f8
	mov	r3, r9
	cmp	r3, #0
	bne	.L91d0e
	mov	r0, r10
	ldrh	r2, [r0, #0xe]
	ldrh	r3, [r0, #0xa]
	add	r1, r2, r3
	cmp	r5, #0
	beq	.L91d06
	ldrh	r2, [r5, #0xe]
	ldrh	r3, [r5, #0xa]
	add	r2, r3
	cmp	r1, r2
	bge	.L91d06
	mov	r1, r2
.L91d06:
	cmp	r1, #0xf
	ble	.L91d0e
	mov	r1, #0
	str	r1, [sp]
.L91d0e:
	ldr	r2, =0xcc2
	add	r3, r6, r2
	add	r2, #2
	mov	r0, #0
	ldrsh	r1, [r3, r0]
	add	r3, r6, r2
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r0, [sp]
	mov	r3, #0
	bl	_Func_28df4
	mov	r5, r0
	cmp	r5, #0
	beq	.L91d48
	mov	r1, #4
	mov	r0, r8
	bl	Func_924d4
	mov	r0, r11
	bl	_Func_19e48
	bl	_Func_19a54
	mov	r0, r8
	mov	r1, #4
	bl	Func_91c44
	b	.L91d62
.L91d48:
	mov	r1, #3
	mov	r0, r8
	bl	Func_924d4
	mov	r0, r11
	bl	_Func_19e48
	bl	_Func_19a54
	mov	r0, r8
	mov	r1, #3
	bl	Func_91c44
.L91d62:
	mov	r0, r5
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_91c7c

@ ShowConfirmPrompt
@ r0=prompt id. Thin wrapper on _Func_28df4 with all three option arguments
@ zeroed -- the plain yes/no case. Returns the choice.
.thumb_func_start Func_91d84
	push	{lr}
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bl	_Func_28df4
	pop	{r1}
	bx	r1
.func_end Func_91d84

@ ShowItemReceivedMessage
@ r0=party member id, r1=item id. Registers the acquisition with _Func_78e28,
@ plays sound 0x53, then shows message 0x1E with the member in substitution
@ slot 1 and the item in slot 4.
.thumb_func_start Func_91d94
	push	{r5, r6, lr}
	mov	r6, r1
	mov	r5, r0
	bl	_Func_78e28
	mov	r0, #0x53
	bl	_Func_f9080
	mov	r0, r5
	mov	r1, #1
	bl	_Func_19908
	mov	r0, r6
	mov	r1, #4
	bl	_Func_19908
	ldr	r0, =0x1e
	mov	r1, #3
	bl	_Func_1776c
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_91d94

@ ShowScreenOverlay
@ Takes no arguments. Calls Func_8fefc with the overlay parameters cached at
@ iwram_1ebc+0x1C0 and +0x1C8, then sets the overlay-active flag at +0x1C6 to 1.
@ Func_91df4 is the matching hide.
.thumb_func_start Func_91dc8
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r5, [r3]
	lsl	r2, #1
	add	r3, r5, r2
	add	r2, #8
	ldr	r0, [r3]
	add	r3, r5, r2
	ldr	r1, [r3]
	bl	Func_8fefc
	mov	r3, #0xe3
	lsl	r3, #1
	add	r2, r5, r3
	mov	r3, #1
	strh	r3, [r2]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_91dc8

@ HideScreenOverlay
@ Takes no arguments. Calls Func_901c0 with the same parameters Func_91dc8 used
@ and clears the overlay-active flag at iwram_1ebc+0x1C6.
.thumb_func_start Func_91df4
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r5, [r3]
	lsl	r2, #1
	add	r3, r5, r2
	add	r2, #8
	ldr	r0, [r3]
	add	r3, r5, r2
	ldr	r1, [r3]
	bl	Func_901c0
	mov	r3, #0xe3
	lsl	r3, #1
	add	r2, r5, r3
	mov	r3, #0
	strh	r3, [r2]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_91df4

@ WaitSceneDelay
@ Takes no arguments. Blocks for the frame count stored at iwram_1ebc+0x1C8 --
@ the scene's configured inter-step delay. Unlike Func_9163c this ignores the
@ fast-forward flag, so it cannot be skipped.
.thumb_func_start Func_91e20
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe4
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_30f8
	pop	{r0}
	bx	r0
.func_end Func_91e20

@ SetSceneTargetA
@ r0, r1 = target coordinates. Stores them at ewram_240+0x1C0 and +0x1C2 and
@ resets the pending message id at iwram_1ebc+0x170 to the 0x3E7 "none" value.
.thumb_func_start Func_91e3c
	ldr	r3, =iwram_1ebc
	mov	r2, #0xb8
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldr	r2, =0x3e7
	mov	r4, #0xe0
	strh	r2, [r3]
	ldr	r2, =ewram_240
	lsl	r4, #1
	add	r3, r2, r4
	strh	r0, [r3]
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r2, r0
	strh	r1, [r3]
	bx	lr
.func_end Func_91e3c

@ SetSceneTargetB
@ r0, r1 = target coordinates. As Func_91e3c but writes the second target pair
@ at ewram_240+0x1C4 and +0x1C6.
.thumb_func_start Func_91e6c
	ldr	r3, =iwram_1ebc
	mov	r2, #0xb8
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldr	r2, =0x3e7
	mov	r4, #0xe2
	strh	r2, [r3]
	ldr	r2, =ewram_240
	lsl	r4, #1
	add	r3, r2, r4
	strh	r0, [r3]
	mov	r0, #0xe3
	lsl	r0, #1
	add	r3, r2, r0
	strh	r1, [r3]
	bx	lr
.func_end Func_91e6c

@ SetPendingMessageId
@ r0=message id. Stores it at iwram_1ebc+0x170, where the interaction handlers
@ in rom_8d5dc.s leave the line a trigger wants shown.
.thumb_func_start Func_91e9c
	ldr	r3, =iwram_1ebc
	mov	r2, #0xb8
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	strh	r0, [r3]
	bx	lr
.func_end Func_91e9c

@ LoadMapByIdAndEntrance
@ r0=map id, r1=entrance index. Resolves the destination with Func_8b05c,
@ caching the result at iwram_1ebc+0x17C, then starts the transition with
@ Func_8b320.
@ Two special cases: map 0x62 entered at entrance 0 forces ewram_240+0x1B4 to
@ 0x21, and in scene mode 3 (+0x19E) the player's position is handed to
@ Func_8adf0 first so the arrival point is recorded.
.thumb_func_start Func_91eb0
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1ebc
	mov	r6, r1
	ldr	r7, [r3]
	mov	r5, r0
	bl	Func_8b05c
	mov	r1, #0xbe
	lsl	r1, #1
	add	r3, r7, r1
	strh	r0, [r3]
	cmp	r5, #0x62
	bne	.L91ed8
	cmp	r6, #0
	bne	.L91ed8
	ldr	r3, =ewram_240
	ldr	r2, =0x21
	add	r1, #0x5a
	add	r3, r1
	strh	r2, [r3]
.L91ed8:
	mov	r2, #0xcf
	lsl	r2, #1
	add	r3, r7, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #3
	bne	.L91ef8
	ldr	r3, =ewram_240
	add	r2, #0x56
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_8ba1c
	add	r0, #8
	bl	Func_8adf0
.L91ef8:
	mov	r0, r5
	mov	r1, r6
	bl	Func_8b320
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_91eb0

@ LoadMapByName
@ r0=packed destination -- bit 11 is a mode flag, the low byte is the id --
@ r1=name/label index. When bit 11 is clear it first runs Func_9537c.
@ Stores (r1 + 0x12C) with the mode flag ORed in at ewram_240+0x234, resolves
@ the destination with Func_8b074 into iwram_1ebc+0x17C, applies the same scene
@ mode 3 fixup as Func_91eb0, and starts the transition with Func_8b320(0, 0).
.thumb_func_start Func_91f14
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	mov	r5, #0x80
	ldr	r3, [r3]
	lsl	r5, #4
	mov	r8, r3
	and	r5, r0
	mov	r3, #0xff
	mov	r7, r1
	and	r0, r3
	cmp	r5, #0
	bne	.L91f34
	bl	Func_9537c
.L91f34:
	mov	r1, #0x96
	lsl	r1, #1
	ldr	r6, =ewram_240
	add	r3, r7, r1
	mov	r1, #0x8d
	lsl	r1, #2
	orr	r3, r5
	add	r2, r6, r1
	strh	r3, [r2]
	mov	r0, r7
	bl	Func_8b074
	mov	r3, #0xbe
	lsl	r3, #1
	add	r3, r8
	strh	r0, [r3]
	mov	r3, #0xcf
	lsl	r3, #1
	add	r3, r8
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L91f74
	mov	r1, #0xfa
	lsl	r1, #1
	add	r3, r6, r1
	ldr	r0, [r3]
	bl	Func_8ba1c
	add	r0, #8
	bl	Func_8adf0
.L91f74:
	mov	r1, #0
	mov	r0, #0
	bl	Func_8b320
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_91f14

@ SetSpawnPositionA
@ r0, r1 = coordinates written to ewram_240+0x1CE and +0x1D0 -- where the player
@ appears after the next map load.
.thumb_func_start Func_91f90
	ldr	r3, =ewram_240
	mov	r4, #0xe7
	lsl	r4, #1
	add	r2, r3, r4
	strh	r0, [r2]
	mov	r2, #0xe8
	lsl	r2, #1
	add	r3, r2
	strh	r1, [r3]
	bx	lr
.func_end Func_91f90

@ SetSpawnPositionB
@ r0, r1 = coordinates written to ewram_240+0x1D2 and +0x1D4, the secondary
@ spawn point.
.thumb_func_start Func_91fa8
	ldr	r3, =ewram_240
	mov	r4, #0xe9
	lsl	r4, #1
	add	r2, r3, r4
	strh	r0, [r2]
	mov	r2, #0xea
	lsl	r2, #1
	add	r3, r2
	strh	r1, [r3]
	bx	lr
.func_end Func_91fa8

@ SetSceneTargetAClearMessage
@ r0, r1 = target coordinates. Same as Func_91e3c -- writes ewram_240+0x1C0 /
@ +0x1C2 and resets iwram_1ebc+0x170 to 0x3E7 -- but reaches the scene block
@ through its own load rather than the caller's.
.thumb_func_start Func_91fc0
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	ldr	r3, =ewram_240
	mov	r12, r3
	mov	r3, #0xe0
	lsl	r3, #1
	add	r3, r12
	strh	r0, [r3]
	mov	r3, #0xe1
	lsl	r3, #1
	add	r3, r12
	strh	r1, [r3]
	mov	r3, #0xb8
	lsl	r3, #1
	add	r2, r3
	ldr	r3, =0x3e7
	strh	r3, [r2]
	bx	lr
.func_end Func_91fc0

@ StartLoopingSound
@ r0=sound id, or -1 for the 0x121 default. Records the id at iwram_1ebc+0xCC8
@ so Func_9202c can stop it later, plays 0x12A as the attack, then the sound
@ itself.
.thumb_func_start Func_91ff0
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	ldr	r2, =0xcc8
	ldr	r3, [r3]
	mov	r5, r0
	add	r3, r2
	strh	r5, [r3]
	mov	r2, #1
	lsl	r3, r5, #16
	asr	r3, #16
	neg	r2, r2
	cmp	r3, r2
	bne	.L9200c
	ldr	r5, =0x121
.L9200c:
	mov	r0, #0x95
	lsl	r0, #1
	bl	_Func_f9080
	mov	r0, r5
	bl	_Func_f9080
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_91ff0

@ StopLoopingSound
@ Takes no arguments. Replays the sound id cached at iwram_1ebc+0xCC8 -- the
@ engine's convention for ending a looping cue -- unless the slot holds -1.
.thumb_func_start Func_9202c
	push	{lr}
	ldr	r3, =iwram_1ebc
	ldr	r2, =0xcc8
	ldr	r3, [r3]
	add	r3, r2
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	mov	r3, #1
	neg	r3, r3
	cmp	r0, r3
	beq	.L92046
	bl	_Func_f9080
.L92046:
	pop	{r0}
	bx	r0
.func_end Func_9202c

@ GetSlotEntityChecked
@ r0=slot. Func_8ba1c with an explicit null result; the extra compare is
@ redundant since Func_8ba1c already returns 0 out of range, but callers use
@ this name where a null is expected rather than exceptional.
.thumb_func_start Func_92054
	push	{lr}
	bl	Func_8ba1c
	cmp	r0, #0
	bne	.L92060
	mov	r0, #0
.L92060:
	pop	{r1}
	bx	r1
.func_end Func_92054

@ SetSlotEntitySpeed
@ r0=slot, r1=max speed, r2=acceleration. Writes +0x30 and +0x34 of the slot's
@ entity. No-op on an empty slot.
.thumb_func_start Func_92064
	push	{r5, r6, lr}
	mov	r6, r1
	mov	r5, r2
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L92076
	str	r5, [r0, #0x34]
	str	r6, [r0, #0x30]
.L92076:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_92064

@ SetSlotScriptWithTurn
@ r0=slot, r1=script. Sets bit 0 of the entity's +0x5A so the mover turns it to
@ face its direction of travel, then installs the script with Func_93a6c.
.thumb_func_start Func_9207c
	push	{r5, lr}
	mov	r5, r1
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L9209a
	mov	r1, r0
	add	r1, #0x5a
	ldrb	r2, [r1]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r1]
	mov	r1, r5
	bl	Func_93a6c
.L9209a:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_9207c

@ SetSlotWalkBehaviour
@ r0=slot. Sets bit 0 of +0x5A so the entity turns to face its heading, then
@ installs the default walk behaviour with _Func_c4ac.
.thumb_func_start Func_920a0
	push	{lr}
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L920ba
	mov	r1, r0
	add	r1, #0x5a
	ldrb	r2, [r1]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r1]
	bl	_Func_c4ac
.L920ba:
	pop	{r0}
	bx	r0
.func_end Func_920a0

@ SetSlotFollowTarget
@ r0=slot, r1=target slot. Stores the target entity in the script argument slot
@ at +0x68 and installs the follow script Data_9ff40 through Func_93a6c.
.thumb_func_start Func_920c0
	push	{r5, r6, lr}
	mov	r6, r1
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L920de
	mov	r0, r6
	bl	Func_92054
	ldr	r1, =Data_9ff40
	str	r0, [r5, #0x68]
	mov	r0, r5
	bl	Func_93a6c
.L920de:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_920c0

@ WaitForSlotScriptIdle
@ r0=slot. Blocks until the slot's script VM reaches opcode 0x10 via _Func_c4ec
@ (which gives up after 299 frames). No-op on an empty slot.
.thumb_func_start Func_920e8
	push	{lr}
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L920f6
	bl	_Func_c4ec
.L920f6:
	pop	{r0}
	bx	r0
.func_end Func_920e8

@ SetSlotScriptAndWait
@ r0=slot, r1=script. Func_9207c followed by _Func_c4ec: installs the script
@ with the turn-to-face bit set, then blocks until it settles on opcode 0x10.
.thumb_func_start Func_920fc
	push	{r5, r6, lr}
	mov	r6, r1
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L92122
	mov	r1, r5
	add	r1, #0x5a
	ldrb	r2, [r1]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r1]
	mov	r1, r6
	bl	Func_93a6c
	mov	r0, r5
	bl	_Func_c4ec
.L92122:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_920fc

@ MoveSlotTo
@ r0=slot, r1=x in whole units, r2=z in whole units. Clears the freeze flag at
@ +0x5B, installs the default behaviour with _Func_c4ac, and starts a move to
@ (r1 << 16, current y, r2 << 16) via _Func_d14c. Returns immediately -- the
@ walk happens over subsequent frames.
.thumb_func_start Func_92128
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L92150
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	bl	_Func_c4ac
	lsl	r1, r6, #16
	ldr	r2, [r5, #0xc]
	lsl	r3, r7, #16
	mov	r0, r5
	bl	_Func_d14c
.L92150:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92128

@ MoveSlotToAndWait
@ r0=slot, r1=x, r2=z. Func_92128 followed by _Func_ca6c, so the call blocks
@ until the entity arrives (or the 600-frame timeout fires).
.thumb_func_start Func_92158
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L92186
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	bl	_Func_c4ac
	mov	r0, r5
	lsl	r1, r6, #16
	ldr	r2, [r5, #0xc]
	lsl	r3, r7, #16
	bl	_Func_d14c
	mov	r0, r5
	bl	_Func_ca6c
.L92186:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92158

@ WalkSlotTo
@ r0=slot, r1=x, r2=z. Func_92128 with the walk animation (2) selected first,
@ so the entity is animated while it travels. Does not wait.
.thumb_func_start Func_9218c
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L921bc
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	bl	_Func_c4ac
	mov	r0, r5
	mov	r1, #2
	bl	_Func_c300
	lsl	r1, r6, #16
	ldr	r2, [r5, #0xc]
	lsl	r3, r7, #16
	mov	r0, r5
	bl	_Func_d14c
.L921bc:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_9218c

@ WalkSlotToAndWait
@ r0=slot, r1=x, r2=z. The complete scripted walk: select animation 2, move,
@ block on _Func_ca6c until arrival, then return to the idle animation 1.
.thumb_func_start Func_921c4
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L92202
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	bl	_Func_c4ac
	mov	r0, r5
	mov	r1, #2
	bl	_Func_c300
	lsl	r1, r6, #16
	ldr	r2, [r5, #0xc]
	lsl	r3, r7, #16
	mov	r0, r5
	bl	_Func_d14c
	mov	r0, r5
	bl	_Func_ca6c
	mov	r0, r5
	mov	r1, #1
	bl	_Func_c300
.L92202:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_921c4

@ WalkSlotThroughDoorway
@ r0=slot, r1=argument for Func_92b08, r2=z offset. A two-stage move used for
@ doorways: first the entity is snapped onto the nearest 16-unit grid line in x
@ -- the (x mod 16) computation at .L9222c -- and walks 8 units clear of it,
@ blocking until it arrives. Func_92b08 then runs the transition itself, and a
@ second _Func_d14c carries the entity r2 further along z on the other side.
.thumb_func_start Func_92208
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r7, r0
	mov	r8, r1
	mov	r10, r2
	bl	Func_8ba1c
	mov	r6, r0
	cmp	r6, #0
	beq	.L92280
	mov	r2, #0xa
	ldrsh	r3, [r6, r2]
	mov	r5, r3
	cmp	r3, #0
	bge	.L9222c
	add	r5, #0xf
.L9222c:
	asr	r5, #4
	lsl	r5, #4
	mov	r2, r6
	add	r2, #0x5b
	sub	r5, r3, r5
	mov	r3, #0
	strb	r3, [r2]
	mov	r0, r6
	bl	_Func_c4ac
	lsl	r5, #16
	mov	r0, r6
	mov	r1, #2
	bl	_Func_c300
	asr	r5, #16
	mov	r3, #8
	ldr	r1, [r6, #8]
	sub	r3, r5
	lsl	r3, #16
	add	r1, r3
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x10]
	mov	r0, r6
	bl	_Func_d14c
	mov	r0, r6
	bl	_Func_ca6c
	mov	r0, r7
	mov	r1, r8
	bl	Func_92b08
	mov	r3, r10
	lsl	r0, r3, #16
	ldr	r3, [r6, #0x10]
	ldr	r1, [r6, #8]
	add	r3, r0
	ldr	r2, [r6, #0xc]
	mov	r0, r6
	bl	_Func_d14c
.L92280:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92208

@ MoveSlotBy
@ r0=slot, r1=x delta, r2=z delta (whole units). Like Func_92128 but relative:
@ the deltas are added to the entity's current +0x08 and +0x10. Does not wait
@ and does not change the animation.
.thumb_func_start Func_9228c
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L922bc
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	bl	_Func_c4ac
	ldr	r1, [r5, #8]
	lsl	r3, r6, #16
	add	r1, r3
	ldr	r3, [r5, #0x10]
	lsl	r0, r7, #16
	add	r3, r0
	ldr	r2, [r5, #0xc]
	mov	r0, r5
	bl	_Func_d14c
.L922bc:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_9228c

@ WalkSlotBy
@ r0=slot, r1=x delta, r2=z delta. Func_9228c with the walk animation (2)
@ selected first. Does not wait.
.thumb_func_start Func_922c4
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L922fc
	mov	r2, r5
	add	r2, #0x5b
	mov	r3, #0
	strb	r3, [r2]
	bl	_Func_c4ac
	mov	r0, r5
	mov	r1, #2
	bl	_Func_c300
	ldr	r1, [r5, #8]
	lsl	r3, r6, #16
	add	r1, r3
	ldr	r3, [r5, #0x10]
	lsl	r0, r7, #16
	add	r3, r0
	ldr	r2, [r5, #0xc]
	mov	r0, r5
	bl	_Func_d14c
.L922fc:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_922c4

@ WalkSlotByAndWait
@ r0=slot, r1=x delta, r2=z delta. Func_922c4 followed by _Func_ca6c and a
@ return to the idle animation 1 -- the relative counterpart of Func_921c4.
.thumb_func_start Func_92304
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r5, r0
	mov	r6, r1
	mov	r8, r2
	bl	Func_8ba1c
	mov	r1, r6
	mov	r7, r0
	mov	r2, r8
	mov	r0, r5
	bl	Func_922c4
	cmp	r7, #0
	beq	.L92332
	mov	r0, r7
	bl	_Func_ca6c
	mov	r0, r7
	mov	r1, #1
	bl	_Func_c300
.L92332:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92304

@ SpawnFollowerAtPlayerAndWalk
@ r0=slot, r1=x delta, r2=z delta, r3=facing angle. Used to bring a follower
@ into a scene: sets the entity's speed to 0x9999 / 0x4CCC via Func_92064,
@ snaps it onto the player's position with Func_923e4, clears the freeze flag,
@ selects the walk animation, walks it out by (r1, r2) with Func_9228c, then
@ installs the script .L9fbcc and stores the facing angle at +0x64.
.thumb_func_start Func_9233c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r6, r0
	mov	r7, r1
	mov	r8, r2
	mov	r10, r3
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L923a6
	ldr	r2, =0x4ccc
	mov	r0, r6
	ldr	r1, =0x9999
	bl	Func_92064
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_92054
	cmp	r0, #0
	beq	.L9237c
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, r6
	bl	Func_923e4
.L9237c:
	mov	r2, r5
	mov	r3, #0
	add	r2, #0x5b
	strb	r3, [r2]
	mov	r0, r5
	mov	r1, #2
	bl	_Func_c300
	mov	r2, r8
	mov	r0, r6
	mov	r1, r7
	bl	Func_9228c
	ldr	r1, =.L9fbcc
	mov	r0, r5
	bl	_Func_c2d8
	mov	r3, r5
	add	r3, #0x64
	mov	r2, r10
	strh	r2, [r3]
.L923a6:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_9233c

@ WaitForSlotArrival
@ r0=slot. Blocks on _Func_ca6c until the entity's move completes, then selects
@ the idle animation 1. The tail half of Func_921c4, callable on its own after
@ a non-waiting move.
.thumb_func_start Func_923c4
	push	{r5, lr}
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L923dc
	bl	_Func_ca6c
	mov	r0, r5
	mov	r1, #1
	bl	_Func_c300
.L923dc:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_923c4

@ PlaceSlotAt
@ r0=slot, r1=x (16.16), r2=z (16.16). Teleports the entity: installs the
@ default behaviour, zeroes all three velocity words, clears the x and y targets
@ to the 0x80000000 sentinel and writes the position directly.
@ When terrain-follow is enabled (bit 0 of +0x55) it also resamples the ground
@ height at the destination with _Func_11f54, using the entity's tile-type byte
@ at +0x22, and shifts y by the difference from the cached height at +0x14 so
@ the entity lands on the surface rather than at the old altitude.
.thumb_func_start Func_923e4
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L9244a
	bl	_Func_c4ac
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x28]
	str	r3, [r5, #0x2c]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x38]
	str	r6, [r5, #8]
	str	r7, [r5, #0x10]
	mov	r3, r5
	add	r3, #0x55
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L9244a
	mov	r3, r5
	add	r3, #0x22
	mov	r1, r6
	ldrb	r0, [r3]
	cmp	r1, #0
	bge	.L9242a
	ldr	r3, =0xffff
	add	r1, r3
.L9242a:
	mov	r2, r7
	asr	r1, #16
	cmp	r2, #0
	bge	.L92436
	ldr	r3, =0xffff
	add	r2, r3
.L92436:
	asr	r2, #16
	bl	_Func_11f54
	ldr	r3, [r5, #0xc]
	ldr	r2, [r5, #0x14]
	lsl	r0, #16
	sub	r3, r2
	add	r3, r0
	str	r3, [r5, #0xc]
	str	r0, [r5, #0x14]
.L9244a:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_923e4

@ PlaceSlotAt3D
@ r0=slot, r1=x, r2=y, r3=z (all 16.16). The three-axis form of Func_923e4:
@ installs the default behaviour, zeroes the velocity, clears the x and y
@ targets and writes all three position words. When terrain-follow is enabled
@ (bit 0 of +0x55) the y written by the caller is then corrected against the
@ ground height sampled by _Func_11f54 at the destination.
.thumb_func_start Func_92454
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r1
	mov	r6, r2
	mov	r8, r3
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L924c4
	bl	_Func_c4ac
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x28]
	str	r3, [r5, #0x2c]
	mov	r3, #0x80
	lsl	r3, #24
	mov	r2, r8
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x38]
	str	r7, [r5, #8]
	str	r6, [r5, #0xc]
	str	r2, [r5, #0x10]
	mov	r3, r5
	add	r3, #0x55
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L924c4
	mov	r3, r5
	add	r3, #0x22
	mov	r1, r7
	ldrb	r0, [r3]
	cmp	r1, #0
	bge	.L924a4
	ldr	r3, =0xffff
	add	r1, r3
.L924a4:
	mov	r3, r8
	asr	r1, #16
	cmp	r3, #0
	bge	.L924b0
	ldr	r2, =0xffff
	add	r3, r2
.L924b0:
	asr	r2, r3, #16
	bl	_Func_11f54
	ldr	r3, [r5, #0xc]
	ldr	r2, [r5, #0x14]
	lsl	r0, #16
	sub	r3, r2
	add	r3, r0
	str	r3, [r5, #0xc]
	str	r0, [r5, #0x14]
.L924c4:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92454

@ SetSlotAnimation
@ r0=slot, r1=animation index. Forwards to _Func_c300 for the slot's entity.
@ No-op on an empty slot.
.thumb_func_start Func_924d4
	push	{r5, lr}
	mov	r5, r1
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L924e6
	mov	r1, r5
	bl	_Func_c300
.L924e6:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_924d4

@ SetSlotAnimSpeed
@ r0=slot, r1=speed. Forwards to _Func_c344 for the slot's entity; 0x10 is
@ normal speed. No-op on an empty slot.
.thumb_func_start Func_924ec
	push	{r5, lr}
	mov	r5, r1
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L924fe
	mov	r1, r5
	bl	_Func_c344
.L924fe:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_924ec

@ WaitForSlotAnimationChange
@ r0=slot. Samples the actor's current-animation byte at +0x24 and blocks until
@ it changes, checking once per frame and giving up after 0x5A (90) frames.
@ Where Func_91c44 waits for a specific animation to end, this waits for
@ whatever is playing now to be replaced.
.thumb_func_start Func_92504
	push	{r5, r6, r7, lr}
	sub	sp, #4
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L9253e
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L9253e
	ldr	r3, [r0, #0x50]
	mov	r6, r3
	add	r6, #0x24
	ldrb	r3, [r6]
	mov	r7, sp
	mov	r5, #0
	str	r3, [r7]
	b	.L9252c
.L9252a:
	add	r5, #1
.L9252c:
	cmp	r5, #0x59
	bgt	.L9253e
	mov	r0, #1
	bl	Func_30f8
	ldrb	r2, [r6]
	ldr	r3, [r7]
	cmp	r3, r2
	beq	.L9252a
.L9253e:
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92504

@ SetSlotAnimationAndWait
@ r0=slot, r1=animation index. Func_924d4 followed by Func_92504, so the call
@ returns once the requested animation has run and been replaced.
.thumb_func_start Func_92548
	push	{r5, lr}
	mov	r5, r0
	bl	Func_924d4
	mov	r0, r5
	bl	Func_92504
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_92548

@ Nop5 -- empty hook.
.thumb_func_start Func_9255c
	bx	lr
.func_end Func_9255c

@ SetSlotFacingAndScript
@ r0=slot, r1, r2 = facing/script parameters applied to the slot's entity.
@ Tail of the slot-helper family; the body follows the same
@ resolve-then-forward shape as its neighbours.
.thumb_func_start Func_92560
	push	{r5, r6, lr}
	mov	r5, r1
	mov	r6, r2
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L92596
	mov	r1, r0
	add	r1, #0x55
	ldrb	r3, [r1]
	mov	r2, #2
	orr	r2, r3
	lsl	r3, r5, #16
	strb	r2, [r1]
	str	r3, [r0, #0x28]
	cmp	r5, #5
	ble	.L9258a
	mov	r0, #0x99
	bl	_Func_f9080
	b	.L92590
.L9258a:
	mov	r0, #0x98
	bl	_Func_f9080
.L92590:
	mov	r0, r6
	bl	Func_9163c
.L92596:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_92560

	.section .rodata

.L9fbcc:
	.incrom 0x9fbcc, 0x9fbec
