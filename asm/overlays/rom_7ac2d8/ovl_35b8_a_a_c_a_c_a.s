	.include "macros.inc"
	.include "gba.inc"

@ 106 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectIndicesU, Random x2, OvlFunc_common0_10c, DialogueWait
@   CopyMapRectIndicesU x2
.thumb_func_start OvlFunc_924_200b860
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	sub	sp, #0x38
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r1, #0x3a
	mov	r2, #0x46
	mov	r3, #0x22
	mov	r0, #0x4a
	bl	__CopyMapTiles
	add	r1, sp, #0x10
	mov	r3, #7
	str	r3, [r1, #4]
	mov	r3, #0x80
	lsl	r3, #8
	ldr	r2, =0xffff3334
	str	r3, [r1, #8]
	str	r3, [r1, #0xc]
	mov	r3, #1
	mov	r10, r1
	mov	r7, #0
	mov	r9, r2
	mov	r8, r3
.L3898:
	mov	r6, #0
.L389a:
	mov	r3, r6
	mov	r1, r8
	and	r3, r1
	cmp	r3, #0
	beq	.L38fe
	bl	__Random
	lsl	r0, #3
	lsr	r0, #16
	lsl	r5, r0, #1
	add	r5, r0
	lsl	r3, r5, #4
	add	r5, r3
	lsl	r3, r5, #8
	add	r5, r3
	bl	__Random
	lsl	r0, #3
	lsr	r0, #16
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r2, r3, #4
	add	r3, r2
	lsl	r2, r3, #8
	add	r3, r2
	add	r3, r9
	lsl	r1, r7, #4
	str	r3, [sp, #4]
	neg	r2, r6
	mov	r3, #0x90
	sub	r2, r1
	lsl	r3, #12
	mov	r1, #0x88
	lsl	r1, #18
	str	r3, [sp, #8]
	add	r5, r9
	mov	r3, r10
	lsl	r2, #16
	mov	r0, #0xd2
	add	r2, r1
	str	r3, [sp, #0xc]
	mov	r1, #0
	lsl	r0, #15
	mov	r3, r5
	str	r1, [sp]
	bl	OvlFunc_common0_10c
	mov	r0, #1
	bl	__CutsceneWait
.L38fe:
	add	r6, #1
	cmp	r6, #7
	bls	.L389a
	mov	r1, r8
	mov	r3, #0x22
	sub	r3, r7
	str	r1, [sp]
	str	r1, [sp, #4]
	mov	r0, #0x4a
	mov	r1, #0x3b
	mov	r2, #0x46
	bl	__CopyMapTiles
	mov	r2, r8
	mov	r3, #0x21
	sub	r3, r7
	str	r2, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x4a
	mov	r1, #0x3a
	mov	r2, #0x46
	add	r7, #1
	bl	__CopyMapTiles
	cmp	r7, #1
	bls	.L3898
	add	sp, #0x38
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_200b860

@ 127 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectIndicesU, Random, OvlFunc_common0_10c, Random
@   OvlFunc_common0_10c, DialogueWait, CopyMapRectIndicesU x2
.thumb_func_start OvlFunc_924_200b948
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x38
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #0x4a
	mov	r3, #0x26
	mov	r0, #0x4c
	mov	r1, #0x3d
	bl	__CopyMapTiles
	add	r2, sp, #0x10
	mov	r3, #5
	str	r3, [r2, #4]
	mov	r3, #0x80
	lsl	r3, #8
	str	r3, [r2, #8]
	str	r3, [r2, #0xc]
	mov	r8, r2
	mov	r2, #0x90
	mov	r3, #1
	lsl	r2, #12
	mov	r7, #0
	mov	r9, r3
	mov	r11, r2
.L3986:
	ldr	r3, =0xfffe0000
	mov	r6, #1
	mov	r10, r3
.L398c:
	mov	r3, r6
	mov	r2, r9
	and	r3, r2
	cmp	r3, #0
	beq	.L3a0c
	mov	r5, #2
	and	r5, r6
	cmp	r5, #0
	beq	.L39d2
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsr	r3, #16
	mov	r0, #0x69
	sub	r0, r3
	lsl	r2, r7, #19
	mov	r3, r10
	sub	r2, r3, r2
	ldr	r3, =0x22e0000
	add	r2, r3
	mov	r3, #0
	str	r3, [sp]
	ldr	r3, =0xffffc000
	str	r3, [sp, #4]
	mov	r3, r11
	str	r3, [sp, #8]
	mov	r3, r8
	str	r3, [sp, #0xc]
	lsl	r0, #16
	mov	r1, #0
	mov	r3, #0
	bl	OvlFunc_common0_10c
	b	.L3a06
.L39d2:
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	mov	r2, #0x9b
	lsr	r3, #16
	lsl	r2, #2
	lsl	r0, r7, #2
	sub	r2, r3
	add	r0, r6
	mov	r3, #0xb7
	lsl	r3, #16
	lsl	r0, #17
	add	r0, r3
	mov	r3, r11
	str	r3, [sp, #8]
	mov	r3, r8
	str	r3, [sp, #0xc]
	mov	r3, #0x80
	lsl	r2, #16
	mov	r1, #0
	lsl	r3, #7
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	OvlFunc_common0_10c
.L3a06:
	mov	r0, #1
	bl	__CutsceneWait
.L3a0c:
	ldr	r2, =0xfffe0000
	add	r6, #1
	add	r10, r2
	cmp	r6, #7
	bls	.L398c
	mov	r2, r9
	mov	r3, #0x22
	sub	r3, r7
	str	r2, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r2, #0x46
	bl	__CopyMapTiles
	mov	r3, r9
	mov	r2, r7
	str	r3, [sp]
	str	r3, [sp, #4]
	add	r2, #0x4b
	mov	r0, #0x47
	mov	r1, #0x3b
	mov	r3, #0x26
	add	r7, #1
	bl	__CopyMapTiles
	cmp	r7, #2
	bls	.L3986
	add	sp, #0x38
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_200b948

@ 83 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, Random x4, OvlFunc_common0_10c
.thumb_func_start OvlFunc_924_200ba64
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0
	sub	sp, #0x38
	bl	__MapActor_GetActor
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r10, r3
	mov	r1, r10
	mov	r3, #3
	and	r1, r3
	mov	r9, r0
	mov	r10, r1
	cmp	r1, #0
	bne	.L3b06
	mov	r3, #7
	add	r7, sp, #0x10
	str	r3, [r7, #4]
	bl	__Random
	lsl	r0, #1
	lsr	r0, #16
	mov	r3, #1
	and	r0, r3
	cmp	r0, #0
	bne	.L3aa4
	mov	r3, #5
	str	r3, [r7, #4]
.L3aa4:
	ldr	r3, =0xb333
	str	r3, [r7, #8]
	str	r3, [r7, #0xc]
	bl	__Random
	mov	r2, r9
	ldr	r2, [r2, #0xc]
	lsl	r0, #2
	lsr	r0, #16
	mov	r8, r2
	lsl	r0, #16
	add	r8, r0
	bl	__Random
	lsl	r0, #3
	lsr	r0, #16
	lsl	r5, r0, #1
	add	r5, r0
	lsl	r3, r5, #4
	add	r5, r3
	lsl	r3, r5, #8
	add	r5, r3
	bl	__Random
	lsl	r0, #3
	lsr	r0, #16
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r2, r3, #4
	add	r3, r2
	ldr	r6, =0xffff3334
	lsl	r2, r3, #8
	add	r3, r2
	mov	r1, r9
	add	r3, r6
	ldr	r0, [r1, #8]
	ldr	r2, [r1, #0x10]
	str	r3, [sp]
	mov	r3, r10
	str	r3, [sp, #4]
	mov	r3, #0x90
	lsl	r3, #12
	add	r5, r6
	str	r3, [sp, #8]
	mov	r1, r8
	mov	r3, r5
	str	r7, [sp, #0xc]
	bl	OvlFunc_common0_10c
.L3b06:
	add	sp, #0x38
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_200ba64

@ 77 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random x3, OvlFunc_common0_10c
.thumb_func_start OvlFunc_924_200bb24
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r8, r3
	mov	r11, r2
	mov	r3, #3
	mov	r2, r8
	and	r2, r3
	sub	sp, #0x38
	mov	r10, r0
	mov	r9, r1
	mov	r8, r2
	cmp	r2, #0
	bne	.L3bb4
	mov	r3, #7
	add	r7, sp, #0x10
	str	r3, [r7, #4]
	bl	__Random
	lsl	r0, #1
	lsr	r0, #16
	mov	r3, #1
	and	r0, r3
	cmp	r0, #0
	bne	.L3b66
	mov	r3, #5
	str	r3, [r7, #4]
.L3b66:
	ldr	r3, =0xb333
	str	r3, [r7, #8]
	str	r3, [r7, #0xc]
	bl	__Random
	lsl	r0, #3
	lsr	r0, #16
	lsl	r5, r0, #1
	add	r5, r0
	lsl	r3, r5, #4
	add	r5, r3
	lsl	r3, r5, #8
	add	r5, r3
	bl	__Random
	lsl	r0, #3
	lsr	r0, #16
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r2, r3, #4
	add	r3, r2
	ldr	r6, =0xffff3334
	lsl	r2, r3, #8
	add	r3, r2
	add	r3, r6
	str	r3, [sp]
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r3, #0x90
	lsl	r3, #12
	add	r5, r6
	str	r3, [sp, #8]
	mov	r0, r10
	mov	r1, r9
	mov	r2, r11
	mov	r3, r5
	str	r7, [sp, #0xc]
	bl	OvlFunc_common0_10c
.L3bb4:
	add	sp, #0x38
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_200bb24
