	.include "macros.inc"
	.include "gba.inc"

@ 24 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetActorPartsPalette
.thumb_func_start OvlFunc_968_2008558
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r3, [r3]
	ldr	r7, =0x212
	mov	r5, r3
	mov	r6, #8
	add	r5, #0x34
.L566:
	ldmia	r5!, {r0}
	mov	r3, r0
	add	r3, #0x64
	ldrh	r2, [r3]
	lsl	r3, r2, #16
	asr	r3, #20
	cmp	r3, r7
	bne	.L57e
	mov	r1, #0xf
	and	r1, r2
	bl	__Func_80929d8
.L57e:
	add	r6, #1
	cmp	r6, #0x41
	bls	.L566
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_968_2008558
