	.include "macros.inc"

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
@                  with slot 0x0E when they are all busy. MPlayStart starts it and
@                  the id is recorded at ewram_3020 + slot*2.
@     0x050..0x063 JINGLE. Fades the main player, zeroes the volume pair, starts
@                  the song with m4aSongNumStart and sets ewram_3000 to 0x0A -- the
@                  countdown after which UpdateMusicSettings restores the music.
@     up to 0x04F  MUSIC. Skipped when it is already playing (ewram_303c).
@                  SetSoundFXMode is called with 3 for ids 0x43, 0x46 and 0x4B and 2
@                  otherwise. Then m4aSongNumStart starts it, and the volume state is
@                  seeded: BIT 12 OF THE ARGUMENT starts the track silent
@                  (ewram_3008 = 0) so it fades in, otherwise it starts at full
@                  (0x100). ewram_3034 is the target, ewram_3010 = 4 the step.
@
@ The volume state block, used by UpdateMusicSettings every frame:
@
@     ewram_3000  jingle countdown        ewram_3008  current volume
@     ewram_3010  fade step               ewram_3014  fade-out latch
@     ewram_3020  8 halfwords, the id playing in each effect slot
@     ewram_3034  target volume           ewram_303c  the current music id
.thumb_func_start PlaySound  @ 0x080f9080
	push	{r5, r6, r7, lr}
	mov	r5, #0xf0
	ldr	r3, =0xfff
	mov	r6, r0
	lsl	r5, #8
	and	r5, r6
	and	r6, r3
	cmp	r6, #0x11
	bne	.Lf90b0
	ldr	r5, =ewram_2003014
	ldrb	r3, [r5]
	cmp	r3, #0
	beq	.Lf909c
	b	.Lf91e0
.Lf909c:
	ldr	r0, =gMPlayInfo_BGM
	mov	r1, #7
	bl	m4aMPlayFadeOut
	ldrb	r3, [r5]
	ldr	r2, =ewram_200303c
	add	r3, #1
	strb	r3, [r5]
	mov	r3, #0x13
	b	.Lf91c2
.Lf90b0:
	ldr	r3, =0x121
	cmp	r6, r3
	bne	.Lf90c6
	ldr	r3, =ewram_2003020
	mov	r2, #0
	strh	r2, [r3, #6]
	ldr	r0, =gMPlayInfo_02004360
	mov	r1, #3
	bl	m4aMPlayFadeOut
	b	.Lf91e0
.Lf90c6:
	cmp	r6, #0x63
	ble	.Lf9108
	ldr	r7, =gSongTable
	lsl	r4, r6, #3
	add	r3, r4, #4
	ldrh	r2, [r7, r3]
	cmp	r2, #7
	bne	.Lf90f2
	ldr	r1, =gMPlayTable
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
	ldr	r1, =gMPlayTable
	lsl	r5, r2, #1
.Lf90f6:
	add	r3, r5, r2
	lsl	r3, #2
	ldr	r0, [r1, r3]
	ldr	r1, [r7, r4]
	bl	MPlayStart
	ldr	r3, =ewram_2003020
	strh	r6, [r3, r5]
	b	.Lf91e0
.Lf9108:
	cmp	r6, #0x4f
	ble	.Lf9164
	ldr	r0, =gMPlayInfo_BGM
	mov	r1, #0xff
	mov	r2, #0
	bl	m4aMPlayVolumeControl
	ldr	r2, .Lf9130	@ 0
	ldr	r3, =gMusicVolume
	strh	r2, [r3]
	ldr	r3, =gMusicCurVolume
	lsl	r0, r6, #16
	strh	r2, [r3]
	lsr	r0, #16
	bl	m4aSongNumStart
	ldr	r2, =ewram_2003000
	mov	r3, #0xa
	b	.Lf91c2

	.align	2, 0
.Lf9130:
	.word	0
	.pool

.Lf9164:
	cmp	r6, #0x12
	beq	.Lf91e0
	ldr	r2, =ewram_200303c
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
	bl	SetSoundFXMode
	lsl	r0, r6, #16
	lsr	r0, #16
	bl	m4aSongNumStart
	mov	r3, #0x80
	lsl	r3, #5
	and	r3, r5
	cmp	r3, #0
	beq	.Lf91ac
	ldr	r2, =gMusicCurVolume
	ldr	r3, .Lf91a0	@ 0
	b	.Lf91b0

	.align	2, 0
.Lf91a0:
	.word	0
	.pool

.Lf91ac:
	ldr	r2, =gMusicCurVolume
	ldr	r3, .Lf91c8	@ 0x100
.Lf91b0:
	strh	r3, [r2]
	ldr	r2, =gMusicVolume
	ldr	r3, .Lf91c8	@ 0x100
	strh	r3, [r2]
	ldr	r2, =gMusicVolumeDelta
	ldr	r3, .Lf91cc	@ 4
	strh	r3, [r2]
	ldr	r2, =ewram_2003014
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
.func_end PlaySound
