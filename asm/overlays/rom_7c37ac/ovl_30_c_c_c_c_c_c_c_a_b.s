	.include "macros.inc"

@ 29 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TurnSlotToAngle x4, DialogueWait
.thumb_func_start OvlFunc_938_200940c
	push	{r5, lr}
	mov	r1, #0xc0
	mov	r5, r0
	lsl	r1, #7
	mov	r0, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #2
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #3
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	cmp	r5, #0
	beq	.L144a
	mov	r0, r5
	bl	__CutsceneWait
.L144a:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_938_200940c
