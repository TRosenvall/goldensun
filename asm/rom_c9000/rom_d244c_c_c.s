	.include "macros.inc"
	.include "gba.inc"

@ RunRainAnimationImpl -- downpour, then a burst
@ r0 = action descriptor, r1 = variant (0 = Func_d2458 = animation class 7,
@ 1 = Func_d244c). 208 frames, one WaitFrames(1) each. Stores the descriptor at
@ [iwram_1eec]+0x7828 on entry.
@
@ Reads as rain: sixteen drops fall repeatedly and splash where they land, a
@ haze of sparks and particles drifts up, then at frame 0x80 a burst of large
@ sprites erupts and the screen flashes white before the palette swaps. The
@ mechanics are traced; the element is inferred from them.
@
@ SETUP
@   REG_BLDALPHA = 0x1010, REG_BG2CNT = 0x784 (256-colour, char base 1, screen
@     base 7). AnimStart(0).
@   TWO BLITTERS ARE GENERATED HERE: BuildDraw2DFuncEx(0x2E, 7, 7, 3, 2) and
@     BuildDraw2DFuncEx(0x2F, 7, 7, 3, 3). Their pointers are then read straight back
@     out of the allocation table as [iwram_1ef0+0x18] -> sp+0x28 (blitter A)
@     and [iwram_1ef0+0x1c] -> sp+0x2c (blitter B). See BuildDraw2DFuncEx.
@   Assets via GetFile, decompressed with DecompressLZ:
@     0x7d  palette -> 0x5000000, gfx -> [iwram_1eec]+0      (32x64 burst frames,
@           0x800 bytes each at 8bpp)
@     0xb4  gfx -> [iwram_1eec]+0x3000                       (the splash frames)
@     0x73  gfx -> [iwram_1ef4]                              (the ten sizes that
@           Data_ede48 indexes)
@     0xc4  variant 1 only: its palette overwrites 0x5000000
@   [iwram_1eec]+0x7780 = 2, +0x7784 = 0x4B, StartTask registers Task_BlitAnim.
@   sp+0x14 = horizontal direction, multiplied into every vx below:
@     variant 0 -> +1; variant 1 -> -1 unless [desc+4] == 1, then +1.
@   Variant 1 also fixes a spawn origin at sp+0x3c/0x40 = (0x4C or 0x2C, 0x42)
@     depending on [desc+4]. Func_e396c's result is computed and then discarded.
@   Clears +0x7098 (64 entries, stride 0x1C) to -1; seeds the 16 drops at
@     +0x7320 with x = +-((rand & 0x7F) + 0x80) by direction, y = (rand & 7) -
@     0x48 (above the screen), age = -(rand & 0x1F) so they stagger; frees all
@     1024 particles at ewram_10000.
@   Variant 0 only: Func_d6750, WaitFrames(1), then CreateSummonSprite(8, 0x179, 2).
@
@ FOUR PARTICLE SETS, all stride 0x1C, all {x, y, -, vx, vy, -, age}
@   ewram_10000  1024  drifting haze
@   +0x74E0        24  slow sparks, drawn as a 1x2 dot of colour 32 (.Lee188)
@   +0x7080        24  fast burst sprites, 32x64, frame = age/4 from asset 0x7d
@   +0x7320        16  the falling drops
@
@ MAIN LOOP -- frame counter 0..0xCF
@   A or B held and frame in 0x31..0x9F: variant 0 puts the four actors at
@     +0x77D8/DC/E4/E8 into animations 8/9/10/11, every target takes
@     Func_d6888(id, 0xA, 5, -1, 0) + _Func_b8228(id, 4), then frame = 0xA0.
@   every frame     InitMatrixStack(); MatrixSetLook([iwram_1e80], [iwram_1e80]+0xC).
@   frame 0xB2      .gcc2_compiled.(0x86), handing back to rom_b5000.
@   frame 0x30      sound 0x8D.      frame 0x80  sound 0x91.
@   frame 0x80      +0x7784 = 0x32; +0x77A8 = 0x30.
@   frames 0xA0..0xAF  +0x7780 = 1 and +0x7784 = 0x10101010, becoming
@                   0x3F3F3F3F past frame 0xAD -- the white-out. Func_e727c(2,2,2).
@   frame 0xB0      +0x7780 = 3, +0x7784 = ewram_20202 (used as a literal), and
@                   asset 0xb4's palette is copied over 0x5000000.
@   frames 0x21..0xAF  spawn into ewram_10000, 1 per frame and 8 per frame once
@                   past 0x67: x = ((rand & 0xFF) - 0x20) << 16, y = 112.0,
@                   vx = ((rand & 0x7F) + .Lee184[i&3]) << 9 with .Lee184 =
@                   {0, 8, 12, 16}, vy = the same magnitude negated << 11.
@   frames 0x29..0x7F, ODD FRAMES ONLY  spawn one spark into +0x74E0:
@                   speed = (rand & 0xFF) + 0x80, angle = (rand & 0x1FFF) +
@                   0x4E20; vx = speed*sin >> 9, vy = speed*cos >> 9.
@                   Origin (0x4E..0x55, 70.0) for variant 0, sp+0x3c/0x40 for 1.
@   frames 0x81..0xAF  spawn one burst sprite into +0x7080, same speed/angle but
@                   angle - 0x4E20 and the velocities shifted >> 6, so eight
@                   times faster. Origin (68.0, 64.0) or sp+0x3c/0x40.
@   frames 0x28..0x2F  the camera target sp+0x18/sp+0x1c walks by (-8.0, +16.0)
@                   per frame; variant 0 feeds it to Func_e6d3c(3, x, y).
@   variant 0, frame 0x80 / 0xB0  the four actors switch to animations 8,9,10,11
@                   / 0,1,3,4.
@   frame 0x8A      every target takes Func_d6888(id, 0xA, 5, -1, 0) and
@                   _Func_b8228(id, 4).
@   frames 0..0xAF  draw and step each set:
@     +0x7080  blit A(dst, [iwram_1eec] + (age>>2)*0x800, x-0x10, y-0x20, 32, 64)
@              x += vx*dir; y += vy; dies at age 0x18.
@     +0x74E0  blit B(dst, .Lee188, x, y-1, 1, 2) -- a single dot.
@              x += vx*dir; y += vy; vy -= 0x400 each frame, so sparks
@              accelerate upward; dies at age 0x30.
@     ewram_10000  size n = .Lee18a[i&3] = {1,1,2,3}; blit A centred with the
@              graphic at [iwram_1ef4] + Data_ede48[n-1].
@              x += vx*dir; y += vy. Past frame 0x80 vx loses 0x8000 (odd i) or
@              0x2000 (even i) per frame, killing the drift; before that vy
@              loses 0x400 so the haze rises. Dies at age 0x100.
@   frames 0..0xAF  the 16 drops at +0x7320, each in one of two states:
@     y <= 0x37 (still falling), age 0:  x -= 6*dir, y += 6, and blit A draws
@              the size-10 sprite (Data_ede48[9], 10x20) at (x-5, y+0x1E).
@              Crossing y > 0x37 before frame 0x30 plays sound 0x88 -- the
@              impact. Otherwise age just increments.
@     y > 0x37 (landed):  age 0..0xB drives a six-frame splash, frame = age/2,
@              graphic [iwram_1eec] + 0x3000 + .Lee1a0[f], size .Lee18e[f] x
@              .Lee194[f], drawn at (x - w/2, y + .Lee19a[f]). The three tables
@              are w = {24,29,30,27,24,16}, h = {23,32,37,49,53,52}, yoff =
@              {41,32,26,11,4,2} and .Lee1a0 = {0,552,1480,2590,3913,5185} is
@              exactly their running w*h product -- a splash that grows upward
@              from a fixed base. age wraps at 0xC and the drop falls again.
@   end of frame  UpdateScreenShake(8, 8); Func_cd52c(); +0x7824 = 1; WaitFrames(1).
@
@ TEARDOWN
@   StopTask(Task_BlitAnim); Func_2dd8(0x2F) and Func_2dd8(0x2E) release the two
@   generated blitters. Variant 0 additionally calls Anim_Unsummon(3, x, y) and
@   destroys the 8 actors at +0x77D8. AnimEnd restores the view.
.thumb_func_start BaseAnim_Tiamat  @ 0x080d2464
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x48
	str	r1, [sp, #0x38]
	ldr	r5, =iwram_3001ef0
	ldr	r1, [r5]
	mov	r3, r5
	str	r1, [sp, #0x34]
	sub	r3, #0x70
	ldr	r3, [r3]
	str	r3, [sp, #0x30]
	sub	r3, r5, #4
	ldr	r3, [r3]
	ldr	r7, =0x7828
	mov	r10, r3
	ldr	r2, [r5, #4]
	add	r7, r10
	str	r2, [sp, #0x20]
	str	r0, [r7]
	mov	r0, #0
	bl	AnimStart
	ldr	r2, =REG_BLDALPHA
	ldr	r3, .Ld24d8	@ 0x1010
	strh	r3, [r2]
	ldr	r3, .Ld24dc	@ 0x784
	sub	r2, #0x46
	strh	r3, [r2]
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #7
	mov	r8, r3
	mov	r0, #0x2e
	mov	r3, #3
	bl	BuildDraw2DFuncEx
	ldr	r4, [r5, #0x18]
	mov	r3, #3
	mov	r1, #7
	mov	r2, #7
	mov	r0, #0x2f
	str	r4, [sp, #0x28]
	str	r3, [sp]
	bl	BuildDraw2DFuncEx
	ldr	r5, [r5, #0x1c]
	ldr	r0, =_FILE_7d
	str	r5, [sp, #0x2c]
	bl	GetFile
	mov	r5, r0
	b	.Ld24f0

	.align	2, 0
.Ld24d8:
	.word	0x1010
.Ld24dc:
	.word	0x784
	.pool

.Ld24f0:
	mov	r0, #0xa0
	mov	r2, #0x80
	mov	r1, r5
	ldr	r6, =Func_8001af8
	add	r5, #0x80
	lsl	r0, #19
	bl	_call_via_r6
	mov	r1, r10
	mov	r0, r5
	bl	DecompressLZ
	ldr	r0, =_FILE_b4
	bl	GetFile
	mov	r1, #0xc0
	mov	r5, r0
	add	r5, #0x80
	lsl	r1, #6
	add	r1, r10
	mov	r0, r5
	bl	DecompressLZ
	ldr	r0, =_FILE_73
	bl	GetFile
	ldr	r1, [sp, #0x20]
	bl	DecompressLZ
	ldr	r0, [sp, #0x38]
	cmp	r0, #1
	bne	.Ld2542
	ldr	r0, =_FILE_c4
	bl	GetFile
	mov	r1, r0
	mov	r0, #0xa0
	lsl	r0, #19
	mov	r2, #0x80
	bl	_call_via_r6
.Ld2542:
	mov	r3, #0xef
	lsl	r3, #7
	ldr	r2, =0x7784
	add	r3, r10
	mov	r1, r8
	str	r1, [r3]
	add	r2, r10
	mov	r3, #0x4b
	mov	r1, #0x90
	str	r3, [r2]
	ldr	r0, =Task_BlitAnim
	lsl	r1, #3
	bl	StartTask
	ldr	r3, [sp, #0x38]
	mov	r2, #1
	str	r2, [sp, #0x14]
	cmp	r3, #0
	beq	.Ld257a
	ldr	r3, [r7]
	mov	r4, #1
	ldr	r3, [r3, #4]
	neg	r4, r4
	str	r4, [sp, #0x14]
	cmp	r3, #1
	beq	.Ld257a
	mov	r6, #1
	str	r6, [sp, #0x14]
.Ld257a:
	ldr	r0, [sp, #0x38]
	cmp	r0, #1
	bne	.Ld25ae
	ldr	r5, =0x7828
	add	r5, r10
	ldr	r3, [r5]
	add	r6, sp, #0x3c
	ldr	r0, [r3, #8]
	mov	r1, r6
	bl	GetBattleActorPos2
	ldr	r3, [r6]
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	str	r3, [r6]
	mov	r3, #0x42
	str	r3, [r6, #4]
	ldr	r3, [r5]
	ldr	r3, [r3, #4]
	cmp	r3, #1
	bne	.Ld25aa
	mov	r3, #0x4c
	b	.Ld25ac
.Ld25aa:
	mov	r3, #0x2c
.Ld25ac:
	str	r3, [r6]
.Ld25ae:
	ldr	r2, =0xffc40000
	mov	r1, #0xd4
	lsl	r1, #16
	ldr	r3, =0x7098
	str	r2, [sp, #0x1c]
	str	r1, [sp, #0x18]
	mov	r2, #1
	mov	r7, #0
	neg	r2, r2
	add	r3, r10
.Ld25c2:
	add	r7, #1
	str	r2, [r3]
	add	r3, #0x1c
	cmp	r7, #0x40
	bne	.Ld25c2
	ldr	r5, =0x7320
	mov	r7, #0
	mov	r6, #0x7f
	add	r5, r10
.Ld25d4:
	ldr	r3, [sp, #0x14]
	cmp	r3, #1
	bne	.Ld25e4
	bl	Random
	and	r0, r6
	add	r0, #0x80
	b	.Ld25ec
.Ld25e4:
	bl	Random
	and	r0, r6
	sub	r0, #0x80
.Ld25ec:
	str	r0, [r5]
	bl	Random
	mov	r3, #7
	and	r3, r0
	sub	r3, #0x48
	str	r3, [r5, #4]
	bl	Random
	mov	r3, #0x1f
	and	r3, r0
	neg	r3, r3
	add	r7, #1
	str	r3, [r5, #0x18]
	add	r5, #0x1c
	cmp	r7, #0x10
	bne	.Ld25d4
	mov	r1, #1
	mov	r2, #0x80
	ldr	r3, =ewram_2010018
	mov	r7, #0
	neg	r1, r1
	lsl	r2, #2
.Ld261a:
	add	r7, #1
	str	r1, [r3]
	add	r3, #0x1c
	cmp	r7, r2
	bne	.Ld261a
	ldr	r4, [sp, #0x38]
	cmp	r4, #0
	bne	.Ld2644
	ldr	r3, =0x7828
	add	r3, r10
	ldr	r0, [r3]
	bl	Func_80d6750
	mov	r0, #1
	bl	WaitFrames
	ldr	r1, =0x179
	mov	r0, #8
	mov	r2, #2
	bl	CreateSummonSprite
.Ld2644:
	ldr	r0, [sp, #0x30]
	mov	r6, #0
	add	r0, #0xc
	str	r6, [sp, #0x24]
	str	r0, [sp, #0xc]
.Ld264e:
	ldr	r3, =gKeyRepeat
	ldr	r3, [r3]
	mov	r2, #3
	and	r3, r2
	cmp	r3, #0
	beq	.Ld26da
	ldr	r1, [sp, #0x24]
	cmp	r1, #0x30
	ble	.Ld26da
	cmp	r1, #0x9f
	bgt	.Ld26da
	ldr	r2, [sp, #0x38]
	cmp	r2, #0
	bne	.Ld269a
	ldr	r3, =0x77d8
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #8
	bl	_Sprite_SetAnim
	ldr	r3, =0x77dc
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #9
	bl	_Sprite_SetAnim
	ldr	r3, =0x77e4
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #0xa
	bl	_Sprite_SetAnim
	ldr	r3, =0x77e8
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #0xb
	bl	_Sprite_SetAnim
.Ld269a:
	ldr	r3, =0x7828
	mov	r4, r10
	ldr	r3, [r4, r3]
	ldr	r3, [r3, #0x14]
	mov	r7, #0
	cmp	r3, #0
	beq	.Ld26d6
	ldr	r5, =0x7828
	mov	r6, #0x24
	add	r5, r10
.Ld26ae:
	ldr	r3, [r5]
	ldrsh	r0, [r3, r6]
	mov	r3, #0
	str	r3, [sp]
	mov	r2, #5
	mov	r1, #0xa
	sub	r3, #1
	bl	Func_80d6888
	ldr	r3, [r5]
	mov	r1, #4
	ldrsh	r0, [r3, r6]
	bl	_SetBattleActorKnockback
	ldr	r3, [r5]
	ldr	r3, [r3, #0x14]
	add	r7, #1
	add	r6, #2
	cmp	r7, r3
	bne	.Ld26ae
.Ld26d6:
	mov	r3, #0xa0
	str	r3, [sp, #0x24]
.Ld26da:
	bl	InitMatrixStack
	ldr	r0, [sp, #0x30]
	ldr	r1, [sp, #0xc]
	bl	MatrixSetLook
	ldr	r4, [sp, #0x24]
	cmp	r4, #0xb2
	bne	.Ld26f2
	mov	r0, #0x86
	bl	_Func_80bd7dc
.Ld26f2:
	ldr	r6, [sp, #0x24]
	cmp	r6, #0x80
	bne	.Ld2700
	ldr	r2, =0x7784
	mov	r3, #0x32
	add	r2, r10
	str	r3, [r2]
.Ld2700:
	ldr	r0, [sp, #0x24]
	cmp	r0, #0xb0
	bne	.Ld2730
	mov	r2, #0xef
	lsl	r2, #7
	add	r2, r10
	mov	r3, #3
	str	r3, [r2]
	ldr	r2, =0x7784
	ldr	r3, =ewram_2020202
	add	r2, r10
	str	r3, [r2]
	ldr	r0, =_FILE_b4
	bl	GetFile
	mov	r5, r0
	mov	r0, #0xa0
	ldr	r3, =Func_8001af8
	lsl	r0, #19
	mov	r1, r5
	mov	r2, #0x80
	bl	_call_via_r3
	b	.Ld275e
.Ld2730:
	ldr	r3, [sp, #0x24]
	sub	r3, #0xa0
	cmp	r3, #0xf
	bhi	.Ld275e
	mov	r3, #0xef
	lsl	r3, #7
	add	r3, r10
	mov	r2, #1
	str	r2, [r3]
	ldr	r2, =0x7784
	ldr	r3, =0x10101010
	add	r2, r10
	str	r3, [r2]
	ldr	r1, [sp, #0x24]
	cmp	r1, #0xad
	ble	.Ld2754
	ldr	r3, =0x3f3f3f3f
	str	r3, [r2]
.Ld2754:
	mov	r0, #2
	mov	r1, #2
	mov	r2, #2
	bl	Func_80e727c
.Ld275e:
	ldr	r3, [sp, #0x24]
	sub	r3, #0x21
	cmp	r3, #0x8e
	bhi	.Ld27e6
	ldr	r4, [sp, #0x24]
	mov	r2, #0
	mov	r3, #1
	mov	r8, r2
	str	r3, [sp, #0x10]
	cmp	r4, #0x67
	ble	.Ld2778
	mov	r6, #8
	str	r6, [sp, #0x10]
.Ld2778:
	ldr	r1, =.Lee184
	mov	r0, #0x7f
	ldr	r6, =gBuffer
	mov	r7, #0
	mov	r11, r0
	mov	r9, r1
.Ld2784:
	mov	r2, #1
	ldr	r3, [r6, #0x18]
	neg	r2, r2
	cmp	r3, r2
	bne	.Ld27da
	bl	Random
	mov	r3, #0xff
	and	r3, r0
	sub	r3, #0x20
	lsl	r3, #16
	str	r3, [r6]
	mov	r3, #0xe0
	lsl	r3, #15
	str	r3, [r6, #4]
	bl	Random
	mov	r5, #3
	mov	r4, r9
	mov	r3, r11
	and	r5, r7
	and	r0, r3
	ldrb	r3, [r4, r5]
	add	r0, r3
	lsl	r0, #9
	str	r0, [r6, #0xc]
	bl	Random
	mov	r2, r9
	ldrb	r3, [r2, r5]
	mov	r1, r11
	and	r0, r1
	add	r0, r3
	neg	r0, r0
	mov	r3, #0
	lsl	r0, #11
	str	r3, [r6, #0x18]
	str	r0, [r6, #0x10]
	mov	r3, #1
	ldr	r4, [sp, #0x10]
	add	r8, r3
	cmp	r8, r4
	beq	.Ld27e6
.Ld27da:
	mov	r0, #0x80
	add	r7, #1
	lsl	r0, #2
	add	r6, #0x1c
	cmp	r7, r0
	bne	.Ld2784
.Ld27e6:
	ldr	r3, [sp, #0x24]
	sub	r3, #0x29
	cmp	r3, #0x56
	bls	.Ld27f0
	b	.Ld28f8
.Ld27f0:
	ldr	r2, [sp, #0x24]
	mov	r3, #1
	mov	r1, #0
	and	r3, r2
	mov	r11, r1
	cmp	r3, #0
	beq	.Ld28f8
	mov	r3, #0x3c
	ldr	r5, =0x74e0
	add	r3, sp
	mov	r7, #0
	mov	r9, r3
	add	r5, r10
.Ld280a:
	mov	r4, #1
	ldr	r3, [r5, #0x18]
	neg	r4, r4
	cmp	r3, r4
	bne	.Ld28f0
	bl	Random
	mov	r3, #0xff
	and	r3, r0
	add	r3, #0x80
	mov	r8, r3
	bl	Random
	ldr	r3, =0x1fff
	ldr	r1, [sp, #0x38]
	and	r3, r0
	ldr	r0, =0x4e20
	add	r6, r3, r0
	cmp	r1, #0
	bne	.Ld28ac
	bl	Random
	mov	r2, #7
	and	r0, r2
	add	r0, #0x4e
	mov	r3, #0x8c
	lsl	r0, #16
	lsl	r3, #15
	str	r0, [r5]
	b	.Ld28c4

	.pool_aligned

.Ld28ac:
	bl	Random
	mov	r3, #7
	mov	r4, r9
	and	r0, r3
	ldr	r3, [r4]
	add	r0, r3
	sub	r0, #8
	lsl	r0, #16
	str	r0, [r5]
	ldr	r3, [r4, #4]
	lsl	r3, #16
.Ld28c4:
	str	r3, [r5, #4]
	mov	r0, r6
	bl	sin
	mov	r3, r8
	mul	r3, r0
	asr	r3, #9
	str	r3, [r5, #0xc]
	mov	r0, r6
	bl	cos
	mov	r3, r8
	mul	r3, r0
	mov	r6, #1
	asr	r3, #9
	add	r11, r6
	str	r3, [r5, #0x10]
	mov	r0, r11
	mov	r3, #0
	str	r3, [r5, #0x18]
	cmp	r0, #1
	beq	.Ld28f8
.Ld28f0:
	add	r7, #1
	add	r5, #0x1c
	cmp	r7, #0x18
	bne	.Ld280a
.Ld28f8:
	ldr	r1, [sp, #0x24]
	cmp	r1, #0x30
	bne	.Ld2904
	mov	r0, #0x8d
	bl	_PlaySound
.Ld2904:
	ldr	r2, [sp, #0x24]
	cmp	r2, #0x80
	bne	.Ld2910
	mov	r0, #0x91
	bl	_PlaySound
.Ld2910:
	ldr	r3, [sp, #0x24]
	sub	r3, #0x81
	cmp	r3, #0x2e
	bhi	.Ld299e
	mov	r4, #0x3c
	mov	r5, #0xe1
	mov	r3, #0
	add	r4, sp
	lsl	r5, #7
	mov	r11, r3
	mov	r7, #0
	mov	r9, r4
	add	r5, r10
.Ld292a:
	mov	r6, #1
	ldr	r3, [r5, #0x18]
	neg	r6, r6
	cmp	r3, r6
	bne	.Ld2996
	bl	Random
	mov	r3, #0xff
	and	r3, r0
	add	r3, #0x80
	mov	r8, r3
	bl	Random
	ldr	r3, =0x1fff
	ldr	r1, [sp, #0x38]
	and	r3, r0
	ldr	r0, =0xffffb1e0
	add	r6, r3, r0
	cmp	r1, #0
	bne	.Ld295e
	mov	r3, #0x88
	lsl	r3, #15
	str	r3, [r5]
	mov	r3, #0x80
	lsl	r3, #15
	b	.Ld296a
.Ld295e:
	mov	r2, r9
	ldr	r3, [r2]
	lsl	r3, #16
	str	r3, [r5]
	ldr	r3, [r2, #4]
	lsl	r3, #16
.Ld296a:
	str	r3, [r5, #4]
	mov	r0, r6
	bl	sin
	mov	r3, r8
	mul	r3, r0
	asr	r3, #6
	str	r3, [r5, #0xc]
	mov	r0, r6
	bl	cos
	mov	r3, r8
	mul	r3, r0
	asr	r3, #6
	str	r3, [r5, #0x10]
	mov	r3, #0
	str	r3, [r5, #0x18]
	mov	r3, #1
	add	r11, r3
	mov	r4, r11
	cmp	r4, #1
	beq	.Ld299e
.Ld2996:
	add	r7, #1
	add	r5, #0x1c
	cmp	r7, #0x18
	bne	.Ld292a
.Ld299e:
	ldr	r6, [sp, #0x24]
	cmp	r6, #0xaf
	bgt	.Ld2a02
	mov	r5, #0xe1
	lsl	r5, #7
	mov	r7, #0
	add	r5, r10
.Ld29ac:
	ldr	r1, [r5, #0x18]
	cmp	r1, #0
	blt	.Ld29fa
	mov	r0, #2
	ldrsh	r2, [r5, r0]
	mov	r4, #6
	ldrsh	r3, [r5, r4]
	mov	r0, #0x20
	asr	r1, #2
	str	r0, [sp]
	lsl	r1, #11
	mov	r0, #0x40
	sub	r2, #0x10
	sub	r3, #0x20
	str	r0, [sp, #4]
	add	r1, r10
	ldr	r0, [sp, #0x34]
	ldr	r6, [sp, #0x28]
	bl	_call_via_r6
	ldr	r3, [r5, #0xc]
	ldr	r0, [sp, #0x14]
	mov	r2, r0
	mul	r2, r3
	ldr	r3, [r5]
	add	r3, r2
	str	r3, [r5]
	ldr	r2, [r5, #0x10]
	ldr	r3, [r5, #4]
	add	r3, r2
	str	r3, [r5, #4]
	ldr	r3, [r5, #0x18]
	add	r3, #1
	str	r3, [r5, #0x18]
	cmp	r3, #0x18
	bne	.Ld29fa
	mov	r3, #1
	neg	r3, r3
	str	r3, [r5, #0x18]
.Ld29fa:
	add	r7, #1
	add	r5, #0x1c
	cmp	r7, #0x18
	bne	.Ld29ac
.Ld2a02:
	ldr	r5, =0x74e0
	mov	r7, #0
	mov	r6, #1
	add	r5, r10
.Ld2a0a:
	ldr	r3, [r5, #0x18]
	cmp	r3, #0
	blt	.Ld2a56
	mov	r4, #6
	ldrsh	r3, [r5, r4]
	mov	r1, #2
	ldrsh	r2, [r5, r1]
	mov	r1, #2
	sub	r3, #1
	str	r1, [sp, #4]
	str	r6, [sp]
	ldr	r1, =.Lee188
	ldr	r0, [sp, #0x34]
	ldr	r4, [sp, #0x2c]
	bl	_call_via_r4
	ldr	r3, [r5, #0xc]
	ldr	r0, [sp, #0x14]
	mov	r2, r0
	mul	r2, r3
	ldr	r3, [r5]
	add	r3, r2
	str	r3, [r5]
	ldr	r2, [r5, #0x10]
	ldr	r3, [r5, #4]
	add	r3, r2
	str	r3, [r5, #4]
	ldr	r1, =0xfffffc00
	ldr	r3, [r5, #0x18]
	add	r2, r1
	add	r3, #1
	str	r2, [r5, #0x10]
	str	r3, [r5, #0x18]
	cmp	r3, #0x30
	bne	.Ld2a56
	mov	r3, #1
	neg	r3, r3
	str	r3, [r5, #0x18]
.Ld2a56:
	add	r7, #1
	add	r5, #0x1c
	cmp	r7, #0x18
	bne	.Ld2a0a
	ldr	r2, [sp, #0x24]
	cmp	r2, #0xaf
	bgt	.Ld2af6
	ldr	r5, =gBuffer
	mov	r7, #0
.Ld2a68:
	ldr	r3, [r5, #0x18]
	cmp	r3, #0
	blt	.Ld2aea
	ldr	r2, =.Lee18a
	mov	r3, #3
	and	r3, r7
	ldrb	r0, [r2, r3]
	ldr	r2, =Data_ede48
	lsl	r4, r0, #1
	sub	r3, r4, #2
	ldrh	r1, [r2, r3]
	ldr	r3, [sp, #0x20]
	mov	r6, #2
	ldrsh	r2, [r5, r6]
	add	r1, r3, r1
	lsr	r3, r0, #1
	sub	r2, r3
	mov	r6, #6
	ldrsh	r3, [r5, r6]
	str	r0, [sp]
	sub	r3, r0
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x34]
	ldr	r4, [sp, #0x28]
	bl	_call_via_r4
	ldr	r1, [r5, #0xc]
	ldr	r6, [sp, #0x14]
	mov	r2, r6
	mul	r2, r1
	ldr	r3, [r5]
	add	r3, r2
	str	r3, [r5]
	ldr	r2, [r5, #0x10]
	ldr	r3, [r5, #4]
	add	r3, r2
	str	r3, [r5, #4]
	ldr	r0, [sp, #0x24]
	cmp	r0, #0x80
	ble	.Ld2ad0
	mov	r3, #1
	and	r3, r7
	cmp	r3, #0
	beq	.Ld2ac8
	ldr	r2, =0xffff8000
	add	r3, r1, r2
	str	r3, [r5, #0xc]
	b	.Ld2ad6
.Ld2ac8:
	ldr	r4, =0xffffe000
	add	r3, r1, r4
	str	r3, [r5, #0xc]
	b	.Ld2ad6
.Ld2ad0:
	ldr	r6, =0xfffffc00
	add	r3, r2, r6
	str	r3, [r5, #0x10]
.Ld2ad6:
	ldr	r3, [r5, #0x18]
	mov	r0, #0x80
	add	r3, #1
	lsl	r0, #1
	str	r3, [r5, #0x18]
	cmp	r3, r0
	bne	.Ld2aea
	mov	r3, #1
	neg	r3, r3
	str	r3, [r5, #0x18]
.Ld2aea:
	mov	r1, #0x80
	add	r7, #1
	lsl	r1, #2
	add	r5, #0x1c
	cmp	r7, r1
	bne	.Ld2a68
.Ld2af6:
	ldr	r2, [sp, #0x24]
	cmp	r2, #0x80
	bne	.Ld2b04
	ldr	r2, =0x77a8
	mov	r3, #0x30
	add	r2, r10
	str	r3, [r2]
.Ld2b04:
	ldr	r3, [sp, #0x38]
	cmp	r3, #0
	bne	.Ld2b18
	ldr	r4, [sp, #0x24]
	cmp	r4, #0x30
	bne	.Ld2b18
	ldr	r2, =0x77a8
	mov	r3, #8
	add	r2, r10
	str	r3, [r2]
.Ld2b18:
	ldr	r3, [sp, #0x24]
	sub	r3, #0x28
	cmp	r3, #7
	bhi	.Ld2b32
	ldr	r6, [sp, #0x18]
	ldr	r1, [sp, #0x1c]
	ldr	r0, =0xfff80000
	mov	r2, #0x80
	lsl	r2, #13
	add	r0, r6, r0
	add	r2, r1, r2
	str	r0, [sp, #0x18]
	str	r2, [sp, #0x1c]
.Ld2b32:
	ldr	r3, [sp, #0x38]
	cmp	r3, #0
	bne	.Ld2bae
	ldr	r4, [sp, #0x24]
	cmp	r4, #0x80
	bne	.Ld2b6e
	ldr	r3, =0x77d8
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #8
	bl	_Sprite_SetAnim
	ldr	r3, =0x77dc
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #9
	bl	_Sprite_SetAnim
	ldr	r3, =0x77e4
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #0xa
	bl	_Sprite_SetAnim
	ldr	r3, =0x77e8
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #0xb
	bl	_Sprite_SetAnim
.Ld2b6e:
	ldr	r6, [sp, #0x24]
	cmp	r6, #0xb0
	bne	.Ld2ba4
	ldr	r3, =0x77d8
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #0
	bl	_Sprite_SetAnim
	ldr	r3, =0x77dc
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #1
	bl	_Sprite_SetAnim
	ldr	r3, =0x77e4
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #3
	bl	_Sprite_SetAnim
	ldr	r3, =0x77e8
	add	r3, r10
	ldr	r0, [r3]
	mov	r1, #4
	bl	_Sprite_SetAnim
.Ld2ba4:
	mov	r0, #3
	ldr	r1, [sp, #0x18]
	ldr	r2, [sp, #0x1c]
	bl	Func_80e6d3c
.Ld2bae:
	ldr	r0, [sp, #0x24]
	cmp	r0, #0x8a
	bne	.Ld2bf0
	ldr	r3, =0x7828
	mov	r1, r10
	ldr	r3, [r1, r3]
	ldr	r3, [r3, #0x14]
	mov	r7, #0
	cmp	r3, #0
	beq	.Ld2bf0
	ldr	r5, =0x7828
	mov	r6, #0x24
	add	r5, r10
.Ld2bc8:
	ldr	r3, [r5]
	ldrsh	r0, [r3, r6]
	mov	r3, #0
	str	r3, [sp]
	mov	r1, #0xa
	mov	r2, #5
	sub	r3, #1
	bl	Func_80d6888
	ldr	r3, [r5]
	mov	r1, #4
	ldrsh	r0, [r3, r6]
	bl	_SetBattleActorKnockback
	ldr	r3, [r5]
	ldr	r3, [r3, #0x14]
	add	r7, #1
	add	r6, #2
	cmp	r7, r3
	bne	.Ld2bc8
.Ld2bf0:
	ldr	r6, [sp, #0x24]
	cmp	r6, #0xaf
	ble	.Ld2bf8
	b	.Ld2d16
.Ld2bf8:
	ldr	r5, =0x7320
	mov	r0, #0
	mov	r1, #0x14
	mov	r2, #5
	mov	r7, #0
	mov	r11, r0
	mov	r8, r1
	mov	r9, r2
	add	r5, r10
.Ld2c0a:
	ldr	r6, [r5, #4]
	cmp	r6, #0x37
	ble	.Ld2cb4
	ldr	r3, [r5, #0x18]
	cmp	r3, #0xb
	bhi	.Ld2c4c
	lsr	r4, r3, #31
	add	r4, r3, r4
	asr	r4, #1
	ldr	r2, =.Lee1a0
	lsl	r3, r4, #1
	ldrh	r1, [r2, r3]
	mov	r3, #0xc0
	lsl	r3, #6
	add	r1, r10
	add	r1, r3
	ldr	r3, =.Lee18e
	ldrb	r0, [r3, r4]
	ldr	r2, [r5]
	lsr	r3, r0, #1
	sub	r2, r3
	ldr	r3, =.Lee19a
	ldrb	r3, [r3, r4]
	str	r0, [sp]
	ldr	r0, =.Lee194
	ldrb	r0, [r0, r4]
	add	r3, r6, r3
	str	r0, [sp, #4]
	ldr	r4, [sp, #0x28]
	ldr	r0, [sp, #0x34]
	bl	_call_via_r4
	ldr	r3, [r5, #0x18]
.Ld2c4c:
	add	r3, #1
	str	r3, [r5, #0x18]
	cmp	r3, #0xc
	bne	.Ld2d0c
	mov	r6, r11
	str	r6, [r5, #0x18]
	b	.Ld2d0c

	.pool_aligned

.Ld2cb4:
	ldr	r3, [r5, #0x18]
	cmp	r3, #0
	bne	.Ld2d08
	ldr	r0, [sp, #0x14]
	lsl	r3, r0, #1
	ldr	r2, [r5]
	add	r3, r0
	lsl	r3, #1
	sub	r2, r3
	add	r3, r6, #6
	str	r2, [r5]
	str	r3, [r5, #4]
	ldr	r1, [sp, #0x24]
	mov	r4, #0xa
	cmp	r1, #0x2f
	bgt	.Ld2ce2
	cmp	r3, #0x37
	ble	.Ld2ce2
	mov	r0, #0x88
	str	r4, [sp, #8]
	bl	_PlaySound
	ldr	r4, [sp, #8]
.Ld2ce2:
	ldr	r2, =Data_ede48
	mov	r3, r8
	sub	r3, #2
	ldrh	r1, [r2, r3]
	ldr	r2, [sp, #0x20]
	add	r1, r2, r1
	ldr	r2, [r5]
	mov	r3, r9
	sub	r2, r3
	ldr	r3, [r5, #4]
	str	r4, [sp]
	mov	r4, r8
	add	r3, #0x1e
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x34]
	ldr	r6, [sp, #0x28]
	bl	_call_via_r6
	b	.Ld2d0c
.Ld2d08:
	add	r3, #1
	str	r3, [r5, #0x18]
.Ld2d0c:
	add	r7, #1
	add	r5, #0x1c
	cmp	r7, #0x10
	beq	.Ld2d16
	b	.Ld2c0a
.Ld2d16:
	mov	r0, #8
	mov	r1, #8
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r10
	mov	r0, #1
	str	r3, [r2]
	bl	WaitFrames
	ldr	r0, [sp, #0x24]
	add	r0, #1
	str	r0, [sp, #0x24]
	cmp	r0, #0xd0
	beq	.Ld2d3c
	b	.Ld264e
.Ld2d3c:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	ldr	r1, [sp, #0x38]
	cmp	r1, #0
	bne	.Ld2d70
	mov	r0, #3
	ldr	r1, [sp, #0x18]
	ldr	r2, [sp, #0x1c]
	bl	Anim_Unsummon
	ldr	r5, =0x77d8
	mov	r7, #0
	add	r5, r10
.Ld2d64:
	ldmia	r5!, {r0}
	add	r7, #1
	bl	_DeleteSprite
	cmp	r7, #8
	bne	.Ld2d64
.Ld2d70:
	bl	AnimEnd
	add	sp, #0x48
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end BaseAnim_Tiamat

	.section .rodata

.Lee184:
	.incrom 0xee184, 0xee188
.Lee188:
	.incrom 0xee188, 0xee18a
.Lee18a:
	.incrom 0xee18a, 0xee18e
.Lee18e:
	.incrom 0xee18e, 0xee194
.Lee194:
	.incrom 0xee194, 0xee19a
.Lee19a:
	.incrom 0xee19a, 0xee1a0
.Lee1a0:
	.incrom 0xee1a0, 0xee1ac
