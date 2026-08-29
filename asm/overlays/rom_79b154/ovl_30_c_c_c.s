	.include "macros.inc"

@ 55 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, CopyMapRectAttributes x4
.thumb_func_start OvlFunc_907_2008fa0
	push	{r5, lr}
	mov	r0, #8
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L1010
	ldr	r3, [r5, #0x10]
	asr	r2, r3, #20
	cmp	r2, #6
	bne	.Lfcc
	mov	r3, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #2
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	b	.Lfe0
.Lfcc:
	mov	r3, #0xe
	mov	r2, #6
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Lfe0:
	ldr	r3, [r5, #0x10]
	asr	r0, r3, #20
	cmp	r0, #9
	bne	.Lffc
	mov	r3, #0xe
	str	r3, [sp]
	str	r0, [sp, #4]
	mov	r1, #0
	mov	r0, #2
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	b	.L1010
.Lffc:
	mov	r3, #0xe
	mov	r2, #9
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #1
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L1010:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_907_2008fa0

	.section .data
	.global ActorCmd_ARRAY_907__020091c0
	.global gScript_944__02009480
	.global .L1498
	.global .L1600
	.global .L16f0
	.global .L1738
	.global .L1744
	.global .L1a2c
	.global .L1bc4
	.global .L1cf0
	.global .L1d0c
	.global .L1d28
	.global .L11d4
	.global .L11ec
	.global .L130c
	.global .L136c
	.global gOvl_020093fc

ActorCmd_ARRAY_907__020091c0:
	.incbin "overlays/rom_79b154/orig.bin", 0x11c0, (0x11d4-0x11c0)
.L11d4:
	.incbin "overlays/rom_79b154/orig.bin", 0x11d4, (0x11ec-0x11d4)
.L11ec:
	.incbin "overlays/rom_79b154/orig.bin", 0x11ec, (0x130c-0x11ec)
.L130c:
	.incbin "overlays/rom_79b154/orig.bin", 0x130c, (0x136c-0x130c)
.L136c:
	.incbin "overlays/rom_79b154/orig.bin", 0x136c, (0x13fc-0x136c)
gOvl_020093fc:
	.incbin "overlays/rom_79b154/orig.bin", 0x13fc, (0x142c-0x13fc)
	.global gOvl_0200942c
gOvl_0200942c:
	.incbin "overlays/rom_79b154/orig.bin", 0x142c, (0x1480-0x142c)
gScript_944__02009480:
	.incbin "overlays/rom_79b154/orig.bin", 0x1480, (0x1498-0x1480)
.L1498:
	.incbin "overlays/rom_79b154/orig.bin", 0x1498, (0x1600-0x1498)
.L1600:
	.incbin "overlays/rom_79b154/orig.bin", 0x1600, (0x16f0-0x1600)
.L16f0:
	.incbin "overlays/rom_79b154/orig.bin", 0x16f0, (0x1738-0x16f0)
.L1738:
	.incbin "overlays/rom_79b154/orig.bin", 0x1738, (0x1744-0x1738)
.L1744:
	.incbin "overlays/rom_79b154/orig.bin", 0x1744, (0x1a2c-0x1744)
.L1a2c:
	.incbin "overlays/rom_79b154/orig.bin", 0x1a2c, (0x1bc4-0x1a2c)
.L1bc4:
	.incbin "overlays/rom_79b154/orig.bin", 0x1bc4, (0x1cf0-0x1bc4)
.L1cf0:
	.incbin "overlays/rom_79b154/orig.bin", 0x1cf0, (0x1d0c-0x1cf0)
.L1d0c:
	.incbin "overlays/rom_79b154/orig.bin", 0x1d0c, (0x1d28-0x1d0c)
.L1d28:
	.incbin "overlays/rom_79b154/orig.bin", 0x1d28, (0x1d3c-0x1d28)
	.global gOvl_02009d3c
gOvl_02009d3c:
	.incbin "overlays/rom_79b154/orig.bin", 0x1d3c, (0x1d7c-0x1d3c)
	.global gScript_907__02009d7c
gScript_907__02009d7c:
	.incbin "overlays/rom_79b154/orig.bin", 0x1d7c, (0x1d88-0x1d7c)
	.global	.L1d88
.L1d88:
	.incbin "overlays/rom_79b154/orig.bin", 0x1d88
