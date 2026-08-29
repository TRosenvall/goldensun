	.include "macros.inc"
	.include "gba.inc"

@ 28 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SignedDiv, LoadMapByName
.thumb_func_start OvlFunc_881_200808c
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r5, =gState
	mov	r2, #0x8e
	lsl	r2, #2
	ldr	r6, [r3]
	add	r5, r2
	sub	r2, #0x8c
	add	r3, r6, r2
	ldr	r3, [r3]
	lsl	r0, r3, #3
	add	r0, r3
	mov	r1, #0xa
	bl	_divsi3_RAM
	ldr	r3, [r5]
	cmp	r3, r0
	blt	.Lc2
	ldr	r0, =0x809
	mov	r1, #0x2a
	bl	__Func_8091f14
	mov	r3, #0xd4
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #0
	str	r3, [r2]
.Lc2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_200808c

@ 28 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SignedDiv, LoadMapByName
.thumb_func_start OvlFunc_881_20080d4
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r5, =gState
	mov	r2, #0x8e
	lsl	r2, #2
	ldr	r6, [r3]
	add	r5, r2
	sub	r2, #0x8c
	add	r3, r6, r2
	ldr	r3, [r3]
	lsl	r0, r3, #3
	add	r0, r3
	mov	r1, #0xa
	bl	_divsi3_RAM
	ldr	r3, [r5]
	cmp	r3, r0
	blt	.L10a
	ldr	r0, =0x80a
	mov	r1, #0x18
	bl	__Func_8091f14
	mov	r3, #0xd4
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #0
	str	r3, [r2]
.L10a:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_20080d4

@ 14 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   DestroyEntity
.thumb_func_start OvlFunc_881_200811c
	push	{lr}
	mov	r2, r0
	add	r2, #0x64
	mov	r4, #0
	ldrsh	r3, [r2, r4]
	ldrh	r1, [r2]
	cmp	r3, #0
	bgt	.L132
	add	r3, r1, #1
	strh	r3, [r2]
	b	.L136
.L132:
	bl	__DeleteActor
.L136:
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_200811c
