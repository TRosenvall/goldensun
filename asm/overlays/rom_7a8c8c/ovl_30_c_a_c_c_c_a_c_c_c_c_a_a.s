	.include "macros.inc"

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SpawnEntity, SetEntityActorOptions, SetActorPartsPalette
.thumb_func_start OvlFunc_922_2008ed8
	push	{r5, r6, lr}
	mov	r4, r0
	mov	r5, r1
	mov	r6, r2
	mov	r0, r3
	mov	r2, r5
	mov	r1, r4
	mov	r3, r6
	bl	__CreateActor
	mov	r5, r0
	cmp	r5, #0
	beq	.Lf28
	ldr	r1, [r5, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, #4
	orr	r3, r2
	mov	r2, r5
	add	r2, #0x55
	strb	r3, [r1, #9]
	mov	r3, #0
	strb	r3, [r2]
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, r5
	mov	r1, #0xf
	bl	__Func_80929d8
	mov	r1, r5
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r1]
	mov	r0, r5
	b	.Lf2a
.Lf28:
	mov	r0, #0
.Lf2a:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_922_2008ed8
