	.include "macros.inc"
	.include "gba.inc"

@ 120 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, SetSaveBit, TestSaveBit, SetSaveBit
@   TestSaveBit, SetSaveBit, TestSaveBit, SetSaveBit
@   TestSaveBit, SetSaveBit, GetSlotEntityChecked, TestSaveBit x2
@   StartLoopingSound
@ reads save bits 0x109, 0x301, 0x302, 0x303, 0x304; sets 0x206, 0x207, 0x208, 0x209, 0x20a.
.thumb_func_start OvlFunc_960_2008f50
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #0
	mov	r8, r0
	ldr	r0, =0x301
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf6a
	ldr	r0, =0x206
	bl	__SetFlag
.Lf6a:
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf7a
	ldr	r0, =0x207
	bl	__SetFlag
.Lf7a:
	ldr	r0, =0x303
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf8c
	mov	r0, #0x82
	lsl	r0, #2
	bl	__SetFlag
.Lf8c:
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf9e
	ldr	r0, =0x209
	bl	__SetFlag
.Lf9e:
	ldr	r0, =0x305
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lfae
	ldr	r0, =0x20a
	bl	__SetFlag
.Lfae:
	mov	r7, #0x80
	mov	r6, #8
	lsl	r7, #4
.Lfb4:
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.Lfd6
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lfce
	str	r7, [r5, #0x18]
	str	r7, [r5, #0x1c]
.Lfce:
	ldr	r3, [r5, #0x50]
	mov	r2, #0
	add	r3, #0x26
	strb	r2, [r3]
.Lfd6:
	add	r6, #1
	cmp	r6, #0xc
	ble	.Lfb4
	ldr	r6, =gDMATaskCount
	ldr	r5, =REG_IME
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r6]
	cmp	r2, #0x1f
	bgt	.L1008
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r6
	strh	r2, [r6]
	ldr	r2, =0x3f42
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDCNT
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L1008:
	strh	r1, [r5]
	mov	r0, #0xd0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1020
	mov	r3, #0x10
	mov	r0, #0xf4
	mov	r8, r3
	bl	__Func_8091ff0
.L1020:
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r3, [r6]
	cmp	r3, #0x1f
	bgt	.L1050
	lsl	r2, r3, #1
	add	r2, r3
	add	r3, #1
	mov	r0, r8
	strh	r3, [r6]
	mov	r3, #0x10
	lsl	r2, #2
	sub	r3, r0
	add	r2, r6
	lsl	r3, #8
	add	r2, #4
	orr	r3, r0
	stmia	r2!, {r3}
	ldr	r3, =REG_BLDALPHA
	stmia	r2!, {r3}
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r2]
.L1050:
	strh	r1, [r5]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_960_2008f50

@ 133 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, SetSaveBit, TestSaveBit, SetSaveBit
@   TestSaveBit, SetSaveBit, GetSlotEntityChecked, TestSaveBit
@   GetSlotEntityChecked, TestSaveBit, SetSaveBit, TestSaveBit
@   StartLoopingSound
@ reads save bits 0x109, 0x311, 0x312, 0x313, 0x315; sets 0x206, 0x207, 0x208, 0x9b7.
.thumb_func_start OvlFunc_960_2009094
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #0
	mov	r8, r0
	ldr	r0, =0x311
	bl	__GetFlag
	cmp	r0, #0
	beq	.L10ae
	ldr	r0, =0x206
	bl	__SetFlag
.L10ae:
	ldr	r0, =0x312
	bl	__GetFlag
	cmp	r0, #0
	beq	.L10be
	ldr	r0, =0x207
	bl	__SetFlag
.L10be:
	ldr	r0, =0x313
	bl	__GetFlag
	cmp	r0, #0
	beq	.L10d0
	mov	r0, #0x82
	lsl	r0, #2
	bl	__SetFlag
.L10d0:
	mov	r7, #0x80
	mov	r6, #8
	lsl	r7, #4
.L10d6:
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L10fa
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L10f0
	str	r7, [r5, #0x18]
	str	r7, [r5, #0x1c]
.L10f0:
	ldr	r0, [r5, #0x50]
	mov	r2, r0
	add	r2, #0x26
	mov	r3, #0
	strb	r3, [r2]
.L10fa:
	add	r6, #1
	cmp	r6, #0xa
	ble	.L10d6
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L1128
	ldr	r0, [r5, #0x50]
	ldr	r2, [r0, #0x28]
	cmp	r2, #0
	beq	.L1118
	mov	r3, #0xa
	strb	r3, [r2, #5]
.L1118:
	mov	r1, r0
	mov	r2, #1
	add	r1, #0x25
	strb	r2, [r1]
	mov	r2, r0
	mov	r3, #0
	add	r2, #0x26
	strb	r3, [r2]
.L1128:
	ldr	r0, =0x315
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1138
	ldr	r0, =0x9b7
	bl	__SetFlag
.L1138:
	ldr	r6, =gDMATaskCount
	ldr	r5, =REG_IME
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r6]
	cmp	r2, #0x1f
	bgt	.L1164
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r6
	strh	r2, [r6]
	ldr	r2, =0x3f42
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDCNT
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L1164:
	strh	r1, [r5]
	mov	r0, #0xd0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L117c
	mov	r3, #0x10
	mov	r0, #0xf4
	mov	r8, r3
	bl	__Func_8091ff0
.L117c:
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r3, [r6]
	cmp	r3, #0x1f
	bgt	.L11ac
	lsl	r2, r3, #1
	add	r2, r3
	add	r3, #1
	mov	r0, r8
	strh	r3, [r6]
	mov	r3, #0x10
	lsl	r2, #2
	sub	r3, r0
	add	r2, r6
	lsl	r3, #8
	add	r2, #4
	orr	r3, r0
	stmia	r2!, {r3}
	ldr	r3, =REG_BLDALPHA
	stmia	r2!, {r3}
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r2]
.L11ac:
	strh	r1, [r5]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_960_2009094

	.section .data
	.global gOvl_020095c0
	.global .L1a00
	.global .L15f8
	.global .L1610
	.global gScript_930__020096b8
	.global .L1790
	.global .L19c4
	.global .L17b4
	.global gScript_960__020097a8
	.global .L1458
	.global gOvl_02009488
	.global .L14d0
	.global .L1548

	.incbin "overlays/rom_7eaf28/orig.bin", 0x1430, (0x1458-0x1430)
.L1458:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1458, (0x1488-0x1458)
gOvl_02009488:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1488, (0x14d0-0x1488)
.L14d0:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x14d0, (0x1548-0x14d0)
.L1548:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1548, (0x15c0-0x1548)
gOvl_020095c0:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x15c0, (0x15f8-0x15c0)
.L15f8:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x15f8, (0x1610-0x15f8)
.L1610:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1610, (0x16b8-0x1610)
gScript_930__020096b8:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x16b8, (0x1790-0x16b8)
.L1790:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1790, (0x17a8-0x1790)
gScript_960__020097a8:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x17a8, (0x17b4-0x17a8)
.L17b4:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x17b4, (0x19c4-0x17b4)
.L19c4:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x19c4, (0x1a00-0x19c4)
.L1a00:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1a00
