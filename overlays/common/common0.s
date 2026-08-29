	.include "macros.inc"

@ ============================================================================
@ Shared overlay code: the cutscene prop spawner.
@
@ Five functions, and the most widely shared block in the overlays -- NINETEEN
@ of the 96 overlay linker scripts pull common0.o in, so the same bytes appear
@ nineteen times in the ROM. The sources factor it out; the ROM does not.
@
@ Everything here builds on rom_9000's entity system, so the vocabulary is
@ already documented: _Func_c150 spawns, _Func_c2d8 attaches a script,
@ _Func_c300 sets an animation, _Func_c528 and _Func_c548 dress the actor.
@
@ OvlFunc_common0_10c is the interesting one. It is a flag-driven spawner: the
@ low four bits of its flag word pick an animation and a script, and each of
@ bits 16..24 pulls one more field out of an optional parameter block. That
@ block's layout falls straight out of which bit reads which offset, and is
@ written up in the function's own comment.
@
@ OvlFunc_common0_d4 is installed as the per-frame hook at entity+0x6C and
@ establishes the motion fields: +0x44/+0x48/+0x4C is the velocity added to the
@ position at +0x08/+0x0C/+0x10, and +0x30/+0x34 the angular velocity added to
@ the rotation at +0x18/+0x1C. No gravity, no damping.
@
@ The three script blobs .L1, .L2 and .L3 are `.incbin`'d out of
@ rom_78ef88's own image rather than being disassembled.
@ ============================================================================

