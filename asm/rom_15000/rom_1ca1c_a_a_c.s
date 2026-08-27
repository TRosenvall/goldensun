	.include "macros.inc"
	.include "gba.inc"

@ PlaceDialElement
@ r0 = record, r1, r2 = parameters. Reads an angle from the halfword at
@ [r0]+0x576 and a radius from +0x578, converts with Func_888 (the 16.16
@ trigonometric helper) and positions the element. The `.call_via r4` idiom is
@ how Thumb code reaches the ARM-mode helper.
.thumb_func_start Func_801cbd4  @ 0x0801cbd4
	push	{r5, r6, r7, lr}
	mov	r5, r0
	ldr	r0, =0x576
	mov	r6, r2
	add	r2, r5, r0
	ldrh	r0, [r2]
	ldr	r4, =Func_8000888
	lsl	r0, #16
	.call_via r4
	mov	r1, #0xaf
	lsl	r1, #3
	add	r2, r5, r1
	asr	r7, r0, #16
	ldrh	r0, [r2]
	mov	r1, r6
	lsl	r0, #16
	.call_via r4
	ldr	r2, =0x57a
	add	r5, r2
	asr	r6, r0, #16
	ldrh	r0, [r5]
	mov	r1, r3
	lsl	r0, #16
	.call_via r4
	asr	r0, #16
	cmp	r7, #0
	bge	.L1cc14
	mov	r7, #0
.L1cc14:
	cmp	r6, #0
	bge	.L1cc1a
	mov	r6, #0
.L1cc1a:
	cmp	r0, #0
	bge	.L1cc20
	mov	r0, #0
.L1cc20:
	cmp	r7, #0x1f
	ble	.L1cc26
	mov	r7, #0x1f
.L1cc26:
	cmp	r6, #0x1f
	ble	.L1cc2c
	mov	r6, #0x1f
.L1cc2c:
	cmp	r0, #0x1f
	ble	.L1cc32
	mov	r0, #0x1f
.L1cc32:
	lsl	r3, r6, #5
	lsl	r0, #10
	add	r0, r3
	add	r0, r7, r0
	lsl	r0, #16
	lsr	r0, #16
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_801cbd4

@ PlaceDialElementXY
@ r0 = record, r1, r2 = parameters. As Func_1cbd4 but takes its two signed
@ halfwords from the head of the record rather than from +0x576.
.thumb_func_start Func_801cc50  @ 0x0801cc50
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r6, r2
	mov	r2, #0
	ldrsh	r0, [r5, r2]
	ldr	r4, =Func_8000888
	lsl	r0, #16
	.call_via r4
	asr	r7, r0, #16
	mov	r2, #2
	ldrsh	r0, [r5, r2]
	mov	r1, r6
	lsl	r0, #16
	.call_via r4
	asr	r6, r0, #16
	mov	r2, #4
	ldrsh	r0, [r5, r2]
	mov	r1, r3
	lsl	r0, #16
	.call_via r4
	asr	r0, #16
	cmp	r7, #0
	bge	.L1cc8c
	mov	r7, #0
.L1cc8c:
	cmp	r7, #0x1f
	ble	.L1cc92
	mov	r7, #0x1f
.L1cc92:
	cmp	r6, #0
	bge	.L1cc98
	mov	r6, #0
.L1cc98:
	cmp	r6, #0x1f
	ble	.L1cc9e
	mov	r6, #0x1f
.L1cc9e:
	cmp	r0, #0
	bge	.L1cca4
	mov	r0, #0
.L1cca4:
	cmp	r0, #0x1f
	ble	.L1ccaa
	mov	r0, #0x1f
.L1ccaa:
	lsl	r3, r6, #5
	lsl	r0, #10
	add	r0, r3
	add	r0, r7, r0
	lsl	r0, #16
	lsr	r0, #16
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_801cc50

