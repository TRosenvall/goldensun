	.include "macros.inc"
	.include "gba.inc"

@ ============================================================================
@ Overlay 0x7a67d8 -- a map with a RASTER SPLIT and a set of camera nudges.
@
@ Slot 0  OvlFunc_200  map-load entry
@ Slot 1  OvlFunc_40   edge transitions   -> .L35c
@ Slot 2  OvlFunc_4c   map event list     -> .L44c
@ Slot 3  OvlFunc_54   read after slot 4  -> .L474
@ Slot 4  OvlFunc_1b4  map objects        -> .L4a4
@ Slot 5  OvlFunc_48   interactions       -> none (returns 0)
@
@ The distinguishing feature is the scanline effect: OvlFunc_26c runs from the
@ HBlank vector and swaps BG3's horizontal scroll at a chosen line, so the
@ background scrolls at two different rates above and below a horizon.
@ OvlFunc_2a0 recomputes the split each frame; OvlFunc_2e0 installs both.
@
@ The .L590 / .L5b0 / .L610 / .L614 / .L616 symbols at the bottom are .lcomm
@ scratch, not table data -- this overlay allocates its own state.
@ ============================================================================

@ Trigger: loads map 0x3D via slot 8.
.thumb_func_start OvlFunc_30
	push	{lr}
	mov	r0, #8
	mov	r1, #0x3d
	bl	__Func_91f14
	pop	{r0}
	bx	r0
.func_end OvlFunc_30

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_40
	ldr	r0, =.L35c
	bx	lr
.func_end OvlFunc_40

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_48
	mov	r0, #0
	bx	lr
.func_end OvlFunc_48

@ Slot 2: map event list.
.thumb_func_start OvlFunc_4c
	ldr	r0, =.L44c
	bx	lr
.func_end OvlFunc_4c

@ Slot 3: read after slot 4.
.thumb_func_start OvlFunc_54
	ldr	r0, =.L474
	bx	lr
.func_end OvlFunc_54

