	.include "macros.inc"
	.include "gba.inc"

@ LinkDictionaryEntry
@ r0 = code. Splices entry `code` onto the front of its hash chain: the head
@ comes from the table at +0x3404, the entry's next and prev words are at
@ +code*12 and +code*12+4, and the old head's prev is fixed up when there was
@ one. A doubly-linked list, so Func_f7e34 can unlink in constant time.
.thumb_func_start Func_80f7df0  @ 0x080f7df0
	push	{r5, lr}
	ldr	r3, =ewram_2004c00
	lsl	r1, r0, #1
	ldr	r4, [r3]
	ldr	r3, =0x3404
	add	r1, r0
	lsl	r0, #2
	add	r0, r3
	ldr	r2, [r4, r0]
	mov	r0, #0xc0
	lsl	r2, #2
	lsl	r1, #2
	add	r3, r4, r2
	lsl	r0, #6
	add	r3, r0
	add	r5, r1, #4
	str	r3, [r4, r5]
	add	r2, r0
	ldr	r3, [r4, r2]
	str	r3, [r4, r1]
	add	r3, r4, r1
	str	r3, [r4, r2]
	ldr	r2, [r3]
	cmp	r2, #0
	beq	.Lf7e24
	str	r3, [r2, #4]
.Lf7e24:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_80f7df0

@ UnlinkDictionaryEntry
@ r0 = code. Removes the entry from its chain by patching the neighbours'
@ pointers. Does nothing when the entry is not linked.
.thumb_func_start Func_80f7e34  @ 0x080f7e34
	push	{lr}
	ldr	r3, =ewram_2004c00
	ldr	r1, [r3]
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #2
	add	r4, r3, #4
	ldr	r0, [r1, r4]
	cmp	r0, #0
	beq	.Lf7e56
	ldr	r2, [r1, r3]
	cmp	r2, #0
	beq	.Lf7e50
	str	r0, [r2, #4]
.Lf7e50:
	ldr	r2, [r1, r4]
	ldr	r3, [r1, r3]
	str	r3, [r2]
.Lf7e56:
	pop	{r0}
	bx	r0
.func_end Func_80f7e34
