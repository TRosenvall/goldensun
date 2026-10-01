	.include "macros.inc"

	.section .data

	.global Events_TolbiSpring
Events_TolbiSpring:
	.incbin "overlays/rom_7bc690/orig.bin", 0x1f30, (0x1f48-0x1f30)
	.global .L1f48  @ data table read from OvlFunc_933_2008e2c; exported so
	                @ the split can separate that function from this table.
.L1f48:
	.incbin "overlays/rom_7bc690/orig.bin", 0x1f48, (0x1f70-0x1f48)
	.global .L1f70  @ data table read from OvlFunc_933_2008e2c; exported so
	                @ the split can separate that function from this table.
.L1f70:
	.incbin "overlays/rom_7bc690/orig.bin", 0x1f70, (0x1f80-0x1f70)
