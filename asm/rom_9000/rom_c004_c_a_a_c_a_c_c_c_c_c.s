	.include "macros.inc"
	.include "gba.inc"

@ StartBehaviour_135f0
@ r0=entity, r1=parameter. Installs behaviour script .L135f0. When the
@ parameter is non-zero it also overrides the movement tuning for this script:
@ acceleration +0x34 = 0x8000 (half speed), max speed +0x30 = 0x40000, the
@ script argument at +0x68 = r1, and the spawn-tile halfword at +0x64 is reset.
.thumb_func_start Camera_SetTarget  @ 0x0800c4bc
	push	{r5, r6, lr}
	mov	r6, r1
	ldr	r1, =.L135f0
	mov	r5, r0
	bl	Actor_SetScript
	cmp	r6, #0
	beq	.Lc4e2
	mov	r3, #0x80
	lsl	r3, #8
	str	r3, [r5, #0x34]
	mov	r3, #0x80
	lsl	r3, #11
	mov	r2, r5
	str	r3, [r5, #0x30]
	add	r2, #0x64
	mov	r3, #0
	str	r6, [r5, #0x68]
	strh	r3, [r2]
.Lc4e2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Camera_SetTarget

