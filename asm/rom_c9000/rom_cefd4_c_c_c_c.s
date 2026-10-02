	.include "macros.inc"
	.include "gba.inc"

	.section .rodata
@ Exported for the BaseAnim_Spasm text/data split (batch 318): the function
@ moves to C and these three blobs stay in asm, so the C file references them
@ by name.  A .global emits no bytes -- this change is byte-neutral and was
@ gated on its own before the split.  They live HERE rather than in the
@ preamble because split_s.py copies the preamble into EVERY part and
@ correctly refuses when it holds anything but includes and comments.
	.global .Lee096
	.global .Lee09c
	.global .Lee09f

.Lee096:
	.incrom 0xee096, 0xee09c
.Lee09c:
	.incrom 0xee09c, 0xee09f
.Lee09f:
	.incrom 0xee09f, 0xee0a2
