	.include "macros.inc"

@ ============================================================================
@ Overlay 0x7ebdfc -- a small map built around YES/NO PROMPTS.
@
@ Slot 1  OvlFunc_30  edge transitions   -> .L2f0
@ Slot 2  OvlFunc_3c  map event list     -> .L3c8
@ Slot 3  OvlFunc_44  read after slot 4  -> .L4e0 if save bit 0x96F, else .L3f0
@ Slot 5  OvlFunc_38  interactions       -> none (returns 0)
@
@ OvlFunc_68 and OvlFunc_b0 are the same prompt shape at different message
@ bases (0x25B8 and 0x25DC). Each takes the speaker slot in r0, opens the box,
@ puts the yes/no question up through Func_91c7c, and then shows base+2 for yes
@ or base+1 for no -- with a ten-frame beat before the "no" line only. Reserving
@ three consecutive ids per prompt is the convention throughout the overlays.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.L2f0
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L3c8
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: save bit 0x96F selects the later object list.
.thumb_func_start OvlFunc_44
	push	{lr}
	ldr	r0, =0x96f
	bl	__Func_79338
	cmp	r0, #0
	beq	.L54
	ldr	r0, =.L4e0
	b	.L56
.L54:
	ldr	r0, =.L3f0
.L56:
	pop	{r1}
	bx	r1
.func_end OvlFunc_44

@ Prompt at message base 0x25B8. r0 = speaker slot. See the header.
.thumb_func_start OvlFunc_68
	push	{r5, r6, lr}
	ldr	r5, =0x25b8
	mov	r6, r0
	mov	r0, r5
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, r6
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.L96
	mov	r0, #0xa
	bl	__Func_9163c
	add	r0, r5, #1
	bl	__Func_92b94
	b	.L9c
.L96:
	add	r0, r5, #2
	bl	__Func_92b94
.L9c:
	mov	r0, r6
	mov	r1, #0
	bl	__Func_92f84
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_68

@ Prompt at message base 0x25DC. r0 = speaker slot.
.thumb_func_start OvlFunc_b0
	push	{r5, r6, lr}
	ldr	r5, =0x25dc
	mov	r6, r0
	mov	r0, r5
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, r6
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.Lde
	mov	r0, #0xa
	bl	__Func_9163c
	add	r0, r5, #1
	bl	__Func_92b94
	b	.Le4
.Lde:
	add	r0, r5, #2
	bl	__Func_92b94
.Le4:
	mov	r0, r6
	mov	r1, #0
	bl	__Func_92f84
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_b0

@ OpenPassage
@ Takes no arguments. Repaints a 1x1 attribute cell at (0x19, 9) from source
@ (4, 9) and sets save bit 0x201 to record it -- the smallest possible version
@ of the passage-reveal pattern, with no cutscene around it.
.thumb_func_start OvlFunc_f8
	push	{lr}
	sub	sp, #8
	mov	r3, #4
	mov	r2, #9
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #9
	mov	r2, #1
	mov	r3, #1
	mov	r0, #0x19
	bl	__Func_10704
	ldr	r0, =0x201
	bl	__Func_79358
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_f8

@ WarpToRecordedPoint
@ Takes no arguments. Reads the interaction target halfword at
@ [iwram_1ebc]+0x16C, uses it to index the pair table at .L5d0 for an (x, z),
@ plays sound 0x9E, and hands the coordinates to Func_10560 with the descriptor
@ at .L5e8. Then Func_922c4 nudges the player -0x10 on z and the scene delay at
@ +0x1C8 is set to 0x10.
.thumb_func_start OvlFunc_120
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	ldr	r1, =iwram_1ebc
	mov	r2, #0xb6
	ldr	r3, [r1]
	lsl	r2, #1
	add	r3, r2
	mov	r8, r1
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	ldr	r2, =.L5d0
	lsl	r3, r1, #2
	mov	r10, r1
	ldrsh	r6, [r2, r3]
	add	r3, #2
	ldrsh	r5, [r2, r3]
	lsl	r6, #16
	lsl	r5, #16
	mov	r0, #0x9e
	lsr	r6, #16
	lsr	r5, #16
	bl	__Func_f9080
	mov	r1, r6
	mov	r2, r5
	ldr	r0, =.L5e8
	bl	__Func_10560
	mov	r2, #0x10
	mov	r0, #0
	mov	r1, #0
	neg	r2, r2
	bl	__Func_922c4
	mov	r2, r8
	ldr	r3, [r2]
	mov	r1, #0xe4
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0x10
	str	r2, [r3]
	mov	r0, r10
	bl	__Func_91e9c
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_120

