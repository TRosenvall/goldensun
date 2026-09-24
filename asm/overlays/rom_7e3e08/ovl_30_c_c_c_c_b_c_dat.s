	.include "macros.inc"

@ Data rehomed out of ovl_30_c_c_c_c_b_c.s in batch 284 so OvlFunc_957_200bad4 could be
@ converted to C.  2348 bytes across 21 .incbin blocks -- fifteen dot-labelled tables plus
@ gOvl_0200bf70, gOvl_0200c138 and four gScript_957__* behaviour scripts, far past
@ docs/elevation.md's "a handful of readable words" allowance, so rehomed rather than
@ emitted from C.  Every label was ALREADY .global in the original, so no export had to be
@ added.  Byte-identical content; only the object it lives in changed, and overlay.ld's
@ .data line was repointed to match.
	.section .data
	.global .L3f6c
	.global .L45e0
	.global .L4688
	.global .L4724
	.global .L476c
	.global .L4808
	.global .L4850
	.global .L4198
	.global .L41b0
	.global .L4270
	.global .L4318
	.global .L4468
	.global .L3eb4
	.global .L3ef4
	.global .L3f0c
	.global gOvl_0200bf70

.L3eb4:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x3eb4, (0x3ef4-0x3eb4)
.L3ef4:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x3ef4, (0x3f0c-0x3ef4)
.L3f0c:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x3f0c, (0x3f6c-0x3f0c)
.L3f6c:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x3f6c, (0x3f70-0x3f6c)
gOvl_0200bf70:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x3f70, (0x4138-0x3f70)
	.global gOvl_0200c138
gOvl_0200c138:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4138, (0x4198-0x4138)
.L4198:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4198, (0x41b0-0x4198)
.L41b0:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x41b0, (0x4270-0x41b0)
.L4270:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4270, (0x4318-0x4270)
.L4318:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4318, (0x4468-0x4318)
.L4468:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4468, (0x4478-0x4468)
	.global gScript_957__0200c478
gScript_957__0200c478:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4478, (0x44c8-0x4478)
	.global gScript_957__0200c4c8
gScript_957__0200c4c8:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x44c8, (0x4518-0x44c8)
	.global gScript_957__0200c518
gScript_957__0200c518:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4518, (0x457c-0x4518)
	.global gScript_957__0200c57c
gScript_957__0200c57c:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x457c, (0x45e0-0x457c)
.L45e0:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x45e0, (0x4688-0x45e0)
.L4688:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4688, (0x4724-0x4688)
.L4724:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4724, (0x476c-0x4724)
.L476c:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x476c, (0x4808-0x476c)
.L4808:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4808, (0x4850-0x4808)
.L4850:
	.incbin "overlays/rom_7e3e08/orig.bin", 0x4850
