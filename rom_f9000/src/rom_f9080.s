	.include "macros.inc"

@ ============================================================================
@ The sound driver.
@
@ 110 functions. This is Nintendo's M4A driver -- the one the SDK shipped and
@ every GBA game with streamed audio uses -- with a thin game-specific layer on
@ top. Three pieces of evidence pin it down and none of them is a guess:
@
@   * the driver block at iwram_7ff0 begins with 0x68736D53, the ASCII 'Smsh'
@     identifier, and EVERY public entry point checks it before touching
@     anything (Func_fa264, Func_fa280, Func_fb2a4 and the rest)
@   * the track command base is 0xB1: Func_f9c90 subtracts it to index the
@     36-word jump table at .Lfb7a0, so 0xB1 is FINE, 0xB2 GOTO, 0xB3 PATT,
@     0xBB TEMPO, 0xBD VOICE, 0xC0 BEND, and so on
@   * Func_f9a80 copies that whole table into the player, which is how the
@     engine calls its own commands and helpers by index
@
@ THE GAME'S INTERFACE IS ONE FUNCTION. Func_f9080(id) has 1971 call sites --
@ more than any other function in the ROM by a wide margin -- and routes the id
@ by range: 0x11 and 0x121 are fades, 0x12 is ignored, 0x50..0x63 are jingles,
@ anything above 0x63 is a sound effect, and the rest is music. Bit 12 of the
@ argument makes music start silent so it fades in.
@
@ TWO TABLES drive it:
@
@     Data_fc624   the PLAYER table, 8 entries of 12 bytes
@     Data_fc684   the SONG table, 8 bytes per song -- header at +0, player
@                  index at +4
@
@ Player 7 is the shared effect player: Func_f9080 scans players 7 down to 4 for
@ one whose status word is zero and takes the first free one, falling back to 7
@ when they are all busy.
@
@ THE VOLUME STATE, all in EWRAM and all ticked by Func_f91e8 once a frame:
@
@     ewram_3000  jingle countdown       ewram_3008  current volume
@     ewram_3010  fade step              ewram_3014  fade-out latch
@     ewram_3020  8 halfwords, the id playing in each effect slot
@     ewram_3034  target volume          ewram_303c  the current music id
@
@ THE PLAYER (r0 in every command handler): +0x04 status with bit 31 = paused,
@ +0x1C base tempo, +0x1E tempo scale, +0x20 the effective rate, +0x24/+0x26
@ fade reload and counter, +0x28 fade direction, +0x30 the voice group, +0x34
@ the ident, +0x3C a per-tick callback.
@
@ THE TRACK (r1): +0x00 flags, +0x02 pattern depth, +0x03 repeat counter,
@ +0x07 running status, +0x0A key shift, +0x0C tune, +0x0E bend, +0x0F bend
@ range, +0x12 volume, +0x14 pan, +0x16/+0x1A modulation accumulators, +0x18
@ modulation type, +0x19 LFO speed, +0x1B LFO delay, +0x1D priority,
@ +0x1E/+0x1F echo, +0x20 the channel list head, +0x24..+0x2F the 12-byte
@ instrument record, +0x40 the command pointer, +0x44 a THREE-DEEP pattern
@ stack.
@
@ Flag bits 0 and 1 mean "volume or pan changed" and bits 2 and 3 "pitch
@ changed"; every command handler raises the pair that matches what it wrote,
@ and Func_f9f6c acts on them.
@
@ ROBUSTNESS. Func_f9a98 is a guarded byte read that returns zero for a pointer
@ outside the expected range, and every stream reader goes through it -- a
@ corrupt song reads as zeros instead of faulting. Eleven of the 36 jump-table
@ entries point at Func_f9a50, which simply ends the track, so an unimplemented
@ command byte cannot run off into the data.
@
@ THE TABLE IS VERIFIED, not inferred. Its 36 entries match pret's published
@ gMPlayJumpTableTemplate position for position, so the correspondence is exact:
@
@     0 fine     1 goto     2 patt     3 pend     4 rept    5-8 fine
@     9 prio    10 tempo   11 keysh   12 voice   13 vol    14 pan
@    15 bend    16 bendr   17 lfos    18 lfodl   19 mod    20 modt
@    21-22 fine 23 tune    24-26 fine 27 port    28 fine   29 endtie
@    30 SampleFreqSet      31 TrackStop         32 FadeOutBody
@    33 TrkVolPitSet       34 RealClearChain    35 SoundMainBTM
@
@ Command byte = index + 0xB1 for entries 0..29; 30..35 are helper pointers the
@ engine calls indirectly rather than commands a song can issue.
@
@ Func_f92fc is a DEBUG SOUND TEST that steps through ids on the d-pad. Nothing
@ calls it; it is reachable only through its export veneer, like rom_b5000's
@ Func_b56e0 and rom_b0000's Func_b0444.
@ ============================================================================

