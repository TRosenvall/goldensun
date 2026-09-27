	.include "macros.inc"
	.include "gba.inc"

	.section .data
	.global .L4
	.global .L5
	.global gOvlCommon1_3fe4
	.global .L8
	.global .L1
	.global .L2
	.global .L3
	.global .L6

.L1:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3e44, (0x3e4e-0x3e44)
.L2:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3e4e, (0x3e76-0x3e4e)
.L3:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3e76, (0x3ef4-0x3e76)
.L4:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3ef4, (0x3f14-0x3ef4)
.L5:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3f14, (0x3fd0-0x3f14)
.L6:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3fd0, (0x3fe4-0x3fd0)
gOvlCommon1_3fe4:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x3fe4, (0x4008-0x3fe4)
.L8:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x4008, (0x4010-0x4008)
	.word	OvlFunc_common1_172c
	.incbin "overlays/rom_7db0c8/orig.bin", 0x4014, (0x4154-0x4014)
	.global .L9
.L9:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x4154, (0x4194-0x4154)

	.section .data1
	.global .L15
	.global .L16
	.global .L10
	.global .L11
	.global .L12
	.global .L13
	.global .L14

.L10:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x457c, (0x457e-0x457c)
.L11:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x457e, (0x45aa-0x457e)
.L12:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x45aa, (0x4628-0x45aa)
.L13:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x4628, (0x46a6-0x4628)
.L14:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x46a6, (0x46a8-0x46a6)
.L15:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x46a8, (0x46c8-0x46a8)
	.word	OvlFunc_common1_17c0
	.incbin "overlays/rom_7db0c8/orig.bin", 0x46cc, (0x46fc-0x46cc)
.L16:
	.incbin "overlays/rom_7db0c8/orig.bin", 0x46fc, (0x471c-0x46fc)
	.word	OvlFunc_common1_17c0
	.incbin "overlays/rom_7db0c8/orig.bin", 0x4720

	.section .bss
	.global .L17
	.global .L18
	.global .L19
	.global .L20
	.global .L21
	.global .L22
	.global .L23
	.global .L24
	.global .L25
	.global .L26
	.global .L27
	.global .L28
	.global .L29
	.global .L30
	.global .L31
	.global .L32
	.global .L33
	.global .L34
	.global .L35
	.global .L36
	.global .L37
	.global .L38
	.global .L39
	.global .L41
	.global .L42
	.global .L43
	.global .L44
	.global .L45
	.global .L46
	.global .L47
	.global .L48
	.global .L49

	.lcomm	.L17, 4
	.lcomm	.L18, 4
	.lcomm	.L19, 4
	.lcomm	.L20, 4
	.lcomm	.L21, 4
	.lcomm	.L22, 4
	.lcomm	.L23, 4
	.lcomm	.L24, 4
	.lcomm	.L25, 4
	.lcomm	.L26, 4
	.lcomm	.L27, 4
	.lcomm	.L28, 4
	.lcomm	.L29, 4
	.lcomm	.L30, 4
	.lcomm	.L31, 4
	.lcomm	.L32, 4
	.lcomm	.L33, 4
	.lcomm	.L34, 4
	.lcomm	.L35, 4
	.lcomm	.L36, 4
	.lcomm	.L37, 4
	.lcomm	.L38, 4
	.lcomm	.L39, 4
	.lcomm	.L41, 0xc
	.lcomm	.L42, 4
	.lcomm	.L43, 0x30
	.lcomm	.L44, 4
	.lcomm	.L45, 4
	.lcomm	.L46, 4
	.lcomm	.L47, 4
	.lcomm	.L48, 4
	.lcomm	.L49, 4
