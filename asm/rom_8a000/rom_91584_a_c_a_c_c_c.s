	.include "macros.inc"

@ BeginCutscene
@ Takes no arguments. Opens a scripted scene.
@ Calls _Func_1c428 to take over input, resets the player with Func_91660, and
@ tears down any live message box (Func_8e118) if one is flagged at +0xCB6.
@ Then initialises the dialogue state: clears +0xCC2 and +0xCC4, sets the
@ message delay at +0x1C8 to 0x10, clears the fast-forward flag at +0x1CC,
@ and sets the three message id slots at +0x1DA/+0x1DC/+0x1DE to their "none"
@ values (0xFFFF, -1, -1).
@ Registers Task_Cutscene at priority 0xC80, clears event flag 0x132, and captures
@ the current player id from ewram_240+0x1F4 into +0x1F4 while clearing +0x1F8.
@ Func_91750 is the matching close.
.thumb_func_start CutsceneStart  @ 0x080916b0
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r6, [r3]
	bl	_Func_801c428
	bl	Func_8091660
	ldr	r2, =0xcb6
	add	r3, r6, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	beq	.L916ce
	bl	Func_808e118
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
	ldr	r0, =Task_Cutscene
	bl	StartTask
	mov	r0, #0x99
	lsl	r0, #1
	bl	_ClearFlag
	ldr	r3, =gState
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
.func_end CutsceneStart
