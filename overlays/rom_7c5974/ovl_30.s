	.include "macros.inc"

@ ============================================================================
@ Overlay 0x7c5974 -- a town whose ENTIRE DIALOGUE SET is swapped by one save
@ bit.
@
@ Slot 0  OvlFunc_3dc  map-load entry
@ Slot 1  OvlFunc_30   edge transitions   -> .L4f8
@ Slot 2  OvlFunc_3c   map event list     -> .L630
@ Slot 3  OvlFunc_44   read after slot 4  -> .L65c
@ Slot 4  OvlFunc_4c   map objects        -> three variants
@ Slot 5  OvlFunc_38   interactions       -> none (returns 0)
@
@ Save bit 0x941 is the switch. Before it, every villager speaks from the
@ 0x1Bxx block; after it, from 0x24xx-0x25xx. Nine handlers below carry the
@ pair, and the shops open only once the bit is set -- so the town is closed
@ for business until whatever 0x941 records has happened.
@
@ The shop and inn counters use the standard facing arc
@ (`facing - 0xA001 <= 0x3FFE`); see overlays/rom_7b7790/ovl_314.s.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.L4f8
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L630
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: read after slot 4.
.thumb_func_start OvlFunc_44
	ldr	r0, =.L65c
	bx	lr
.func_end OvlFunc_44

@ Slot 4: entrance 0x0A -> .Lc98; otherwise save bit 0x941 selects .La64
@ (after) or .L824 (before).
.thumb_func_start OvlFunc_4c
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xa
	bne	.L62
	ldr	r0, =.Lc98
	b	.L72
.L62:
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L70
	ldr	r0, =.La64
	b	.L72
.L70:
	ldr	r0, =.L824
.L72:
	pop	{r1}
	bx	r1
.func_end OvlFunc_4c

