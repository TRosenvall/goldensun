	.include "macros.inc"

@ GetCharacterBaseRecord
@ r0 = character index. Returns .L844ec + index * 0xB4.
@ The table runs 0x844EC..0x84A8C, which is exactly EIGHT entries of 0xB4 bytes
@ -- one per playable character, matching the eight party membership flags
@ Func_795fc counts. There is no bounds check, so the index must already be
@ 0..7.
.thumb_func_start Func_78ed8
	mov	r3, #0xb4
	mul	r0, r3
	ldr	r2, =.L844ec
	add	r0, r2
	bx	lr
.func_end Func_78ed8

	.section .rodata

.L844ec:
	.incrom 0x844ec, 0x84a8c