@ PlaySound
@ r0 = a sound id in bits 0..11, flags in bits 12..15. THE MOST-CALLED FUNCTION
@ IN THE ROM -- 1971 call sites across every module and every overlay. Nothing
@ is returned.
@
@ It routes the id by range, and the ranges are the whole interface:
@
@     0x011        fade the main player at ewram_4290 out over 7 steps, bump the
@                  fade latch at ewram_3014 and set the current-track byte
@                  ewram_303c to 0x13
@     0x121        fade the player at ewram_4360 out over 3 steps and clear
@                  ewram_3020+6
@     0x012        ignored outright
@     0x064..0xFFF SOUND EFFECT. Data_fc684 + id*8 gives the song header at +0
@                  and the player index at +4. Player 7 is the shared effect
@                  player: it scans players 7 down to 4 for one whose status word
@                  is zero and takes the first free one, falling back to player 7
@                  with slot 0x0E when they are all busy. Func_faa58 starts it and
@                  the id is recorded at ewram_3020 + slot*2.
@     0x050..0x063 JINGLE. Fades the main player, zeroes the volume pair, starts
@                  the song with Func_fa324 and sets ewram_3000 to 0x0A -- the
@                  countdown after which Func_f91e8 restores the music.
@     up to 0x04F  MUSIC. Skipped when it is already playing (ewram_303c).
@                  Func_37d4 is called with 3 for ids 0x43, 0x46 and 0x4B and 2
@                  otherwise. Then Func_fa324 starts it, and the volume state is
@                  seeded: BIT 12 OF THE ARGUMENT starts the track silent
@                  (ewram_3008 = 0) so it fades in, otherwise it starts at full
@                  (0x100). ewram_3034 is the target, ewram_3010 = 4 the step.
@
@ The volume state block, used by Func_f91e8 every frame:
@
@     ewram_3000  jingle countdown        ewram_3008  current volume
@     ewram_3010  fade step               ewram_3014  fade-out latch
@     ewram_3020  8 halfwords, the id playing in each effect slot
@     ewram_3034  target volume           ewram_303c  the current music id
.thumb_func_start Func_f9080
	push	{r5, r6, r7, lr}
	mov	r5, #0xf0
	ldr	r3, =0xfff
	mov	r6, r0
	lsl	r5, #8
	and	r5, r6
	and	r6, r3
	cmp	r6, #0x11
	bne	.Lf90b0
	ldr	r5, =ewram_3014
	ldrb	r3, [r5]
	cmp	r3, #0
	beq	.Lf909c
	b	.Lf91e0
.Lf909c:
	ldr	r0, =ewram_4290
	mov	r1, #7
	bl	Func_fa4bc
	ldrb	r3, [r5]
	ldr	r2, =ewram_303c
	add	r3, #1
	strb	r3, [r5]
	mov	r3, #0x13
	b	.Lf91c2
