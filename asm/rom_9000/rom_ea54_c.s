	.include "macros.inc"

@ SetDialogueVariable
@ r0=packed argument: slot index in bits 14+, value in bits 0-13. Returns the
@ unpacked value. Writes into the dialogue variable block at [iwram_1ebc],
@ addressed as halfwords.
@ Three cases, checked in order:
@   - event flag 0x107 set: slot 0xC1 is forced to 0xFA and the value is
@     discarded.
@   - otherwise, if slot 0xCF currently holds 3, the held shoulder button
@     decides: R (iwram_1c94 & 0x100) writes 0xFC88 to slot 0xC1, L (& 0x200)
@     writes 0xFC87, and neither leaves the block untouched.
@   - otherwise the value is stored into slot 0xBF or 0xC0 according to the
@     index, and any other index is ignored.
.thumb_func_start Func_800ea60  @ 0x0800ea60
	push	{r5, r6, r7, lr}
	ldr	r7, =0x3fff
	ldr	r3, =iwram_3001ebc
	lsr	r6, r0, #14
	and	r7, r0
	ldr	r0, =0x107
	ldr	r5, [r3]
	bl	_GetFlag
	cmp	r0, #0
	beq	.Lea82
	mov	r3, #0xc1
	lsl	r3, #1
	add	r2, r5, r3
	mov	r3, #0xfa
	strh	r3, [r2]
	b	.Lead8
.Lea82:
	mov	r2, #0xcf
	lsl	r2, #1
	add	r3, r5, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.Leac2
	ldr	r1, =gKeyPress
	mov	r2, #0x80
	ldr	r3, [r1]
	lsl	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Leaaa
	mov	r3, #0xc1
	lsl	r3, #1
	add	r2, r5, r3
	ldr	r3, =0xfc88
	strh	r3, [r2]
	b	.Lead8
.Leaaa:
	ldr	r3, [r1]
	mov	r2, #0x80
	lsl	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lead8
	mov	r3, #0xc1
	lsl	r3, #1
	add	r2, r5, r3
	ldr	r3, =0xfc87
	strh	r3, [r2]
	b	.Lead8
.Leac2:
	cmp	r6, #0
	beq	.Leacc
	cmp	r6, #1
	beq	.Lead0
	b	.Lead8
.Leacc:
	mov	r2, #0xbf
	b	.Lead2
.Lead0:
	mov	r2, #0xc0
.Lead2:
	lsl	r2, #1
	add	r3, r5, r2
	strh	r7, [r3]
.Lead8:
	mov	r0, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_800ea60

@ ApplyInputToDialogueVars
@ Takes no arguments. Returns non-zero if any binding matched, 0 otherwise
@ (including when the dialogue block at [iwram_1ebc] is not allocated).
@ Tests the live button state in iwram_1c94 against the configurable key masks
@ stored as halfwords in ewram_240, in priority order:
@   +0x214 -> set dialogue slot 0xB9 to 1
@   +0x210 -> set slot 0xBA to 1
@   +0x216 -> set slot 0xBB to 1
@   +0x218 -> pass the packed value at ewram_240+0x220 to Func_ea60
@   +0x21A -> pass the packed value at ewram_240+0x222 to Func_ea60
@ The first match wins; the remaining bindings are not evaluated.
.thumb_func_start Func_800eaf8  @ 0x0800eaf8
	push	{r5, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r5, #0
	mov	r0, #0
	cmp	r1, #0
	beq	.Leb80
	ldr	r4, =gState
	mov	r2, #0x85
	lsl	r2, #2
	ldr	r0, =gKeyPress
	add	r3, r4, r2
	ldrh	r2, [r3]
	ldr	r3, [r0]
	and	r3, r2
	cmp	r3, #0
	beq	.Leb1e
	mov	r3, #0xb9
	b	.Leb42
.Leb1e:
	mov	r2, #0x84
	lsl	r2, #2
	add	r3, r4, r2
	ldrh	r2, [r3]
	ldr	r3, [r0]
	and	r3, r2
	cmp	r3, #0
	beq	.Leb32
	mov	r3, #0xba
	b	.Leb42
.Leb32:
	ldr	r2, =0x216
	add	r3, r4, r2
	ldrh	r2, [r3]
	ldr	r3, [r0]
	and	r3, r2
	cmp	r3, #0
	beq	.Leb4e
	mov	r3, #0xbb
.Leb42:
	lsl	r3, #1
	add	r2, r1, r3
	mov	r3, #1
	strh	r3, [r2]
	mov	r5, #1
	b	.Leb7e
.Leb4e:
	mov	r2, #0x86
	lsl	r2, #2
	add	r3, r4, r2
	ldrh	r2, [r3]
	ldr	r3, [r0]
	and	r3, r2
	cmp	r3, #0
	beq	.Leb64
	mov	r2, #0x88
	lsl	r2, #2
	b	.Leb74
.Leb64:
	ldr	r2, =0x21a
	add	r3, r4, r2
	ldrh	r2, [r3]
	ldr	r3, [r0]
	and	r3, r2
	cmp	r3, #0
	beq	.Leb7e
	ldr	r2, =0x222
.Leb74:
	add	r3, r4, r2
	ldrh	r0, [r3]
	bl	Func_800ea60
	mov	r5, r0
.Leb7e:
	mov	r0, r5
.Leb80:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_800eaf8

@ SpheresOverlap
@ r0=centre A (vec3, 16.16), r1=radius A, r2=centre B (vec3, 16.16),
@ r3=radius B. Compares the squared distance between the centres against the
@ squared sum of the radii, working in whole pixels after an >>16 on each delta.
@ RETURN SENSE IS INVERTED from the usual convention:
@     0 = the spheres overlap
@    -1 = they are apart
@ Callers such as Func_d924 rely on this -- they treat a negative result as
@ "keep looking". The two `bgt` tests against 0x400000 before the arithmetic are
@ an overflow guard: a delta that large means a garbage coordinate, and it is
@ reported as apart rather than allowed to wrap the squaring.
.thumb_func_start Func_800eba0  @ 0x0800eba0
	push	{r5, r6, lr}
	ldmia	r0!, {r4}
	ldmia	r2!, {r5}
	sub	r4, r5
	asr	r6, r4, #16
	ldmia	r2!, {r5}
	ldmia	r0!, {r4}
	ldr	r2, [r2]
	ldr	r0, [r0]
	add	r1, r3
	mov	r3, #0x80
	sub	r4, r5
	sub	r0, r2
	lsl	r3, #15
	asr	r4, #16
	asr	r0, #16
	cmp	r6, r3
	bgt	.Lebe2
	cmp	r0, r3
	bgt	.Lebe2
	mov	r2, r4
	mul	r2, r4
	mov	r3, r6
	mul	r3, r6
	add	r3, r2
	mov	r2, r0
	mul	r2, r0
	add	r3, r2
	mov	r2, r1
	mul	r2, r1
	mov	r0, #0
	cmp	r3, r2
	blt	.Lebe6
.Lebe2:
	mov	r0, #1
	neg	r0, r0
.Lebe6:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_800eba0