@ Counter: INN 8, speaker slot 0x11. The inn opens ONLY when 0x941 is set --
@ both the facing arc and the save bit must pass. Lines 0x24FB / 0x1BD0.
.thumb_func_start OvlFunc_8c
	push	{lr}
	mov	r0, #0
	bl	__Func_92054
	ldr	r2, =0xffff5fff
	ldrh	r3, [r0, #6]
	add	r3, r2
	ldr	r2, =0x3ffe
	cmp	r3, r2
	bhi	.Lb4
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lb4
	mov	r0, #8
	mov	r1, #0x11
	bl	__Func_b3284
	b	.Le4
.Lb4:
	bl	__Func_916b0
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.Ld2
	ldr	r0, =0x24fb
	bl	__Func_92b94
	mov	r0, #0x11
	mov	r1, #0
	bl	__Func_93054
	b	.Le0
.Ld2:
	ldr	r0, =0x1bd0
	bl	__Func_92b94
	mov	r0, #0x11
	mov	r1, #0
	bl	__Func_93054
.Le0:
	bl	__Func_91750
.Le4:
	pop	{r0}
	bx	r0
.func_end OvlFunc_8c

@ Talk: line 0x1BD5 from slot 0x14 as a question, then sets save bit 0x940.
.thumb_func_start OvlFunc_fc
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x1bd5
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x14
	bl	__Func_93054
	mov	r0, #0x94
	lsl	r0, #4
	bl	__Func_79358
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_fc

@ Talk: line 0x1BDB from slot 0x14, and also sets save bit 0x940.
.thumb_func_start OvlFunc_124
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x1bdb
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x14
	bl	__Func_92f84
	mov	r0, #0x94
	lsl	r0, #4
	bl	__Func_79358
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_124

@ Talk: line 0x24FE from slot 0x12, asked as a question. No before-state line.
.thumb_func_start OvlFunc_14c
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x24fe
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x12
	bl	__Func_93054
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_14c

@ Counter: shop type 0x15 through Func_b29a8, speaker slot 0x15.
@ Lines 0x2507 / 0x1BDC.
.thumb_func_start OvlFunc_16c
	push	{lr}
	mov	r0, #0
	bl	__Func_92054
	ldr	r2, =0xffff5fff
	ldrh	r3, [r0, #6]
	add	r3, r2
	ldr	r2, =0x3ffe
	cmp	r3, r2
	bhi	.L188
	mov	r0, #0x15
	bl	__Func_b29a8
	b	.L1c0
.L188:
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L1aa
	bl	__Func_916b0
	ldr	r0, =0x2507
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	b	.L1c0
.L1aa:
	bl	__Func_916b0
	ldr	r0, =0x1bdc
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
.L1c0:
	pop	{r0}
	bx	r0
.func_end OvlFunc_16c

@ Talk: slot 0x10, lines 0x24FA / 0x1BE0. No counter -- dialogue only.
.thumb_func_start OvlFunc_1d8
	push	{lr}
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L1fc
	bl	__Func_916b0
	ldr	r0, =0x24fa
	bl	__Func_92b94
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	b	.L212
.L1fc:
	bl	__Func_916b0
	ldr	r0, =0x1be0
	bl	__Func_92b94
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
.L212:
	pop	{r0}
	bx	r0
.func_end OvlFunc_1d8

@ Counter: shop 0x19, speaker slot 0x10. Lines 0x24F9 / 0x1BCF.
.thumb_func_start OvlFunc_224
	push	{lr}
	mov	r0, #0
	bl	__Func_92054
	ldr	r2, =0xffff5fff
	ldrh	r3, [r0, #6]
	add	r3, r2
	ldr	r2, =0x3ffe
	cmp	r3, r2
	bhi	.L242
	mov	r0, #0x19
	mov	r1, #0x10
	bl	__Func_b0278
	b	.L27a
.L242:
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L264
	bl	__Func_916b0
	ldr	r0, =0x24f9
	bl	__Func_92b94
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	b	.L27a
.L264:
	bl	__Func_916b0
	ldr	r0, =0x1bcf
	bl	__Func_92b94
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
.L27a:
	pop	{r0}
	bx	r0
.func_end OvlFunc_224

@ Talk: slot 0x0E, lines 0x24F6 / 0x1BDE. Note it neither opens nor closes
@ the cutscene frame, so it runs inside a caller that already has.
.thumb_func_start OvlFunc_294
	push	{lr}
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2b0
	ldr	r0, =0x24f6
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_92f84
	b	.L2be
.L2b0:
	ldr	r0, =0x1bde
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_92f84
.L2be:
	pop	{r0}
	bx	r0
.func_end OvlFunc_294

@ Counter: shop 0x1D, speaker slot 0x0E. The facing test is checked only when
@ 0x941 is set; before that the villager always speaks. Lines 0x24F5 / ...
.thumb_func_start OvlFunc_2d0
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L310
	ldr	r2, =0xffff5fff
	add	r3, r5, r2
	ldr	r2, =0x3ffe
	cmp	r3, r2
	bhi	.L2f8
	mov	r0, #0x1d
	mov	r1, #0xe
	bl	__Func_b0278
	b	.L31e
.L2f8:
	bl	__Func_916b0
	ldr	r0, =0x24f5
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	b	.L31e
.L310:
	ldr	r0, =0x1bcd
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_92f84
.L31e:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_2d0

@ Talk: slot 0x0F, lines 0x24F8 / 0x1BDF. Frameless, like OvlFunc_294.
.thumb_func_start OvlFunc_338
	push	{lr}
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L354
	ldr	r0, =0x24f8
	bl	__Func_92b94
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_92f84
	b	.L362
.L354:
	ldr	r0, =0x1bdf
	bl	__Func_92b94
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_92f84
.L362:
	pop	{r0}
	bx	r0
.func_end OvlFunc_338

@ Counter: shop 0x1E, speaker slot 0x0F. Lines 0x24F7 / 0x1BCE.
.thumb_func_start OvlFunc_374
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	ldr	r0, =0x941
	bl	__Func_79338
	cmp	r0, #0
	beq	.L3b4
	ldr	r2, =0xffff5fff
	add	r3, r5, r2
	ldr	r2, =0x3ffe
	cmp	r3, r2
	bhi	.L39c
	mov	r0, #0x1e
	mov	r1, #0xf
	bl	__Func_b0278
	b	.L3c2
.L39c:
	bl	__Func_916b0
	ldr	r0, =0x24f7
	bl	__Func_92b94
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	b	.L3c2
.L3b4:
	ldr	r0, =0x1bce
	bl	__Func_92b94
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_92f84
.L3c2:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_374

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x209. Arriving by
@ entrance 0x0A clears save bit 0x12F and writes ewram_240+0x1C4 = 0x69 and
@ +0x1C6 = 0x0A -- staging a destination for a later transition.
@
@ Always resets slots 0x17, 0x18 and 0x19 with Func_c528, so those three
@ objects start each visit in their default state.
.thumb_func_start OvlFunc_3dc
	push	{r5, r6, lr}
	ldr	r3, =iwram_1ebc
	mov	r1, #0xe0
	ldr	r3, [r3]
	lsl	r1, #1
	ldr	r2, =0x209
	add	r3, r1
	ldr	r5, =ewram_240
	str	r2, [r3]
	sub	r2, #0x47
	add	r3, r5, r2
	mov	r1, #0
	ldrsh	r6, [r3, r1]
	cmp	r6, #0xa
	bne	.L412
	ldr	r0, =0x12f
	bl	__Func_79374
	mov	r1, #0xe2
	ldr	r2, =0x69
	lsl	r1, #1
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r2, #0xe3
	lsl	r2, #1
	add	r3, r5, r2
	strh	r6, [r3]
.L412:
	mov	r0, #0x17
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x18
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x19
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_3dc

@ Counter: shop type 0x15, speaker slot 0x16, line 0x266B. Not gated on
@ 0x941 -- this one trades from the start.
.thumb_func_start OvlFunc_454
	push	{lr}
	mov	r0, #0
	bl	__Func_92054
	ldr	r2, =0xffff5fff
	ldrh	r3, [r0, #6]
	add	r3, r2
	ldr	r2, =0x3ffe
	cmp	r3, r2
	bhi	.L470
	mov	r0, #0x15
	bl	__Func_b29a8
	b	.L47e
.L470:
	ldr	r0, =0x266b
	bl	__Func_92b94
	mov	r0, #0x16
	mov	r1, #0
	bl	__Func_92f84
.L47e:
	pop	{r0}
	bx	r0
.func_end OvlFunc_454

	.section .data

.L4f8:
	.incbin "overlays/rom_7c5974/orig.bin", 0x4f8, (0x630-0x4f8)
.L630:
	.incbin "overlays/rom_7c5974/orig.bin", 0x630, (0x65c-0x630)
.L65c:
	.incbin "overlays/rom_7c5974/orig.bin", 0x65c, (0x824-0x65c)
.L824:
	.incbin "overlays/rom_7c5974/orig.bin", 0x824, (0xa64-0x824)
.La64:
	.incbin "overlays/rom_7c5974/orig.bin", 0xa64, (0xc98-0xa64)
.Lc98:
	.incbin "overlays/rom_7c5974/orig.bin", 0xc98
