	.include "macros.inc"

@ ============================================================================
@ Overlay 0x7a0010 -- a map that BUILDS one of its tables at load time rather
@ than storing every variant.
@
@ Slot 0  OvlFunc_1c4  map-load entry
@ Slot 1  OvlFunc_5c   edge transitions   -> .L318
@ Slot 2  OvlFunc_68   map event list     -> .L498
@ Slot 3  OvlFunc_70   read after slot 4  -> .L4d8, rewritten in place
@ Slot 4  OvlFunc_1bc  map objects
@ Slot 5  OvlFunc_64   interactions       -> none (returns 0)
@ ============================================================================

@ ResetRecordArray
@ r0 = an array of fifteen 0x18-byte records. Rewrites every one: byte +0x16 to
@ 2, word +0x04 to 1, and the halfword at +0x00 to sprite 0x69 -- except records
@ 4 and 7, which get 0x6E instead. Called only from OvlFunc_70.
.thumb_func_start OvlFunc_30
	push	{r5, lr}
	mov	r3, #0
	mov	r5, #2
	mov	r4, #1
	mov	r1, #0x69
	mov	r2, #0x6e
.L3c:
	strb	r5, [r0, #0x16]
	str	r4, [r0, #4]
	strh	r1, [r0]
	cmp	r3, #4
	beq	.L4a
	cmp	r3, #7
	bne	.L4c
.L4a:
	strh	r2, [r0]
.L4c:
	add	r3, #1
	add	r0, #0x18
	cmp	r3, #0xe
	bls	.L3c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_30

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_5c
	ldr	r0, =.L318
	bx	lr
.func_end OvlFunc_5c

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_64
	mov	r0, #0
	bx	lr
.func_end OvlFunc_64

@ Slot 2: map event list.
.thumb_func_start OvlFunc_68
	ldr	r0, =.L498
	bx	lr
.func_end OvlFunc_68

@ Slot 3: read after slot 4.
@
@ Unusually, this SLOT MUTATES ITS OWN TABLE. When save bit 0x845 is clear it
@ first runs OvlFunc_30 over .L4d8 to reset all fifteen records to their default
@ sprites; once the bit is set the stored table is left as the game last left
@ it. Either way the result goes through Func_8b868, which tags the records
@ whose position falls inside the active bounds, and .L4d8 itself is returned.
@
@ So this map keeps one mutable record array instead of a table per state.
.thumb_func_start OvlFunc_70
	push	{r5, lr}
	ldr	r0, =0x845
	bl	__Func_79338
	cmp	r0, #0
	bne	.L82
	ldr	r0, =.L4d8
	bl	OvlFunc_30
.L82:
	ldr	r5, =.L4d8
	mov	r0, r5
	bl	__Func_8b868
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_70

@ TalkDirectional
@ Takes no arguments. Reads the player's facing from +0x06 before opening the
@ cutscene frame, then picks between two outcomes by range: facings in
@ 0xA001..0xDFFF (the test is `facing - 0xA001 <= 0x3FFE` in unsigned) run
@ Func_b0278 with 0x0D and 0x10, anything else speaks line 0x16AD from slot
@ 0x10. Approaching from the wrong side gets the plain line.
.thumb_func_start OvlFunc_9c
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.Lbe
	mov	r0, #0xd
	mov	r1, #0x10
	bl	__Func_b0278
	b	.Lcc
.Lbe:
	ldr	r0, =0x16ad
	bl	__Func_92b94
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_92f84
.Lcc:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_9c

.thumb_func_start OvlFunc_e4
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L106
	mov	r0, #0xe
	mov	r1, #0x11
	bl	__Func_b0278
	b	.L114
.L106:
	ldr	r0, =0x16af
	bl	__Func_92b94
	mov	r0, #0x11
	mov	r1, #0
	bl	__Func_92f84
.L114:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_e4

.thumb_func_start OvlFunc_12c
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L14e
	mov	r0, #0xf
	mov	r1, #0x12
	bl	__Func_b0278
	b	.L15c
.L14e:
	ldr	r0, =0x16b1
	bl	__Func_92b94
	mov	r0, #0x12
	mov	r1, #0
	bl	__Func_92f84
.L15c:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_12c

.thumb_func_start OvlFunc_174
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L196
	mov	r0, #3
	mov	r1, #0x13
	bl	__Func_b3284
	b	.L1a4
.L196:
	ldr	r0, =0x16b7
	bl	__Func_92b94
	mov	r0, #0x13
	mov	r1, #0
	bl	__Func_92f84
.L1a4:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_174

.thumb_func_start OvlFunc_1bc
	ldr	r0, =.L658
	bx	lr
.func_end OvlFunc_1bc

.thumb_func_start OvlFunc_1c4
	push	{r5, r6, lr}
	ldr	r3, =iwram_1ebc
	mov	r1, #0xe0
	ldr	r3, [r3]
	lsl	r1, #1
	ldr	r2, =0x209
	add	r3, r1
	str	r2, [r3]
	ldr	r0, =0x845
	sub	sp, #8
	bl	__Func_79338
	cmp	r0, #0
	bne	.L1f4
	mov	r5, #8
.L1e2:
	mov	r0, r5
	bl	__Func_92054
	add	r5, #1
	mov	r1, #0
	bl	__Func_c528
	cmp	r5, #0x16
	bls	.L1e2
.L1f4:
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	ldrh	r2, [r3]
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #7
	bne	.L23c
	mov	r5, #0xd
	mov	r6, #8
	mov	r0, #0x22
	mov	r1, #0x22
	mov	r2, #0x12
	mov	r3, #0x10
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_10424
	mov	r0, #0x22
	mov	r1, #0x5e
	mov	r2, #0x12
	mov	r3, #0x4c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_10424
	mov	r0, #0x5e
	mov	r1, #0x22
	mov	r2, #0x4e
	mov	r3, #0x10
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_10424
	b	.L29c
.L23c:
	mov	r3, r2
	sub	r3, #8
	mov	r2, #0x80
	lsl	r3, #16
	lsl	r2, #9
	cmp	r3, r2
	bhi	.L29c
	mov	r5, #0xb
	mov	r6, #8
	mov	r0, #0x22
	mov	r1, #0x2b
	mov	r2, #0x13
	mov	r3, #0x17
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_10424
	mov	r0, #0x22
	mov	r1, #0x5e
	mov	r2, #0x13
	mov	r3, #0x53
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_10424
	mov	r3, #0x17
	mov	r0, #0x5e
	mov	r1, #0x22
	mov	r2, #0x4f
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_10424
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
.L29c:
	mov	r0, #0
	add	sp, #8
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_1c4

	.section .data

.L318:
	.incbin "overlays/rom_7a0010/orig.bin", 0x318, (0x498-0x318)
.L498:
	.incbin "overlays/rom_7a0010/orig.bin", 0x498, (0x4d8-0x498)
.L4d8:
	.incbin "overlays/rom_7a0010/orig.bin", 0x4d8, (0x658-0x4d8)
.L658:
	.incbin "overlays/rom_7a0010/orig.bin", 0x658
