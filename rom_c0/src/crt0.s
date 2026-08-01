	.include "macros.inc"
	.include "gba.inc"

@ ============================================================================
@ The shared runtime.
@
@ rom_c0 is what every other module stands on: the allocator, the task
@ scheduler, the frame loop, input, maths, decompression and the sound engine.
@ It starts at ROM offset 0xC0, immediately after the header, and contains the
@ boot code. 96 functions are exported.
@
@ THE ALLOCATOR IS TWO BUMP ARENAS, NOT A HEAP. This is the single most
@ important thing to know before decompiling anything that calls it.
@
@ A 64-word table at iwram_1e50 holds everything:
@     [iwram_1e50 + 0]        the EWRAM bump pointer
@     [iwram_1e50 + 4]        the IWRAM bump pointer
@     [iwram_1e50 + tag*4]    the pointer handed out for that tag
@ Slots 0 and 1 are the bump pointers themselves, so usable tags are 2..63.
@ Func_4858 resets the whole table and sets the two bases:
@     IWRAM arena  0x3002000 .. 0x3007800   0x5800 bytes
@     EWRAM arena  0x2030000 .. 0x2040000   0x10000 bytes
@ (0x2040000 appears as the literal `0x81 << 18`, and 0x3007800 leaves
@ 0x7800..0x7FFF for the two stacks and the IRQ vector set up in _start.)
@
@ FOUR ALLOCATORS, differing only in ARENA PREFERENCE and whether they take a
@ tag. All four round the size up to a word and return 0 when neither arena has
@ room:
@     Func_48b0(tag, size)  tagged,   IWRAM first, EWRAM fallback
@     Func_48f4(tag, size)  tagged,   EWRAM first, IWRAM fallback
@     Func_4938(size)       anonymous, IWRAM first, EWRAM fallback
@     Func_4970(size)       anonymous, EWRAM first, IWRAM fallback
@ A tagged call whose slot is already non-zero returns the existing pointer
@ without allocating, so tagged allocation is idempotent.
@
@ Func_2dd8(tag) DOES NOT FREE A BLOCK -- it REWINDS THE ARENA. It reads the
@ tag's pointer, clears the slot, and stores that pointer back over the bump
@ pointer of whichever arena it came from (selected by bit 22 of the address).
@ Everything allocated after the freed block is silently reclaimed too.
@ SO FREEING MUST HAPPEN IN REVERSE ALLOCATION ORDER. That is exactly what the
@ callers do -- rom_c9000's handlers free 0x2F then 0x2E, the reverse of how
@ they were generated -- and it is why they can get away with it.
@ Func_488c and Func_48a0 report the free space left in each arena.
@
@ OTHER SERVICES, with the contracts the calling modules rely on:
@     Func_30f8(n)      advance n frames -- every synchronous loop's heartbeat
@     Func_41d8(fn,pri) register a per-frame task; Func_4278(fn) removes it
@     Func_307c(n,line,fn)  arm a scanline trigger
@     Func_3650         poll input into iwram_1ae8 (held), iwram_1c94 (pressed
@                       this frame) and iwram_1b04 (auto-repeat)
@     Func_4458()       16-bit LCG, seed at iwram_1cb4
@     Func_2322/231c    sine / cosine, 0x10000 = a full turn
@     Func_44d0(x,y)    atan2      Func_948(v)  integer square root
@     Func_af0/b1c      signed quotient / remainder
@     Func_b50/b60      unsigned remainder
@     Func_8d4/8d8      fill       Func_1af8    ARM fast memcpy
@     Func_2f40(id)     asset pointer = Data_320000[id]
@     Func_5340(s,d)    decompress by DMAing Func_2544 into scratch and running
@                       it from RAM
@     Func_49ac..Func_5268  the 3D matrix stack and projection
@ ============================================================================

