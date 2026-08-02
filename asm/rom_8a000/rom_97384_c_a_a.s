	.include "macros.inc"
	.include "gba.inc"

@ PrepareEncounterTransition
@ r0=encounter id. Sets up the transition out of the field and into battle.
@ Scene mode 3 (iwram_1ebc+0x19E) takes a separate path that skips the field
@ teardown. The ~40-instruction body is characterised structurally.
.thumb_func_start Func_80974d8  @ 0x080974d8
	push	{r5, r6, lr}
	ldr	r2, =iwram_3001ebc
	mov	r1, #0xcf
	ldr	r3, [r2]
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	sub	sp, #0xc
	mov	r6, r0
	cmp	r3, #3
	bne	.L97504
	mov	r5, sp
	mov	r1, r5
	bl	PhysMove
	ldr	r3, [r5]
	lsl	r3, #16
	str	r3, [r6]
	ldr	r3, [r5, #4]
	lsl	r3, #16
	b	.L97528
.L97504:
	mov	r3, r2
	sub	r3, #0x4c
	ldr	r2, [r3]
	mov	r3, r2
	add	r3, #0xe4
	add	r2, #0xe8
	ldr	r1, [r3]
	ldr	r0, [r2]
	ldr	r3, =0xffff0000
	and	r1, r3
	and	r0, r3
	ldr	r3, [r6]
	sub	r3, r1
	ldr	r2, [r6, #4]
	str	r3, [r6]
	ldr	r3, [r6, #8]
	sub	r3, r2
	sub	r3, r0
.L97528:
	str	r3, [r6, #8]
	mov	r3, #0
	str	r3, [r6, #4]
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80974d8

@ RunEncounterTransition
@ r0=encounter id, r1=style. Allocates the transition state with
@ galloc_ewram(0x16, 0x298), uploads the palettes with Func_97384 and drives the
@ wipe to completion. The ~90-instruction body is characterised structurally.
.thumb_func_start Field_MindRead  @ 0x08097540
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r8, r1
	mov	r1, #0xa6
	lsl	r1, #2
	mov	r6, r0
	mov	r0, #0x16
	sub	sp, #4
	bl	galloc_ewram
	mov	r5, r0
	bl	Func_8097384
	mov	r3, #0
	mov	r0, sp
	str	r3, [r0]
	mov	r1, r5
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x850000a6
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =iwram_3001e40
	ldr	r0, [r3]
	mov	r1, #0xb4
	lsl	r1, #1
	lsl	r0, #1
	bl	__umodsi3
	ldr	r2, =0x28e
	add	r3, r5, r2
	strh	r0, [r3]
	bl	Func_80978c4
	ldr	r2, =0x28d
	add	r3, r5, r2
	sub	r2, #1
	mov	r0, #0
	ldrsb	r0, [r3, r0]
	add	r3, r5, r2
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	sub	r2, #1
	lsl	r3, #5
	lsl	r0, #10
	orr	r0, r3
	add	r3, r5, r2
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	orr	r0, r3
	mov	r3, #0x80
	lsl	r3, #14
	orr	r0, r3
	mov	r1, #1
	bl	Func_8091200
	mov	r0, #8
	bl	Func_8091254
	mov	r2, #0xa4
	lsl	r2, #2
	add	r3, r5, r2
	add	r2, #2
	strh	r6, [r3]
	add	r3, r5, r2
	mov	r2, r8
	strh	r2, [r3]
	mov	r3, #0xa5
	lsl	r3, #2
	add	r5, r3
	mov	r3, #8
	strb	r3, [r5]
	bl	Func_8097a7c
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Task_08097644
	bl	StartTask
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Field_MindRead

