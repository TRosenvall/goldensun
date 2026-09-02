	.include "macros.inc"

@ 147 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, Random, PlaceSlotAt, GetSlotEntityChecked
@   PlaceSlotAt, GetSlotEntityChecked x2, PlaceSlotAt, SpawnEntity
@   SignedDiv, SetEntityScript, SetActorPartsPalette, Random
@   Sin
.thumb_func_start OvlFunc_969_200da28
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0x17
	bl	__MapActor_GetActor
	ldr	r3, =iwram_3001e70
	mov	r10, r0
	ldr	r5, [r3]
	bl	__Random
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #4
	lsr	r3, #16
	lsl	r3, #16
	add	r5, #0xe8
	mov	r8, r3
	mov	r0, #2
	ldrsh	r3, [r5, r0]
	cmp	r3, #0x81
	bgt	.L5aac
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L5a7e
	mov	r1, #0x98
	mov	r2, #0xa4
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #0x17
	bl	__MapActor_SetPos
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r5, #0x80
	lsl	r5, #9
	b	.L5a94
.L5a7e:
	mov	r1, #0x98
	mov	r2, #0xab
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #0x17
	bl	__MapActor_SetPos
	mov	r0, #0x17
	bl	__MapActor_GetActor
	ldr	r5, =0x14ccc
.L5a94:
	str	r5, [r0, #0x18]
	mov	r0, #0x17
	bl	__MapActor_GetActor
	str	r5, [r0, #0x1c]
	b	.L5ab6

	.pool_aligned

.L5aac:
	mov	r0, #0x17
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L5ab6:
	mov	r1, r10
	cmp	r1, #0
	beq	.L5b80
	ldr	r3, =iwram_3001e40
	ldr	r6, [r3]
	mov	r3, #0xf
	and	r6, r3
	cmp	r6, #0
	bne	.L5b80
	mov	r0, r10
	ldr	r2, [r0, #0xc]
	ldr	r1, [r1, #8]
	mov	r3, #0x80
	lsl	r3, #12
	add	r2, r8
	add	r1, r3
	add	r2, r3
	ldr	r3, [r0, #0x10]
	mov	r0, #0x8e
	lsl	r0, #1
	bl	__CreateActor
	mov	r1, #0xc0
	lsl	r1, #11
	mov	r7, r0
	mov	r0, r8
	bl	_divsi3_RAM
	mov	r8, r0
	mov	r1, r8
	lsl	r1, #16
	mov	r8, r1
	cmp	r7, #0
	beq	.L5b80
	ldr	r1, =gScript_969__0200e734
	mov	r0, r7
	ldr	r5, [r7, #0x50]
	bl	__Actor_SetScript
	mov	r1, #5
	mov	r0, r7
	bl	__Func_80929d8
	mov	r3, r7
	add	r3, #0x55
	strb	r6, [r3]
	bl	__Random
	ldr	r3, =0xffff000
	mov	r2, r7
	and	r3, r0
	add	r2, #0x64
	ldr	r0, .L5b5c	@ 0
	strh	r3, [r2]
	mov	r3, r7
	mov	r9, r0
	add	r3, #0x66
	ldr	r0, =0xfffff
	strh	r6, [r3]
	mov	r2, r8
	ldr	r3, =OvlFunc_969_200db90
	mov	r1, r10
	and	r0, r2
	str	r1, [r7, #0x68]
	str	r3, [r7, #0x6c]
	asr	r0, #4
	bl	__sin
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #3
	asr	r3, #16
	str	r3, [r7, #0x30]
	mov	r3, r5
	add	r3, #0x26
	mov	r0, r9
	strb	r0, [r3]
	mov	r1, r10
	ldr	r3, [r1, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	b	.L5b74

	.align	2, 0
.L5b5c:
	.word	0
	.pool

.L5b74:
	ldrb	r1, [r5, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r1
	orr	r3, r2
	strb	r3, [r5, #9]
.L5b80:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_969_200da28

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Cos, Sin
.thumb_func_start OvlFunc_969_200db90
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r5, r0
	mov	r6, r5
	add	r6, #0x64
	ldrh	r2, [r6]
	ldr	r1, [r5, #0x68]
	mov	r8, r2
	mov	r0, r8
	mov	r10, r1
	bl	__cos
	ldr	r3, [r5, #0x30]
	add	r3, #0x1c
	mov	r2, r3
	mul	r2, r0
	mov	r1, r10
	ldr	r3, [r1, #8]
	mov	r0, r8
	add	r3, r2
	str	r3, [r5, #8]
	bl	__sin
	mov	r2, #0xa4
	ldr	r3, [r5, #8]
	lsl	r2, #16
	lsl	r0, #4
	add	r0, r2
	str	r0, [r5, #0x10]
	str	r3, [r5, #0x38]
	str	r0, [r5, #0x40]
	ldr	r1, =0xfffffe00
	ldrh	r3, [r6]
	add	r3, r1
	strh	r3, [r6]
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_969_200db90

	.section .data
	.global gScript_969__0200dfc4
	.global gScript_969__0200e004
	.global gScript_969__0200e03c
	.global gScript_969__0200e074
	.global gScript_969__0200e088
	.global gScript_969__0200e0ac
	.global gScript_969__0200e0d0
	.global gScript_969__0200e0f4
	.global gScript_969__0200e130
	.global gScript_969__0200e16c
	.global gScript_969__0200e22c
	.global gScript_969__0200e324
	.global gScript_969__0200e360
	.global gScript_969__0200e39c
	.global gScript_969__0200e3c0
	.global gOvl_0200e464
	.global gOvl_0200e478
	.global .L66e8
	.global gOvl_0200e6ec
	.global gOvl_0200e3d4

gScript_969__0200dfc4:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x5fc4, (0x6004-0x5fc4)
gScript_969__0200e004:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6004, (0x603c-0x6004)
gScript_969__0200e03c:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x603c, (0x6074-0x603c)
gScript_969__0200e074:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6074, (0x6088-0x6074)
gScript_969__0200e088:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6088, (0x60ac-0x6088)
gScript_969__0200e0ac:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x60ac, (0x60d0-0x60ac)
gScript_969__0200e0d0:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x60d0, (0x60f4-0x60d0)
gScript_969__0200e0f4:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x60f4, (0x6130-0x60f4)
gScript_969__0200e130:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6130, (0x616c-0x6130)
gScript_969__0200e16c:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x616c, (0x61cc-0x616c)
	.global gScript_969__0200e1cc
gScript_969__0200e1cc:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x61cc, (0x622c-0x61cc)
gScript_969__0200e22c:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x622c, (0x62d0-0x622c)
	.global gScript_969__0200e2d0
gScript_969__0200e2d0:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x62d0, (0x6324-0x62d0)
gScript_969__0200e324:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6324, (0x6360-0x6324)
gScript_969__0200e360:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6360, (0x639c-0x6360)
gScript_969__0200e39c:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x639c, (0x63c0-0x639c)
gScript_969__0200e3c0:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x63c0, (0x63d4-0x63c0)
gOvl_0200e3d4:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x63d4, (0x6464-0x63d4)
gOvl_0200e464:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6464, (0x6478-0x6464)
gOvl_0200e478:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6478, (0x66e8-0x6478)
.L66e8:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x66e8, (0x66ec-0x66e8)
gOvl_0200e6ec:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x66ec, (0x6734-0x66ec)
	.global gScript_969__0200e734
gScript_969__0200e734:
	.incbin "overlays/rom_7f6e64/orig.bin", 0x6734

	.section .bss
	.global .L6760
	.global .L6764

	.lcomm	.L6760, 4
	.lcomm	.L6764, 4