@ _start -- the ROM entry point
@ Sets up both stacks and hands control to the game.
@     IRQ mode  (CPSR 0b10010) stack at iwram_7fa0
@     System    (CPSR 0b11111) stack at iwram_7f00
@ Installs .Lintr_main as the BIOS IRQ vector by writing it to iwram_7ffc, then
@ calls Func_2e00 -- the main loop -- through `bx`, so the game runs in ARM or
@ Thumb as that function chooses.
@ The `b _start` after the call is unreachable in practice: Func_2e00 never
@ returns. It is there so a return would reboot rather than run off the end.
.arm_func_start _start
	mov	r0, #0b10010	@ IRQ mode
	msr	CPSR_fc, r0
	ldr	sp, .Lsp_irq

	mov	r0, #0b11111	@ System mode
	msr	CPSR_fc, r0
	ldr	sp, .Lsp_sys

	ldr	r1, =iwram_7ffc	@ IRQ vector
	adr	r0, .Lintr_main
	str	r0, [r1]

	ldr	r1, =Func_2e00
	mov	lr, pc
	bx	r1

	b	_start

.Lsp_sys:
	.word	iwram_7f00
.Lsp_irq:
	.word	iwram_7fa0
.func_end _start

@ TransformVariants -- a bank of five IWRAM-resident routines
@ Not called directly. Func_37d4 (rom_3650.s) DMA3-copies ONE of these into
@ Label_1348 with `Func_404 + n * 0x98` and a transfer of 0x26 words -- exactly
@ 0x98 bytes -- then the copy runs from IWRAM.
@ The region spans 0x404..0x6FC, which is 0x2F8 bytes, exactly five variants of
@ 0x98. Each does the same shape of packed fixed-point work: rotate the two
@ accumulators, arithmetic-shift a field out of each, round the negative case up
@ by adding 0x20 or 0x40, and pack two 16-bit results into one word with
@ `add rN, rN, lsl #16`. The variants differ only in their shift amounts, i.e.
@ in the fixed-point scale they target.
@ Copied to IWRAM rather than run from ROM because it is a per-pixel inner loop
@ and ROM is wait-stated.
.arm_func_start Func_404
	asrs	r1, r11, #20
	bicpl	r1, #0x1f
	adds	r1, r12, asr #20
	addmi	r1, #0x20
	add	r1, r1, asr #1
	add	r4, r1, r1, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r1, r11, #20
	bicpl	r1, #0x1f
	adds	r1, r12, asr #20
	addmi	r1, #0x20
	add	r1, r1, asr #1
	add	r2, r1, r1, lsl #16
	ror	r11, #8
	ror	r12, #8
	asrs	r1, r11, #20
	bicpl	r1, #0x1f
	adds	r1, r12, asr #20
	addmi	r1, #0x20
	add	r1, r1, asr #1
	add	r3, r1, r1, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r1, r11, #20
	bicpl	r1, #0x1f
	adds	r1, r12, asr #20
	addmi	r1, #0x20
	add	r1, r1, asr #1
	add	r1, r1, lsl #16
	b	.L49c
	nop
	nop
	nop
	nop
	nop
	nop
	nop
.L49c:
	asrs	r7, r11, #19
	adds	r7, r12, asr #20
	addmi	r7, #0x30
	asrs	r6, r12, #19
	adds	r6, r11, asr #20
	addmi	r6, #0x30
	adds	r4, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r7, r11, #19
	adds	r7, r12, asr #20
	addmi	r7, #0x30
	asrs	r6, r12, #19
	adds	r6, r11, asr #20
	addmi	r6, #0x30
	adds	r2, r7, r6, lsl #16
	ror	r11, #8
	ror	r12, #8
	asrs	r7, r11, #19
	adds	r7, r12, asr #20
	addmi	r7, #0x30
	asrs	r6, r12, #19
	adds	r6, r11, asr #20
	addmi	r6, #0x30
	adds	r3, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r7, r11, #19
	adds	r7, r12, asr #20
	addmi	r7, #0x30
	asrs	r6, r12, #19
	adds	r6, r11, asr #20
	addmi	r6, #0x30
	adds	r1, r7, r6, lsl #16
	b	.L534
	nop
	nop
	nop
