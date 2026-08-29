	.include "macros.inc"

.thumb_func_start OvlFunc_959_200a52c
	push	{lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2542
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #2
	bl	__MapActor_SetPos
.L2542:
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2556
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #3
	bl	__MapActor_SetPos
.L2556:
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L256a
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__MapActor_SetPos
.L256a:
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #2
	ldr	r1, =0xb333
	ldr	r2, =0x5999
	bl	__MapActor_SetSpeed
	mov	r1, #0xe4
	mov	r0, #2
	lsl	r1, #1
	mov	r2, #0xc0
	bl	__Func_809218c
	mov	r0, #3
	ldr	r1, =0xb333
	ldr	r2, =0x5999
	bl	__MapActor_SetSpeed
	mov	r1, #0xdc
	mov	r0, #3
	lsl	r1, #1
	mov	r2, #0xb8
	bl	__Func_809218c
	mov	r0, #1
	ldr	r1, =0xb333
	ldr	r2, =0x5999
	bl	__MapActor_SetSpeed
	mov	r1, #0xe0
	lsl	r1, #1
	mov	r2, #0xf0
	mov	r0, #1
	bl	__Func_809218c
	mov	r0, #2
	bl	__MapActor_WaitMovement
	mov	r1, #0xc
	mov	r2, #0
	mov	r0, #2
	bl	__Func_809280c
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r0, #3
	bl	__MapActor_WaitMovement
	mov	r0, #1
	mov	r1, #0xc
	mov	r2, #0
	bl	__Func_809280c
	mov	r0, #3
	mov	r1, #0xc
	mov	r2, #0
	bl	__Func_809280c
	mov	r0, #0xf
	bl	__CutsceneWait
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_200a52c
