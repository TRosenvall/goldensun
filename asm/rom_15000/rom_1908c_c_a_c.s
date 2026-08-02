	.include "macros.inc"
	.include "gba.inc"

@ RunMenuModal
@ r0.. = menu parameters. Drives a menu to completion, one WaitFrames(1) per
@ frame, and closes its window with CloseUIBox on the way out.
.thumb_func_start Func_80197c4  @ 0x080197c4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	mov	r5, #0xc4
	mov	r7, #0xa0
	mov	r8, r3
	lsl	r5, #3
	lsl	r7, #3
	add	r5, r8
	add	r7, r8
	mov	r6, #0
.L197de:
	ldr	r0, [r5]
	cmp	r0, #0
	beq	.L197f0
	ldrh	r3, [r0, #0x16]
	cmp	r3, #0
	beq	.L197f0
	mov	r1, #0
	bl	CloseUIBox
.L197f0:
	add	r6, #1
	add	r5, #0x28
	cmp	r6, #3
	bne	.L197de
.L197f8:
	mov	r5, #0xc4
	lsl	r5, #3
	mov	r1, #1
	add	r5, r8
	mov	r6, #0
.L19802:
	ldr	r2, [r5]
	cmp	r2, #0
	beq	.L1981a
	ldr	r3, [r2, #0x18]
	cmp	r3, #0
	bne	.L19818
	ldrh	r3, [r2, #0x16]
	cmp	r3, #0
	bne	.L19818
	str	r3, [r5]
	b	.L1981a
.L19818:
	mov	r1, #0
.L1981a:
	add	r6, #1
	add	r5, #0x28
	cmp	r6, #3
	bne	.L19802
	mov	r6, #0
	cmp	r1, #0
	bne	.L19842
	mov	r0, #1
	bl	WaitFrames
	b	.L197f8
.L19830:
	ldrh	r3, [r7, #0x16]
	cmp	r3, #0
	beq	.L1983e
	mov	r0, r7
	mov	r1, #0
	bl	CloseUIBox
.L1983e:
	add	r7, #0x24
	add	r6, #1
.L19842:
	cmp	r6, #8
	bne	.L19830
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80197c4

@ RepaintTextRegion
@ r0 = message-box slot. Repaints the region a message box occupies, restoring
@ the tilemap with ClearUIRegion and redrawing the frame with Func_170f8. Called by
@ Func_16868 when a box has more text to reveal.
.thumb_func_start Func_8019854  @ 0x08019854
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r5, r0
	ldr	r3, [r5]
	ldrh	r1, [r3, #0xc]
	mov	r9, r1
	ldrh	r1, [r3, #0xe]
	mov	r11, r1
	ldrh	r1, [r3, #8]
	sub	sp, #8
	ldrh	r2, [r3, #0x12]
	str	r1, [sp, #4]
	ldrh	r3, [r3, #0xa]
	str	r3, [sp]
	cmp	r2, #4
	bne	.L198c4
	add	r1, #2
	mov	r6, r9
	mov	r7, r11
	mov	r8, r1
	sub	r6, #1
	sub	r7, #1
	add	r3, #2
	mov	r2, r8
	mov	r0, r6
	mov	r1, r7
	mov	r10, r3
	bl	Func_80170f8
	ldrh	r3, [r5, #0x14]
	ldr	r2, =0xffff
	add	r3, r2
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	lsr	r2, r3, #16
	cmp	r2, #0
	bne	.L198c4
	ldr	r3, [r5]
	mov	r0, r6
	strh	r2, [r3, #0x12]
	mov	r1, r7
	mov	r2, r8
	mov	r3, r10
	bl	ClearUIRegion
	mov	r0, r9
	mov	r1, r11
	ldr	r2, [sp, #4]
	ldr	r3, [sp]
	bl	Func_80170f8
.L198c4:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8019854

@ ClearCallbackTable
@ Takes no arguments. Clears the eight-entry callback table -- pointers at
@ [iwram_1e8c]+0x12BC (4 bytes each) and their ids at +0x12DC (2 bytes each) --
@ used by Func_19908 and Func_19944.
.thumb_func_start Func_80198dc  @ 0x080198dc
	push	{lr}
	ldr	r3, =iwram_3001e8c
	ldr	r4, =0x12dc
	ldr	r3, [r3]
	add	r2, r3, r4
	sub	r4, #0x20
	mov	r1, #0
	mov	r0, #0
	add	r3, r4
.L198ee:
	add	r1, #1
	stmia	r3!, {r0}
	strh	r0, [r2]
	add	r2, #2
	cmp	r1, #8
	bne	.L198ee
	pop	{r0}
	bx	r0
.func_end Func_80198dc

@ RegisterCallback
@ r0 = callback, r1 = id. Finds the first free slot in the eight-entry table --
@ free meaning its id halfword at [iwram_1e8c]+0x12DC is zero -- stores the
@ pointer at +0x12BC and the id alongside, and returns the slot index.
.thumb_func_start Func_8019908  @ 0x08019908
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e8c
	mov	r7, r1
	ldr	r1, [r3]
	ldr	r3, =0x12bc
	ldr	r4, =0x12dc
	mov	r6, r0
	mov	r5, #8
	mov	r0, #0
	add	r2, r1, r3
.L1991c:
	ldrh	r3, [r4, r1]
	cmp	r3, #0
	bne	.L19928
	str	r6, [r2]
	strh	r7, [r4, r1]
	b	.L19932
.L19928:
	add	r0, #1
	add	r2, #4
	add	r4, #2
	cmp	r0, r5
	bne	.L1991c
.L19932:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8019908

@ LookupCallback
@ r0 = id, r1 = non-zero to also remove it. Scans the eight-entry table for a
@ matching id and returns its callback pointer, clearing both the pointer and
@ the id when r1 is set. Returns 0 when the id is not registered.
.thumb_func_start Func_8019944  @ 0x08019944
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e8c
	ldr	r4, =0x12dc
	ldr	r2, [r3]
	ldrh	r3, [r4, r2]
	mov	r5, r0
	mov	r7, r1
	mov	r1, #0
	mov	r6, #0
	mov	r12, r1
	ldr	r0, =0x12bc
	cmp	r3, r5
	bne	.L1996a
	ldr	r6, [r0, r2]
	cmp	r7, #0
	beq	.L19988
	str	r1, [r0, r2]
	strh	r1, [r4, r2]
	b	.L19988
.L1996a:
	add	r1, #1
	add	r0, #4
	add	r4, #2
	cmp	r1, #7
	bhi	.L19988
	ldrh	r3, [r4, r2]
	cmp	r3, r5
	bne	.L1996a
	ldr	r6, [r0, r2]
	cmp	r7, #0
	beq	.L19988
	mov	r3, r12
	str	r3, [r0, r2]
	mov	r3, r12
	strh	r3, [r4, r2]
.L19988:
	mov	r0, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8019944

@ PollConfirmKey
@ r0 = mask. Returns whether a confirm-style press is active this frame.
@ Reads the HELD key state at iwram_1ae8 against 0x303 -- A, B, and the two
@ shoulder buttons. The byte at [iwram_1e8c]+0x12F9 gates a _Func_f954c check,
@ which suppresses input while that subsystem is busy.
.thumb_func_start Func_801999c  @ 0x0801999c
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e8c
	ldr	r2, =0x12f9
	ldr	r3, [r3]
	add	r3, r2
	ldrb	r3, [r3]
	mov	r6, r0
	mov	r5, #0
	cmp	r3, #0
	beq	.L199ba
	bl	_Func_80f954c
	cmp	r0, #0
	bne	.L199ba
	mov	r5, #1
.L199ba:
	ldr	r3, =gKeyHeld
	ldr	r2, =0x303
	ldr	r3, [r3]
	and	r3, r2
	cmp	r3, #0
	beq	.L199c8
	mov	r5, #1
.L199c8:
	cmp	r5, #0
	beq	.L199d4
	mov	r3, #0
	strh	r3, [r6, #0x14]
	mov	r0, #1
	b	.L199d6
.L199d4:
	mov	r0, #0
.L199d6:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_801999c

@ PollMenuKey
@ r0 = mask. The NEWLY-PRESSED counterpart to Func_1999c, reading iwram_1c94
@ (pressed this frame) and iwram_1af8 rather than the held state, so menu
@ navigation does not repeat while a key is down.
@ The mode byte at [iwram_1e8c]+0xEA4 selects between two interpretations.
.thumb_func_start Func_80199ec  @ 0x080199ec
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e8c
	ldr	r1, =0x12f9
	ldr	r5, [r3]
	add	r3, r5, r1
	ldrb	r3, [r3]
	mov	r7, r0
	mov	r6, #0
	cmp	r3, #0
	beq	.L19a0a
	bl	_Func_80f954c
	cmp	r0, #0
	bne	.L19a0a
	mov	r6, #1
.L19a0a:
	ldr	r3, =gKeyPress
	ldr	r1, =0xea4
	ldr	r2, [r3]
	add	r3, r5, r1
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L19a1c
	ldr	r3, =iwram_3001af8
	ldr	r2, [r3]
.L19a1c:
	ldr	r3, =0x303
	and	r3, r2
	cmp	r3, #0
	beq	.L19a26
	mov	r6, #1
.L19a26:
	cmp	r6, #0
	beq	.L19a32
	mov	r3, #0
	strh	r3, [r7, #0x14]
	mov	r0, #1
	b	.L19a34
.L19a32:
	mov	r0, #0
.L19a34:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80199ec

@ RunMenuModalSimple
@ r0.. = menu parameters. The short form of Func_197c4: spins on WaitFrames(1)
@ and closes with CloseUIBox, without the extra state that one tracks.
.thumb_func_start Func_8019a54  @ 0x08019a54
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e8c
	mov	r2, #0xc4
	ldr	r3, [r3]
	lsl	r2, #3
	add	r5, r3, r2
	mov	r6, #0
.L19a62:
	ldr	r0, [r5]
	cmp	r0, #0
	beq	.L19a88
	ldr	r3, [r0, #0x18]
	cmp	r3, #0
	bne	.L19a88
	ldrh	r2, [r0, #0x16]
	mov	r3, r2
	cmp	r3, #0
	beq	.L19a88
	ldrh	r3, [r0, #0x14]
	cmp	r3, #0
	beq	.L19a88
	mov	r1, #2
	and	r1, r2
	lsl	r1, #16
	lsr	r1, #16
	bl	CloseUIBox
.L19a88:
	add	r6, #1
	add	r5, #0x28
	cmp	r6, #3
	bne	.L19a62
	mov	r0, #0xa
	bl	WaitFrames
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8019a54

@ RunChoicePrompt
@ r0 = string id, r1.. = options. Opens a message box with Func_165d8 and a
@ window with CreateUIBox, renders the prompt through BufferString and TextBox,
@ then waits on .gcc2_compiled. / .gcc2_compiled. before closing with CloseUIBox.
@ This is the yes/no and multi-option prompt used across the game.
.thumb_func_start Func_8019aa0  @ 0x08019aa0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	sub	sp, #0x18
	mov	r8, r3
	mov	r6, r1
	mov	r3, #8
	mov	r1, #1
	mov	r9, r2
	str	r3, [sp, #0x14]
	str	r3, [sp, #0x10]
	mov	r7, r0
	bl	BufferString
	mov	r2, #0xeb
	lsl	r2, #4
	lsl	r3, r0, #1
	add	r3, r2
	mov	r2, r8
	ldrh	r3, [r2, r3]
	mov	r5, #0
	mov	r10, r0
	cmp	r3, #0
	beq	.L19b84
	add	r0, sp, #8
	add	r1, sp, #0x14
	add	r2, sp, #0x10
	add	r3, sp, #0xc
	str	r0, [sp]
	mov	r0, r7
	bl	TextBox
	ldr	r2, [sp, #0xc]
	mov	r3, #0x1e
	sub	r3, r2
	ldr	r4, [sp, #8]
	asr	r0, r3, #1
	mov	r3, #0xf
	sub	r3, r4
	asr	r3, #1
	mov	r7, r9
	add	r1, r3, r7
	str	r0, [sp, #0x14]
	str	r1, [sp, #0x10]
	cmp	r6, #0
	beq	.L19b10
	mov	r3, r4
	str	r5, [sp]
	bl	CreateUIBox
	mov	r5, r0
	b	.L19b22
.L19b10:
	mov	r3, #2
	str	r3, [sp]
	mov	r2, #0
	mov	r3, #0
	bl	CreateUIBox
	mov	r5, r0
	strh	r6, [r5, #8]
	strh	r6, [r5, #0xa]
.L19b22:
	mov	r3, #0
	mov	r0, r5
	mov	r1, r10
	mov	r2, #0
	str	r3, [sp]
	str	r3, [sp, #4]
	bl	Func_80165d8
	cmp	r0, #0
	bne	.L19b46
	mov	r0, r5
	mov	r1, #1
	bl	CloseUIBox
	b	.L19b84
.L19b40:
	mov	r0, #1
	bl	WaitFrames
.L19b46:
	bl	Func_8017364
	cmp	r0, #0
	beq	.L19b40
	cmp	r6, #0
	beq	.L19b6e
	mov	r0, r5
	mov	r1, #0
	bl	CloseUIBox
	b	.L19b62
.L19b5c:
	mov	r0, #1
	bl	WaitFrames
.L19b62:
	mov	r0, r5
	bl	Func_8017394
	cmp	r0, #0
	beq	.L19b5c
	b	.L19b76
.L19b6e:
	mov	r0, r5
	mov	r1, #1
	bl	CloseUIBox
.L19b76:
	ldr	r3, =0x12f4
	mov	r2, #0
	add	r3, r8
	strh	r2, [r3]
	ldr	r3, =0x12f6
	add	r3, r8
	strh	r2, [r3]
.L19b84:
	add	sp, #0x18
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8019aa0

