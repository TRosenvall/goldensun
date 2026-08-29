	.include "macros.inc"
	.include "gba.inc"

@ NoOp
@ A bare `bx lr`.
.thumb_func_start Func_49a8
	bx	lr
.func_end Func_49a8

@ LoadIdentity
@ Takes no arguments. Resets the current transform to the identity, allocating
@ the matrix storage with Func_48f4 on first use.
@ THE MATRIX STACK IS THE 3D PIPELINE every module shares: Func_49ac to start,
@ Func_4bd4 / Func_4c1c / Func_4c6c to rotate about each axis, Func_4cb4 to
@ scale, Func_4a28 / Func_4a44 to store and replay a built matrix, and
@ Func_5268 to project a vector to screen. rom_c9000, rom_b5000 and rom_15000
@ all drive it in that order.
.thumb_func_start Func_49ac
	push	{r5, lr}
	mov	r1, #0x30
	mov	r0, #2
	ldr	r5, =iwram_1d2c
	bl	Func_48f4
	ldr	r2, =iwram_1cc4
	mov	r3, #0
	str	r3, [r2]
	str	r0, [r5]
	ldr	r3, =Data_ac0
	mov	r0, r3
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_49ac

@ PushMatrix
@ Takes no arguments. Saves the current transform.
.thumb_func_start Func_49e8
	push	{r5, lr}
	ldr	r5, =iwram_1cc4
	ldr	r3, [r5]
	cmp	r3, #0
	bgt	.L4a0c
	ldr	r4, =iwram_1d2c
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Data_ac0
	ldr	r1, [r4]
	ldr	r2, =0x8400000c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, [r5]
	add	r3, #1
	str	r3, [r5]
	ldr	r3, [r4]
	add	r3, #0x30
	str	r3, [r4]
.L4a0c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_49e8

@ StoreMatrix
@ r0 = destination. Writes the current transform out as 0x30 bytes -- twelve
@ words, a 3x4 matrix. rom_c9000's Func_dc968 pre-builds 384 of these so its
@ debris can tumble without per-frame trigonometry.
.thumb_func_start Func_4a28
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Data_ac0
	ldr	r2, =0x8400000c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bx	lr
.func_end Func_4a28

@ LoadMatrix
@ r0 = source. Installs a previously stored 0x30-byte transform as the current
@ one. The replay half of Func_4a28.
.thumb_func_start Func_4a44
	ldr	r3, =REG_DMA3SAD
	ldr	r1, =Data_ac0
	ldr	r2, =0x8400000c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bx	lr
.func_end Func_4a44

@ MultiplyMatrix
@ r0 = matrix. Post-multiplies the current transform by it.
.thumb_func_start Func_4a5c
	push	{lr}
	ldr	r2, =iwram_1cc4
	ldr	r3, [r2]
	cmp	r3, #0
	ble	.L4a7c
	sub	r3, #1
	str	r3, [r2]
	ldr	r3, =iwram_1d2c
	ldr	r0, [r3]
	sub	r0, #0x30
	str	r0, [r3]
	ldr	r1, =Data_ac0
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x8400000c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.L4a7c:
	pop	{r0}
	bx	r0
.func_end Func_4a5c

@ TranslateMatrix
@ r0 = a 3-vector. Adds a translation to the current transform.
.thumb_func_start Func_4a94
	ldr	r3, =Data_ac0
	mov	r0, r3
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	bx	lr
.func_end Func_4a94

