	.include "macros.inc"
	.include "gba.inc"

@ TrackStop
@ r0 = player, r1 = track. Walks the track's channel list at +0x20 and stops
@ every channel on it, clearing each one's status byte and its back-pointer to
@ the track. A channel of the CGB type goes through the oscillator-off handler
@ in the driver block at iwram_7ff0 first, so the PSG hardware is silenced as
@ well as the mixer state.
@ Entry 31 of the jump table, and the routine Func_fab7c calls when a fade-out
@ reaches zero.
.thumb_func_start Func_f9ef8
	push	{r4, r5, r6, lr}
	mov	r5, r1
	ldrb	r1, [r5]
	mov	r0, #0x80
	tst	r0, r1
	beq	.Lf9f30
	ldr	r4, [r5, #0x20]
	cmp	r4, #0
	beq	.Lf9f2e
	mov	r6, #0
.Lf9f0c:
	ldrb	r0, [r4]
	cmp	r0, #0
	beq	.Lf9f26
	ldrb	r0, [r4, #1]
	mov	r3, #7
	and	r0, r3
	beq	.Lf9f24
	ldr	r3, =iwram_7ff0
	ldr	r3, [r3]
	ldr	r3, [r3, #0x2c]
	bl	Func_f9ee8
.Lf9f24:
	strb	r6, [r4]
.Lf9f26:
	str	r6, [r4, #0x2c]
	ldr	r4, [r4, #0x34]
	cmp	r4, #0
	bne	.Lf9f0c
.Lf9f2e:
	str	r4, [r5, #0x20]
.Lf9f30:
	pop	{r4, r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_f9ef8

@ ComputeChannelVolume
@ r4 = channel, r5 = track. The CHANNEL-level counterpart of Func_fac44: that
@ one rebuilds the track's stereo pair, this one turns it into the two levels
@ the mixer reads. Turns the track's volume (+0x12) and pan (+0x14) into
@ the two 8-bit mixer levels at channel+0x02 and +0x03:
@
@     right = (0x80 + pan) * vol * player.volMR >> 14
@     left  = (0x7F - pan) * vol * player.volML >> 14
@
@ both clamped at 0xFF. Pan is signed around zero, so the two expressions are
@ symmetric; the 0x80 versus 0x7F asymmetry is what makes hard-left and
@ hard-right reachable with a single byte.
.thumb_func_start Func_f9f3c
	ldrb	r1, [r4, #0x12]
	mov	r0, #0x14
	ldrsb	r2, [r4, r0]
	mov	r3, #0x80
	add	r3, r2
	mul	r3, r1
	ldrb	r0, [r5, #0x10]
	mul	r0, r3
	asr	r0, #14
	cmp	r0, #0xff
	bls	.Lf9f54
	mov	r0, #0xff
.Lf9f54:
	strb	r0, [r4, #2]
	mov	r3, #0x7f
	sub	r3, r2
	mul	r3, r1
	ldrb	r0, [r5, #0x11]
	mul	r0, r3
	asr	r0, #14
	cmp	r0, #0xff
	bls	.Lf9f68
	mov	r0, #0xff
.Lf9f68:
	strb	r0, [r4, #3]
	bx	lr
.func_end Func_f9f3c

@ UpdateChannelVolumes
@ r0.. = parameters. Walks the channels of a track and refreshes each one's
@ mixer state -- Func_f9f3c for the stereo levels, Func_fa1fc for the pitch,
@ Func_fa678 to release and Func_fac44 for the envelope step. 269 lines; traced
@ structurally.
.thumb_func_start Func_f9f6c
	push	{r4, r5, r6, r7, lr}
	mov	r4, r8
	mov	r5, r9
	mov	r6, r10
	mov	r7, r11
	push	{r4, r5, r6, r7}
	sub	sp, #0x18
	str	r1, [sp]
	mov	r5, r2
	ldr	r1, =iwram_7ff0
	ldr	r1, [r1]
	str	r1, [sp, #4]
	ldr	r1, =Data_fba14
	add	r0, r1
	ldrb	r0, [r0]
	strb	r0, [r5, #4]
	ldr	r3, [r5, #0x40]
	ldrb	r0, [r3]
	cmp	r0, #0x80
	bcs	.Lf9fb2
	strb	r0, [r5, #5]
	add	r3, #1
	ldrb	r0, [r3]
	cmp	r0, #0x80
	bcs	.Lf9fb0
	strb	r0, [r5, #6]
	add	r3, #1
	ldrb	r0, [r3]
	cmp	r0, #0x80
	bcs	.Lf9fb0
	ldrb	r1, [r5, #4]
	add	r1, r0
	strb	r1, [r5, #4]
	add	r3, #1
.Lf9fb0:
	str	r3, [r5, #0x40]
.Lf9fb2:
	mov	r0, #0
	str	r0, [sp, #0x14]
	mov	r4, r5
	add	r4, #0x24
	ldrb	r2, [r4]
	mov	r0, #0xc0
	tst	r0, r2
	beq	.Lfa004
	ldrb	r3, [r5, #5]
	mov	r0, #0x40
	tst	r0, r2
	beq	.Lf9fd2
	ldr	r1, [r5, #0x2c]
	add	r1, r3
	ldrb	r0, [r1]
	b	.Lf9fd4
.Lf9fd2:
	mov	r0, r3
.Lf9fd4:
	lsl	r1, r0, #1
	add	r1, r0
	lsl	r1, #2
	ldr	r0, [r5, #0x28]
	add	r1, r0
	mov	r9, r1
	mov	r6, r9
	ldrb	r1, [r6]
	mov	r0, #0xc0
	tst	r0, r1
	beq	.Lf9fec
	b	.Lfa152
.Lf9fec:
	mov	r0, #0x80
	tst	r0, r2
	beq	.Lfa008
	ldrb	r1, [r6, #3]
	mov	r0, #0x80
	tst	r0, r1
	beq	.Lfa000
	sub	r1, #0xc0
	lsl	r1, #1
	str	r1, [sp, #0x14]
.Lfa000:
	ldrb	r3, [r6, #1]
	b	.Lfa008
.Lfa004:
	mov	r9, r4
	ldrb	r3, [r5, #5]
.Lfa008:
	str	r3, [sp, #8]
	ldr	r6, [sp]
	ldrb	r1, [r6, #9]
	ldrb	r0, [r5, #0x1d]
	add	r0, r1
	cmp	r0, #0xff
	bls	.Lfa018
	mov	r0, #0xff
.Lfa018:
	str	r0, [sp, #0x10]
	mov	r6, r9
	ldrb	r0, [r6]
	mov	r6, #7
	and	r6, r0
	str	r6, [sp, #0xc]
	beq	.Lfa058
	ldr	r0, [sp, #4]
	ldr	r4, [r0, #0x1c]
	cmp	r4, #0
	bne	.Lfa030
	b	.Lfa152
.Lfa030:
	sub	r6, #1
	lsl	r0, r6, #6
	add	r4, r0
	ldrb	r1, [r4]
	mov	r0, #0xc7
	tst	r0, r1
	beq	.Lfa0ac
	mov	r0, #0x40
	tst	r0, r1
	bne	.Lfa0ac
	ldrb	r1, [r4, #0x13]
	ldr	r0, [sp, #0x10]
	cmp	r1, r0
	bcc	.Lfa0ac
	beq	.Lfa050
	b	.Lfa152
.Lfa050:
	ldr	r0, [r4, #0x2c]
	cmp	r0, r5
	bcs	.Lfa0ac
	b	.Lfa152
.Lfa058:
	ldr	r6, [sp, #0x10]
	mov	r7, r5
	mov	r2, #0
	mov	r8, r2
	ldr	r4, [sp, #4]
	ldrb	r3, [r4, #6]
	add	r4, #0x50
.Lfa066:
	ldrb	r1, [r4]
	mov	r0, #0xc7
	tst	r0, r1
	beq	.Lfa0ac
	mov	r0, #0x40
	tst	r0, r1
	beq	.Lfa080
	cmp	r2, #0
	bne	.Lfa084
	add	r2, #1
	ldrb	r6, [r4, #0x13]
	ldr	r7, [r4, #0x2c]
	b	.Lfa09e
.Lfa080:
	cmp	r2, #0
	bne	.Lfa0a0
.Lfa084:
	ldrb	r0, [r4, #0x13]
	cmp	r0, r6
	bcs	.Lfa090
	mov	r6, r0
	ldr	r7, [r4, #0x2c]
	b	.Lfa09e
.Lfa090:
	bhi	.Lfa0a0
	ldr	r0, [r4, #0x2c]
	cmp	r0, r7
	bls	.Lfa09c
	mov	r7, r0
	b	.Lfa09e
.Lfa09c:
	bcc	.Lfa0a0
.Lfa09e:
	mov	r8, r4
.Lfa0a0:
	add	r4, #0x40
	sub	r3, #1
	bgt	.Lfa066
	mov	r4, r8
	cmp	r4, #0
	beq	.Lfa152
.Lfa0ac:
	mov	r0, r4
	bl	Func_fa678
	mov	r1, #0
	str	r1, [r4, #0x30]
	ldr	r3, [r5, #0x20]
	str	r3, [r4, #0x34]
	cmp	r3, #0
	beq	.Lfa0c0
	str	r4, [r3, #0x30]
.Lfa0c0:
	str	r4, [r5, #0x20]
	str	r5, [r4, #0x2c]
	ldrb	r0, [r5, #0x1b]
	strb	r0, [r5, #0x1c]
	cmp	r0, r1
	beq	.Lfa0d2
	mov	r1, r5
	bl	Func_fa1ac
.Lfa0d2:
	ldr	r0, [sp]
	mov	r1, r5
	bl	Func_fac44
	ldr	r0, [r5, #4]
	str	r0, [r4, #0x10]
	ldr	r0, [sp, #0x10]
	strb	r0, [r4, #0x13]
	ldr	r0, [sp, #8]
	strb	r0, [r4, #8]
	ldr	r0, [sp, #0x14]
	strb	r0, [r4, #0x14]
	mov	r6, r9
	ldrb	r0, [r6]
	strb	r0, [r4, #1]
	ldr	r7, [r6, #4]
	str	r7, [r4, #0x24]
	ldr	r0, [r6, #8]
	str	r0, [r4, #4]
	ldrh	r0, [r5, #0x1e]
	strh	r0, [r4, #0xc]
	bl	Func_f9f3c
	ldrb	r1, [r4, #8]
	mov	r0, #8
	ldrsb	r0, [r5, r0]
	add	r3, r1, r0
	bpl	.Lfa10c
	mov	r3, #0
.Lfa10c:
	ldr	r6, [sp, #0xc]
	cmp	r6, #0
	beq	.Lfa13a
	mov	r6, r9
	ldrb	r0, [r6, #2]
	strb	r0, [r4, #0x1e]
	ldrb	r1, [r6, #3]
	mov	r0, #0x80
	tst	r0, r1
	bne	.Lfa126
	mov	r0, #0x70
	tst	r0, r1
	bne	.Lfa128
.Lfa126:
	mov	r1, #8
.Lfa128:
	strb	r1, [r4, #0x1f]
	ldrb	r2, [r5, #9]
	mov	r1, r3
	ldr	r0, [sp, #0xc]
	ldr	r3, [sp, #4]
	ldr	r3, [r3, #0x30]
	bl	Func_f9ee8
	b	.Lfa144
.Lfa13a:
	ldrb	r2, [r5, #9]
	mov	r1, r3
	mov	r0, r7
	bl	Func_fa1fc
.Lfa144:
	str	r0, [r4, #0x20]
	mov	r0, #0x80
	strb	r0, [r4]
	ldrb	r1, [r5]
	mov	r0, #0xf0
	and	r0, r1
	strb	r0, [r5]
.Lfa152:
	add	sp, #0x18
	pop	{r0, r1, r2, r3, r4, r5, r6, r7}
	mov	r8, r0
	mov	r9, r1
	mov	r10, r2
	mov	r11, r3
	pop	{r0}
	bx	r0
.func_end Func_f9f6c

@ TrackEndOfTie
@ r0 = player, r1 = track. Ends a tied note: finds the channel holding it and
@ sets the release bit. Command 0xCE, index 29.
.thumb_func_start Func_fa16c
	push	{r4, r5}
	ldr	r2, [r1, #0x40]
	ldrb	r3, [r2]
	cmp	r3, #0x80
	bcs	.Lfa17e
	strb	r3, [r1, #5]
	add	r2, #1
	str	r2, [r1, #0x40]
	b	.Lfa180
.Lfa17e:
	ldrb	r3, [r1, #5]
.Lfa180:
	ldr	r1, [r1, #0x20]
	cmp	r1, #0
	beq	.Lfa1a8
	mov	r4, #0x83
	mov	r5, #0x40
.Lfa18a:
	ldrb	r2, [r1]
	tst	r2, r4
	beq	.Lfa1a2
	tst	r2, r5
	bne	.Lfa1a2
	ldrb	r0, [r1, #0x11]
	cmp	r0, r3
	bne	.Lfa1a2
	mov	r0, #0x40
	orr	r2, r0
	strb	r2, [r1]
	b	.Lfa1a8
.Lfa1a2:
	ldr	r1, [r1, #0x34]
	cmp	r1, #0
	bne	.Lfa18a
.Lfa1a8:
	pop	{r4, r5}
	bx	lr
.func_end Func_fa16c

@ MarkModulationChanged
@ r1 = track. Zeroes the modulation accumulators at track+0x16 and +0x1A, then
@ raises flag bits 2 and 3 when the modulation type at +0x18 is zero and bits 0
@ and 1 otherwise -- pitch modulation versus volume modulation, which is exactly
@ what the two flag pairs mean everywhere else in this driver.
.thumb_func_start Func_fa1ac
	mov	r2, #0
	strb	r2, [r1, #0x16]
	strb	r2, [r1, #0x1a]
	ldrb	r2, [r1, #0x18]
	cmp	r2, #0
	bne	.Lfa1bc
	mov	r2, #0xc
	b	.Lfa1be
.Lfa1bc:
	mov	r2, #3
.Lfa1be:
	ldrb	r3, [r1]
	orr	r3, r2
	strb	r3, [r1]
	bx	lr
.func_end Func_fa1ac

@ FetchTrackByteRaw
@ r1 = track. Reads the byte at the command pointer and advances it, WITHOUT the
@ guard Func_f9ab4 applies. Used only by the two commands below, where the byte
@ has already been bounds-checked by the caller.
.thumb_func_start Func_fa1c8
	ldr	r2, [r1, #0x40]
	add	r3, r2, #1
	str	r3, [r1, #0x40]
	ldrb	r3, [r2]
	bx	lr
.func_end Func_fa1c8

@ TrackLfoSpeed
@ r1 = track. Stores the next byte at track+0x19 and, when it is zero, calls
@ Func_fa1ac to clear the modulation state. Command 0xC2, index 17.
.thumb_func_start Func_fa1d4
	mov	r12, lr
	bl	Func_fa1c8
	strb	r3, [r1, #0x19]
	cmp	r3, #0
	bne	.Lfa1e4
	bl	Func_fa1ac
.Lfa1e4:
	bx	r12
.func_end Func_fa1d4

@ TrackModDepth
@ r1 = track. Stores the next byte at track+0x17, clearing the modulation state
@ when it is zero. Command 0xC4, index 19.
.thumb_func_start Func_fa1e8
	mov	r12, lr
	bl	Func_fa1c8
	strb	r3, [r1, #0x17]
	cmp	r3, #0
	bne	.Lfa1f8
	bl	Func_fa1ac
.Lfa1f8:
	bx	r12
.func_end Func_fa1e8

@ ComputeChannelPitch
@ r0.. = parameters. Turns a key, tune and bend into the mixer's frequency step,
@ using Func_f95e0 for the 32x32 high multiply. 41 lines; traced structurally.
.thumb_func_start Func_fa1fc
	push	{r4, r5, r6, r7, lr}
	mov	r12, r0
	lsl	r1, #24
	lsr	r6, r1, #24
	lsl	r7, r2, #24
	cmp	r6, #0xb2
	bls	.Lfa210
	mov	r6, #0xb2
	mov	r7, #0xff
	lsl	r7, #24
.Lfa210:
	ldr	r3, =Lfb830
	add	r0, r6, r3
	ldrb	r5, [r0]
	ldr	r4, =Lfb8e4
	mov	r2, #0xf
	mov	r0, r5
	and	r0, r2
	lsl	r0, #2
	add	r0, r4
	lsr	r1, r5, #4
	ldr	r5, [r0]
	lsr	r5, r1
	add	r0, r6, #1
	add	r0, r3
	ldrb	r1, [r0]
	mov	r0, r1
	and	r0, r2
	lsl	r0, #2
	add	r0, r4
	lsr	r1, #4
	ldr	r0, [r0]
	lsr	r0, r1
	mov	r1, r12
	ldr	r4, [r1, #4]
	sub	r0, r5
	mov	r1, r7
	bl	Func_f95e0
	mov	r1, r0
	add	r1, r5, r1
	mov	r0, r4
	bl	Func_f95e0
	pop	{r4, r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_fa1fc

@ NoOp
@ A bare `bx lr`.
.thumb_func_start Func_fa260
	bx	lr
.func_end Func_fa260

@ ResumePlayer
@ r0 = player. Clears bit 31 of the status word at player+0x04 -- the paused
@ flag -- but only when player+0x34 carries the 'Smsh' ident, so a pointer into
@ uninitialised memory is ignored.
.thumb_func_start Func_fa264
	mov	r2, r0
	ldr	r3, [r2, #0x34]
	ldr	r0, =0x68736d53
	cmp	r3, r0
	bne	.Lfa276
	ldr	r0, [r2, #4]
	ldr	r1, =0x7fffffff
	and	r0, r1
	str	r0, [r2, #4]
.Lfa276:
	bx	lr
.func_end Func_fa264

@ StartPlayerFadeOut
@ r0 = player, r1 = the fade length. Sets the fade counter and reload at
@ player+0x26 and +0x24 and the direction at +0x28 to 0x100 -- downward.
@ Guarded by the ident check.
.thumb_func_start Func_fa280
	mov	r2, r0
	lsl	r1, #16
	lsr	r1, #16
	ldr	r3, [r2, #0x34]
	ldr	r0, =0x68736d53
	cmp	r3, r0
	bne	.Lfa298
	strh	r1, [r2, #0x26]
	strh	r1, [r2, #0x24]
	mov	r0, #0x80
	lsl	r0, #1
	strh	r0, [r2, #0x28]
.Lfa298:
	bx	lr
.func_end Func_fa280

@ InitSoundDriver
@ Takes no arguments. Brings the whole driver up: Func_fa55c initialises the
@ mixer state, Func_fa6a0 the DirectSound hardware, Func_fa83c the timer, and
@ Func_fa9e0 the channel pool. Func_6864 does the bulk clears. The blocks it
@ touches -- ewram_3050, ewram_4090, ewram_4350 and iwram_7000 -- are the driver
@ work area, and iwram_7ff0 ends up pointing at it.
.thumb_func_start Func_fa2a0
	push	{r4, r5, r6, lr}
	ldr	r0, =Func_f9674
	mov	r1, #2
	neg	r1, r1
	and	r0, r1
	ldr	r1, =iwram_7000
	ldr	r2, =0x4000100
	bl	Func_6864
	ldr	r0, =ewram_3050
	bl	Func_fa6a0
	ldr	r0, =ewram_4090
	bl	Func_fa55c
	ldr	r0, =0x97f800
	bl	Func_fa83c
	ldr	r0, =8
	lsl	r0, #16
	lsr	r0, #16
	cmp	r0, #0
	beq	.Lfa2ee
	ldr	r5, =Data_fc624
	mov	r6, r0
.Lfa2d2:
	ldr	r4, [r5]
	ldr	r1, [r5, #4]
	ldrb	r2, [r5, #8]
	mov	r0, r4
	bl	Func_fa9e0
	ldrh	r0, [r5, #0xa]
	strb	r0, [r4, #0xb]
	ldr	r0, =ewram_4350
	str	r0, [r4, #0x18]
	add	r5, #0xc
	sub	r6, #1
	cmp	r6, #0
	bne	.Lfa2d2
.Lfa2ee:
	pop	{r4, r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_fa2a0
