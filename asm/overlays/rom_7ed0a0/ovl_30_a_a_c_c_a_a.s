	.include "macros.inc"

@ 67 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetEntityAnimation x2, Random x2, OvlFunc_ae8
.thumb_func_start OvlFunc_964_2008f4c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #2
	and	r3, r2
	sub	sp, #0x38
	mov	r8, r0
	cmp	r3, #0
	beq	.Lf6a
	mov	r1, #1
	bl	__Actor_SetAnim
	b	.Lf72
.Lf6a:
	mov	r0, r8
	mov	r1, #2
	bl	__Actor_SetAnim
.Lf72:
	ldr	r3, =iwram_3001e40
	ldr	r7, [r3]
	mov	r3, #3
	and	r7, r3
	mov	r0, #0
	cmp	r7, #0
	bne	.Lfd0
	ldr	r3, =0x4ccc
	add	r6, sp, #0x10
	str	r3, [r6, #8]
	str	r3, [r6, #0xc]
	mov	r3, #5
	str	r3, [r6, #4]
	bl	__Random
	lsl	r3, r0, #3
	sub	r3, r0
	mov	r1, r8
	lsr	r3, #16
	ldr	r5, [r1, #8]
	sub	r3, #3
	lsl	r3, #16
	add	r5, r3
	bl	__Random
	lsl	r3, r0, #3
	sub	r3, r0
	mov	r1, r8
	lsr	r3, #16
	ldr	r2, [r1, #0x10]
	sub	r3, #3
	lsl	r3, #16
	ldr	r1, [r1, #0xc]
	add	r2, r3
	mov	r3, #0x80
	lsl	r3, #13
	add	r1, r3
	ldr	r3, =0x90001
	mov	r0, r5
	str	r3, [sp, #8]
	mov	r3, #0
	str	r7, [sp]
	str	r7, [sp, #4]
	str	r6, [sp, #0xc]
	bl	OvlFunc_964_2008ae8
	mov	r0, #0
.Lfd0:
	add	sp, #0x38
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_964_2008f4c

@ Adjusts slot 0 (the player), 0x8 directly.
@ Takes the entity with Func_92054 and writes its fields in place rather
@ than going through the slot helpers -- touches +0x8, +0xc, +0x10.
.thumb_func_start OvlFunc_964_2008fe8
	push	{r5, lr}
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r2, =0xffd00000
	ldr	r3, [r0, #0xc]
	cmp	r3, r2
	ble	.L1020
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r3, #0xa
	bne	.L1020
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	str	r3, [r5, #8]
	ldr	r3, =0xffe00000
	mov	r0, #8
	str	r3, [r5, #0xc]
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	b	.L1026
.L1020:
	mov	r3, #0
	str	r3, [r5, #8]
	str	r3, [r5, #0xc]
.L1026:
	str	r3, [r5, #0x10]
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_964_2008fe8

@ WaitForField
@ r0 = entity. Yields one frame at a time with WaitFrames(1) until the
@ watched field settles, giving up after 0x3C (one second) frames so a stuck entity cannot
@ hang the caller. The same shape recurs across several overlays.
.thumb_func_start OvlFunc_964_2009038
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, #0x3c
.L103e:
	cmp	r6, #0
	beq	.L1054
	mov	r0, #1
	bl	__WaitFrames
	ldr	r3, [r5, #0xc]
	ldr	r2, [r5, #0x14]
	sub	r6, #1
	cmp	r3, r2
	bgt	.L103e
	b	.L1056
.L1054:
	ldr	r2, [r5, #0x14]
.L1056:
	mov	r3, #0
	str	r3, [r5, #0x28]
	mov	r3, #0x80
	lsl	r3, #24
	str	r2, [r5, #0xc]
	str	r3, [r5, #0x3c]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_964_2009038