@ ShiftPlayerAndRider
@ r0 = x delta, r1 = z delta, both in whole tiles (shifted left 20 to reach the
@ 16.16 position fields).
@
@ Moves TWO entities together: the one whose slot is stored at ewram_240+0x1F4,
@ and whatever is parked at [iwram_1ebc]+0x1E0. For each, it adds the deltas to
@ +0x08 and +0x10 and then re-derives the ground height with Func_11f54 --
@ using the entity's own tile-type byte at +0x22 to pick the layer -- writing
@ the result to BOTH +0x0C and +0x14 so the entity lands rather than floats.
@ Either entity may be absent, and each is checked for null separately.
.thumb_func_start OvlFunc_5c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r2, #0xfa
	mov	r8, r3
	ldr	r3, =ewram_240
	lsl	r2, #1
	add	r3, r2
	mov	r6, r0
	ldr	r0, [r3]
	mov	r7, r1
	bl	__Func_92054
	mov	r5, r0
	lsl	r6, #20
	lsl	r7, #20
	cmp	r5, #0
	beq	.L9e
	ldr	r1, [r5, #8]
	ldr	r2, [r5, #0x10]
	add	r1, r6
	add	r2, r7
	str	r1, [r5, #8]
	str	r2, [r5, #0x10]
	mov	r3, r5
	add	r3, #0x22
	ldrb	r0, [r3]
	bl	__Func_11f54
	str	r0, [r5, #0xc]
	str	r0, [r5, #0x14]
.L9e:
	mov	r3, #0xf0
	lsl	r3, #1
	add	r3, r8
	ldr	r5, [r3]
	cmp	r5, #0
	beq	.Lc4
	ldr	r1, [r5, #8]
	ldr	r2, [r5, #0x10]
	add	r1, r6
	add	r2, r7
	str	r1, [r5, #8]
	str	r2, [r5, #0x10]
	mov	r3, r5
	add	r3, #0x22
	ldrb	r0, [r3]
	bl	__Func_11f54
	str	r0, [r5, #0xc]
	str	r0, [r5, #0x14]
.Lc4:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5c

@ Shift by (0, +5) tiles.
.thumb_func_start OvlFunc_d8
	push	{lr}
	mov	r0, #0
	mov	r1, #5
	bl	OvlFunc_5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_d8

@ Shift by (0, -5) tiles.
.thumb_func_start OvlFunc_e8
	push	{lr}
	mov	r1, #5
	neg	r1, r1
	mov	r0, #0
	bl	OvlFunc_5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_e8

@ Shift by (0, +5) tiles -- a duplicate of OvlFunc_d8 at a second trigger.
.thumb_func_start OvlFunc_f8
	push	{lr}
	mov	r0, #0
	mov	r1, #5
	bl	OvlFunc_5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_f8

@ Shift by (0, -5) tiles -- duplicate of OvlFunc_e8.
.thumb_func_start OvlFunc_108
	push	{lr}
	mov	r1, #5
	neg	r1, r1
	mov	r0, #0
	bl	OvlFunc_5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_108

@ Shift by (0, +6) tiles.
.thumb_func_start OvlFunc_118
	push	{lr}
	mov	r0, #0
	mov	r1, #6
	bl	OvlFunc_5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_118

@ Shift by (0, -6) tiles.
.thumb_func_start OvlFunc_128
	push	{lr}
	mov	r1, #6
	neg	r1, r1
	mov	r0, #0
	bl	OvlFunc_5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_128

@ Trigger: sound 0x7B, then leaves the interaction target halfword at
@ [iwram_1ebc]+0x16C pending as the next message id.
.thumb_func_start OvlFunc_138
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	mov	r0, #0x7b
	ldr	r5, [r3]
	bl	__Func_f9080
	mov	r3, #0xb6
	lsl	r3, #1
	add	r5, r3
	mov	r3, #0
	ldrsh	r0, [r5, r3]
	bl	__Func_91e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_138

@ ApplyRasterBlend
@ Takes no arguments. Allocates the 0x540-byte overlay state in mode 9, then
@ programs the blend: REG_BLDCNT = 0x3F42, REG_BLDALPHA = 0x0C04, and three
@ fields inside the state block at [iwram_1ecc] -- +0x534 = 0x3F3F,
@ +0x536 = 0x1F, +0x52A = 0x0A. The same block is set up again by OvlFunc_200
@ on map load, so this is the re-entry path.
.thumb_func_start OvlFunc_15c
	push	{lr}
	mov	r0, #9
	bl	__Func_8fe38
	ldr	r2, =REG_BLDCNT
	ldr	r3, .L190	@ 0x3f42
	strh	r3, [r2]
	ldr	r3, .L194	@ 0xc04
	add	r2, #2
	strh	r3, [r2]
	ldr	r3, =iwram_1ecc
	ldr	r2, [r3]
	ldr	r3, =0x534
	add	r1, r2, r3
	ldr	r3, =0x3f3f
	strh	r3, [r1]
	ldr	r3, =0x536
	add	r1, r2, r3
	mov	r3, #0x1f
	strh	r3, [r1]
	ldr	r3, =0x52a
	add	r2, r3
	mov	r3, #0xa
	strh	r3, [r2]
	b	.L1b0

	.align	2, 0
.L190:
	.word	0x3f42
.L194:
	.word	0xc04
	.pool

.L1b0:
	pop	{r0}
	bx	r0
.func_end OvlFunc_15c

@ Slot 4: map object table.
.thumb_func_start OvlFunc_1b4
	ldr	r0, =.L4a4
	bx	lr
.func_end OvlFunc_1b4

@ Trigger: opens and immediately closes the cutscene frame -- a beat with no
@ content, which still consumes the interaction.
.thumb_func_start OvlFunc_1bc
	push	{lr}
	bl	__Func_916b0
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_1bc

@ Empty hook.
.thumb_func_start OvlFunc_1cc
	bx	lr
.func_end OvlFunc_1cc

@ Empty hook.
.thumb_func_start OvlFunc_1d0
	bx	lr
.func_end OvlFunc_1d0

@ Trigger: copies the two words at iwram_1ae8 and iwram_1b04 into the .L590
@ scratch as halfwords, then calls Func_5ee0 with the .L5b0 buffer.
.thumb_func_start OvlFunc_1d4
	push	{lr}
	ldr	r3, =iwram_1ae8
	ldr	r0, =.L590
	ldr	r3, [r3]
	strh	r3, [r0]
	ldr	r3, =iwram_1b04
	ldr	r3, [r3]
	ldr	r1, =.L5b0
	strh	r3, [r0, #2]
	bl	__Func_5ee0
	pop	{r0}
	bx	r0
.func_end OvlFunc_1d4

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x100, allocates the
@ overlay state in mode 9, and programs the same blend registers and state
@ fields as OvlFunc_15c. Finishes by installing the raster effect through
@ OvlFunc_2e0.
.thumb_func_start OvlFunc_200
	push	{r5, lr}
	ldr	r5, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r3, [r5]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	mov	r0, #9
	bl	__Func_8fe38
	ldr	r2, =REG_BLDCNT
	ldr	r3, .L244	@ 0x3f42
	strh	r3, [r2]
	ldr	r3, .L248	@ 0xc04
	add	r2, #2
	strh	r3, [r2]
	ldr	r2, [r5, #0x10]
	ldr	r3, =0x534
	add	r1, r2, r3
	ldr	r3, =0x3f3f
	strh	r3, [r1]
	ldr	r3, =0x536
	add	r1, r2, r3
	mov	r3, #0x1f
	strh	r3, [r1]
	ldr	r3, =0x52a
	add	r2, r3
	mov	r3, #0xa
	strh	r3, [r2]
	bl	OvlFunc_2e0
	mov	r0, #0
	b	.L264

	.align	2, 0
.L244:
	.word	0x3f42
.L248:
	.word	0xc04
	.pool

.L264:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_200

@ RasterSplitHandler
@ The HBlank handler, installed on IRQ 1. Compares REG_VCOUNT against the split
@ line cached in .L610 and writes REG_BG3HOFS from .L614 below it or .L616
@ above -- one comparison and one store, which is all an HBlank handler can
@ afford. The two scroll values differ, so BG3 appears to scroll at two speeds.
.thumb_func_start OvlFunc_26c
	push	{lr}
	ldr	r3, =REG_VCOUNT
	ldrh	r2, [r3]
	ldr	r3, =.L610
	ldr	r3, [r3]
	cmp	r2, r3
	blt	.L27e
	ldr	r3, =.L614
	b	.L280
.L27e:
	ldr	r3, =.L616
.L280:
	ldrh	r2, [r3]
	ldr	r3, =REG_BG3HOFS
	strh	r2, [r3]
	pop	{r0}
	bx	r0
.func_end OvlFunc_26c

@ UpdateRasterSplit
@ The per-frame task that feeds OvlFunc_26c. Reads the camera record at
@ [iwram_1e70]+0x104: the halfword at +6 gives the horizon, stored as
@ .L610 = 0xC0 - that, and the halfword at +2 becomes the lower scroll .L614.
@ The upper scroll .L616 is that same value minus [iwram_1e40] >> 2, so the far
@ layer moves at a quarter rate -- the parallax.
.thumb_func_start OvlFunc_2a0
	ldr	r3, =iwram_1e70
	mov	r1, #0x82
	ldr	r2, [r3]
	lsl	r1, #1
	add	r2, r1
	mov	r3, #6
	ldrsh	r1, [r2, r3]
	ldr	r0, =.L610
	mov	r3, #0xc0
	sub	r3, r1
	str	r3, [r0]
	ldr	r3, =.L614
	mov	r1, #2
	ldrsh	r2, [r2, r1]
	strh	r2, [r3]
	ldr	r3, =iwram_1e40
	ldr	r3, [r3]
	ldr	r1, =.L616
	lsr	r3, #2
	sub	r2, r3
	strh	r2, [r1]
	bx	lr
.func_end OvlFunc_2a0

@ InstallRasterSplit
@ Registers OvlFunc_26c on IRQ 1 with Func_307c and OvlFunc_2a0 as a task at
@ priority 0xC80 with Func_41d8.
.thumb_func_start OvlFunc_2e0
	push	{lr}
	ldr	r2, =OvlFunc_26c
	mov	r0, #1
	mov	r1, #0
	bl	__Func_307c
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_2a0
	bl	__Func_41d8
	pop	{r0}
	bx	r0
.func_end OvlFunc_2e0

	.section .data

.L35c:
	.incbin "overlays/rom_7a67d8/orig.bin", 0x35c, (0x44c-0x35c)
.L44c:
	.incbin "overlays/rom_7a67d8/orig.bin", 0x44c, (0x474-0x44c)
.L474:
	.incbin "overlays/rom_7a67d8/orig.bin", 0x474, (0x4a4-0x474)
.L4a4:
	.incbin "overlays/rom_7a67d8/orig.bin", 0x4a4

	.section .bss

	.lcomm	.Lunused_588, 8
	.lcomm	.L590, 0x20
	.lcomm	.L5b0, 0x60
	.lcomm	.L610, 4
	.lcomm	.L614, 2
	.lcomm	.L616, 2
