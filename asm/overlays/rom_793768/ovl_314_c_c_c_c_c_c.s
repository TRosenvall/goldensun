	.include "macros.inc"

	.section .data
	.global .L2828
	.global .L283e
	.global .L2854
	.global .L286a
	.global .L2880
	.global .L2896
	.global .L28ac
	.global .L2414
	.global .L2630
	.global gOvl_0200a094
	.global .L20cc
	.global .L227c
	.global gOvl_02009f5c

	.incbin "overlays/rom_793768/orig.bin", 0x1bc0, (0x1f5c-0x1bc0)
gOvl_02009f5c:
	.incbin "overlays/rom_793768/orig.bin", 0x1f5c, (0x2094-0x1f5c)
gOvl_0200a094:
	.incbin "overlays/rom_793768/orig.bin", 0x2094, (0x20cc-0x2094)
.L20cc:
	.incbin "overlays/rom_793768/orig.bin", 0x20cc, (0x227c-0x20cc)
.L227c:
	.incbin "overlays/rom_793768/orig.bin", 0x227c, (0x2414-0x227c)
.L2414:
	.incbin "overlays/rom_793768/orig.bin", 0x2414, (0x2630-0x2414)
.L2630:
	.incbin "overlays/rom_793768/orig.bin", 0x2630, (0x2828-0x2630)
.L2828:
	.incbin "overlays/rom_793768/orig.bin", 0x2828, (0x283e-0x2828)
.L283e:
	.incbin "overlays/rom_793768/orig.bin", 0x283e, (0x2854-0x283e)
.L2854:
	.incbin "overlays/rom_793768/orig.bin", 0x2854, (0x286a-0x2854)
.L286a:
	.incbin "overlays/rom_793768/orig.bin", 0x286a, (0x2880-0x286a)
.L2880:
	.incbin "overlays/rom_793768/orig.bin", 0x2880, (0x2896-0x2880)
.L2896:
	.incbin "overlays/rom_793768/orig.bin", 0x2896, (0x28ac-0x2896)
.L28ac:
	.incbin "overlays/rom_793768/orig.bin", 0x28ac, (0x28c4-0x28ac)
	.global gScript_898__0200a8c4
gScript_898__0200a8c4:
	.incbin "overlays/rom_793768/orig.bin", 0x28c4, (0x28dc-0x28c4)
	.global gScript_898__0200a8dc
gScript_898__0200a8dc:
	.incbin "overlays/rom_793768/orig.bin", 0x28dc
