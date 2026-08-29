	.include "macros.inc"
	.include "gba.inc"

@ StartSong
@ r0 = song id. Resolves it through the two tables and starts it:
@
@     Data_fc684 + id*8    the SONG TABLE -- header pointer at +0, player index
@                          at +4
@     Data_fc624 + n*12    the PLAYER TABLE -- eight players, 12 bytes each
@
@ Func_faa58 does the actual start. Every music and jingle path in Func_f9080
@ funnels through here.
.thumb_func_start Func_fa324
	push	{lr}
	lsl	r0, #16
	ldr	r2, =Data_fc624
	ldr	r1, =Data_fc684
	lsr	r0, #13
	add	r0, r1
	ldrh	r3, [r0, #4]
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #2
	add	r1, r2
	ldr	r2, [r1]
	ldr	r1, [r0]
	mov	r0, r2
	bl	Func_faa58
	pop	{r0}
	bx	r0
.func_end Func_fa324

@ StartSongOnPlayer
@ r0 = song id, r1 = an override. As Func_fa324 but with the player chosen by
@ the caller rather than by the song table.
.thumb_func_start Func_fa350
	push	{lr}
	lsl	r0, #16
	ldr	r2, =Data_fc624
	ldr	r1, =Data_fc684
	lsr	r0, #13
	add	r0, r1
	ldrh	r3, [r0, #4]
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #2
	add	r1, r2
	ldr	r1, [r1]
	ldr	r3, [r1]
	ldr	r2, [r0]
	cmp	r3, r2
	beq	.Lfa384
	mov	r0, r1
	mov	r1, r2
	bl	Func_faa58
	b	.Lfa398

	.pool_aligned

.Lfa384:
	ldr	r2, [r1, #4]
	ldrh	r0, [r1, #4]
	cmp	r0, #0
	beq	.Lfa390
	cmp	r2, #0
	bge	.Lfa398
.Lfa390:
	mov	r0, r1
	mov	r1, r3
	bl	Func_faa58
.Lfa398:
	pop	{r0}
	bx	r0
.func_end Func_fa350

@ StartSongResumed
@ r0 = song id. Func_fa324 with a Func_fa264 first, so a player that was paused
@ starts playing again rather than staying stopped.
.thumb_func_start Func_fa39c
	push	{lr}
	lsl	r0, #16
	ldr	r2, =Data_fc624
	ldr	r1, =Data_fc684
	lsr	r0, #13
	add	r0, r1
	ldrh	r3, [r0, #4]
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #2
	add	r1, r2
	ldr	r1, [r1]
	ldr	r3, [r1]
	ldr	r2, [r0]
	cmp	r3, r2
	beq	.Lfa3d0
	mov	r0, r1
	mov	r1, r2
	bl	Func_faa58
	b	.Lfa3ec

	.pool_aligned

.Lfa3d0:
	ldr	r2, [r1, #4]
	ldrh	r0, [r1, #4]
	cmp	r0, #0
	bne	.Lfa3e2
	mov	r0, r1
	mov	r1, r3
	bl	Func_faa58
	b	.Lfa3ec
.Lfa3e2:
	cmp	r2, #0
	bge	.Lfa3ec
	mov	r0, r1
	bl	Func_fa264
.Lfa3ec:
	pop	{r0}
	bx	r0
.func_end Func_fa39c

@ StopSong
@ r0 = song id. Looks the song up the same way Func_fa324 does and stops its
@ player with Func_fab3c -- but ONLY when the player is still running that
@ song, so stopping a song that has already been replaced does nothing.
.thumb_func_start Func_fa3f0
	push	{lr}
	lsl	r0, #16
	ldr	r2, =Data_fc624
	ldr	r1, =Data_fc684
	lsr	r0, #13
	add	r0, r1
	ldrh	r3, [r0, #4]
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #2
	add	r1, r2
	ldr	r2, [r1]
	ldr	r1, [r2]
	ldr	r0, [r0]
	cmp	r1, r0
	bne	.Lfa416
	mov	r0, r2
	bl	Func_fab3c
.Lfa416:
	pop	{r0}
	bx	r0
.func_end Func_fa3f0

@ ResumeSong
@ r0 = song id. The Func_fa3f0 of resuming: same identity check, then
@ Func_fa264.
.thumb_func_start Func_fa424
	push	{lr}
	lsl	r0, #16
	ldr	r2, =Data_fc624
	ldr	r1, =Data_fc684
	lsr	r0, #13
	add	r0, r1
	ldrh	r3, [r0, #4]
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #2
	add	r1, r2
	ldr	r2, [r1]
	ldr	r1, [r2]
	ldr	r0, [r0]
	cmp	r1, r0
	bne	.Lfa44a
	mov	r0, r2
	bl	Func_fa264
.Lfa44a:
	pop	{r0}
	bx	r0
.func_end Func_fa424

@ StopAllSongs
@ Takes no arguments. Walks all eight entries of Data_fc624, twelve bytes apart,
@ and stops each player with Func_fab3c.
.thumb_func_start Func_fa458
	push	{r4, r5, lr}
	ldr	r0, =8
	lsl	r0, #16
	lsr	r0, #16
	cmp	r0, #0
	beq	.Lfa476
	ldr	r5, =Data_fc624
	mov	r4, r0
.Lfa468:
	ldr	r0, [r5]
	bl	Func_fab3c
	add	r5, #0xc
	sub	r4, #1
	cmp	r4, #0
	bne	.Lfa468
.Lfa476:
	pop	{r4, r5}
	pop	{r0}
	bx	r0
.func_end Func_fa458
