	.include "macros.inc"
	.include "gba.inc"

@ Talk: line 0xe39, shown.
.thumb_func_start OvlFunc_951_2008074
	push	{r5, r6, lr}
	ldr	r5, =0xe39
	mov	r6, r0
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, r6
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.La2
	mov	r0, #0xa
	bl	__CutsceneWait
	add	r0, r5, #1
	bl	__MessageID
	b	.La8
.La2:
	add	r0, r5, #2
	bl	__MessageID
.La8:
	mov	r0, r6
	mov	r1, #0
	bl	__ActorMessage
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_951_2008074

@ Talk: line 0xe19, shown.
.thumb_func_start OvlFunc_951_20080bc
	push	{r5, r6, lr}
	ldr	r5, =0xe19
	mov	r6, r0
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, r6
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.Lea
	mov	r0, #0xa
	bl	__CutsceneWait
	add	r0, r5, #1
	bl	__MessageID
	b	.Lf0
.Lea:
	add	r0, r5, #2
	bl	__MessageID
.Lf0:
	mov	r0, r6
	mov	r1, #0
	bl	__ActorMessage
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_951_20080bc
