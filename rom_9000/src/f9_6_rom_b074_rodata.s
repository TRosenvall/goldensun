	.include "macros.inc"

	.section .rodata

@ rodata_12fa0 -- font resource directory. 4-byte entries of {u16 id, u16 size}
@ scanned by Func_b6b8 (f9_5_rom_b6b8.s) to locate a glyph resource by id; a
@ zero id terminates the table. Originally a file-local symbol inside
@ rom_b074.s; promoted to .global when split into this file so Func_b6b8 can
@ still reach it.
.global rodata_12fa0
rodata_12fa0:
.incrom 0x12fa0, 0x1307c
