	.include "macros.inc"

@ .rodata rehomed out of rom_b0070_c_c_c_c.s in batch 284 so InnHeal and Func_80b386c
@ could be converted to C.  Fifteen .L-labelled .incrom blocks, all ALREADY .global in
@ the original, so no export had to be added.  Byte-identical content; only the object it
@ lives in changed, and stage1.ld's .rodata line for this stem was repointed to match.
	.section .rodata
	.global .Lb4ab2
	.global .Lb4ab6
	.global .Lb41ac
	.global .Lb3940
	.global .Lb39c0
	.global .Lb3a40
	.global .Lb3ac0
	.global .Lb3b40
	.global .Lb3bc0
	.global .Lb3d40
	.global .Lb3e80
	.global .Lb3f80
	.global .Lb4100
	.global .Lb413c
	.global .Lb4146

.Lb3940:
	.incrom 0xb3940, 0xb39c0
.Lb39c0:
	.incrom 0xb39c0, 0xb3a40
.Lb3a40:
	.incrom 0xb3a40, 0xb3ac0
.Lb3ac0:
	.incrom 0xb3ac0, 0xb3b40
.Lb3b40:
	.incrom 0xb3b40, 0xb3bc0
.Lb3bc0:
	.incrom 0xb3bc0, 0xb3d40
.Lb3d40:
	.incrom 0xb3d40, 0xb3e80
.Lb3e80:
	.incrom 0xb3e80, 0xb3f80
.Lb3f80:
	.incrom 0xb3f80, 0xb4100
.Lb4100:
	.incrom 0xb4100, 0xb413c
.Lb413c:
	.incrom 0xb413c, 0xb4146
.Lb4146:
	.incrom 0xb4146, 0xb41ac
.Lb41ac:
	.incrom 0xb41ac, 0xb4ab2
.Lb4ab2:
	.incrom 0xb4ab2, 0xb4ab6
.Lb4ab6:
	.incrom 0xb4ab6, 0xb4ac2
