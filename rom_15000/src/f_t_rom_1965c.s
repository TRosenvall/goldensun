	.include "macros.inc"
	.include "gba.inc"

@ LayoutMenuString
@ r0 = string id. Lays a string out with Func_18038 and returns its measured
@ extent from the ring at [iwram_1e8c]+0xEB0, for sizing a menu box.
.thumb_func_start Func_1965c
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e8c
	ldr	r6, [r3]
	ldr	r3, =0x12b2
	mov	r5, r2
	add	r2, r6, r3
	mov	r3, #0
	mov	r7, r1
	strh	r3, [r2]
	mov	r1, #1
	bl	Func_18038
	sub	r5, #1
	mov	r0, #0
	cmp	r0, r5
	bcs	.L196a8
	mov	r2, #0xeb
	lsl	r2, #4
	ldrh	r3, [r6, r2]
	strh	r3, [r7]
	lsl	r3, #16
	cmp	r3, #0
	beq	.L196a8
	mov	r12, r5
	add	r2, r6, r2
	mov	r4, #0
.L19690:
	add	r0, #1
	add	r4, #2
	cmp	r0, r12
	bcs	.L196ac
	add	r2, #2
	ldrh	r3, [r2]
	mov	r1, r4
	strh	r3, [r1, r7]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L19690
	b	.L196ae
.L196a8:
	mov	r1, #0
	b	.L196ae
.L196ac:
	lsl	r1, r0, #1
.L196ae:
	ldr	r3, .L196b8	@ 0
	strh	r3, [r1, r7]
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
	.align	2, 0
.L196b8:
	.word	0
.func_end Func_1965c

@ ReleaseMenuBuffers
@ r0, r1 = parameters, r2 = a bitmask of which buffers to release.
@ Walks the allocation slots in iwram_1e50 -- the same tag table rom_c9000 uses
@ -- freeing each selected entry with Func_2dd8 and reallocating with Func_48b0
@ where a replacement is wanted. Func_19bac does the DMA copy for entries that
@ are being resized rather than dropped.
.thumb_func_start Func_196c4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1e50
	add	r3, #0xc8
	mov	r7, r2
	ldr	r2, [r3]
	mov	r10, r2
	mov	r8, r3
	mov	r3, r10
	sub	sp, #0xc
	mov	r9, r0
	mov	r6, r1
	cmp	r3, #0
	bne	.L19706
	ldr	r5, =0x140
	mov	r0, #0x32
	mov	r1, r5
	bl	Func_48b0
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_15430
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r2, r8
	ldr	r3, [r2]
.L19706:
	mov	r5, sp
	mov	r1, r9
	mov	r0, r5
	mov	r8, r3
	bl	Func_19bac
	ldr	r3, =0xffff
	mov	r9, r3
	b	.L19770
.L19718:
	cmp	r0, #0xe
	beq	.L19730
	cmp	r0, #0xe
	bhi	.L1972a
	cmp	r0, #0xc
	bhi	.L19766
	cmp	r0, #8
	bcc	.L19766
	b	.L19750
.L1972a:
	cmp	r0, #0xf
	beq	.L19750
	b	.L19766
.L19730:
	sub	r7, #3
	cmp	r7, #0
	ble	.L1977a
	strh	r0, [r6]
	mov	r0, r5
	bl	_call_via_r8
	add	r6, #2
	add	r0, r9
	strh	r0, [r6]
	mov	r0, r5
	bl	_call_via_r8
	add	r6, #2
	add	r0, r9
	b	.L1976c
.L19750:
	sub	r7, #1
	cmp	r7, #0
	ble	.L1977a
	strh	r0, [r6]
	mov	r0, r5
	bl	_call_via_r8
	ldr	r2, =0xffff
	add	r6, #2
	add	r0, r2
	b	.L1976c
.L19766:
	sub	r7, #1
	cmp	r7, #0
	ble	.L1977a
.L1976c:
	strh	r0, [r6]
	add	r6, #2
.L19770:
	mov	r0, r5
	bl	_call_via_r8
	cmp	r0, #0
	bne	.L19718
.L1977a:
	mov	r3, r10
	cmp	r3, #0
	bne	.L19786
	mov	r0, #0x32
	bl	Func_2dd8