.Lf90b0:
	ldr	r3, =0x121
	cmp	r6, r3
	bne	.Lf90c6
	ldr	r3, =ewram_3020
	mov	r2, #0
	strh	r2, [r3, #6]
	ldr	r0, =ewram_4360
	mov	r1, #3
	bl	Func_fa4bc
	b	.Lf91e0
.Lf90c6:
	cmp	r6, #0x63
	ble	.Lf9108
	ldr	r7, =Data_fc684
	lsl	r4, r6, #3
	add	r3, r4, #4
	ldrh	r2, [r7, r3]
	cmp	r2, #7
	bne	.Lf90f2
	ldr	r1, =Data_fc624
.Lf90d8:
	lsl	r5, r2, #1
	add	r3, r5, r2
	lsl	r3, #2
	ldr	r3, [r1, r3]
	ldrb	r3, [r3, #4]
	cmp	r3, #0
	beq	.Lf90f6
	sub	r2, #1
	cmp	r2, #3
	bgt	.Lf90d8
	mov	r2, #7
	mov	r5, #0xe
	b	.Lf90f6
.Lf90f2:
	ldr	r1, =Data_fc624
	lsl	r5, r2, #1
.Lf90f6:
	add	r3, r5, r2
	lsl	r3, #2
	ldr	r0, [r1, r3]
	ldr	r1, [r7, r4]
	bl	Func_faa58
	ldr	r3, =ewram_3020
	strh	r6, [r3, r5]
	b	.Lf91e0
.Lf9108:
	cmp	r6, #0x4f
	ble	.Lf9164
	ldr	r0, =ewram_4290
	mov	r1, #0xff
	mov	r2, #0
	bl	Func_fb2cc
	ldr	r2, .Lf9130	@ 0
	ldr	r3, =ewram_3034
	strh	r2, [r3]
	ldr	r3, =ewram_3008
	lsl	r0, r6, #16
	strh	r2, [r3]
	lsr	r0, #16
	bl	Func_fa324
	ldr	r2, =ewram_3000
	mov	r3, #0xa
	b	.Lf91c2

	.align	2, 0
.Lf9130:
	.word	0
	.pool

.Lf9164:
	cmp	r6, #0x12
	beq	.Lf91e0
	ldr	r2, =ewram_303c
	ldrb	r3, [r2]
	cmp	r6, r3
	beq	.Lf91e0
	strb	r6, [r2]
	cmp	r6, #0x46
	beq	.Lf9180
	cmp	r6, #0x4b
	beq	.Lf9180
	mov	r0, #2
	cmp	r6, #0x43
	bne	.Lf9182
.Lf9180:
	mov	r0, #3
.Lf9182:
	bl	Func_37d4
	lsl	r0, r6, #16
	lsr	r0, #16
	bl	Func_fa324
	mov	r3, #0x80
	lsl	r3, #5
	and	r3, r5
	cmp	r3, #0
	beq	.Lf91ac
	ldr	r2, =ewram_3008
	ldr	r3, .Lf91a0	@ 0
	b	.Lf91b0

	.align	2, 0
.Lf91a0:
	.word	0
	.pool

.Lf91ac:
	ldr	r2, =ewram_3008
	ldr	r3, .Lf91c8	@ 0x100
.Lf91b0:
	strh	r3, [r2]
	ldr	r2, =ewram_3034
	ldr	r3, .Lf91c8	@ 0x100
	strh	r3, [r2]
	ldr	r2, =ewram_3010
	ldr	r3, .Lf91cc	@ 4
	strh	r3, [r2]
	ldr	r2, =ewram_3014
	mov	r3, #0
.Lf91c2:
	strb	r3, [r2]
	b	.Lf91e0

	.align	2, 0
.Lf91c8:
	.word	0x100
.Lf91cc:
	.word	4
	.pool

.Lf91e0:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_f9080

@ UpdateSoundVolume
@ Called every frame. Runs the volume fade toward the target: while ewram_3008
@ differs from ewram_3034 it moves by the step at ewram_3010 and pushes the
@ result to the player with Func_fb2a4. It also ticks the jingle countdown at
@ ewram_3000 -- when it reaches 1 and the jingle player at ewram_4210 has gone
@ idle, the music is restored to full volume. Func_fb334 and Func_fb2cc apply
@ the result to the players and Func_f9c44 keeps the DMA fed.
.thumb_func_start Func_f91e8
	push	{r5, r6, lr}
	ldr	r1, =ewram_3000
	ldrb	r3, [r1]
	mov	r2, r3
	cmp	r2, #0
	beq	.Lf920e
	cmp	r2, #1
	bne	.Lf920a
	ldr	r3, =ewram_4210
	ldrb	r3, [r3, #4]
	cmp	r3, #0
	bne	.Lf920e
	strb	r3, [r1]
	ldr	r2, =ewram_3034
	ldr	r3, .Lf9230	@ 0x100
	strh	r3, [r2]
	b	.Lf920e
.Lf920a:
	add	r3, #0xff
	strb	r3, [r1]
.Lf920e:
	ldr	r3, =ewram_3034
	ldr	r1, =ewram_3008
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	mov	r5, #0
	ldrsh	r3, [r1, r5]
	ldrh	r4, [r1]
	cmp	r2, r3
	beq	.Lf9272
	sub	r0, r2, r3
	cmp	r0, #0
	ble	.Lf9248
	ldr	r3, =ewram_3010
	ldrh	r3, [r3]
	add	r3, r4, r3
	b	.Lf924e

	.align	2, 0
.Lf9230:
	.word	0x100
	.pool

.Lf9248:
	ldr	r3, =ewram_3010
	ldrh	r3, [r3]
	sub	r3, r4, r3
.Lf924e:
	strh	r3, [r1]
	ldr	r3, =ewram_3034
	ldr	r1, =ewram_3008
	ldrh	r4, [r3]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r5, #0
	ldrsh	r2, [r1, r5]
	sub	r3, r2
	eor	r3, r0
	cmp	r3, #0
	bge	.Lf9268
	strh	r4, [r1]
.Lf9268:
	ldrh	r2, [r1]
	ldr	r0, =ewram_4290
	mov	r1, #0xff
	bl	Func_fb2cc
.Lf9272:
	ldr	r3, =ewram_3030
	ldr	r1, =ewram_3038
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	mov	r5, #0
	ldrsh	r3, [r1, r5]
	ldrh	r4, [r1]
	cmp	r2, r3
	beq	.Lf92d6
	sub	r0, r2, r3
	cmp	r0, #0
	ble	.Lf9292
	ldr	r3, =ewram_300c
	ldrh	r3, [r3]
	add	r3, r4, r3
	b	.Lf9298
.Lf9292:
	ldr	r3, =ewram_300c
	ldrh	r3, [r3]
	sub	r3, r4, r3
.Lf9298:
	strh	r3, [r1]
	ldr	r3, =ewram_3030
	ldr	r6, =ewram_3038
	ldrh	r1, [r3]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r4, #0
	ldrsh	r2, [r6, r4]
	sub	r3, r2
	eor	r3, r0
	cmp	r3, #0
	bge	.Lf92b2
	strh	r1, [r6]
.Lf92b2:
	ldr	r5, =ewram_4290
	ldrh	r1, [r6]
	mov	r0, r5
	bl	Func_fb2a4
	mov	r0, #0
	ldrsh	r3, [r6, r0]
	lsl	r2, r3, #1
	add	r2, r3
	mov	r3, #0xf4
	lsl	r2, #18
	lsl	r3, #24
	add	r2, r3
	asr	r2, #16
	mov	r0, r5
	mov	r1, #0xff
	bl	Func_fb334
.Lf92d6:
	bl	Func_f9c44
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_f91e8

@ RunSoundTestLoop
@ Takes no arguments. A DEBUG SOUND TEST: it loops on Func_30f8(1) reading the
@ auto-repeat key state at iwram_1b04, steps an id with Func_b1c wrapping, plays
@ it with Func_f9080 and stashes the value at iwram_7804. Func_37d4 sets the
@ priority. Nothing in the ROM calls it -- it is reachable only through its
@ export veneer, like rom_b5000's Func_b56e0 and rom_b0000's Func_b0444.
.thumb_func_start Func_f92fc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xc
	mov	r9, sp
	ldr	r2, =Lfb794
	mov	r3, r9
	ldmia	r2!, {r0, r1, r4}
	stmia	r3!, {r0, r1, r4}
	mov	r0, #2
	mov	r7, #0
	mov	r11, r0
	ldr	r3, =iwram_7804
	str	r7, [r3]
	mov	r1, #0x14
	mov	r2, #0
	mov	r10, r1
	mov	r8, r2
	mov	r6, r9
.Lf932a:
	mov	r3, r10
	cmp	r3, #0
	beq	.Lf9336
	mov	r4, #1
	neg	r4, r4
	add	r10, r4
.Lf9336:
	ldr	r2, =Label_12cc
	ldr	r3, [r2]
	cmp	r3, #0
	beq	.Lf9346
	mov	r3, #0
	mov	r0, #0x14
	str	r3, [r2]
	mov	r10, r0
.Lf9346:
	ldr	r5, =iwram_1b04
	ldr	r3, [r5]
	mov	r2, #4
	and	r3, r2
	cmp	r3, #0
	beq	.Lf9362
	mov	r0, r11
	add	r0, #1
	mov	r1, #5
	bl	Func_b1c_from_thumb
	mov	r11, r0
	bl	Func_37d4
.Lf9362:
	ldr	r3, [r5]
	mov	r2, #0x80
	lsl	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lf9374
	ldr	r3, [r6]
	add	r3, #0xa
	str	r3, [r6]
.Lf9374:
	ldr	r3, [r5]
	mov	r2, #0x80
	lsl	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lf9386
	ldr	r3, [r6]
	sub	r3, #0xa
	str	r3, [r6]
.Lf9386:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.Lf9396
	ldr	r3, [r6]
	add	r3, #1
	str	r3, [r6]
.Lf9396:
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.Lf93a6
	ldr	r3, [r6]
	sub	r3, #1
	str	r3, [r6]
.Lf93a6:
	ldr	r3, [r5]
	mov	r2, #0x40
	and	r3, r2
	cmp	r3, #0
	beq	.Lf93be
	cmp	r7, #0
	ble	.Lf93be
	mov	r1, #4
	neg	r1, r1
	sub	r6, #4
	add	r8, r1
	sub	r7, #1
.Lf93be:
	ldr	r5, =iwram_1b04
	ldr	r3, [r5]
	mov	r2, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.Lf93d6
	cmp	r7, #1
	bgt	.Lf93d6
	mov	r2, #4
	add	r6, #4
	add	r8, r2
	add	r7, #1
.Lf93d6:
	ldr	r3, [r5]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lf93ea
	mov	r3, r8
	mov	r4, r9
	ldr	r0, [r3, r4]
	bl	Func_f9080
.Lf93ea:
	ldr	r3, [r5]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lf93fa
	mov	r0, #0x13
	bl	Func_f9080
.Lf93fa:
	ldr	r3, [r5]
	mov	r2, #8
	and	r3, r2
	cmp	r3, #0
	beq	.Lf940a
	mov	r0, #0x11
	bl	Func_f9080
.Lf940a:
	ldr	r3, [r5]
	mov	r2, #4
	and	r3, r2
	cmp	r3, #0
	beq	.Lf941a
	ldr	r0, =0x121
	bl	Func_f9080
.Lf941a:
	mov	r0, #1
	bl	Func_30f8
	b	.Lf932a
.func_end Func_f92fc

@ ResetSoundState
@ Takes no arguments. Zeroes the whole volume state block -- ewram_3000, 3004,
@ 3008, 300C, 3010, 3014, 3020 and 3030 -- and calls Func_fa2a0 to bring the
@ driver up. rom_c0's Func_2e00 calls this at boot; it is the only caller.
.thumb_func_start Func_f9438
	push	{lr}
	bl	Func_fa2a0
	ldr	r2, =ewram_303c
	mov	r3, #0xff
	strb	r3, [r2]
	ldr	r3, =ewram_3000
	mov	r1, #0
	strb	r1, [r3]
	ldr	r2, .Lf947c	@ 0x100
	ldr	r3, =ewram_3034
	strh	r2, [r3]
	ldr	r3, =ewram_3008
	ldr	r0, .Lf9480	@ 4
	strh	r2, [r3]
	ldr	r3, =ewram_3010
	strh	r0, [r3]
	ldr	r3, =ewram_3030
	strh	r2, [r3]
	ldr	r3, =ewram_3038
	strh	r2, [r3]
	ldr	r3, =ewram_300c
	strh	r0, [r3]
	ldr	r3, =ewram_3014
	strb	r1, [r3]
	ldr	r3, =ewram_3040
	strb	r1, [r3]
	ldr	r3, =ewram_3004
	ldr	r2, =ewram_3020
	strb	r1, [r3]
	ldr	r1, .Lf9484	@ 0
	mov	r3, #7
	b	.Lf94b8

	.align	2, 0
.Lf947c:
	.word	0x100
.Lf9480:
	.word	4
.Lf9484:
	.word	0
	.pool

.Lf94b8:
	sub	r3, #1
	strh	r1, [r2]
	add	r2, #2
	cmp	r3, #0
	bge	.Lf94b8
	pop	{r0}
	bx	r0
.func_end Func_f9438

@ SetMusicVolume
@ r0 = volume. Applies it to the main player at ewram_4290 through Func_fb2a4.
.thumb_func_start Func_f94c8
	push	{lr}
	mov	r1, r0
	lsl	r1, #16
	ldr	r0, =ewram_4290
	lsr	r1, #16
	bl	Func_fb2a4
	pop	{r0}
	bx	r0
.func_end Func_f94c8

@ SetMusicPitch
@ r0 = value. Applies it to the main player through Func_fb334.
.thumb_func_start Func_f94e0
	push	{lr}
	mov	r2, r0
	lsl	r2, #16
	ldr	r0, =ewram_4290
	asr	r2, #16
	mov	r1, #0xff
	bl	Func_fb334
	pop	{r0}
	bx	r0
.func_end Func_f94e0

@ ClearFadeState
@ Takes no arguments. Zeroes ewram_300C and ewram_3030.
.thumb_func_start Func_f94f8
	ldr	r3, =ewram_3030
	strh	r0, [r3]
	ldr	r3, =ewram_300c
	strh	r1, [r3]
	bx	lr
.func_end Func_f94f8

@ FadeMusicOut
@ r0 = the fade length. Records it at ewram_3008 and ewram_3034 and starts the
@ fade on the main player with Func_fb2cc.
.thumb_func_start Func_f950c
	push	{r5, lr}
	mov	r2, r0
	lsl	r2, #16
	asr	r5, r2, #16
	ldr	r0, =ewram_4290
	lsr	r2, #16
	mov	r1, #0xff
	bl	Func_fb2cc
	ldr	r3, =ewram_3034
	strh	r5, [r3]
	ldr	r3, =ewram_3008
	strh	r5, [r3]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_f950c

@ SetVolumeTarget
@ r0 = target. Writes ewram_3010 and ewram_3034 -- the step and the target the
@ per-frame fade in Func_f91e8 walks toward.
.thumb_func_start Func_f9538
	ldr	r3, =ewram_3034
	strh	r0, [r3]
	ldr	r3, =ewram_3010
	strh	r1, [r3]
	bx	lr
.func_end Func_f9538

@ SetJingleCountdown
@ r0 = frames. Stores the byte at ewram_3000.
.thumb_func_start Func_f954c
	ldr	r3, =ewram_3000
	ldrb	r0, [r3]
	bx	lr
.func_end Func_f954c
