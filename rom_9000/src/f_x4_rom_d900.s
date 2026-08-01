	.include "macros.inc"
	.include "gba.inc"

@ ScriptOp_CallF9080
@ Script opcode handler. r0=entity. Passes the operand at script[cursor+1] to
@ the overlay export _Func_f9080 (rom_f9000), then advances the cursor by 2 and
@ returns 1. The entity itself is not forwarded, so the operand is a global id
@ rather than anything entity-relative. Rename once _Func_f9080 is identified.
.thumb_func_start Func_d900
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r0, [r3, #4]
	bl	_Func_f9080
	ldrh	r3, [r5, #4]
	add	r3, #2
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d900

	.section .rodata

@ .L13240 -- the 5-word default/idle entity script installed by Func_d7e8.
@ PROMOTED TO .global: Func_d7e8 now lives in f_x1_rom_d7e8.c and has to reach
@ this across a file boundary. The local .L13240 is kept so nothing else in the
@ original disassembly has to change; L13240 is an alias at the same address.
	.global	L13240
L13240:
.L13240:
	.incrom 0x13240, 0x13254
