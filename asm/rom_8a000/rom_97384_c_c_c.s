	.include "macros.inc"
	.include "gba.inc"

@ SetEncounterDisplayRegisters
@ Takes no arguments. Puts the display into encounter-transition state: sets the
@ active flag at [iwram_1e8c]+0xEA4, and writes 0x739C into the two window
@ registers at 0x50001E2 and 0x50001E6.
.thumb_func_start Func_8097a7c  @ 0x08097a7c
	push	{lr}
	ldr	r3, =iwram_3001e8c
	ldr	r2, =0xea4
	ldr	r3, [r3]
	add	r3, r2
	mov	r2, #1
	strb	r2, [r3]
	ldr	r2, .L97ac4	@ 0x739c
	ldr	r3, =0x50001e2
	strh	r2, [r3]
	add	r3, #4
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r2, [r3]
	mov	r1, #0x90
	add	r3, #2
	strh	r2, [r3]
	lsl	r1, #3
	ldr	r0, =Func_8097868
	bl	StartTask
	b	.L97ad8

	.align	2, 0
.L97ac4:
	.word	0x739c
	.pool

.L97ad8:
	pop	{r0}
	bx	r0
.func_end Func_8097a7c

@ RestoreDisplayRegisters
@ Takes no arguments. Undoes Func_97a7c: unregisters the flash task Func_97868,
@ restores the window registers at 0x50001E2 / 0x50001E6 to 0x7FFF and 0, and
@ resets the blend state so the field renders normally again.
.thumb_func_start Func_8097adc  @ 0x08097adc
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e8c
	ldr	r0, =Func_8097868
	ldr	r5, [r3]
	bl	StopTask
	ldr	r2, =0x50001e2
	ldr	r3, .L97b1c	@ 0x7fff
	ldr	r6, .L97b20	@ 0
	strh	r3, [r2]
	ldr	r3, =0x50001e6
	strh	r6, [r3]
	ldr	r3, .L97b24	@ 0x294a
	add	r2, #0x14
	strh	r3, [r2]
	ldr	r3, .L97b28	@ 0x5294
	add	r2, #2
	strh	r3, [r2]
	ldr	r1, =0x205
	ldr	r3, =gState
	add	r2, r3, r1
	ldrb	r0, [r2]
	ldr	r2, =0x206
	add	r3, r2
	ldrb	r1, [r3]
	bl	_SetUIColor
	ldr	r3, =0xea4
	add	r5, r3
	strb	r6, [r5]
	b	.L97b4c

	.align	2, 0
.L97b1c:
	.word	0x7fff
.L97b20:
	.word	0
.L97b24:
	.word	0x294a
.L97b28:
	.word	0x5294
	.pool

.L97b4c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8097adc

	.section .rodata
	.global .La0108
	.global .La0128

.La0108:
	.incrom 0xa0108, 0xa0128
.La0128:
	.incrom 0xa0128, 0xa012c
