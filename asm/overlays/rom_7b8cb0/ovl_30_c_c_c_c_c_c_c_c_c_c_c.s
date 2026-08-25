	.include "macros.inc"

@ 84 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random x3, SetActorPartsPalette, Random, DestroyEntity
.thumb_func_start OvlFunc_931_2008c44
	push	{r5, r6, lr}
	mov	r5, r0
	bl	__Random
	mov	r6, r5
	lsl	r0, #1
	add	r6, #0x64
	lsr	r0, #16
	mov	r1, #0
	ldrsh	r2, [r6, r1]
	sub	r0, #1
	lsl	r0, #16
	ldr	r3, [r5, #8]
	lsl	r2, #12
	asr	r0, #1
	add	r2, r0
	add	r3, r2
	str	r3, [r5, #8]
	mov	r2, #0
	ldrsh	r3, [r6, r2]
	cmp	r3, #3
	bgt	.Lc92
	bl	__Random
	ldr	r3, [r5, #0x10]
	lsl	r0, #15
	ldr	r1, =0xffff0000
	lsr	r0, #16
	sub	r3, r0
	add	r3, r1
	str	r3, [r5, #0x10]
	ldr	r2, =0x2666
	ldr	r3, [r5, #0x18]
	add	r3, r2
	str	r3, [r5, #0x18]
	ldr	r1, =0xfffff5c3
	ldr	r3, [r5, #0x1c]
	add	r3, r1
	b	.Lca8
.Lc92:
	ldr	r3, [r5, #0x10]
	mov	r2, #0x80
	lsl	r2, #10
	add	r3, r2
	str	r3, [r5, #0x10]
	ldr	r2, =0x7ae
	ldr	r3, [r5, #0x18]
	add	r3, r2
	str	r3, [r5, #0x18]
	ldr	r3, [r5, #0x1c]
	add	r3, r2
.Lca8:
	str	r3, [r5, #0x1c]
	bl	__Random
	mov	r1, #0
	ldrsh	r3, [r6, r1]
	mul	r3, r0
	lsr	r3, #16
	ldrh	r2, [r6]
	cmp	r3, #0
	bne	.Lcc6
	mov	r0, r5
	mov	r1, #7
	bl	__Func_80929d8
	ldrh	r2, [r6]
.Lcc6:
	lsl	r3, r2, #16
	cmp	r3, #0
	beq	.Lcd0
	sub	r3, r2, #1
	b	.Lcde
.Lcd0:
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsr	r3, #16
	lsl	r3, #1
	add	r3, #2
.Lcde:
	strh	r3, [r6]
	ldr	r3, [r5, #0x68]
	sub	r3, #1
	str	r3, [r5, #0x68]
	cmp	r3, #0
	bne	.Lcf2
	mov	r0, r5
	str	r3, [r5, #0x6c]
	bl	__DeleteActor
.Lcf2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_2008c44

@ 33 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SpawnEntity, OvlFunc_c0c, SetEntityAnimation
.thumb_func_start OvlFunc_931_2008d08
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e40
	ldr	r6, [r3]
	mov	r3, #3
	and	r6, r3
	cmp	r6, #0
	bne	.Ld4a
	mov	r1, #0x80
	mov	r3, #0xc8
	mov	r0, #0xde
	lsl	r1, #15
	mov	r2, #0
	lsl	r3, #17
	bl	__CreateActor
	mov	r5, r0
	cmp	r5, #0
	beq	.Ld4a
	mov	r3, r5
	mov	r2, #0x14
	add	r3, #0x64
	strh	r2, [r3]
	add	r3, #2
	strh	r6, [r3]
	str	r2, [r5, #0x68]
	bl	OvlFunc_931_2008c0c
	ldr	r3, =OvlFunc_931_2008c44
	mov	r0, r5
	str	r3, [r5, #0x6c]
	mov	r1, #1
	bl	__Actor_SetAnim
.Ld4a:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_2008d08

@ Cutscene: roughly 69 instructions of straight-line script --
@ 4 turns, 2 animation changes, 0 dialogue lines, 3 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Sets save bit 0x8ff.
.thumb_func_start OvlFunc_931_2008d58
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0xfc
	mov	r1, #1
	mov	r2, #0xe1
	mov	r3, #1
	lsl	r2, #17
	neg	r1, r1
	lsl	r0, #14
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #0x12
	bl	__MapActor_SetAnim
	mov	r0, #1
	neg	r0, r0
	bl	__Func_8091ff0
	ldr	r0, =OvlFunc_931_2008d08
	bl	__StopTask
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0x12
	mov	r2, #0
	bl	__Func_809280c
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r2, #0x28
	lsl	r1, #8
	mov	r0, #0x12
	bl	__Func_8092adc
	mov	r0, #0x93
	bl	__PlaySound
	mov	r1, #2
	mov	r0, #0x12
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xb0
	mov	r2, #0x28
	mov	r0, #0x12
	lsl	r1, #8
	bl	__Func_8092adc
	bl	OvlFunc_931_20087b8
	mov	r0, #0
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__Func_8093530
	mov	r1, #4
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	ldr	r0, =0x8ff
	bl	__SetFlag
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_2008d58

	.section .data
	.global .L1e70
	.global .L13f4
	.global .L140c
	.global .L15bc
	.global .L1724
	.global .L19f4
	.global .L10f0
	.global .L1120
	.global .L1288

	.incbin "overlays/rom_7b8cb0/orig.bin", 0xfc8, (0x10f0-0xfc8)
.L10f0:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x10f0, (0x1120-0x10f0)
.L1120:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1120, (0x1288-0x1120)
.L1288:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1288, (0x1390-0x1288)
	.global gOvl_02009390
gOvl_02009390:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1390, (0x13f4-0x1390)
.L13f4:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x13f4, (0x140c-0x13f4)
.L140c:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x140c, (0x15bc-0x140c)
.L15bc:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x15bc, (0x1724-0x15bc)
.L1724:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1724, (0x1730-0x1724)
	.global gScript_930__02009730
gScript_930__02009730:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1730, (0x19f4-0x1730)
.L19f4:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x19f4, (0x1e70-0x19f4)
.L1e70:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1e70
