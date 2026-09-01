	.include "macros.inc"
	.include "gba.inc"

@ SetChannelSample
@ r0.. = parameters. Points a channel at its sample data through VerifyFlashSector.
.thumb_func_start Func_8005868  @ 0x08005868
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001f1c
	ldr	r3, [r3]
	ldr	r2, =ewram_2004c04
	lsl	r0, #16
	mov	r6, r3
	lsr	r5, r0, #16
	add	r6, #0x40
	ldr	r3, [r2]
	mov	r0, r5
	mov	r1, r6
	bl	_call_via_r3
	lsl	r0, #16
	cmp	r0, #0
	beq	.L588c
	mov	r0, #1
	b	.L589c
.L588c:
	mov	r0, r5
	mov	r1, r6
	bl	VerifyFlashSector
	mov	r3, r0
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
.L589c:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_8005868

@ AllocateChannel
@ r0.. = parameters. Claims a free mixer channel via .gcc2_compiled. and initialises
@ it with ReadFlash.
.thumb_func_start Func_80058ac  @ 0x080058ac
	push	{r5, lr}
	ldr	r3, =iwram_3001f1c
	ldr	r5, [r3]
	lsl	r0, #16
	add	r5, #0x40
	mov	r3, #0x80
	mov	r2, r5
	lsr	r0, #16
	lsl	r3, #5
	mov	r1, #0
	sub	sp, #0x10
	bl	ReadFlash
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	mov	r1, sp
	ldr	r2, =0x84000004
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r2, #0x80
	ldr	r1, =REG_DMA3SAD
	lsl	r2, #24
.L58d8:
	ldr	r3, [r1, #8]
	and	r3, r2
	cmp	r3, #0
	bne	.L58d8
	bl	Func_8005ae0
	mov	r3, sp
	ldrh	r3, [r3, #8]
	lsl	r0, #16
	lsr	r0, #16
	sub	r0, r3
	add	sp, #0x10
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_80058ac
