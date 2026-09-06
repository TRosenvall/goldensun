	.include "macros.inc"
	.include "gba.inc"

@ 191 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_32c, SetEntityScript x3
.thumb_func_start OvlFunc_968_200cbd8
	push	{r5, r6, r7, lr}
	mov	r5, r0
	ldr	r3, [r5, #8]
	sub	sp, #0xc
	mov	r0, sp
	str	r3, [r0]
	ldr	r1, =0xfff00000
	ldr	r3, [r5, #0xc]
	add	r3, r1
	str	r3, [r0, #4]
	ldr	r3, [r5, #0x10]
	mov	r1, #0
	str	r3, [r0, #8]
	bl	OvlFunc_968_200832c
	mov	r7, r0
	ldr	r6, [r7, #0x50]
	ldr	r3, [r6, #0x28]
	mov	r1, #0x80
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	lsl	r1, #1
	cmp	r3, r1
	beq	.L4c0a
	b	.L4d4c
.L4c0a:
	ldr	r2, [r5, #0x24]
	mov	r4, r2
	cmp	r2, #0
	bge	.L4c14
	neg	r4, r2
.L4c14:
	ldr	r3, [r5, #0x2c]
	mov	r1, r3
	cmp	r3, #0
	bge	.L4c1e
	neg	r1, r3
.L4c1e:
	cmp	r4, r1
	ble	.L4c38
	mov	r3, r2
	cmp	r3, #0
	bge	.L4c2c
	ldr	r2, =0xffff
	add	r3, r2
.L4c2c:
	cmp	r3, #0
	bge	.L4c34
	ldr	r4, =.L51a4
	b	.L4c4a
.L4c34:
	ldr	r4, =.L51a8
	b	.L4c4a
.L4c38:
	cmp	r3, #0
	bge	.L4c40
	ldr	r1, =0xffff
	add	r3, r1
.L4c40:
	cmp	r3, #0
	bge	.L4c48
	ldr	r4, =.L51ac
	b	.L4c4a
.L4c48:
	ldr	r4, =.L51b0
.L4c4a:
	ldrb	r1, [r4]
	mov	r0, r1
	cmp	r0, #0
	beq	.L4c74
	mov	r2, r6
	add	r2, #0x24
	ldrb	r3, [r2]
	cmp	r3, r0
	beq	.L4c6e
	mov	r6, r2
.L4c5e:
	add	r4, #1
	ldrb	r1, [r4]
	mov	r2, r1
	cmp	r2, #0
	beq	.L4c74
	ldrb	r3, [r6]
	cmp	r3, r2
	bne	.L4c5e
.L4c6e:
	mov	r3, r1
	cmp	r3, #0
	bne	.L4c7e
.L4c74:
	mov	r0, r5
	ldr	r1, =gScript_968__0200d564
	bl	__Actor_SetScript
	b	.L4d54
.L4c7e:
	ldr	r3, =gState
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xb9
	cmp	r2, r3
	bne	.L4ce8
	ldr	r0, =.L5128
	mov	r4, #0
	ldr	r6, [r5, #8]
	ldr	r3, [r0, r4]
	asr	r2, r6, #20
	cmp	r2, r3
	bne	.L4ca8
	ldr	r3, [r5, #0x10]
	ldr	r2, [r0, #4]
	asr	r3, #20
	cmp	r3, r2
	beq	.L4cc4
.L4ca8:
	add	r4, #1
	cmp	r4, #3
	bhi	.L4cc4
	lsl	r1, r4, #3
	ldr	r3, [r0, r1]
	asr	r2, r6, #20
	cmp	r2, r3
	bne	.L4ca8
	ldr	r3, [r5, #0x10]
	add	r2, r1, #4
	ldr	r2, [r0, r2]
	asr	r3, #20
	cmp	r3, r2
	bne	.L4ca8
.L4cc4:
	mov	r6, #0
	lsl	r4, #2
	b	.L4cd0
.L4cca:
	add	r3, r1, #1
	str	r3, [r0, r4]
	add	r6, #1
.L4cd0:
	ldr	r0, =.L772c
	ldr	r1, [r0, r4]
	ldrb	r2, [r1]
	cmp	r2, #0
	beq	.L4c74
	ldr	r3, [r7, #0x50]
	add	r3, #0x24
	ldrb	r3, [r3]
	cmp	r2, r3
	bne	.L4cca
	ldr	r3, =.L777c
	b	.L4d3e
.L4ce8:
	ldr	r0, =.L5164
	mov	r4, #0
	ldr	r6, [r5, #8]
	ldr	r3, [r0, r4]
	asr	r2, r6, #20
	cmp	r2, r3
	bne	.L4d00
	ldr	r3, [r5, #0x10]
	ldr	r2, [r0, #4]
	asr	r3, #20
	cmp	r3, r2
	beq	.L4d1c
.L4d00:
	add	r4, #1
	cmp	r4, #7
	bhi	.L4d1c
	lsl	r1, r4, #3
	ldr	r3, [r0, r1]
	asr	r2, r6, #20
	cmp	r2, r3
	bne	.L4d00
	ldr	r3, [r5, #0x10]
	add	r2, r1, #4
	ldr	r2, [r0, r2]
	asr	r3, #20
	cmp	r3, r2
	bne	.L4d00
.L4d1c:
	mov	r6, #0
	lsl	r4, #2
	b	.L4d28
.L4d22:
	add	r3, r1, #1
	str	r3, [r0, r4]
	add	r6, #1
.L4d28:
	ldr	r0, =.L778c
	ldr	r1, [r0, r4]
	ldrb	r2, [r1]
	cmp	r2, #0
	beq	.L4c74
	ldr	r3, [r7, #0x50]
	add	r3, #0x24
	ldrb	r3, [r3]
	cmp	r2, r3
	bne	.L4d22
	ldr	r3, =.L77ec
.L4d3e:
	ldr	r2, [r3, r4]
	lsl	r3, r6, #2
	ldr	r1, [r3, r2]
	mov	r0, r5
	bl	__Actor_SetScript
	b	.L4d54
.L4d4c:
	ldr	r1, =gScript_968__0200d564
	mov	r0, r5
	bl	__Actor_SetScript
.L4d54:
	mov	r0, #0
	add	sp, #0xc
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_968_200cbd8

	.section .data
	.global .L5148
	.global .L5164
	.global .L577c
	.global gScript_968__0200d7c8
	.global gScript_968__0200dac8
	.global .L5d3c
	.global .L6e44
	.global .L6f1c
	.global .L7120
	.global .L7300
	.global .L73b4
	.global .L74f8
	.global .L5128
	.global gScript_968__0200d21c
	.global gScript_968__0200d3c4
	.global gScript_968__0200d488
	.global gScript_968__0200d508
	.global .L5ce8
	.global .L5d12
	.global gOvl_0200e740
	.global .L68ec
	.global gScript_945__0200e904
	.global .L69c4
	.global .L6b74
	.global .L6c04
	.global .L6c64
	.global .L6cf4
	.global .L50e8
	.global .L51d4
	.global .L5d68
	.global .L5dc8
	.global .L6020
	.global .L6230
	.global .L6350
	.global .L6548

	.incbin "overlays/rom_7f2f14/orig.bin", 0x5040, (0x50e8-0x5040)
.L50e8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x50e8, (0x5128-0x50e8)
.L5128:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5128, (0x5148-0x5128)
.L5148:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5148, (0x5164-0x5148)
.L5164:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5164, (0x51a4-0x5164)
.L51a4:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x51a4, (0x51a8-0x51a4)
.L51a8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x51a8, (0x51ac-0x51a8)
.L51ac:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x51ac, (0x51b0-0x51ac)
.L51b0:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x51b0, (0x51d4-0x51b0)
.L51d4:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x51d4, (0x521c-0x51d4)
gScript_968__0200d21c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x521c, (0x52cc-0x521c)
	.global .L52cc  @ referenced from OvlFunc_968_200c968; exported so the
	                @ split can separate that function from this table.
.L52cc:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x52cc, (0x53c4-0x52cc)
gScript_968__0200d3c4:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x53c4, (0x5488-0x53c4)
gScript_968__0200d488:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5488, (0x5508-0x5488)
gScript_968__0200d508:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5508, (0x5564-0x5508)
	.global gScript_968__0200d564
gScript_968__0200d564:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5564, (0x577c-0x5564)
.L577c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x577c, (0x57c8-0x577c)
gScript_968__0200d7c8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x57c8, (0x5ac8-0x57c8)
gScript_968__0200dac8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5ac8, (0x5ce8-0x5ac8)
.L5ce8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5ce8, (0x5d12-0x5ce8)
.L5d12:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5d12, (0x5d3c-0x5d12)
.L5d3c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5d3c, (0x5d68-0x5d3c)
.L5d68:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5d68, (0x5dc8-0x5d68)
.L5dc8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x5dc8, (0x6020-0x5dc8)
.L6020:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6020, (0x6230-0x6020)
.L6230:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6230, (0x6350-0x6230)
.L6350:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6350, (0x6548-0x6350)
.L6548:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6548, (0x6740-0x6548)
gOvl_0200e740:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6740, (0x68ec-0x6740)
.L68ec:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x68ec, (0x6904-0x68ec)
gScript_945__0200e904:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6904, (0x69c4-0x6904)
.L69c4:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x69c4, (0x6b74-0x69c4)
.L6b74:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6b74, (0x6c04-0x6b74)
.L6c04:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6c04, (0x6c64-0x6c04)
.L6c64:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6c64, (0x6cf4-0x6c64)
.L6cf4:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6cf4, (0x6e44-0x6cf4)
.L6e44:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6e44, (0x6f1c-0x6e44)
.L6f1c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x6f1c, (0x7120-0x6f1c)
.L7120:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x7120, (0x7300-0x7120)
.L7300:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x7300, (0x73b4-0x7300)
.L73b4:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x73b4, (0x74f8-0x73b4)
.L74f8:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x74f8, (0x772c-0x74f8)
.L772c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x772c, (0x777c-0x772c)
.L777c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x777c, (0x778c-0x777c)
.L778c:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x778c, (0x77ec-0x778c)
.L77ec:
	.incbin "overlays/rom_7f2f14/orig.bin", 0x77ec
