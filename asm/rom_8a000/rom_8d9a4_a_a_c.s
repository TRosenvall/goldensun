	.include "macros.inc"
	.include "gba.inc"

@ SpawnMapObject
@ r0=object record. Instantiates one map object: creates its entity, applies its
@ sprite and script and binds it to a scene slot. The ~130-instruction body is
@ characterised structurally.
.thumb_func_start Func_808ddec  @ 0x0808ddec
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r1, #1
	sub	sp, #4
	neg	r1, r1
	mov	r2, #0x20
	mov	r9, r0
	str	r1, [sp]
	mov	r11, r2
	bl	GetFieldActor
	mov	r7, r0
	cmp	r7, #0
	beq	.L8def2
	mov	r3, #0
	mov	r10, r3
.L8de16:
	cmp	r10, r9
	beq	.L8dee8
	mov	r0, r10
	bl	GetFieldActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L8dee8
	mov	r1, #0x59
	add	r1, r6
	ldrb	r2, [r1]
	mov	r3, #8
	and	r3, r2
	mov	r8, r1
	cmp	r3, #0
	bne	.L8dee8
	ldr	r4, [r6, #0xc]
	ldr	r1, [r7, #0xc]
	sub	r3, r4, r1
	cmp	r3, #0
	blt	.L8de48
	ldr	r2, =0x2fffff
	cmp	r3, r2
	ble	.L8de50
	b	.L8dee8
.L8de48:
	ldr	r2, =0x2fffff
	sub	r3, r1, r4
	cmp	r3, r2
	bgt	.L8dee8
.L8de50:
	ldr	r2, [r6, #8]
	ldr	r3, [r7, #8]
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L8de5e
	ldr	r3, =0xffff
	add	r0, r3
.L8de5e:
	sub	r2, r4, r1
	asr	r0, #16
	cmp	r2, #0
	bge	.L8de6a
	ldr	r1, =0xffff
	add	r2, r1
.L8de6a:
	asr	r1, r2, #16
	ldr	r3, [r7, #0x10]
	ldr	r2, [r6, #0x10]
	sub	r2, r3
	cmp	r2, #0
	bge	.L8de7a
	ldr	r3, =0xffff
	add	r2, r3
.L8de7a:
	asr	r3, r2, #16
	mov	r2, r0
	mul	r2, r0
	mov	r0, r2
	mov	r2, r1
	mul	r2, r1
	mov	r1, r3
	mul	r1, r3
	add	r0, r2
	mov	r3, r1
	add	r0, r3
	ldr	r3, =Func_8000948
	bl	_call_via_r3
	mov	r3, r8
	ldrb	r2, [r3]
	mov	r3, #4
	and	r3, r2
	mov	r5, r0
	cmp	r3, #0
	beq	.L8deb2
	lsl	r0, r5, #2
	add	r0, r5
	lsl	r0, #1
	mov	r1, #0xd
	bl	__divsi3
	mov	r5, r0
.L8deb2:
	cmp	r5, r11
	bge	.L8dee8
	ldr	r3, [r7, #0x10]
	ldr	r0, [r6, #0x10]
	ldr	r1, [r6, #8]
	sub	r0, r3
	ldr	r3, [r7, #8]
	sub	r1, r3
	bl	atan2
	lsl	r0, #16
	lsr	r0, #16
	cmp	r5, #0xb
	ble	.L8dee2
	ldrh	r3, [r7, #6]
	sub	r3, r0, r3
	lsl	r3, #16
	ldr	r1, =0xffffd001
	asr	r0, r3, #16
	cmp	r0, r1
	blt	.L8dee8
	ldr	r2, =0x2fff
	cmp	r0, r2
	bgt	.L8dee8
.L8dee2:
	mov	r3, r10
	str	r3, [sp]
	mov	r11, r5
.L8dee8:
	mov	r1, #1
	add	r10, r1
	mov	r2, r10
	cmp	r2, #0x42
	ble	.L8de16
.L8def2:
	ldr	r0, [sp]
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_808ddec

@ UpdateMapObjects
@ Takes no arguments. Per-frame pass over the spawned map objects, applying
@ their state changes. The ~150-instruction body is characterised
@ structurally.
.thumb_func_start Func_808df1c  @ 0x0808df1c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xc
	mov	r8, r1
	mov	r1, #1
	str	r0, [sp, #8]
	neg	r1, r1
	mov	r0, r8
	str	r1, [sp, #4]
	bl	Func_808ddb8
	str	r0, [sp]
	ldr	r0, [sp, #8]
	bl	GetFieldActor
	mov	r7, r0
	cmp	r7, #0
	bne	.L8df4c
	b	.L8e05c
.L8df4c:
	ldrh	r3, [r7, #6]
	mov	r2, #0x80
	lsl	r2, #6
	add	r2, r3
	mov	r3, #0xc0
	lsl	r3, #8
	and	r2, r3
	mov	r3, #0
	mov	r11, r2
	mov	r9, r3
.L8df60:
	ldr	r1, [sp, #8]
	cmp	r9, r1
	beq	.L8e052
	mov	r0, r9
	bl	GetFieldActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L8e052
	mov	r2, #0x59
	add	r2, r6
	mov	r10, r2
	ldrb	r2, [r2]
	mov	r3, #8
	and	r3, r2
	cmp	r3, #0
	bne	.L8e052
	mov	r0, #0x80
	mov	r3, r8
	lsl	r0, #12
	cmp	r3, #0xd
	bne	.L8df90
	mov	r0, #0xc0
	lsl	r0, #14
.L8df90:
	mov	r1, r8
	cmp	r1, #5
	bne	.L8df9a
	mov	r0, #0x80
	lsl	r0, #15
.L8df9a:
	mov	r2, r8
	cmp	r2, #2
	bne	.L8dfa4
	mov	r0, #0x80
	lsl	r0, #13
.L8dfa4:
	ldr	r1, [r6, #0xc]
	ldr	r3, [r7, #0xc]
	sub	r2, r1, r3
	cmp	r2, #0
	blt	.L8dfb4
	cmp	r2, r0
	ble	.L8dfba
	b	.L8e052
.L8dfb4:
	sub	r3, r1
	cmp	r3, r0
	bgt	.L8e052
.L8dfba:
	ldr	r2, [r6, #8]
	ldr	r3, [r7, #8]
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L8dfc8
	ldr	r3, =0xffff
	add	r0, r3
.L8dfc8:
	ldr	r2, [r6, #0x10]
	ldr	r3, [r7, #0x10]
	sub	r2, r3
	asr	r0, #16
	cmp	r2, #0
	bge	.L8dfd8
	ldr	r1, =0xffff
	add	r2, r1
.L8dfd8:
	asr	r3, r2, #16
	mov	r1, r3
	mul	r1, r3
	mov	r2, r0
	mul	r2, r0
	mov	r3, r1
	mov	r0, r2
	add	r0, r3
	ldr	r3, =Func_8000948
	bl	_call_via_r3
	mov	r3, r10
	ldrb	r2, [r3]
	mov	r3, #0x10
	and	r3, r2
	mov	r5, r0
	cmp	r3, #0
	beq	.L8e006
	lsl	r0, r5, #1
	mov	r1, #3
	bl	__divsi3
	mov	r5, r0
.L8e006:
	ldr	r1, [sp]
	cmp	r5, r1
	bge	.L8e052
	ldr	r3, [r7, #0x10]
	ldr	r0, [r6, #0x10]
	ldr	r1, [r6, #8]
	sub	r0, r3
	ldr	r3, [r7, #8]
	sub	r1, r3
	bl	atan2
	mov	r2, #0xc0
	lsl	r0, #16
	lsr	r0, #16
	lsl	r2, #5
	cmp	r5, #0x13
	ble	.L8e02c
	mov	r2, #0x80
	lsl	r2, #5
.L8e02c:
	mov	r3, r8
	cmp	r3, #2
	bne	.L8e036
	mov	r2, #0x80
	lsl	r2, #6
.L8e036:
	cmp	r5, #0xb
	ble	.L8e04c
	mov	r1, r11
	sub	r3, r0, r1
	lsl	r3, #16
	asr	r0, r3, #16
	cmp	r0, #0
	bge	.L8e048
	neg	r0, r0
.L8e048:
	cmp	r0, r2
	bge	.L8e052
.L8e04c:
	mov	r2, r9
	str	r2, [sp, #4]
	str	r5, [sp]
.L8e052:
	mov	r3, #1
	add	r9, r3
	mov	r1, r9
	cmp	r1, #0x42
	ble	.L8df60
.L8e05c:
	ldr	r0, [sp, #4]
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_808df1c
