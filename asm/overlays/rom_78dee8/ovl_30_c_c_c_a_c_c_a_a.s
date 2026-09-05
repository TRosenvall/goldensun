	.include "macros.inc"

@ SetupArea10
@ Takes no arguments. Map-load setup for area 0x10, and the busiest function in
@ the overlay.
@
@ Writes 0x204 to [iwram_1ebc]+0x1C0 as the step delay. If save bit 0x814 is
@ set it plays scene 0x8D through Func_91ff0, resets the map transition to
@ 0x10000 on all three axes, and calls StartEarthquake.
@
@ The body is a 16-entry jump table at .La78 on the entrance id at
@ ewram_240+0x1C2, minus one. Seven of the sixteen entries go straight to the
@ exit, so most entrances need no setup at all; the rest group into four
@ handlers, which is why entrances 0x0B..0x0D and 0x0E..0x10 behave alike in
@ the table slots above -- they share a jump-table target.
.thumb_func_start OvlFunc_895_2008a24
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r0, #0xe0
	ldr	r3, [r3]
	lsl	r0, #1
	mov	r2, #0x81
	add	r3, r0
	lsl	r2, #2
	str	r2, [r3]
	ldr	r0, =0x814
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	beq	.La5c
	mov	r0, #0x8d
	bl	__Func_8091ff0
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #9
	lsl	r2, #9
	bl	__Func_8012330
	bl	__StartEarthquake
.La5c:
	ldr	r1, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r1, r2
	mov	r0, #0
	ldrsh	r3, [r3, r0]
	sub	r3, #1
	cmp	r3, #0xf
	bls	.La70
	b	.Lcda
.La70:
	ldr	r2, =.La78
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.La78:
	.word	.Lab8
	.word	.Lab8
	.word	.Lb10
	.word	.Lcda
	.word	.Lcda
	.word	.Lcda
	.word	.Lcda
	.word	.Lb1e
	.word	.Lcda
	.word	.Lcda
	.word	.Lb42
	.word	.Lb42
	.word	.Lb42
	.word	.Lc30
	.word	.Lc30
	.word	.Lc30
.Lab8:
	ldr	r0, =0x81a
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lac4
	b	.Lcda
.Lac4:
	mov	r5, #1
	mov	r0, #1
	mov	r1, #0x6d
	mov	r2, #4
	mov	r3, #0x51
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0
	mov	r1, #0x46
	mov	r2, #0x1e
	mov	r3, #0x2a
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r6, #3
	mov	r0, #0
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #2
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	bl	__Func_800fe9c
	b	.Lcda
.Lb10:
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	b	.Lcda
.Lb1e:
	mov	r0, #0x90
	ldr	r2, =0x10
	lsl	r0, #2
	add	r3, r1, r0
	strh	r2, [r3]
	ldr	r3, =0x242
	add	r2, r1, r3
	mov	r3, #8
	strh	r3, [r2]
	ldr	r0, =0x802
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb3c
	b	.Lcda
.Lb3c:
	bl	OvlFunc_895_2008d1c
	b	.Lcda
.Lb42:
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x11
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r0, =0x804
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lbd4
	bl	OvlFunc_895_2008f8c
.Lbd4:
	ldr	r0, =0x303
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lbee
	mov	r1, #0xbb
	mov	r2, #0x88
	mov	r0, #9
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	b	.Lc06
.Lbee:
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc06
	mov	r1, #0xbf
	mov	r2, #0x88
	mov	r0, #9
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
.Lc06:
	ldr	r0, =0x301
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc14
	mov	r1, #0xe3
	b	.Lc22
.Lc14:
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lcda
	mov	r1, #0xe7
.Lc22:
	mov	r2, #0x88
	mov	r0, #0xa
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	b	.Lcda
.Lc30:
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r0, =0x825
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lc86
	bl	OvlFunc_895_200961c
.Lc86:
	mov	r0, #1
	bl	OvlFunc_895_20097c0
	mov	r0, #0x8d
	lsl	r0, #2
	bl	__SetFlag
	ldr	r0, =0x821
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lcda
	mov	r5, #1
	mov	r0, #0
	mov	r1, #0x47
	mov	r2, #0x64
	mov	r3, #0x47
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r0, #0x7a
	mov	r1, #0x14
	mov	r2, #0x78
	mov	r3, #0x1e
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r3, #0x78
	mov	r2, #0x1e
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x7a
	mov	r1, #0x14
	mov	r2, #1
	mov	r3, #2
	bl	__Func_8010704
	bl	__Func_800fe9c
.Lcda:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2008a24