@ WarpToRecordedPoint -- the twin of OvlFunc_120, differing only in the
@ descriptor it passes Func_10560 (.L5fe rather than .L5e8) and in ending with
@ Func_91e9c on the interaction target, so the destination also becomes the
@ next pending message.
.thumb_func_start OvlFunc_194
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	ldr	r1, =iwram_1ebc
	mov	r2, #0xb6
	ldr	r3, [r1]
	lsl	r2, #1
	add	r3, r2
	mov	r8, r1
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	ldr	r2, =.L5d0
	lsl	r3, r1, #2
	mov	r10, r1
	ldrsh	r6, [r2, r3]
	add	r3, #2
	ldrsh	r5, [r2, r3]
	lsl	r6, #16
	lsl	r5, #16
	mov	r0, #0x9e
	lsr	r6, #16
	lsr	r5, #16
	bl	__Func_f9080
	mov	r1, r6
	mov	r2, r5
	ldr	r0, =.L5fe
	bl	__Func_10560
	mov	r2, #0x10
	mov	r0, #0
	mov	r1, #0
	neg	r2, r2
	bl	__Func_922c4
	mov	r2, r8
	ldr	r3, [r2]
	mov	r1, #0xe4
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0x10
	str	r2, [r3]
	mov	r0, r10
	bl	__Func_91e9c
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_194

@ Second table selector on save bit 0x96F: .L758 once set, .L614 before.
@ Reached from a table rather than an export slot.
.thumb_func_start OvlFunc_208
	push	{lr}
	ldr	r0, =0x96f
	bl	__Func_79338
	cmp	r0, #0
	beq	.L218
	ldr	r0, =.L758
	b	.L21a
.L218:
	ldr	r0, =.L614
.L21a:
	pop	{r1}
	bx	r1
.func_end OvlFunc_208

@ Slot 0: map-load entry.
@
@ Arriving through entrance 0x5A sets save bit 0x96F -- so that one doorway is
@ what advances this map's state. Then the scene step delay at
@ [iwram_1ebc]+0x1C0 is set to 0x100 and the message delay at +0x1C8 to 0x18.
@
@ If save bit 0x201 is already set the passage OvlFunc_f8 opens is re-applied
@ on load and slot 0x10 is put into animation 4, so the room reflects work the
@ player did on a previous visit.
.thumb_func_start OvlFunc_22c
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x5a
	bne	.L244
	ldr	r0, =0x96f
	bl	__Func_79358
.L244:
	ldr	r3, =iwram_1ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x18
	str	r3, [r2]
	ldr	r0, =0x201
	bl	__Func_79338
	cmp	r0, #0
	beq	.L270
	bl	OvlFunc_f8
	mov	r0, #0x10
	mov	r1, #4
	bl	__Func_924d4
.L270:
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_22c

	.section .data

.L2f0:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x2f0, (0x3c8-0x2f0)
.L3c8:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x3c8, (0x3f0-0x3c8)
.L3f0:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x3f0, (0x4e0-0x3f0)
.L4e0:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x4e0, (0x5d0-0x4e0)
.L5d0:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x5d0, (0x5e8-0x5d0)
.L5e8:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x5e8, (0x5fe-0x5e8)
.L5fe:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x5fe, (0x614-0x5fe)
.L614:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x614, (0x758-0x614)
.L758:
	.incbin "overlays/rom_7ebdfc/orig.bin", 0x758
