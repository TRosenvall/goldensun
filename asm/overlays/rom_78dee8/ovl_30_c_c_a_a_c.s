	.include "macros.inc"

@ RevealPassageA
@ Takes no arguments. The one-shot reveal cutscene for passage A. Returns
@ immediately unless arming bit 0xF01 is set and opened bit 0x81A is clear, so
@ it plays exactly once.
@
@ The sequence, which OvlFunc_420 mirrors for passage B:
@   - Func_8e118 dismisses any open box, sound 0xB6 starts the rumble.
@   - CopyMapTiles copies a 0x2A x 0x1E rectangle from (0x46, 0x1E), then
@     .gcc2_compiled. pushes it to VRAM -- the wall cracking.
@   - Line 0x1032, then sound 0xB7 and the real edit: a 3x1 metatile copy at
@     (0x1D, 3) followed by the SAME rectangle through Func_10704, so the
@     collision attributes move with the artwork and the new gap is walkable.
@     A second copy from (0x6D, 4) lays in the passage interior.
@   - Three .gcc2_compiled. calls shake the map: +0x10000/+0x10000, then
@     +0x20000/+0x10000, then -1/-1 with 0xE666 to settle. Negative arguments
@     are skipped by .gcc2_compiled., which is how the settle leaves x and z alone.
@   - MapActor_Emote effect 0x100 on the player, then four .gcc2_compiled. turns --
@     0x4000, 0x8000, 0, 0x4000 -- so the player looks around the new opening.
@   - Line 0x1033 (the id is bumped with `add r8, #1` from 0x1032), then save
@     bits 0x143 and 0x81A are set to record it.
.thumb_func_start OvlFunc_895_2008258
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r0, =0xf01
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.L26c
	b	.L39c
.L26c:
	ldr	r0, =0x81a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L278
	b	.L39c
.L278:
	bl	__CutsceneStart
	bl	__Func_808e118
	mov	r0, #0xb6
	bl	__PlaySound
	mov	r5, #1
	mov	r2, #0x1e
	mov	r1, #0x46
	mov	r3, #0x2a
	mov	r0, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	bl	__Func_800fe9c
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r3, =0x1032
	mov	r8, r3
	mov	r1, #1
	mov	r0, r8
	bl	__Func_801776c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xb7
	bl	__PlaySound
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
	mov	r3, #0x51
	mov	r0, #1
	mov	r1, #0x6d
	mov	r2, #4
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	bl	__Func_800fe9c
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #9
	lsl	r2, #9
	lsl	r0, #9
	bl	__Func_8012330
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #4
	mov	r2, #0x14
	bl	__MapActor_Jump
	mov	r0, #0
	mov	r1, #6
	mov	r2, #0x28
	bl	__MapActor_Jump
	mov	r0, #1
	mov	r1, #1
	ldr	r2, =0xe666
	neg	r1, r1
	neg	r0, r0
	bl	__Func_8012330
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r3, #1
	add	r8, r3
	mov	r1, #1
	mov	r0, r8
	bl	__Func_801776c
	ldr	r0, =0x143
	bl	__SetFlag
	ldr	r0, =0x81a
	bl	__SetFlag
	bl	__CutsceneEnd
.L39c:
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2008258
