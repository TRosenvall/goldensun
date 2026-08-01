	.include "macros.inc"
	.include "gba.inc"

	.section .rodata

@ Entity behaviour scripts. Each is a word array of opcodes consumed by the VM
@ in Func_cacc / Func_a494 and dispatched through Data_13624. .L1358c is the
@ default installed by Func_c150 at spawn; the rest are selected by the
@ Func_c46c..Func_c4bc family. They sit immediately before Data_13624 in rodata.
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L1358c
L1358c:
.L1358c:
	.incrom 0x1358c, 0x13590
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L13590
L13590:
.L13590:
	.incrom 0x13590, 0x135a8
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L135a8
L135a8:
.L135a8:
	.incrom 0x135a8, 0x135c0
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L135c0
L135c0:
.L135c0:
	.incrom 0x135c0, 0x135d8
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L135d8
L135d8:
.L135d8:
	.incrom 0x135d8, 0x135f0
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L135f0
L135f0:
.L135f0:
	.incrom 0x135f0, 0x13608
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L13608
L13608:
.L13608:
	.incrom 0x13608, 0x13620
@ PROMOTED: referenced from rom_c004.s across the split
	.global	L13620
L13620:
.L13620:
	.incrom 0x13620, 0x13624