@ LayOutDial
@ r0.. = parameters. Places every element of the dial by repeated Func_1cc50,
@ dividing the circle with Func_b1c. 147 lines; traced structurally.
.thumb_func_start SetUIColor  @ 0x0801ccc0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r6, r1
	add	r0, #0xc
	mov	r1, #0x18
	sub	sp, #8
	bl	__modsi3
	mov	r5, r0
	lsl	r5, #18
	asr	r5, #16
	ldr	r2, =.L36750
	mov	r0, r5
	mov	r1, #0x60
	mov	r8, r2
	bl	__modsi3
	lsl	r0, #16
	mov	r2, r8
	asr	r0, #16
	sub	r6, #7
	ldrb	r3, [r2, r0]
	lsl	r6, #16
	asr	r6, #16
	add	r3, r6
	mov	r0, r5
	lsl	r3, #16
	asr	r3, #16
	mov	r1, #0x60
	add	r0, #0x20
	mov	r10, r3
	bl	__modsi3
	mov	r2, r8
	ldrb	r3, [r2, r0]
	add	r5, #0x40
	add	r3, r6
	lsl	r3, #16
	mov	r0, r5
	mov	r1, #0x60
	asr	r7, r3, #16
	bl	__modsi3
	mov	r2, r8
	ldrb	r3, [r2, r0]
	add	r3, r6
	lsl	r3, #16
	mov	r2, r10
	asr	r3, #16
	cmp	r2, #0
	bge	.L1cd30
	mov	r2, #0
	mov	r10, r2
.L1cd30:
	mov	r2, r10
	cmp	r2, #0x1f
	ble	.L1cd3a
	mov	r2, #0x1f
	mov	r10, r2
.L1cd3a:
	cmp	r7, #0
	bge	.L1cd40
	mov	r7, #0
.L1cd40:
	cmp	r7, #0x1f
	ble	.L1cd46
	mov	r7, #0x1f
.L1cd46:
	cmp	r3, #0
	bge	.L1cd4c
	mov	r3, #0
.L1cd4c:
	cmp	r3, #0x1f
	ble	.L1cd52
	mov	r3, #0x1f
.L1cd52:
	mov	r5, sp
	strh	r3, [r5, #4]
	ldr	r3, =0xeeee
	mov	r2, r10
	strh	r2, [r5]
	mov	r10, r3
	ldr	r2, =0xcccc
	mov	r1, r10
	strh	r7, [r5, #2]
	ldr	r3, =0x11110
	mov	r0, r5
	mov	r9, r2
	bl	Func_801cc50
	ldr	r6, =0xbbbb
	ldr	r3, =0x50001e8
	mov	r2, r6
	strh	r0, [r3]
	ldr	r1, =0xd555
	mov	r3, r10
	mov	r0, r5
	bl	Func_801cc50
	ldr	r3, =0x50001ea
	strh	r0, [r3]
	ldr	r3, =0xaaaa
	mov	r8, r3
	mov	r1, r6
	mov	r2, r8
	mov	r3, r9
	mov	r0, r5
	bl	Func_801cc50
	ldr	r3, =0x50001ec
	ldr	r1, =0xa221
	strh	r0, [r3]
	ldr	r2, =0x9999
	mov	r3, r8
	mov	r0, r5
	bl	Func_801cc50
	ldr	r3, =0x50001ee
	ldr	r1, =0x10888
	strh	r0, [r3]
	ldr	r2, =0xdddd
	ldr	r3, =0x13333
	mov	r0, r5
	bl	Func_801cc50
	ldr	r3, =0x50001f0
	mov	r2, r10
	strh	r0, [r3]
	ldr	r1, =0x12221
	ldr	r3, =0x15555
	mov	r0, r5
	bl	Func_801cc50
	ldr	r3, =0x50001f2
	mov	r2, #0x80
	strh	r0, [r3]
	ldr	r1, =0x13bbb
	lsl	r2, #9
	ldr	r3, =0x17777
	mov	r0, r5
	bl	Func_801cc50
	ldr	r3, =0x50001f4
	add	sp, #8
	strh	r0, [r3]
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end SetUIColor

@ StepPhaseDown
@ r0 = record. Decrements the three-state phase halfword at [r0]+0x574,
@ wrapping 0 back to 2.
.thumb_func_start Func_801ce48  @ 0x0801ce48
	push	{lr}
	ldr	r1, =0x574
	add	r0, r1
	ldrh	r2, [r0]
	mov	r3, r2
	cmp	r3, #0
	bne	.L1ce5a
	mov	r3, #2
	b	.L1ce5e
.L1ce5a:
	ldr	r1, =0xffff
	add	r3, r2, r1
.L1ce5e:
	strh	r3, [r0]
	pop	{r0}
	bx	r0
.func_end Func_801ce48