@ SetActorFacingBits
@ r0 = entity, r1 = facing 0..3. Replaces bits 2 and 3 of the actor's flag byte at
@ +0x09 with the low two bits of r1, leaving everything else alone. The actor is
@ reached through the entity's +0x50.
.thumb_func_start OvlFunc_common0_0
	ldr	r0, [r0, #0x50]
	mov	r3, #3
	ldrb	r2, [r0, #9]
	and	r1, r3
	mov	r3, #0xd
	neg	r3, r3
	lsl	r1, #2
	and	r3, r2
	orr	r3, r1
	strb	r3, [r0, #9]
	bx	lr
.func_end OvlFunc_common0_0

@ SpawnPropForeground
@ r0 = x, r1 = y, r2 = z (16.16), r3 = packed resource descriptor.
@ Spawns a cutscene prop through _Func_c150 and dresses it: clears bits 0 and 2
@ of the actor's +0x09, zeroes the entity's script state at +0x55, sets +0x59 to
@ 8, clears the actor options with _Func_c528, takes palette 0x0E through
@ _Func_929d8 and OAM priority 1 through _Func_c548.
@
@ Returns the entity, or 0 when the table was full. The only differences from
@ OvlFunc_common0_70 are the palette, the +0x09 bit and the priority -- this one
@ draws in front.
.thumb_func_start OvlFunc_common0_18
	push	{r5, r6, lr}
	mov	r4, r0
	mov	r5, r1
	mov	r6, r2
	mov	r0, r3
	mov	r2, r5
	mov	r1, r4
	mov	r3, r6
	bl	__Func_c150
	mov	r5, r0
	cmp	r5, #0
	beq	.L96
	ldr	r1, [r5, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, r5
	strb	r3, [r1, #9]
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	add	r2, #4
	mov	r3, #8
	strb	r3, [r2]
	mov	r1, #0
	bl	__Func_c528
	mov	r0, r5
	mov	r1, #0xe
	bl	__Func_929d8
	mov	r0, r5
	mov	r1, #1
	bl	__Func_c548
	mov	r0, r5
	b	.L98
.L96:
	mov	r0, #0
.L98:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_common0_18

@ SpawnPropBackground
@ r0 = x, r1 = y, r2 = z, r3 = descriptor. The counterpart of
@ OvlFunc_common0_18: it SETS bit 2 of the actor's +0x09 rather than clearing it,
@ takes palette 0x0F, leaves the OAM priority alone, and clears bit 0 of the
@ entity's +0x23 while setting bit 1.
.thumb_func_start OvlFunc_common0_70
	push	{r5, r6, lr}
	mov	r4, r0
	mov	r5, r1
	mov	r6, r2
	mov	r0, r3
	mov	r2, r5
	mov	r1, r4
	mov	r3, r6
	bl	__Func_c150
	mov	r5, r0
	cmp	r5, #0
	beq	.Lfa
	ldr	r1, [r5, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, #4
	orr	r3, r2
	mov	r2, r5
	strb	r3, [r1, #9]
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	add	r2, #4
	mov	r3, #8
	strb	r3, [r2]
	mov	r1, #0
	bl	__Func_c528
	mov	r0, r5
	mov	r1, #0xf
	bl	__Func_929d8
	mov	r1, r5
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #0xfe
	and	r3, r2
	mov	r2, #2
	orr	r3, r2
	strb	r3, [r1]
	mov	r0, r5
	b	.Lfc
.Lfa:
	mov	r0, #0
.Lfc:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_common0_70

@ StepPropPhysics
@ r0 = entity. The per-frame integrator OvlFunc_common0_10c installs at
@ entity+0x6C. One Euler step over five quantities, and it establishes the
@ entity's motion fields:
@
@     +0x08 += +0x44      x   += velocity x
@     +0x0C += +0x48      y   += velocity y
@     +0x10 += +0x4C      z   += velocity z
@     +0x18 += +0x30      rotation x += angular velocity x
@     +0x1C += +0x34      rotation y += angular velocity y
@
@ and the actor's own +0x1E advances by the entity's +0x64, which is the sprite
@ rotation rate. No gravity term and no damping -- whatever the spawner set is
@ what it keeps.
.thumb_func_start OvlFunc_common0_d4
	ldr	r3, [r0, #8]
	ldr	r2, [r0, #0x44]
	add	r3, r2
	str	r3, [r0, #8]
	ldr	r2, [r0, #0x48]
	ldr	r3, [r0, #0xc]
	add	r3, r2
	str	r3, [r0, #0xc]
	ldr	r2, [r0, #0x4c]
	ldr	r3, [r0, #0x10]
	add	r3, r2
	str	r3, [r0, #0x10]
	ldr	r2, [r0, #0x30]
	ldr	r3, [r0, #0x18]
	add	r3, r2
	str	r3, [r0, #0x18]
	ldr	r2, [r0, #0x34]
	ldr	r3, [r0, #0x1c]
	add	r3, r2
	str	r3, [r0, #0x1c]
	ldr	r1, [r0, #0x50]
	add	r0, #0x64
	ldrh	r3, [r1, #0x1e]
	ldrh	r2, [r0]
	add	r3, r2
	strh	r3, [r1, #0x1e]
	bx	lr
.func_end OvlFunc_common0_d4

@ SpawnPropDetailed
@ r0 = x, r1 = y, r2 = z, r3 = velocity x, arg5 = velocity y, arg6 = velocity z,
@ arg7 = a flag word, arg8 = an optional parameter block. Returns the entity or 0.
@
@ The general prop spawner the cutscenes use, and the reason this file is shared
@ by nineteen overlays. The base spawn is descriptor 0xDE, or the one at
@ params+0x18 when flag bit 20 is set. Then:
@
@   * the low four bits of the flag word pick BOTH the animation (that value plus
@     one, through _Func_c300) and the script, from the three-entry table .L4 --
@     each 0x38 bytes, incbin'd out of rom_78ef88's own image
@   * the velocity triple goes to +0x44, +0x48 and +0x4C and OvlFunc_common0_d4 is
@     installed at +0x6C to integrate it
@   * bits 2 and 3 of the PLAYER's actor +0x09 are copied onto the new one, so the
@     prop inherits the party's facing
@   * the angular velocity at +0x30/+0x34 and the sprite rate at +0x64 start zero
@
@ Everything after that is optional, one flag bit per field of the parameter
@ block, which is what makes the block's layout readable:
@
@     bit 16  +0x04  palette, via _Func_929d8
@     bit 17  +0x00  facing, low two bits into actor +0x09 bits 2-3, and entity
@                    +0x23 bit 0 is cleared
@     bit 18  +0x10, +0x14  TARGET rotation -- the angular velocity is
@                    (target - current) / script+0x0C, so the prop turns to face
@                    it over exactly the script's duration
@     bit 19  +0x08, +0x0C  starting rotation into +0x18 and +0x1C
@     bit 20  +0x18  the resource descriptor
@     bit 21  +0x1C  a script, applied after forcing animation 1
@     bit 22  +0x20  the actor's +0x1E
@     bit 23  +0x22  the sprite rotation rate at +0x64
@     bit 24  +0x24  a REPLACEMENT update hook at +0x6C, overriding
@                    OvlFunc_common0_d4
@
@ Bit 18 is evaluated after bit 19, so a caller that sets both gets a rotation
@ that starts at the given angle and sweeps to the target. 245 lines; the flag
@ dispatch is traced, the arithmetic structurally.
.thumb_func_start OvlFunc_common0_10c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #8
	mov	r6, r1
	ldr	r1, [sp, #0x30]
	mov	r5, r0
	mov	r0, #0
	mov	r8, r2
	str	r3, [sp, #4]
	mov	r10, r1
	ldr	r7, [sp, #0x34]
	bl	__Func_92054
	mov	r3, #0x80
	lsl	r3, #13
	mov	r2, r10
	and	r3, r2
	mov	r9, r0
	cmp	r3, #0
	beq	.L17a
	cmp	r7, #0
	beq	.L17a
	mov	r3, #0x18
	ldrsh	r0, [r7, r3]
	mov	r2, r6
	b	.L17e
.L17a:
	mov	r2, r6
	mov	r0, #0xde
.L17e:
	mov	r1, r5
	mov	r3, r8
	bl	__Func_c150
	mov	r6, r0
	cmp	r6, #0
	bne	.L18e
	b	.L2f6
.L18e:
	ldr	r1, [r6, #0x50]
	mov	r8, r1
	mov	r1, r10
	mov	r5, #0xf
	add	r1, #1
	and	r1, r5
	mov	r0, r6
	bl	__Func_c300
	mov	r3, r10
	ldr	r2, =.L4
	and	r3, r5
	lsl	r3, #2
	ldr	r1, [r2, r3]
	mov	r0, r6
	mov	r11, r3
	bl	__Func_c2d8
	mov	r3, r6
	mov	r0, #0
	add	r3, #0x55
	strb	r0, [r3]
	mov	r3, r8
	add	r3, #0x26
	strb	r0, [r3]
	ldr	r3, =OvlFunc_common0_d4
	str	r3, [r6, #0x6c]
	ldr	r3, [sp, #4]
	str	r3, [r6, #0x44]
	ldr	r3, [sp, #0x28]
	str	r3, [r6, #0x48]
	ldr	r3, [sp, #0x2c]
	mov	r1, r9
	str	r3, [r6, #0x4c]
	ldr	r3, [r1, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	mov	r3, r8
	ldrb	r1, [r3, #9]
	mov	r3, #0xd
	neg	r3, r3
	mov	r9, r3
	and	r3, r1
	orr	r3, r2
	mov	r2, r6
	mov	r1, r8
	add	r2, #0x64
	strb	r3, [r1, #9]
	mov	r3, r2
	str	r0, [r6, #0x30]
	str	r0, [r6, #0x34]
	str	r2, [sp]
	strh	r0, [r3]
	ldr	r3, =0xffff0000
	mov	r1, r10
	and	r3, r1
	mov	r5, #3
	cmp	r3, #0
	beq	.L2f6
	cmp	r7, #0
	beq	.L2f6
	mov	r3, #0x80
	lsl	r3, #9
	and	r3, r1
	cmp	r3, #0
	beq	.L21c
	ldr	r1, [r7, #4]
	mov	r0, r6
	bl	__Func_929d8
.L21c:
	mov	r3, #0x80
	lsl	r3, #10
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L248
	mov	r1, r6
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #0xfe
	and	r3, r2
	strb	r3, [r1]
	mov	r3, r8
	ldrb	r2, [r7]
	ldrb	r1, [r3, #9]
	and	r2, r5
	mov	r3, r9
	and	r3, r1
	lsl	r2, #2
	orr	r3, r2
	mov	r1, r8
	strb	r3, [r1, #9]
.L248:
	mov	r2, #0x80
	lsl	r2, #12
	mov	r3, r10
	and	r2, r3
	cmp	r2, #0
	beq	.L25c
	ldr	r3, [r7, #8]
	str	r3, [r6, #0x18]
	ldr	r3, [r7, #0xc]
	str	r3, [r6, #0x1c]
.L25c:
	mov	r3, #0x80
	lsl	r3, #11
	mov	r1, r10
	and	r3, r1
	cmp	r3, #0
	beq	.L2a6
	ldr	r3, =.L4
	mov	r1, r11
	ldr	r5, [r3, r1]
	cmp	r2, #0
	beq	.L28a
	ldr	r0, [r7, #0x10]
	ldr	r3, [r6, #0x18]
	ldr	r1, [r5, #0xc]
	sub	r0, r3
	bl	_Func_af0
	str	r0, [r6, #0x30]
	ldr	r0, [r7, #0x14]
	ldr	r3, [r6, #0x1c]
	ldr	r1, [r5, #0xc]
	sub	r0, r3
	b	.L2a0
.L28a:
	ldr	r0, [r7, #0x10]
	ldr	r2, =0xffff0000
	ldr	r1, [r5, #0xc]
	add	r0, r2
	bl	_Func_af0
	str	r0, [r6, #0x30]
	ldr	r0, [r7, #0x14]
	ldr	r3, =0xffff0000
	ldr	r1, [r5, #0xc]
	add	r0, r3
.L2a0:
	bl	_Func_af0
	str	r0, [r6, #0x34]
.L2a6:
	mov	r3, #0x80
	lsl	r3, #14
	mov	r1, r10
	and	r3, r1
	cmp	r3, #0
	beq	.L2c2
	mov	r0, r6
	mov	r1, #1
	bl	__Func_c300
	ldr	r1, [r7, #0x1c]
	mov	r0, r6
	bl	__Func_c2d8
.L2c2:
	mov	r3, #0x80
	lsl	r3, #15
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L2d4
	ldrh	r3, [r7, #0x20]
	mov	r1, r8
	strh	r3, [r1, #0x1e]
.L2d4:
	mov	r3, #0x80
	lsl	r3, #16
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L2e6
	ldrh	r3, [r7, #0x22]
	ldr	r1, [sp]
	strh	r3, [r1]
.L2e6:
	mov	r3, #0x80
	lsl	r3, #17
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L2f6
	ldr	r3, [r7, #0x24]
	str	r3, [r6, #0x6c]
.L2f6:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_common0_10c

	.section .data

.L1:
	.incbin "overlays/rom_78ef88/orig.bin", 0x4b1c, (0x4b54-0x4b1c)
.L2:
	.incbin "overlays/rom_78ef88/orig.bin", 0x4b54, (0x4b8c-0x4b54)
.L3:
	.incbin "overlays/rom_78ef88/orig.bin", 0x4b8c, (0x4bc4-0x4b8c)

	.section .data1

.L4:
	.word	.L1
	.word	.L2
	.word	.L3