.L534:
	asrs	r6, r4, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r4, #16
	asrs	r7, r4, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r4, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r2, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r2, #16
	asrs	r7, r2, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r2, r7, r6, lsl #16
	ror	r11, #8
	ror	r12, #8
	asrs	r6, r3, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r3, #16
	asrs	r7, r3, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r3, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r1, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r1, #16
	asrs	r7, r1, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r1, r7, r6, lsl #16
	asrs	r6, r4, #19
	adds	r6, r11, asr #18
	addmi	r6, #0x48
	ror	r4, #16
	asrs	r7, r4, #19
	adds	r7, r12, asr #18
	addmi	r7, #0x48
	adds	r4, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r2, #19
	adds	r6, r11, asr #18
	addmi	r6, #0x48
	ror	r2, #16
	asrs	r7, r2, #19
	adds	r7, r12, asr #18
	addmi	r7, #0x48
	adds	r2, r7, r6, lsl #16
	ror	r11, #8
	ror	r12, #8
	asrs	r6, r3, #19
	adds	r6, r11, asr #18
	addmi	r6, #0x48
	ror	r3, #16
	asrs	r7, r3, #19
	adds	r7, r12, asr #18
	addmi	r7, #0x48
	adds	r3, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r1, #19
	adds	r6, r11, asr #18
	addmi	r6, #0x48
	ror	r1, #16
	asrs	r7, r1, #19
	adds	r7, r12, asr #18
	addmi	r7, #0x48
	adds	r1, r7, r6, lsl #16
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	mov	r4, #0
	b	.L6fc
	adds	r7, r11, asr #18
	addmi	r7, #0x40
	adds	r4, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r2, #19
	adds	r6, r12, asr #18
	addmi	r6, #0x40
	ror	r2, #16
	asrs	r7, r2, #19
	adds	r7, r11, asr #18
	addmi	r7, #0x40
	adds	r2, r7, r6, lsl #16
	ror	r11, #8
	ror	r12, #8
	asrs	r6, r3, #19
	adds	r6, r12, asr #18
	addmi	r6, #0x40
	ror	r3, #16
	asrs	r7, r3, #19
	adds	r7, r11, asr #18
	addmi	r7, #0x40
	adds	r3, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r1, #19
	adds	r6, r12, asr #18
	addmi	r6, #0x40
	ror	r1, #16
	asrs	r7, r1, #19
	adds	r7, r11, asr #18
	addmi	r7, #0x40
	adds	r1, r7, r6, lsl #16
.L6fc:
.func_end Func_404

@ DispatchIfMatching
@ r0 = object. Loads [r0] and compares it against a fixed word; returns
@ immediately unless they match, otherwise saves the high registers and calls
@ through [r0+0x24] when [r0+0x20] is non-zero.
@ A guarded virtual dispatch -- the leading word acts as a type tag.
.thumb_func_start Func_6fc
	ldr	r0, .L76c
	ldr	r0, [r0]
	ldr	r2, .L768
	ldr	r3, [r0]
	cmp	r2, r3
	beq	.L70c
	bx	lr
.L70a:
	bx	r3
.L70c:
	push	{r5, r6, r7, lr}
	mov	r1, r8
	mov	r2, r9
	mov	r3, r10
	mov	r4, r11
	push	{r0, r1, r2, r3, r4}
	sub	sp, #0x18
	ldr	r3, [r0, #0x20]
	cmp	r3, #0
	beq	.L728
	ldr	r0, [r0, #0x24]
	bl	.L70a
	ldr	r0, [sp, #0x18]
.L728:
	ldr	r3, [r0, #0x28]
	bl	.L70a
	ldr	r0, [sp, #0x18]
	ldr	r3, [r0, #0x10]
	str	r3, [sp]
	ldr	r5, .L760
	add	r5, r0
	ldrb	r4, [r0, #4]
	sub	r7, r4, #1
	bls	.L748
	ldrb	r1, [r0, #0xb]
	sub	r1, r7
	mov	r2, r3
	mul	r2, r1
	add	r5, r2
.L748:
	str	r5, [sp, #8]
	ldr	r4, [sp, #0x18]
	ldr	r0, [r4, #0x14]
	mov	r9, r0
	ldr	r0, [r4, #0x18]
	mov	r12, r0
	ldrb	r0, [r4, #6]
	add	r4, #0x50
	ldr	r3, .L75c
	bx	r3

.L75c:	.word	Func_dc8
.L760:	.word	0x350
	.word	REG_VCOUNT
.L768:	.word	0x68736d53
.L76c:	.word	iwram_7ff0
.func_end Func_6fc

.Lintr_main:
