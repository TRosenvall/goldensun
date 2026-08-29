	.include "macros.inc"

@ StageArea64
@ Takes no arguments. Entrance-dependent setup for area 0x64:
@   entrance 3               one CopyMapTiles metatile copy at (0x1E, 0x0E),
@   entrances 9..0x0F, 0x11  if save bit 0x911 is set, Func_92924 on slots
@                            0x0A..0x0E and 0x11..0x13 -- eight objects
@                            switched to their after-state.
@ Every other entrance needs nothing.
.thumb_func_start OvlFunc_937_200833c
	push	{lr}
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	sp, #8
	cmp	r3, #0xf
	bgt	.L35a
	cmp	r3, #9
	bge	.L376
	cmp	r3, #3
	beq	.L360
	b	.L3c2
.L35a:
	cmp	r3, #0x11
	beq	.L376
	b	.L3c2
.L360:
	mov	r3, #4
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0xe
	mov	r2, #0x1e
	mov	r3, #0x10
	bl	__CopyMapTiles
	b	.L3d8
.L376:
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3b8
	mov	r0, #0xa
	bl	__DeleteFieldActor
	mov	r0, #0xb
	bl	__DeleteFieldActor
	mov	r0, #0xc
	bl	__DeleteFieldActor
	mov	r0, #0xd
	bl	__DeleteFieldActor
	mov	r0, #0xe
	bl	__DeleteFieldActor
	mov	r0, #0x11
	bl	__DeleteFieldActor
	mov	r0, #0x12
	bl	__DeleteFieldActor
	mov	r0, #0x13
	bl	__DeleteFieldActor
	mov	r0, #0xf
	bl	__DeleteFieldActor
	b	.L3d8
.L3b8:
	mov	r0, #0xd
	mov	r1, #2
	bl	__Func_8092950
	b	.L3d8
.L3c2:
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3d8
	mov	r0, #0x10
	bl	__DeleteFieldActor
	mov	r0, #0x11
	bl	__DeleteFieldActor
.L3d8:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_937_200833c

	.section .data
	.global .L784
	.global .L8d4
	.global .La0c
	.global .La3c
	.global .La48
	.global .Lc88
	.global .Leb0
	.global MapEntrance_ARRAY_937__020084a0
	.global .L4d0
	.global .L6c8

MapEntrance_ARRAY_937__020084a0:
	.incbin "overlays/rom_7c3044/orig.bin", 0x4a0, (0x4d0-0x4a0)
.L4d0:
	.incbin "overlays/rom_7c3044/orig.bin", 0x4d0, (0x6c8-0x4d0)
.L6c8:
	.incbin "overlays/rom_7c3044/orig.bin", 0x6c8, (0x728-0x6c8)
	.global gOvl_02008728
gOvl_02008728:
	.incbin "overlays/rom_7c3044/orig.bin", 0x728, (0x784-0x728)
.L784:
	.incbin "overlays/rom_7c3044/orig.bin", 0x784, (0x79c-0x784)
	.global gScript_906__0200879c
gScript_906__0200879c:
	.incbin "overlays/rom_7c3044/orig.bin", 0x79c, (0x8d4-0x79c)
.L8d4:
	.incbin "overlays/rom_7c3044/orig.bin", 0x8d4, (0xa0c-0x8d4)
.La0c:
	.incbin "overlays/rom_7c3044/orig.bin", 0xa0c, (0xa3c-0xa0c)
.La3c:
	.incbin "overlays/rom_7c3044/orig.bin", 0xa3c, (0xa48-0xa3c)
.La48:
	.incbin "overlays/rom_7c3044/orig.bin", 0xa48, (0xc88-0xa48)
.Lc88:
	.incbin "overlays/rom_7c3044/orig.bin", 0xc88, (0xeb0-0xc88)
.Leb0:
	.incbin "overlays/rom_7c3044/orig.bin", 0xeb0, (0xef8-0xeb0)
	.global .Lef8
.Lef8:
	.incbin "overlays/rom_7c3044/orig.bin", 0xef8
