	.include "macros.inc"
  
@ LoadFontResource
@ r0=slot (0-7), r1=destination buffer, r2=resource id, r3=translation table
@ selector (0 = none). Returns the glyph area (width * height) from the loaded
@ header, or 0 on failure.
@ Loads a font/glyph resource into one of the 8 slots at [iwram_1e68], stride 8:
@   - fetches the resource header via _Func_185008 (overlay export)
@   - records (slot << 12) | id at slot+0x1C and the buffer at slot+0x20
@   - scans the rodata_12fa0 table (4-byte entries: id halfword + size halfword,
@     see f9_6_rom_b074_rodata.s) for the matching id, bailing out after 0xFF
@     entries or on a 0 terminator
@   - decompresses/copies the payload with Func_2f40 and Func_5340
@   - relocates the resource's internal pointer table in place: each non-zero
@     entry is rebased from a relative offset to an absolute address
@   - when r3 is non-zero, remaps every payload byte <= 0xDF through the
@     translation table at Data_92b8 + (r3 - 1) * 0x100 (selector values above 5
@     fall back to table 0)
.thumb_func_start Func_b6b8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r6, r0
	sub	sp, #4
	mov	r7, r1
	mov	r4, r2
	mov	r8, r3
	cmp	r6, #7
	bls	.Lb6d2
.Lb6ce:
	mov	r0, #0
	b	.Lb77e
.Lb6d2:
	ldr	r3, =iwram_1e68
	ldr	r5, [r3]
	mov	r0, r4
	lsl	r3, r6, #3
	add	r5, r3
	str	r4, [sp]
	bl	_Func_185008
	ldr	r4, [sp]
	ldr	r2, =rodata_12fa0
	lsl	r3, r6, #12
	add	r5, #0x1c
	orr	r3, r4
	str	r3, [r5]
	mov	r1, #2
	ldrsh	r3, [r2, r1]
	lsl	r3, #16
	lsr	r3, #16
	mov	r10, r0
	str	r7, [r5, #4]
	ldrh	r0, [r2]
	add	r1, r2, #6
	mov	r5, #0
.Lb700:
	add	r2, #4
	cmp	r3, #0
	beq	.Lb6ce
	cmp	r3, r4
	beq	.Lb71e
	add	r5, #1
	cmp	r5, #0xff
	bhi	.Lb71e
	mov	r6, #0
	ldrsh	r3, [r1, r6]
	lsl	r3, #16
	lsr	r3, #16
	ldrh	r0, [r2]
	add	r1, #4
	b	.Lb700
.Lb71e:
	bl	Func_2f40
	mov	r1, r7
	bl	Func_5340
	ldr	r3, [r7]
	mov	r4, r7
	mov	r5, #0
	cmp	r3, #0
	beq	.Lb746
	mov	r2, r3
.Lb734:
	add	r3, r2, r7
	add	r5, #1
	stmia	r4!, {r3}
	cmp	r5, #0xff
	bhi	.Lb746
	ldr	r3, [r4]
	mov	r2, r3
	cmp	r3, #0
	bne	.Lb734
.Lb746:
	mov	r1, r8
	cmp	r1, #0
	beq	.Lb774
	mov	r2, r8
	sub	r2, #1
	add	r5, r4, #4
	add	r0, r7, r0
	cmp	r2, #4
	bls	.Lb75a
	mov	r2, #0
.Lb75a:
	ldr	r3, =Data_92b8
	lsl	r2, #8
	add	r2, r3
	cmp	r5, r0
	bcs	.Lb774
.Lb764:
	ldrb	r4, [r5]
	cmp	r4, #0xdf
	bhi	.Lb76e
	ldrb	r4, [r2, r4]
	strb	r4, [r5]
.Lb76e:
	add	r5, #1
	cmp	r5, r0
	bcc	.Lb764
.Lb774:
	mov	r2, r10
	ldrb	r3, [r2]
	ldrb	r2, [r2, #1]
	mov	r0, r2
	mul	r0, r3
.Lb77e:
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_b6b8