@ BuildRotationMatrix
@ r0.. = three angles. Composes a full orientation from all three axes in one
@ pass, which is cheaper than chaining the single-axis routines.
.thumb_func_start Func_4ab0
	push	{r5, r6, lr}
	mov	r6, r11
	mov	r5, r10
	push	{r5, r6}
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6}
	mov	r5, r0
	ldr	r0, [r5]
	sub	sp, #0x30
	bl	Func_2322
	mov	r10, r0
	ldr	r0, [r5]
	bl	Func_231c
	mov	r9, r0
	ldr	r0, [r5, #4]
	bl	Func_2322
	mov	r8, r0
	ldr	r0, [r5, #4]
	bl	Func_231c
	mov	r11, r0
	ldr	r0, [r5, #8]
	bl	Func_2322
	mov	r6, r0
	ldr	r0, [r5, #8]
	bl	Func_231c
	mov	r14, r0
	ldr	r3, =Func_888
	mov	r0, r11
	mov	r1, r14
	.call_via r3
	mov	r5, sp
	str	r0, [r5]
	mov	r1, r6
	mov	r0, r11
	.call_via r3
	mov	r1, r8
	neg	r2, r1
	str	r0, [r5, #4]
	str	r2, [r5, #8]
	mov	r0, r10
	.call_via r3
	mov	r1, r14
	.call_via r3
	mov	r4, r0
	mov	r1, r6
	mov	r0, r9
	.call_via r3
	sub	r4, r0
	str	r4, [r5, #0xc]
	mov	r0, r10
	mov	r1, r8
	.call_via r3
	mov	r1, r6
	.call_via r3
	mov	r4, r0
	mov	r1, r14
	mov	r0, r9
	.call_via r3
	add	r4, r0
	str	r4, [r5, #0x10]
	mov	r0, r10
	mov	r1, r11
	.call_via r3
	str	r0, [r5, #0x14]
	mov	r1, r8
	mov	r0, r9
	.call_via r3
	mov	r1, r14
	.call_via r3
	mov	r4, r0
	mov	r1, r6
	mov	r0, r10
	.call_via r3
	add	r4, r0
	str	r4, [r5, #0x18]
	mov	r0, r9
	mov	r1, r8
	.call_via r3
	mov	r1, r6
	.call_via r3
	mov	r4, r0
	mov	r1, r14
	mov	r0, r10
	.call_via r3
	sub	r4, r0
	str	r4, [r5, #0x1c]
	mov	r0, r9
	mov	r1, r11
	.call_via r3
	mov	r3, #0
	str	r0, [r5, #0x20]
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x28]
	str	r3, [r5, #0x2c]
	mov	r0, r5
	ldr	r3, =Func_a30
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r3}
	mov	r11, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_4ab0

@ RotateMatrix
@ r0 = matrix, r1 = angle. Post-multiplies the current matrix by a rotation
@ about one axis, taking the sine and cosine from Func_2322 / Func_231c.
@ Func_4bd4, Func_4c1c and Func_4c6c are the three axes; callers build a full
@ orientation by chaining them after Func_49ac.
.thumb_func_start Func_4bd4
	push	{r5, r6, lr}
	sub	sp, #0x30
	mov	r5, r0
	bl	Func_2322
	mov	r6, r0
	mov	r0, r5
	bl	Func_231c
	mov	r12, r0
	mov	r5, sp
	mov	r0, r5
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	mov	r3, r12
	str	r6, [r5, #0x14]
	neg	r6, r6
	str	r3, [r5, #0x10]
	str	r3, [r5, #0x20]
	str	r6, [r5, #0x1c]
	ldr	r3, =Func_a30
	mov	r0, r5
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_4bd4

@ RotateMatrix
@ r0 = matrix, r1 = angle. Post-multiplies the current matrix by a rotation
@ about one axis, taking the sine and cosine from Func_2322 / Func_231c.
@ Func_4bd4, Func_4c1c and Func_4c6c are the three axes; callers build a full
@ orientation by chaining them after Func_49ac.
.thumb_func_start Func_4c1c
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	sub	sp, #0x30
	mov	r5, r0
	bl	Func_2322
	mov	r8, r0
	mov	r0, r5
	bl	Func_231c
	mov	r6, r0
	mov	r5, sp
	mov	r0, r5
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	mov	r2, r8
	neg	r3, r2
	str	r3, [r5, #8]
	str	r6, [r5]
	str	r2, [r5, #0x18]
	str	r6, [r5, #0x20]
	ldr	r3, =Func_a30
	mov	r0, r5
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_4c1c

@ RotateMatrix
@ r0 = matrix, r1 = angle. Post-multiplies the current matrix by a rotation
@ about one axis, taking the sine and cosine from Func_2322 / Func_231c.
@ Func_4bd4, Func_4c1c and Func_4c6c are the three axes; callers build a full
@ orientation by chaining them after Func_49ac.
.thumb_func_start Func_4c6c
	push	{r5, r6, lr}
	sub	sp, #0x30
	mov	r5, r0
	bl	Func_2322
	mov	r6, r0
	mov	r0, r5
	bl	Func_231c
	mov	r12, r0
	mov	r5, sp
	mov	r0, r5
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	mov	r3, r12
	str	r6, [r5, #4]
	neg	r6, r6
	str	r3, [r5]
	str	r3, [r5, #0x10]
	str	r6, [r5, #0xc]
	ldr	r3, =Func_a30
	mov	r0, r5
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_4c6c

@ ScaleMatrix
@ r0.. = scale factors. Post-multiplies the current transform by a scale.
.thumb_func_start Func_4cb4
	push	{r5, r6, lr}
	sub	sp, #0x30
	mov	r6, r0
	mov	r5, sp
	mov	r0, r5
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	ldr	r3, [r6]
	str	r3, [r5, #0x24]
	ldr	r3, [r6, #4]
	str	r3, [r5, #0x28]
	ldr	r3, [r6, #8]
	mov	r0, r5
	str	r3, [r5, #0x2c]
	ldr	r3, =Func_a30
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_4cb4

@ ScaleMatrixUniform
@ r0 = scale. Func_4cb4 with the same factor on all three axes.
.thumb_func_start Func_4cf0
	push	{r5, r6, lr}
	sub	sp, #0x30
	mov	r6, r0
	mov	r5, sp
	mov	r0, r5
	mov	r1, #0x80
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	lsl	r1, #9
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	stmia	r0!, {r1, r2, r3, r4}
	ldr	r3, [r6]
	str	r3, [r5]
	ldr	r3, [r6, #4]
	str	r3, [r5, #0x10]
	ldr	r3, [r6, #8]
	mov	r0, r5
	str	r3, [r5, #0x20]
	ldr	r3, =Func_a30
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_4cf0

@ BuildLookAt
@ r0.. = parameters. Builds a view transform aiming from one point at another,
@ using Func_2322 / Func_231c for the derived angles.
.thumb_func_start Func_4d2c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r5, r0
	ldr	r0, [r5]
	mov	r7, r1
	sub	sp, #0x30
	bl	Func_2322
	mov	r10, r0
	ldr	r0, [r5]
	bl	Func_231c
	mov	r9, r0
	ldr	r0, [r5, #4]
	bl	Func_2322
	mov	r8, r0
	ldr	r0, [r5, #4]
	bl	Func_231c
	mov	r11, r0
	ldr	r0, [r5, #8]
	bl	Func_2322
	mov	r6, r0
	ldr	r0, [r5, #8]
	bl	Func_231c
	mov	r14, r0
	ldr	r3, =Func_888
	mov	r0, r11
	mov	r1, r14
	.call_via r3
	mov	r5, sp
	str	r0, [r5]
	mov	r1, r6
	mov	r0, r11
	.call_via r3
	mov	r1, r8
	neg	r2, r1
	str	r0, [r5, #4]
	str	r2, [r5, #8]
	mov	r0, r10
	.call_via r3
	mov	r1, r14
	.call_via r3
	mov	r4, r0
	mov	r1, r6
	mov	r0, r9
	.call_via r3
	sub	r4, r0
	str	r4, [r5, #0xc]
	mov	r0, r10
	mov	r1, r8
	.call_via r3
	mov	r1, r6
	.call_via r3
	mov	r4, r0
	mov	r1, r14
	mov	r0, r9
	.call_via r3
	add	r4, r0
	str	r4, [r5, #0x10]
	mov	r0, r10
	mov	r1, r11
	.call_via r3
	str	r0, [r5, #0x14]
	mov	r1, r8
	mov	r0, r9
	.call_via r3
	mov	r1, r14
	.call_via r3
	mov	r4, r0
	mov	r1, r6
	mov	r0, r10
	.call_via r3
	add	r4, r0
	str	r4, [r5, #0x18]
	mov	r0, r9
	mov	r1, r8
	.call_via r3
	mov	r1, r6
	.call_via r3
	mov	r4, r0
	mov	r1, r14
	mov	r0, r10
	.call_via r3
	sub	r4, r0
	str	r4, [r5, #0x1c]
	mov	r0, r9
	mov	r1, r11
	.call_via r3
	str	r0, [r5, #0x20]
	ldr	r3, [r7]
	str	r3, [r5, #0x24]
	ldr	r3, [r7, #4]
	str	r3, [r5, #0x28]
	ldr	r3, [r7, #8]
	mov	r0, r5
	str	r3, [r5, #0x2c]
	ldr	r3, =Func_a30
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_4d2c

@ BuildProjection
@ r0.. = parameters. Builds the projection half of the pipeline. 148 lines;
@ traced structurally.
.thumb_func_start Func_4e54
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x38
	str	r1, [sp, #4]
	str	r2, [sp]
	mov	r5, r0
	ldr	r0, [r5]
	bl	Func_2322
	mov	r10, r0
	ldr	r0, [r5]
	bl	Func_231c
	mov	r9, r0
	ldr	r0, [r5, #4]
	bl	Func_2322
	mov	r8, r0
	ldr	r0, [r5, #4]
	bl	Func_231c
	mov	r11, r0
	ldr	r0, [r5, #8]
	bl	Func_2322
	mov	r6, r0
	ldr	r0, [r5, #8]
	bl	Func_231c
	ldr	r2, [sp]
	mov	r14, r0
	ldr	r7, [r2]
	ldr	r3, =Func_888
	mov	r0, r11
	mov	r1, r14
	.call_via r3
	mov	r1, r0
	mov	r0, r7
	.call_via r3
	add	r5, sp, #8
	str	r0, [r5]
	mov	r1, r6
	mov	r0, r11
	.call_via r3
	mov	r1, r0
	mov	r0, r7
	.call_via r3
	mov	r2, r8
	str	r0, [r5, #4]
	neg	r1, r2
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #8]
	ldr	r2, [sp]
	mov	r0, r10
	ldr	r7, [r2, #4]
	mov	r1, r8
	.call_via r3
	mov	r1, r14
	.call_via r3
	mov	r4, r0
	mov	r1, r6
	mov	r0, r9
	.call_via r3
	sub	r4, r0
	mov	r1, r4
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #0xc]
	mov	r1, r8
	mov	r0, r10
	.call_via r3
	mov	r1, r6
	.call_via r3
	mov	r4, r0
	mov	r1, r14
	mov	r0, r9
	.call_via r3
	add	r4, r0
	mov	r1, r4
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #0x10]
	mov	r1, r11
	mov	r0, r10
	.call_via r3
	mov	r1, r0
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #0x14]
	ldr	r2, [sp]
	mov	r0, r9
	ldr	r7, [r2, #8]
	mov	r1, r8
	.call_via r3
	mov	r1, r14
	.call_via r3
	mov	r4, r0
	mov	r1, r6
	mov	r0, r10
	.call_via r3
	add	r4, r0
	mov	r1, r4
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #0x18]
	mov	r1, r8
	mov	r0, r9
	.call_via r3
	mov	r1, r6
	.call_via r3
	mov	r4, r0
	mov	r1, r14
	mov	r0, r10
	.call_via r3
	sub	r4, r0
	mov	r1, r4
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #0x1c]
	mov	r1, r11
	mov	r0, r9
	.call_via r3
	mov	r1, r0
	mov	r0, r7
	.call_via r3
	str	r0, [r5, #0x20]
	ldr	r2, [sp, #4]
	ldr	r3, [r2]
	str	r3, [r5, #0x24]
	ldr	r3, [r2, #4]
	str	r3, [r5, #0x28]
	ldr	r3, [r2, #8]
	mov	r0, r5
	str	r3, [r5, #0x2c]
	ldr	r3, =Func_a30
	bl	_call_via_r3
	add	sp, #0x38
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_4e54

@ TransformVectorBatch
@ r0 = source vectors, r1 = destination, r2 = count. Applies the current
@ transform to a run of vectors, scaling through Func_45d4. 206 lines; traced
@ structurally.
.thumb_func_start Func_4fe4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x40
	str	r0, [sp, #0x20]
	mov	r0, sp
	add	r0, #0x24
	str	r0, [sp, #0xc]
	mov	r4, r1
	mov	r9, r2
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =.L7994
	ldr	r1, [sp, #0xc]
	ldr	r2, =0x84000007
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r1, [sp, #0x20]
	ldr	r2, [r4]
	ldr	r3, [r1]
	sub	r2, r3
	str	r2, [sp, #0x1c]
	ldr	r3, [r1, #4]
	ldr	r2, [r4, #4]
	sub	r2, r3
	str	r2, [sp, #0x18]
	ldr	r3, [r1, #8]
	ldr	r2, [r4, #8]
	sub	r2, r3
	str	r2, [sp, #0x14]
	ldr	r4, [sp, #0x18]
	ldr	r2, [sp, #0x1c]
	ldr	r0, [sp, #0x14]
	asr	r1, r2, #8
	asr	r3, r4, #8
	asr	r2, r0, #8
	str	r2, [sp]
	str	r2, [sp, #4]
	ldr	r4, [sp, #0xc]
	mov	r2, r3
	mov	r0, r1
	bl	_call_via_r4
	ldr	r3, =Func_948
	bl	_call_via_r3
	mov	r1, r0
	ldr	r0, =Func_b60
	mov	r11, r0
	mov	r0, #0x80
	lsl	r0, #24
	bl	_call_via_r11
	lsr	r3, r0, #15
	neg	r3, r3
	ldr	r5, =Func_888
	ldr	r0, [sp, #0x1c]
	mov	r1, r3
	.call_via r5
	str	r0, [sp, #0x1c]
	mov	r1, r3
	ldr	r0, [sp, #0x18]
	.call_via r5
	str	r0, [sp, #0x18]
	mov	r1, r3
	ldr	r0, [sp, #0x14]
	.call_via r5
	ldr	r1, [sp, #0x1c]
	neg	r1, r1
	mov	r8, r1
	str	r0, [sp, #0x14]
	ldr	r0, [sp, #0x18]
	mov	r1, r0
	.call_via r5
	mov	r3, #0x80
	lsl	r3, #9
	sub	r0, r3, r0
	cmp	r0, #0
	ble	.L50a6
	bl	Func_45d4
	mov	r1, r0
	mov	r0, #0x80
	lsl	r0, #24
	bl	_call_via_r11
	lsl	r3, r0, #1
.L50a6:
	ldr	r0, [sp, #0x14]
	mov	r1, r3
	.call_via r5
	str	r0, [sp, #0x10]
	mov	r1, r3
	mov	r0, r8
	.call_via r5
	mov	r8, r0
	mov	r1, r8
	ldr	r0, [sp, #0x18]
	.call_via r5
	mov	r10, r0
	ldr	r1, [sp, #0x10]
	ldr	r0, [sp, #0x14]
	.call_via r5
	mov	r3, r0
	mov	r1, r8
	ldr	r0, [sp, #0x1c]
	.call_via r5
	sub	r6, r3, r0
	ldr	r1, [sp, #0x10]
	ldr	r0, [sp, #0x18]
	.call_via r5
	neg	r7, r0
	mov	r3, r6
	str	r7, [sp]
	str	r7, [sp, #4]
	mov	r1, r10
	mov	r2, r6
	ldr	r4, [sp, #0xc]
	mov	r0, r10
	bl	_call_via_r4
	bl	Func_45d4
	mov	r1, r0
	mov	r0, #0x80
	lsl	r0, #24
	bl	_call_via_r11
	lsl	r3, r0, #1
	mov	r1, r3
	mov	r0, r10
	.call_via r5
	mov	r10, r0
	mov	r1, r3
	mov	r0, r6
	.call_via r5
	mov	r6, r0
	mov	r1, r3
	mov	r0, r7
	.call_via r5
	ldr	r1, [sp, #0x20]
	mov	r7, r0
	ldr	r1, [r1, #4]
	ldr	r0, [sp, #0x20]
	ldr	r2, [sp, #0x20]
	ldr	r0, [r0]
	str	r1, [sp, #8]
	ldr	r3, [sp, #0x10]
	ldr	r5, [r2, #8]
	mov	r11, r0
	mov	r4, r9
	mov	r0, #0
	mov	r1, r8
	str	r3, [r4]
	str	r0, [r4, #0xc]
	str	r1, [r4, #0x18]
	ldr	r1, [sp, #0x10]
	str	r0, [sp]
	str	r0, [sp, #4]
	ldr	r4, [sp, #0xc]
	mov	r2, r5
	mov	r3, r8
	mov	r0, r11
	bl	_call_via_r4
	mov	r1, r9
	mov	r2, r10
	neg	r0, r0
	str	r0, [r1, #0x24]
	str	r6, [r1, #0x10]
	str	r7, [r1, #0x1c]
	str	r2, [r1, #4]
	ldr	r2, [sp, #8]
	str	r5, [sp]
	str	r7, [sp, #4]
	ldr	r4, [sp, #0xc]
	mov	r1, r10
	mov	r3, r6
	mov	r0, r11
	bl	_call_via_r4
	mov	r1, r9
	neg	r0, r0
	str	r0, [r1, #0x28]
	ldr	r2, [sp, #0x1c]
	str	r2, [r1, #8]
	ldr	r3, [sp, #0x18]
	str	r3, [r1, #0x14]
	ldr	r4, [sp, #0x14]
	str	r4, [r1, #0x20]
	ldr	r1, [sp, #0x1c]
	str	r5, [sp]
	str	r4, [sp, #4]
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #0x18]
	ldr	r4, [sp, #0xc]
	mov	r0, r11
	bl	_call_via_r4
	mov	r1, r9
	neg	r0, r0
	str	r0, [r1, #0x2c]
	add	sp, #0x40
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_4fe4

@ TransformVector
@ r0 = vector, r1 = destination. Func_4fe4 for a single vector.
.thumb_func_start Func_51d8
	push	{lr}
	ldr	r2, =Data_ac0
	bl	Func_4fe4
	pop	{r0}
	bx	r0
.func_end Func_51d8

@ TransformVectorInPlace
@ r0 = vector. Func_4fe4 writing the result back over the source.
.thumb_func_start Func_51e8
	push	{r5, lr}
	sub	sp, #0x30
	mov	r5, sp
	mov	r2, r5
	bl	Func_4fe4
	ldr	r3, =Func_a30
	mov	r0, r5
	bl	_call_via_r3
	add	sp, #0x30
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_51e8

	.section .rodata

.L7994:
	.incrom 0x7994, 0x79b0
