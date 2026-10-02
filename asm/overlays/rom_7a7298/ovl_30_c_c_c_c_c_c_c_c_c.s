	.include "macros.inc"

	.section .data
@ Exported for the batch-318 text/data split: the function moves to C and
@ this blob stays in asm, so the C file references it by name.  A .global
@ emits no bytes -- gated on its own before the split.
	.global .L2430
	.global .L3190
	.global .L31a8
	.global .L31c0
	.global .L31d6
	.global gScript_921__0200a4f4
	.global .L2508
	.global gScript_921__0200a564
	.global .L31c0
	.global .L31d6
	.global .L29e0
	.global gOvl_0200aa58
	.global .L2ad0
	.global .L2c80
	.global .L2db8
	.global .L2798
	.global .L28a0

	.global gTable_921__0200a3f0
gTable_921__0200a3f0:
	.incbin "overlays/rom_7a7298/orig.bin", 0x23f0, (0x2430-0x23f0)
.L2430:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2430, (0x24f4-0x2430)
gScript_921__0200a4f4:
	.incbin "overlays/rom_7a7298/orig.bin", 0x24f4, (0x2508-0x24f4)
.L2508:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2508, (0x2564-0x2508)
gScript_921__0200a564:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2564, (0x25ec-0x2564)
	.global gScript_921__0200a5ec
gScript_921__0200a5ec:
	.incbin "overlays/rom_7a7298/orig.bin", 0x25ec, (0x264c-0x25ec)
	.global gScript_921__0200a64c
gScript_921__0200a64c:
	.incbin "overlays/rom_7a7298/orig.bin", 0x264c, (0x2670-0x264c)
	.global gScript_921__0200a670
gScript_921__0200a670:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2670, (0x26e0-0x2670)
	.global gScript_921__0200a6e0
gScript_921__0200a6e0:
	.incbin "overlays/rom_7a7298/orig.bin", 0x26e0, (0x274c-0x26e0)
	.global gScript_921__0200a74c
gScript_921__0200a74c:
	.incbin "overlays/rom_7a7298/orig.bin", 0x274c, (0x2760-0x274c)
	.global gScript_921__0200a760
gScript_921__0200a760:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2760, (0x2798-0x2760)
.L2798:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2798, (0x28a0-0x2798)
.L28a0:
	.incbin "overlays/rom_7a7298/orig.bin", 0x28a0, (0x2990-0x28a0)
	.global gOvl_0200a990
gOvl_0200a990:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2990, (0x29e0-0x2990)
.L29e0:
	.incbin "overlays/rom_7a7298/orig.bin", 0x29e0, (0x2a58-0x29e0)
gOvl_0200aa58:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2a58, (0x2ad0-0x2a58)
.L2ad0:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2ad0, (0x2c80-0x2ad0)
.L2c80:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2c80, (0x2db8-0x2c80)
.L2db8:
	.incbin "overlays/rom_7a7298/orig.bin", 0x2db8, (0x3190-0x2db8)
.L3190:
	.incbin "overlays/rom_7a7298/orig.bin", 0x3190, (0x31a8-0x3190)
.L31a8:
	.incbin "overlays/rom_7a7298/orig.bin", 0x31a8, (0x31c0-0x31a8)
.L31c0:
	.incbin "overlays/rom_7a7298/orig.bin", 0x31c0, (0x31d6-0x31c0)
.L31d6:
	.incbin "overlays/rom_7a7298/orig.bin", 0x31d6

	.section .bss
	.global .L31f0

	.lcomm	.L31f0, 0xc
