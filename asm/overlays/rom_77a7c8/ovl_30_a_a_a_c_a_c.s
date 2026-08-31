	.include "macros.inc"
	.include "gba.inc"

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
