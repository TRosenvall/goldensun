	.include "macros.inc"

@ Cutscene: roughly 172 instructions of straight-line script --
@ 0 turns, 1 animation change, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bit 0x300.
@ Sets save bit 0x9a7.
.thumb_func_start OvlFunc_967_200904c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r7, =gState
	mov	r1, #0xe1
	lsl	r1, #1
	add	r1, r7
	mov	r2, #0
	ldrsh	r3, [r1, r2]
	mov	r11, r1
	cmp	r3, #0x5a
	bne	.L1072
	ldr	r0, =0x9a7
	bl	__SetFlag
.L1072:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r1, [r3]
	ldr	r3, =0x209
	lsl	r2, #1
	str	r3, [r1, r2]
	ldrsh	r3, [r7, r2]
	mov	r9, r3
	ldr	r3, =0xb3
	cmp	r9, r3
	bne	.L112a
	mov	r0, #0x14
	bl	__MapActor_GetActor
	mov	r2, #0
	mov	r10, r2
	mov	r3, r0
	add	r3, #0x23
	mov	r1, r10
	strb	r1, [r3]
	mov	r2, r0
	add	r2, #0x59
	ldrb	r3, [r2]
	mov	r6, #4
	orr	r3, r6
	strb	r3, [r2]
	ldr	r1, [r0, #0x50]
	mov	r5, #0xd
	ldrb	r2, [r1, #9]
	neg	r5, r5
	mov	r3, r5
	and	r3, r2
	mov	r2, #8
	mov	r8, r2
	mov	r2, r8
	orr	r3, r2
	strb	r3, [r1, #9]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x23
	mov	r1, r10
	strb	r1, [r3]
	mov	r2, r0
	add	r2, #0x59
	ldrb	r3, [r2]
	orr	r3, r6
	strb	r3, [r2]
	ldr	r1, [r0, #0x50]
	ldrb	r2, [r1, #9]
	mov	r3, r5
	and	r3, r2
	mov	r2, r8
	orr	r3, r2
	strb	r3, [r1, #9]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r2, r0
	add	r2, #0x59
	ldrb	r3, [r2]
	orr	r6, r3
	mov	r3, r0
	add	r3, #0x23
	mov	r1, r10
	strb	r6, [r2]
	strb	r1, [r3]
	ldr	r2, [r0, #0x50]
	ldrb	r3, [r2, #9]
	and	r5, r3
	mov	r3, r8
	orr	r5, r3
	strb	r5, [r2, #9]
	mov	r1, #6
	mov	r0, #0xf
	bl	__MapActor_SetAnim
	mov	r3, r11
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	cmp	r2, #0xc
	bne	.L112a
	mov	r1, #0xe2
	lsl	r1, #1
	add	r3, r7, r1
	mov	r1, r9
	strh	r1, [r3]
	mov	r1, #0xe3
	lsl	r1, #1
	add	r3, r7, r1
	strh	r2, [r3]
.L112a:
	ldr	r5, =gState
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r5, r2
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xb4
	cmp	r2, r3
	bne	.L11ac
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r2, #0x59
	add	r2, r0
	mov	r12, r2
	ldrb	r2, [r2]
	mov	r3, #4
	orr	r3, r2
	mov	r2, r12
	strb	r3, [r2]
	mov	r3, r0
	mov	r1, #0
	add	r3, #0x23
	strb	r1, [r3]
	ldr	r1, [r0, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, #8
	orr	r3, r2
	mov	r0, #0xc0
	strb	r3, [r1, #9]
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L117e
	ldr	r1, =ActorCmd_ARRAY_944__02009314
	mov	r0, #0xe
	bl	__MapActor_SetBehavior
.L117e:
	mov	r3, #0xe1
	lsl	r3, #1
	add	r5, r3
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	cmp	r3, #0x63
	bne	.L11ac
	bl	OvlFunc_967_2008508
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r1, #6
	bl	__Actor_SetBehavior
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r1, #6
	bl	__Actor_SetBehavior
	mov	r3, #0x15
	strh	r3, [r5]
.L11ac:
	mov	r0, #0
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_967_200904c

	.section .data
	.global .L1734
	.global .L189c
	.global .L1974
	.global gOvl_02009690
	.global .L16b0

	.global ActorCmd_ARRAY_944__02009314
	.global ActorCmd_ARRAY_967__02009314
ActorCmd_ARRAY_944__02009314:
ActorCmd_ARRAY_967__02009314:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1314, (0x1438-0x1314)
	.global gOvl_02009438
gOvl_02009438:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1438, (0x1690-0x1438)
gOvl_02009690:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1690, (0x16b0-0x1690)
.L16b0:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x16b0, (0x16d0-0x16b0)
	.global gOvl_020096d0
gOvl_020096d0:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x16d0, (0x1734-0x16d0)
.L1734:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1734, (0x189c-0x1734)
.L189c:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x189c, (0x1974-0x189c)
.L1974:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1974, (0x1a94-0x1974)
	.global .L1a94
.L1a94:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1a94, (0x1ca4-0x1a94)
	.global gScript_887__02009ca4
gScript_887__02009ca4:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1ca4, (0x1eb4-0x1ca4)
	.global .L1eb4
.L1eb4:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x1eb4, (0x2010-0x1eb4)
	.global .L2010
.L2010:
	.incbin "overlays/rom_7f21b8/orig.bin", 0x2010