.L19786:
	ldr	r3, .L1979c	@ 0
	add	sp, #0xc
	strh	r3, [r6]
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
	.align	2, 0
.L1979c:
	.word	0
.func_end Func_196c4

@ ClearSlot
@ r0 = slot. Zeroes the first word if it is non-zero. A guard so callers can
@ release a slot without checking first.
.thumb_func_start Func_197b4
	push	{lr}
	ldr	r3, [r0]
	cmp	r3, #0
	beq	.L197c0
	mov	r3, #0
	str	r3, [r0]
.L197c0:
	pop	{r1}
	bx	r1
.func_end Func_197b4

@ RunMenuModal
@ r0.. = menu parameters. Drives a menu to completion, one Func_30f8(1) per
@ frame, and closes its window with Func_16418 on the way out.
.thumb_func_start Func_197c4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1e8c
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
	bl	Func_16418
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
	bl	Func_30f8
	b	.L197f8
.L19830:
	ldrh	r3, [r7, #0x16]
	cmp	r3, #0
	beq	.L1983e
	mov	r0, r7
	mov	r1, #0
	bl	Func_16418
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
.func_end Func_197c4

@ RepaintTextRegion
@ r0 = message-box slot. Repaints the region a message box occupies, restoring
@ the tilemap with Func_16178 and redrawing the frame with Func_170f8. Called by
@ Func_16868 when a box has more text to reveal.
.thumb_func_start Func_19854
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
	bl	Func_170f8
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
	bl	Func_16178
	mov	r0, r9
	mov	r1, r11
	ldr	r2, [sp, #4]
	ldr	r3, [sp]
	bl	Func_170f8
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
.func_end Func_19854

@ ClearCallbackTable
@ Takes no arguments. Clears the eight-entry callback table -- pointers at
@ [iwram_1e8c]+0x12BC (4 bytes each) and their ids at +0x12DC (2 bytes each) --
@ used by Func_19908 and Func_19944.
.thumb_func_start Func_198dc
	push	{lr}
	ldr	r3, =iwram_1e8c
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
.func_end Func_198dc

@ RegisterCallback
@ r0 = callback, r1 = id. Finds the first free slot in the eight-entry table --
@ free meaning its id halfword at [iwram_1e8c]+0x12DC is zero -- stores the
@ pointer at +0x12BC and the id alongside, and returns the slot index.
.thumb_func_start Func_19908
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e8c
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
.func_end Func_19908

@ LookupCallback
@ r0 = id, r1 = non-zero to also remove it. Scans the eight-entry table for a
@ matching id and returns its callback pointer, clearing both the pointer and
@ the id when r1 is set. Returns 0 when the id is not registered.
.thumb_func_start Func_19944
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e8c
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
.func_end Func_19944

@ PollConfirmKey
@ r0 = mask. Returns whether a confirm-style press is active this frame.
@ Reads the HELD key state at iwram_1ae8 against 0x303 -- A, B, and the two
@ shoulder buttons. The byte at [iwram_1e8c]+0x12F9 gates a _Func_f954c check,
@ which suppresses input while that subsystem is busy.
.thumb_func_start Func_1999c
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e8c
	ldr	r2, =0x12f9
	ldr	r3, [r3]
	add	r3, r2
	ldrb	r3, [r3]
	mov	r6, r0
	mov	r5, #0
	cmp	r3, #0
	beq	.L199ba
	bl	_Func_f954c
	cmp	r0, #0
	bne	.L199ba
	mov	r5, #1
.L199ba:
	ldr	r3, =iwram_1ae8
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
.func_end Func_1999c

@ PollMenuKey
@ r0 = mask. The NEWLY-PRESSED counterpart to Func_1999c, reading iwram_1c94
@ (pressed this frame) and iwram_1af8 rather than the held state, so menu
@ navigation does not repeat while a key is down.
@ The mode byte at [iwram_1e8c]+0xEA4 selects between two interpretations.
.thumb_func_start Func_199ec
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e8c
	ldr	r1, =0x12f9
	ldr	r5, [r3]
	add	r3, r5, r1
	ldrb	r3, [r3]
	mov	r7, r0
	mov	r6, #0
	cmp	r3, #0
	beq	.L19a0a
	bl	_Func_f954c
	cmp	r0, #0
	bne	.L19a0a
	mov	r6, #1
.L19a0a:
	ldr	r3, =iwram_1c94
	ldr	r1, =0xea4
	ldr	r2, [r3]
	add	r3, r5, r1
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L19a1c
	ldr	r3, =iwram_1af8
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
.func_end Func_199ec

@ RunMenuModalSimple
@ r0.. = menu parameters. The short form of Func_197c4: spins on Func_30f8(1)
@ and closes with Func_16418, without the extra state that one tracks.
.thumb_func_start Func_19a54
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e8c
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
	bl	Func_16418
.L19a88:
	add	r6, #1
	add	r5, #0x28
	cmp	r6, #3
	bne	.L19a62
	mov	r0, #0xa
	bl	Func_30f8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_19a54

@ RunChoicePrompt
@ r0 = string id, r1.. = options. Opens a message box with Func_165d8 and a
@ window with Func_162d4, renders the prompt through Func_18038 and Func_187ac,
@ then waits on Func_17364 / Func_17394 before closing with Func_16418.
@ This is the yes/no and multi-option prompt used across the game.
.thumb_func_start Func_19aa0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1e8c
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
	bl	Func_18038
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
	bl	Func_187ac
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
	bl	Func_162d4
	mov	r5, r0
	b	.L19b22
.L19b10:
	mov	r3, #2
	str	r3, [sp]
	mov	r2, #0
	mov	r3, #0
	bl	Func_162d4
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
	bl	Func_165d8
	cmp	r0, #0
	bne	.L19b46
	mov	r0, r5
	mov	r1, #1
	bl	Func_16418
	b	.L19b84
.L19b40:
	mov	r0, #1
	bl	Func_30f8
.L19b46:
	bl	Func_17364
	cmp	r0, #0
	beq	.L19b40
	cmp	r6, #0
	beq	.L19b6e
	mov	r0, r5
	mov	r1, #0
	bl	Func_16418
	b	.L19b62
.L19b5c:
	mov	r0, #1
	bl	Func_30f8
.L19b62:
	mov	r0, r5
	bl	Func_17394
	cmp	r0, #0
	beq	.L19b5c
	b	.L19b76
.L19b6e:
	mov	r0, r5
	mov	r1, #1
	bl	Func_16418
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
.func_end Func_19aa0

@ LayoutStringMode1
@ r0 = string id. Func_18038(id, 1) -- the layout pass in mode 1, which callers
@ use when the text is going somewhere other than the standard box.
.thumb_func_start Func_19ba0
	push	{lr}
	mov	r1, #1
	bl	Func_18038
	pop	{r1}
	bx	r1
.func_end Func_19ba0

@ CopyMenuBuffer
@ r0, r1 = source and destination. Allocates scratch with Func_4938, DMA3-copies
@ the buffer, and releases the scratch with Func_2df0. Used by Func_196c4 when a
@ buffer is being resized rather than freed.
.thumb_func_start Func_19bac
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r8, r0
	mov	r10, r1
	ldr	r5, =0x60
	mov	r0, r5
	bl	Func_4938
	mov	r2, #0x84
	mov	r6, r0
	lsr	r5, #2
	lsl	r2, #24
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_15570
	mov	r1, r6
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r8
	mov	r1, r10
	bl	_call_via_r6
	mov	r0, r6
	bl	Func_2df0
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_19bac

@ BuildMenuLayout
@ r0.. = layout parameters. Computes the row and column positions for a menu's
@ entries. 148 lines of pure arithmetic with no calls out; traced structurally.
.thumb_func_start Func_19bfc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r5, [r0]
	mov	r2, #0xff
	lsr	r3, r5, #8
	and	r5, r2
	ldr	r2, =HuffmanTreePointers
	lsl	r3, #3
	ldr	r1, [r2, r3]
	add	r3, #4
	ldr	r2, [r2, r3]
	lsl	r3, r5, #1
	ldrh	r3, [r3, r2]
	add	r1, r3
	mov	r10, r0
	ldr	r6, [r0, #4]
	sub	r2, r1, #1
	mov	r3, #0
	mov	r5, #0x80
	ldr	r0, [r0, #8]
	mov	r4, #1
	mov	r9, r2
	mov	r14, r3
	mov	r7, #1
	mov	r8, r5
	b	.L19c96
.L19c3a:
	mov	r2, r0
	mov	r3, #1
	and	r2, r3
	asr	r0, #1
	cmp	r2, #0
	beq	.L19c96
	cmp	r0, #0
	bne	.L19c58
	ldrb	r0, [r6]
	mov	r2, r0
	and	r2, r3
	asr	r0, #1
	mov	r3, r8
	add	r6, #1
	orr	r0, r3
.L19c58:
	cmp	r2, #0
	beq	.L19c96
	mov	r5, #1
	mov	r2, #0x80
	mov	r3, #0
	mov	r11, r5
	mov	r12, r2
.L19c66:
	mov	r2, r4
	mov	r5, r11
	and	r2, r5
	asr	r4, #1
	cmp	r2, #0
	beq	.L19c88
	cmp	r4, #0
	bne	.L19c84
	ldrb	r4, [r1]
	mov	r2, r4
	and	r2, r5
	asr	r4, #1
	mov	r5, r12
	add	r1, #1
	orr	r4, r5
.L19c84:
	cmp	r2, #0
	bne	.L19c8c
.L19c88:
	add	r3, #1
	b	.L19c92
.L19c8c:
	mov	r2, #1
	add	r14, r2
	sub	r3, #1
.L19c92:
	cmp	r3, #0
	bge	.L19c66
.L19c96:
	mov	r2, r4
	and	r2, r7
	asr	r4, #1
	cmp	r2, #0
	beq	.L19c3a
	cmp	r4, #0
	bne	.L19cb2
	ldrb	r4, [r1]
	mov	r3, r8
	mov	r2, r4
	asr	r4, #1
	add	r1, #1
	and	r2, r7
	orr	r4, r3
.L19cb2:
	cmp	r2, #0
	beq	.L19c3a
	mov	r5, r14
	lsl	r3, r5, #1
	add	r1, r3, r5
	lsl	r3, r1, #2
	mov	r2, #7
	and	r3, r2
	cmp	r3, #0
	bne	.L19cda
	mov	r2, r9
	lsr	r3, r1, #1
	sub	r3, r2, r3
	ldrb	r2, [r3]
	sub	r3, #1
	lsl	r5, r2, #4
	ldrb	r2, [r3]
	lsr	r3, r2, #4
	orr	r5, r3
	b	.L19cee
.L19cda:
	lsr	r3, r1, #1
	mov	r5, r9
	sub	r3, r5, r3
	ldrb	r2, [r3]
	mov	r1, #0xf
	sub	r3, #1
	and	r2, r1
	ldrb	r5, [r3]
	lsl	r2, #8
	orr	r5, r2
.L19cee:
	mov	r2, r10
	str	r0, [r2, #8]
	str	r5, [r2]
	str	r6, [r2, #4]
	mov	r0, r5
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_19bfc

@ InitMenuLayer
@ Takes no arguments. Called from Func_15f30 during UI bring-up. Sets the two
@ halfwords at [iwram_1e8c]+0x12EC and +0x12EE to 0x3E7 (999) -- sentinel values
@ meaning "no selection", since real indices are small.
.thumb_func_start Func_19d0c
	ldr	r3, =iwram_1e8c
	ldr	r0, =0x12ec
	ldr	r3, [r3]
	ldr	r2, .L19d20	@ 0x3e7
	add	r1, r3, r0
	add	r0, #2
	strh	r2, [r1]
	add	r1, r3, r0
	strh	r2, [r1]
	bx	lr
	.align	2, 0
.L19d20:
	.word	0x3e7
.func_end Func_19d0c

	.section .rodata

@ PROMOTED: referenced from rom_1908c.s across the split
	.global	L33e60
L33e60:
.L33e60:
	.incrom 0x33e60, 0x33eb0
@ PROMOTED: referenced from rom_1908c.s across the split
	.global	L33eb0
L33eb0:
.L33eb0:
	.incrom 0x33eb0, 0x33ee8
@ PROMOTED: referenced from rom_1908c.s across the split
	.global	L33ee8
L33ee8:
.L33ee8:
	.incrom 0x33ee8, 0x33ef8
