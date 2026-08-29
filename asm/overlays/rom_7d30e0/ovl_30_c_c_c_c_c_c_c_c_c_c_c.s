	.include "macros.inc"
	.include "gba.inc"

@ 55 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x9, RegisterTask x3
.thumb_func_start OvlFunc_948_200a290
	push	{r5, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, #1
	add	r0, #0x59
	strb	r5, [r0]
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x59
	strb	r5, [r0]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	add	r0, #0x59
	strb	r5, [r0]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	add	r0, #0x59
	strb	r5, [r0]
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r5, =0xb333
	str	r5, [r0, #0x18]
	mov	r0, #9
	bl	__MapActor_GetActor
	str	r5, [r0, #0x18]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	str	r5, [r0, #0x18]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	str	r5, [r0, #0x18]
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r1, #0xc8
	str	r5, [r0, #0x18]
	lsl	r1, #4
	ldr	r0, =OvlFunc_948_20097ac
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_948_200941c
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_948_2009308
	bl	__StartTask
	ldr	r2, =0x3f42
	ldr	r3, =REG_BLDCNT
	strh	r2, [r3]
	ldr	r2, =0x607
	add	r3, #2
	strh	r2, [r3]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_200a290

@ Cutscene: roughly 108 instructions of straight-line script --
@ 0 turns, 2 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x200, 0x9c8, 0x9c9.
.thumb_func_start OvlFunc_948_200a334
	push	{r5, lr}
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r5, #0
	add	r0, #0x55
	mov	r1, #0xc8
	strb	r5, [r0]
	lsl	r1, #4
	ldr	r0, =OvlFunc_948_2009e94
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_948_2009edc
	bl	__StartTask
	mov	r0, #0x6b
	mov	r1, #0
	mov	r2, #0
	bl	__Func_808edac
	ldr	r0, =0xed9
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2372
	mov	r0, #0xe
	mov	r1, #2
	bl	__MapActor_SetAnim
.L2372:
	bl	OvlFunc_948_2009ac8
	bl	OvlFunc_948_2009c28
	bl	OvlFunc_948_2009cf8
	bl	OvlFunc_948_2009e54
	bl	OvlFunc_948_2009e74
	mov	r1, #3
	mov	r0, #8
	bl	__Func_8092b08
	mov	r0, #0xb
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r5, [r0]
	mov	r0, #0xc
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r5, [r0]
	bl	OvlFunc_948_2009df8
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L23be
	bl	OvlFunc_948_2009984
	mov	r0, #0xd
	mov	r1, #5
	bl	__MapActor_SetAnim
.L23be:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L2440
	ldr	r0, =0x9ca
	bl	__GetFlag
	mov	r5, r0
	cmp	r5, #0
	beq	.L23ee
	mov	r1, #0xd6
	mov	r2, #0xce
	mov	r0, #0xf
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r0, #0xf
	bl	__MapActor_GetActor
	ldr	r3, =OvlFunc_948_2008aa8
	str	r3, [r0, #0x6c]
	b	.L2440
.L23ee:
	ldr	r0, =0x9c9
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2418
	mov	r1, #0xde
	mov	r2, #0xa6
	lsl	r1, #18
	mov	r0, #0xf
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r0, #0xf
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x50]
	mov	r1, #0x10
	strh	r5, [r3, #0x1e]
	bl	__Actor_SetAnimSpeed
	b	.L2440
.L2418:
	ldr	r0, =0x9c8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2432
	mov	r1, #0x92
	mov	r2, #0xaa
	mov	r0, #0xf
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	b	.L2440
.L2432:
	mov	r1, #0x92
	mov	r2, #0xa6
	mov	r0, #0xf
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
.L2440:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_200a334

	.section .data
	.global gScript_948__0200a6fc
	.global .L2808
	.global gOvl_0200a970
	.global gScript_884__0200a998
	.global .L29b0
	.global .L2a40
	.global .L2ad0
	.global .L2ba8
	.global .L2bb4
	.global .L2cb0
	.global gScript_953__0200adac
	.global .L2f74
	.global .L2644
	.global .L2684
	.global .L269c
	.global .L2868
	.global .L2898
	.global .L28e0
	.global gOvl_0200a928

.L2644:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2644, (0x2684-0x2644)
.L2684:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2684, (0x269c-0x2684)
.L269c:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x269c, (0x26fc-0x269c)
gScript_948__0200a6fc:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x26fc, (0x2808-0x26fc)
.L2808:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2808, (0x2868-0x2808)
.L2868:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2868, (0x2898-0x2868)
.L2898:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2898, (0x28e0-0x2898)
.L28e0:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x28e0, (0x2928-0x28e0)
gOvl_0200a928:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2928, (0x2970-0x2928)
gOvl_0200a970:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2970, (0x2998-0x2970)
gScript_884__0200a998:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2998, (0x29b0-0x2998)
.L29b0:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x29b0, (0x2a40-0x29b0)
.L2a40:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2a40, (0x2ad0-0x2a40)
.L2ad0:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2ad0, (0x2ba8-0x2ad0)
.L2ba8:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2ba8, (0x2bb4-0x2ba8)
.L2bb4:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2bb4, (0x2cb0-0x2bb4)
.L2cb0:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2cb0, (0x2dac-0x2cb0)
gScript_953__0200adac:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2dac, (0x2f74-0x2dac)
.L2f74:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2f74, (0x2f80-0x2f74)
	.global .L2f80
.L2f80:
	.incbin "overlays/rom_7d30e0/orig.bin", 0x2f80
