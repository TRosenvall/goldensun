	.include "macros.inc"
	.include "gba.inc"

@ RunTitleSequence -- EXPORT SLOT 0, the overlay's main entry
@ Takes no arguments. Called through __start_overlay+0x04 by rom_8a000's
@ Func_8d590 and FieldMain. Dispatches on the area sub-id at ewram_240+0x1C2:
@
@     0x0A  _Func_92054 resolves the party entity and clears its +0x55, sound
@           0x4B, OvlFunc_2e8(0), 0x78 frames or a keypress, then
@           _Func_91e3c(0, 2)
@     0x09  sound 0x43, THE CREDIT ROLL via _Func_f03f0(0), sound 0x11, a fade
@           through _Func_3b70(0x3C) / _Func_3ce0, _Func_9163c(0xF0), sound
@           0x13, and _Func_91e3c(1, 2)
@     else  _Func_2f3c(0x0B), then the boot path -- sound 0x13, the two logo
@           splashes _Func_f2b70(0) and _Func_f2d54(0), _Func_1f77c to test for a
@           save file, and THE TITLE SCREEN as _Func_f26ec(1) when one exists or
@           _Func_f26ec(0) when it does not, with sounds 0x46 and 0x40
@           respectively
@
@ So this single function is the game's whole front end: logos, title, new game,
@ continue and the ending credits, selected by one field. 135 lines; the dispatch
@ is traced, the arms structurally.
.thumb_func_start OvlFunc_879_2008054
	push	{r5, r6, lr}
	ldr	r2, =gState
	mov	r1, #0xe1
	lsl	r1, #1
	add	r5, r2, r1
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	cmp	r3, #0xa
	bne	.Lb4
	mov	r1, #0xfa
	lsl	r1, #1
	add	r3, r2, r1
	ldr	r0, [r3]
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0x4b
	bl	__PlaySound
	mov	r0, #0
	bl	OvlFunc_879_20082e8
	mov	r0, #0x78
	bl	__WaitFrames
	ldr	r2, =gKeyPress
	ldr	r3, [r2]
	mov	r5, #0
	cmp	r3, #0
	bne	.Laa
	mov	r6, r2
.L96:
	mov	r0, #1
	bl	__WaitFrames
	ldr	r2, =0xe0f
	add	r5, #1
	cmp	r5, r2
	bgt	.Laa
	ldr	r3, [r6]
	cmp	r3, #0
	beq	.L96
.Laa:
	ldr	r0, =0
	mov	r1, #2
	bl	__SetDestMap
	b	.L196
.Lb4:
	cmp	r3, #9
	bne	.Lea
	mov	r0, #0x43
	bl	__PlaySound
	mov	r0, #0
	bl	__StartGS1Credits
	mov	r0, #0x11
	bl	__PlaySound
	mov	r0, #0x3c
	bl	__Func_8003b70
	bl	__Func_8003ce0
	mov	r0, #0xf0
	bl	__CutsceneWait
	mov	r0, #0x13
	bl	__PlaySound
	ldr	r0, =1
	mov	r1, #2
	bl	__SetDestMap
	b	.L196
.Lea:
	ldr	r0, =0xb
	bl	__Func_8002f3c
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	cmp	r3, #2
	bne	.L15c
.Lf8:
	mov	r0, #0x13
	bl	__PlaySound
	mov	r0, #0
	bl	__NintendoLogo
	mov	r0, #0
	bl	__CamelotLogo
	bl	__Func_801f77c
	cmp	r0, #0
	ble	.L152
	mov	r0, #0x46
	bl	__PlaySound
	mov	r0, #1
	bl	__StartTitleScreen
	cmp	r0, #0
	bne	.L152
	mov	r0, #0x11
	bl	__PlaySound
	mov	r0, #0x1e
	bl	__Func_8003b70
	bl	__Func_8003ce0
	ldr	r2, =gKeyHeld
	ldr	r3, [r2]
	mov	r5, #0
	cmp	r3, #0
	bne	.Lf8
	mov	r6, r2
.L13e:
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0x77
	bgt	.Lf8
	ldr	r3, [r6]
	cmp	r3, #0
	beq	.L13e
	b	.Lf8
.L152:
	ldr	r0, =1
	mov	r1, #1
	bl	__SetDestMap
	b	.L17a
.L15c:
	mov	r0, #0x40
	bl	__PlaySound
	mov	r0, #0
	bl	__StartTitleScreen
	bl	__Func_8077f70
	ldr	r0, =4
	mov	r1, #0x10
	bl	__SetDestMap
	mov	r0, #0x11
	bl	__PlaySound
.L17a:
	mov	r0, #0x11
	bl	__PlaySound
	mov	r0, #0x1e
	bl	__Func_8003b70
	bl	__Func_8003ce0
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0x13
	bl	__PlaySound
.L196:
	mov	r0, #0
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_879_2008054
