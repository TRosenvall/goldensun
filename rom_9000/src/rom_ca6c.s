	.include "macros.inc"
	.include "gba.inc"

@ WaitForEntityIdle
@ r0=entity. Blocks until Func_ca98 reports the entity has no movement target
@ left, yielding one frame at a time with Func_30f8(1). Gives up after 0x257
@ (599) frames so a stuck entity cannot hang the caller.
.thumb_func_start Func_ca6c
	push	{r5, r6, lr}
	mov	r6, r0
	mov	r5, #0
	b	.Lca7c
.Lca74:
	mov	r0, #1
	bl	Func_30f8
	add	r5, #1
.Lca7c:
	ldr	r3, =0x257
	cmp	r5, r3
	bgt	.Lca8c
	mov	r0, r6
	bl	Func_ca98
	cmp	r0, #0
	beq	.Lca74
.Lca8c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_ca6c

@ IsEntityIdle
@ r0=entity. Returns 1 when every active movement target is cleared to the
@ 0x80000000 "none" sentinel, else 0. The x target (+0x38) and z target (+0x40)
@ are always required; the y target (+0x3C) is only checked when vertical
@ motion is enabled, i.e. when the mode byte at +0x55 is 0.
.thumb_func_start Func_ca98
	push	{lr}
	mov	r3, r0
	add	r3, #0x55
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.Lcab2
	mov	r2, #0x80
	ldr	r3, [r0, #0x38]
	lsl	r2, #24
	cmp	r3, r2
	bne	.Lcac4
	ldr	r2, [r0, #0x3c]
	b	.Lcab8
.Lcab2:
	mov	r3, #0x80
	ldr	r2, [r0, #0x38]
	lsl	r3, #24
.Lcab8:
	cmp	r2, r3
	bne	.Lcac4
	ldr	r3, [r0, #0x40]
	mov	r0, #1
	cmp	r3, r2
	beq	.Lcac6
.Lcac4:
	mov	r0, #0
.Lcac6:
	pop	{r1}
	bx	r1
.func_end Func_ca98

@ UpdateEntities
@ Per-frame update over the 0x40 entities at [iwram_1e64], stride 0x70. Takes no
@ arguments. This is the full Thumb version of the reduced ARM loop Func_a494
@ (rom_92b8.s) -- same entity layout, more behaviour.
@ Per entity, skipping slots with a null script (+0x00) or a set freeze flag
@ (+0x5B):
@   1) run the hook at +0x6C
@   2) tick the wait timer at +0x5E; at zero, run the script VM -- opcodes
@      <= 0x3F dispatch through Data_13624, higher values just advance the
@      cursor at +0x04
@   3) unless suspended by +0x61, integrate movement. With a target set
@      (+0x38/+0x3C/+0x40 != 0x80000000) accelerate toward it by +0x34 and clamp
@      the speed to +0x30, using Func_948/Func_45d4 for length and Func_8ac for
@      the reciprocal; with no target, decay the existing velocity instead.
@   4) apply the mode bits at +0x55:
@        bit 0 -- follow terrain: Func_11f54 returns the ground height for the
@                 tile type at +0x22; the step is clamped to half of +0x34 and
@                 tripled to bound the climb rate
@        bit 1 -- gravity/landing against the height cached at +0x14, with the
@                 restitution factor at +0x44 and the cutoff at +0x48
@        bit 2 -- vertical oscillation driven by the .L131c0 table indexed by
@                 (+0x44 & 0x3F) >> 1, scaled by +0x48 and shifted right 4 or 6
@                 depending on bit 3
@        bit 4 -- suppress the speed clamp in step 3
@   5) when bit 7 of +0x59 is set, test the new position with Func_d924; a hit
@      bumps the collision counter at +0x60 and abandons the move
@   6) detect arrival on the axis named by +0x56 (0x10 = x, 0x11 = y, 0x12 = z)
@      by sign-change; on arrival optionally snap to the target (+0x58) and
@      clear all three targets
@   7) when bit 0 of +0x5A is set, turn the facing angle at +0x06 toward the
@      direction of travel via Func_44d0, clamped to +/-0x1000 per frame
.thumb_func_start Func_cacc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1e64
	ldr	r6, [r3]
	mov	r1, r6
	mov	r2, r6
	sub	sp, #0x30
	mov	r0, #0x3f
	add	r1, #0x55
	add	r2, #0x56
	str	r0, [sp, #0x20]
	str	r1, [sp, #4]
	str	r2, [sp]
.Lcaf0:
	mov	r3, #0
	str	r3, [sp, #0xc]
	ldr	r2, [r6]
	cmp	r2, #0
	bne	.Lcafc
	b	.Ld0fe
.Lcafc:
	ldr	r3, [r6, #0x6c]
	cmp	r3, #0
	beq	.Lcb0a
	mov	r0, r6
	bl	_call_via_r3
	ldr	r2, [r6]
.Lcb0a:
	cmp	r2, #0
	bne	.Lcb10
	b	.Ld0fe
.Lcb10:
	ldr	r4, [sp, #4]
	ldrb	r3, [r4, #6]
	cmp	r3, #0
	beq	.Lcb1a
	b	.Ld0fe
.Lcb1a:
	ldr	r0, [sp]
	mov	r4, #8
	ldrsh	r3, [r0, r4]
	ldrh	r1, [r0, #8]
	cmp	r3, #0
	beq	.Lcb2c
	sub	r3, r1, #1
	strh	r3, [r0, #8]
	b	.Lcb5a
.Lcb2c:
	ldr	r5, =Data_13624
	b	.Lcb36
.Lcb30:
	add	r3, r1, #1
	strh	r3, [r6, #4]
.Lcb34:
	ldr	r2, [r6]
.Lcb36:
	mov	r4, #4
	ldrsh	r3, [r6, r4]
	lsl	r3, #2
	ldr	r3, [r2, r3]
	ldrh	r1, [r6, #4]
	cmp	r3, #0x3f
	bhi	.Lcb30
	lsl	r3, #2
	ldr	r3, [r5, r3]
	mov	r0, r6
	bl	_call_via_r3
	cmp	r0, #0
	bne	.Lcb34
	ldr	r3, [r6]
	cmp	r3, #0
	bne	.Lcb5a
	b	.Ld0fe
.Lcb5a:
	ldr	r0, [r6, #8]
	str	r0, [sp, #0x1c]
	ldr	r1, [r6, #0xc]
	str	r1, [sp, #0x18]
	ldr	r2, [r6, #0x10]
	ldr	r4, [sp, #4]
	str	r2, [sp, #0x14]
	ldrb	r3, [r4, #0xc]
	cmp	r3, #0
	beq	.Lcb70
	b	.Lcfd6
.Lcb70:
	str	r4, [sp, #8]
	ldrb	r3, [r4]
	cmp	r3, #0
	beq	.Lcb7a
	b	.Lccf8
.Lcb7a:
	mov	r0, #0x80
	ldr	r3, [r6, #0x38]
	lsl	r0, #24
	cmp	r3, r0
	bne	.Lcb86
	b	.Lcc7c
.Lcb86:
	ldr	r1, [sp, #0x1c]
	sub	r0, r3, r1
	cmp	r0, #0
	bge	.Lcb92
	ldr	r2, =0xffff
	add	r0, r2
.Lcb92:
	ldr	r3, [r6, #0x3c]
	ldr	r4, [sp, #0x18]
	asr	r0, #16
	mov	r8, r0
	sub	r0, r3, r4
	cmp	r0, #0
	bge	.Lcba4
	ldr	r1, =0xffff
	add	r0, r1
.Lcba4:
	ldr	r3, [r6, #0x40]
	ldr	r2, [sp, #0x14]
	asr	r0, #16
	mov	r11, r0
	sub	r0, r3, r2
	cmp	r0, #0
	bge	.Lcbb6
	ldr	r3, =0xffff
	add	r0, r3
.Lcbb6:
	asr	r0, #16
	mov	r9, r0
	mov	r4, r8
	mov	r1, r11
	mov	r0, r8
	mul	r0, r4
	mov	r3, r11
	mul	r3, r1
	mov	r4, r9
	mov	r2, r9
	mul	r2, r4
	add	r0, r3
	add	r0, r2
	ldr	r3, =Func_948
	bl	_call_via_r3
	cmp	r0, #0
	bne	.Lcbe8
	ldr	r0, [r6, #0x38]
	str	r0, [sp, #0x1c]
	ldr	r1, [r6, #0x3c]
	str	r1, [sp, #0x18]
	ldr	r2, [r6, #0x40]
	str	r2, [sp, #0x14]
	b	.Lcfd6
.Lcbe8:
	ldr	r1, [r6, #0x34]
	ldr	r3, =Func_8ac
	lsl	r0, #16
	bl	_call_via_r3
	mov	r5, r0
	mov	r3, r8
	mul	r3, r5
	ldr	r0, [r6, #0x24]
	add	r0, r3
	mov	r3, r11
	mul	r3, r5
	mov	r2, r9
	mul	r2, r5
	ldr	r1, [r6, #0x28]
	add	r1, r3
	ldr	r3, [r6, #0x2c]
	ldr	r4, =Func_888
	add	r3, r2
	mov	r10, r0
	str	r1, [r6, #0x28]
	mov	r7, r1
	str	r0, [r6, #0x24]
	str	r3, [r6, #0x2c]
	mov	r9, r3
	mov	r8, r4
	mov	r1, r10
	.call_via r8
	mov	r3, r0
	mov	r1, r7
	mov	r0, r7
	.call_via r8
	mov	r4, r0
	mov	r1, r9
	mov	r0, r9
	.call_via r8
	add	r3, r4
	add	r3, r0
	mov	r0, r3
	bl	Func_45d4
	ldr	r1, [r6, #0x30]
	cmp	r0, r1
	bgt	.Lcc4e
	b	.Lcfd6
.Lcc4e:
	ldr	r2, =Func_8ac
	bl	_call_via_r2
	mov	r5, r0
	mov	r1, r5
	mov	r0, r10
	.call_via r8
	str	r0, [r6, #0x24]
	mov	r1, r5
	mov	r0, r7
	.call_via r8
	str	r0, [r6, #0x28]
	mov	r1, r5
	mov	r0, r9
	.call_via r8
	str	r0, [r6, #0x2c]
	b	.Lcfd6
.Lcc7c:
	ldr	r3, [r6, #0x24]
	ldr	r0, [r6, #0x2c]
	ldr	r4, [r6, #0x28]
	mov	r8, r3
	mov	r9, r0
	mov	r11, r4
	ldr	r7, =Func_888
	mov	r0, r8
	mov	r1, r8
	.call_via r7
	mov	r3, r0
	mov	r1, r11
	mov	r0, r11
	.call_via r7
	mov	r4, r0
	mov	r1, r9
	mov	r0, r9
	.call_via r7
	add	r3, r4
	add	r3, r0
	mov	r0, r3
	bl	Func_45d4
	cmp	r0, #0
	beq	.Lccf0
	ldr	r3, [r6, #0x34]
	sub	r1, r0, r3
	cmp	r1, #0
	bge	.Lccc4
	mov	r1, #0
.Lccc4:
	ldr	r3, =Func_8ac
	bl	_call_via_r3
	mov	r5, r0
	mov	r1, r5
	mov	r0, r8
	.call_via r7
	str	r0, [r6, #0x24]
	mov	r1, r5
	mov	r0, r11
	.call_via r7
	str	r0, [r6, #0x28]
	mov	r1, r5
	mov	r0, r9
	.call_via r7
	str	r0, [r6, #0x2c]
	b	.Lcfd6
.Lccf0:
	str	r0, [r6, #0x24]
	str	r0, [r6, #0x28]
	str	r0, [r6, #0x2c]
	b	.Lcfd6
.Lccf8:
	mov	r1, #0x80
	ldr	r3, [r6, #0x38]
	lsl	r1, #24
	cmp	r3, r1
	bne	.Lcd04
	b	.Lce0e
.Lcd04:
	ldr	r2, [sp, #0x1c]
	sub	r0, r3, r2
	cmp	r0, #0
	bge	.Lcd10
	ldr	r3, =0xffff
	add	r0, r3
.Lcd10:
	ldr	r3, [r6, #0x40]
	ldr	r4, [sp, #0x14]
	asr	r0, #16
	mov	r8, r0
	sub	r0, r3, r4
	cmp	r0, #0
	bge	.Lcd22
	ldr	r1, =0xffff
	add	r0, r1
.Lcd22:
	asr	r0, #16
	mov	r9, r0
	mov	r2, r8
	mov	r4, r9
	mov	r3, r9
	mul	r3, r4
	mov	r0, r8
	mul	r0, r2
	add	r0, r3
	ldr	r3, =Func_948
	bl	_call_via_r3
	ldr	r1, =0xffffff
	lsl	r0, #16
	cmp	r0, r1
	bgt	.Lcd70
	ldr	r3, [r6, #0x38]
	ldr	r2, [sp, #0x1c]
	ldr	r4, [sp, #0x14]
	sub	r2, r3, r2
	ldr	r3, [r6, #0x40]
	mov	r8, r2
	sub	r4, r3, r4
	mov	r9, r4
	mov	r0, r8
	ldr	r4, =Func_888
	mov	r1, r8
	.call_via r4
	mov	r3, r0
	mov	r1, r9
	mov	r0, r9
	.call_via r4
	add	r3, r0
	mov	r0, r3
	bl	Func_45d4
.Lcd70:
	cmp	r0, #0
	bne	.Lcd9c
	ldr	r0, [r6, #0x38]
	str	r0, [sp, #0x1c]
	ldr	r1, [r6, #0x40]
	str	r1, [sp, #0x14]
	b	.Lce66

	.pool_aligned

.Lcd9c:
	ldr	r2, =Func_8ac
	ldr	r1, [r6, #0x34]
	mov	r11, r2
	bl	_call_via_r11
	mov	r5, r0
	ldr	r7, =Func_888
	mov	r0, r8
	mov	r1, r5
	.call_via r7
	ldr	r4, [r6, #0x24]
	add	r4, r0
	str	r4, [r6, #0x24]
	mov	r0, r9
	mov	r1, r5
	.call_via r7
	ldr	r3, [r6, #0x2c]
	mov	r10, r4
	add	r3, r0
	str	r3, [r6, #0x2c]
	mov	r9, r3
	mov	r0, r10
	mov	r1, r10
	.call_via r7
	mov	r3, r0
	mov	r1, r9
	mov	r0, r9
	.call_via r7
	add	r3, r0
	mov	r0, r3
	bl	Func_45d4
	ldr	r1, [r6, #0x30]
	cmp	r0, r1
	ble	.Lce66
	bl	_call_via_r11
	mov	r5, r0
	mov	r1, r5
	mov	r0, r10
	.call_via r7
	str	r0, [r6, #0x24]
	mov	r1, r5
	mov	r0, r9
	.call_via r7
	b	.Lce64
.Lce0e:
	ldr	r3, [r6, #0x24]
	ldr	r4, [r6, #0x2c]
	mov	r8, r3
	mov	r9, r4
	ldr	r7, =Func_888
	mov	r0, r8
	mov	r1, r8
	.call_via r7
	mov	r3, r0
	mov	r1, r9
	mov	r0, r9
	.call_via r7
	add	r3, r0
	mov	r0, r3
	bl	Func_45d4
	cmp	r0, #0
	beq	.Lce62
	ldr	r3, [r6, #0x34]
	sub	r1, r0, r3
	cmp	r1, #0
	bge	.Lce42
	mov	r1, #0
.Lce42:
	ldr	r3, =Func_8ac
	bl	_call_via_r3
	mov	r5, r0
	mov	r1, r5
	mov	r0, r8
	.call_via r7
	str	r0, [r6, #0x24]
	mov	r1, r5
	mov	r0, r9
	.call_via r7
	b	.Lce64
.Lce62:
	str	r0, [r6, #0x24]
.Lce64:
	str	r0, [r6, #0x2c]
.Lce66:
	ldr	r0, [sp, #8]
	ldrb	r2, [r0]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	bne	.Lce74
	b	.Lcf42
.Lce74:
	ldr	r3, [r6, #0x24]
	ldr	r1, [sp, #0x1c]
	ldr	r2, [sp, #0x14]
	add	r1, r3
	ldr	r3, [r6, #0x2c]
	add	r2, r3
	mov	r3, r6
	add	r3, #0x22
	ldrb	r0, [r3]
	mov	r10, r1
	mov	r9, r2
	bl	Func_11f54
	str	r0, [sp, #0x10]
	ldr	r4, [sp, #0x18]
	ldr	r3, [r6, #0x14]
	sub	r7, r0, r3
	sub	r3, r0, r4
	ldr	r0, =0xfffc0000
	cmp	r3, r0
	ble	.Lcea2
	add	r4, r7
	str	r4, [sp, #0x18]
.Lcea2:
	cmp	r7, #0
	bge	.Lcea8
	neg	r7, r7
.Lcea8:
	ldr	r3, [r6, #0x34]
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	cmp	r7, r3
	ble	.Lceb6
	mov	r7, r3
.Lceb6:
	lsl	r3, r7, #1
	add	r7, r3, r7
	cmp	r7, #0
	beq	.Lcf3e
	ldr	r1, [sp, #8]
	ldrb	r2, [r1]
	mov	r3, #0x10
	and	r3, r2
	cmp	r3, #0
	bne	.Lcf3e
	ldr	r2, [r6, #0x24]
	ldr	r0, =Func_888
	ldr	r3, [r6, #0x28]
	ldr	r4, [r6, #0x2c]
	mov	r8, r2
	mov	r10, r0
	mov	r11, r3
	mov	r9, r4
	mov	r0, r8
	mov	r1, r8
	.call_via r10
	mov	r3, r0
	mov	r1, r11
	mov	r0, r11
	.call_via r10
	mov	r4, r0
	mov	r1, r9
	mov	r0, r9
	.call_via r10
	add	r3, r4
	add	r3, r0
	mov	r0, r3
	bl	Func_45d4
	cmp	r0, #0
	beq	.Lcf3e
	sub	r1, r0, r7
	cmp	r1, #0
	bge	.Lcf12
	mov	r1, #0
.Lcf12:
	ldr	r3, =Func_8ac
	bl	_call_via_r3
	mov	r5, r0
	mov	r1, r5
	mov	r0, r8
	.call_via r10
	str	r0, [r6, #0x24]
	mov	r1, r5
	mov	r0, r11
	.call_via r10
	str	r0, [r6, #0x28]
	mov	r1, r5
	mov	r0, r9
	.call_via r10
	str	r0, [r6, #0x2c]
.Lcf3e:
	ldr	r1, [sp, #0x10]
	str	r1, [r6, #0x14]
.Lcf42:
	ldr	r3, [sp, #8]
	ldrb	r2, [r3]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lcf8a
	ldr	r4, [r6, #0x14]
	ldr	r0, [sp, #0x18]
	str	r4, [sp, #0x10]
	cmp	r0, r4
	ble	.Lcf60
	ldr	r3, [r6, #0x28]
	ldr	r2, [r6, #0x48]
	sub	r3, r2
	b	.Lcf88
.Lcf60:
	ldr	r0, [r6, #0x28]
	cmp	r0, #0
	bge	.Lcf8a
	ldr	r1, [sp, #0x10]
	str	r1, [sp, #0x18]
	ldr	r3, =Func_888
	ldr	r1, [r6, #0x44]
	.call_via r3
	neg	r3, r0
	mov	r2, r3
	str	r3, [r6, #0x28]
	cmp	r2, #0
	bge	.Lcf80
	mov	r2, r0
.Lcf80:
	ldr	r3, [r6, #0x48]
	cmp	r2, r3
	bgt	.Lcf8a
	mov	r3, #0
.Lcf88:
	str	r3, [r6, #0x28]
.Lcf8a:
	ldr	r3, [sp, #8]
	ldrb	r2, [r3]
	mov	r3, #4
	and	r3, r2
	cmp	r3, #0
	beq	.Lcfd6
	ldr	r1, [r6, #0x44]
	mov	r3, #0x3f
	and	r1, r3
	mov	r3, #8
	and	r3, r2
	cmp	r3, #0
	beq	.Lcfba
	ldr	r2, =.L131c0
	lsr	r3, r1, #1
	lsl	r3, #2
	ldr	r2, [r2, r3]
	ldr	r3, [r6, #0x48]
	mul	r3, r2
	cmp	r3, #0
	bge	.Lcfb6
	add	r3, #0xf
.Lcfb6:
	asr	r3, #4
	b	.Lcfce
.Lcfba:
	ldr	r2, =.L131c0
	lsr	r3, r1, #1
	lsl	r3, #2
	ldr	r2, [r2, r3]
	ldr	r3, [r6, #0x48]
	mul	r3, r2
	cmp	r3, #0
	bge	.Lcfcc
	add	r3, #0x3f
.Lcfcc:
	asr	r3, #6
.Lcfce:
	str	r3, [r6, #0x28]
	ldr	r3, [r6, #0x44]
	add	r3, #1
	str	r3, [r6, #0x44]
.Lcfd6:
	ldr	r3, [r6, #0x24]
	ldr	r4, [sp, #0x1c]
	add	r4, r3
	str	r4, [sp, #0x1c]
	ldr	r0, [sp, #0x18]
	ldr	r3, [r6, #0x28]
	add	r0, r3
	str	r0, [sp, #0x18]
	ldr	r1, [sp, #0x14]
	ldr	r3, [r6, #0x2c]
	add	r1, r3
	str	r1, [sp, #0x14]
	ldr	r3, [sp, #4]
	ldrb	r2, [r3, #4]
	mov	r3, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.Ld01c
	add	r1, sp, #0x24
	str	r0, [r1, #4]
	str	r4, [r1]
	ldr	r4, [sp, #0x14]
	mov	r0, r6
	str	r4, [r1, #8]
	bl	Func_d924
	cmp	r0, #0
	beq	.Ld018
	ldr	r0, [sp, #4]
	ldrb	r3, [r0, #0xb]
	add	r3, #1
	strb	r3, [r0, #0xb]
	b	.Ld0fe
.Ld018:
	ldr	r1, [sp, #4]
	strb	r0, [r1, #0xb]
.Ld01c:
	ldr	r1, [sp]
	ldrb	r3, [r1]
	cmp	r3, #0x11
	beq	.Ld042
	cmp	r3, #0x11
	bgt	.Ld02e
	cmp	r3, #0x10
	beq	.Ld034
	b	.Ld07a
.Ld02e:
	cmp	r3, #0x12
	beq	.Ld060
	b	.Ld07a
.Ld034:
	ldr	r2, [r6, #0x38]
	ldr	r3, [sp, #0x1c]
	cmp	r3, r2
	beq	.Ld076
	ldr	r3, [r6, #8]
	ldr	r4, [sp, #0x1c]
	b	.Ld06c
.Ld042:
	ldr	r2, [r6, #0x3c]
	ldr	r3, [sp, #0x18]
	cmp	r3, r2
	beq	.Ld076
	ldr	r3, [r6, #0xc]
	ldr	r4, [sp, #0x18]
	b	.Ld06c

	.pool_aligned

.Ld060:
	ldr	r2, [r6, #0x40]
	ldr	r3, [sp, #0x14]
	cmp	r3, r2
	beq	.Ld076
	ldr	r3, [r6, #0x10]
	ldr	r4, [sp, #0x14]
.Ld06c:
	sub	r3, r2
	sub	r2, r4, r2
	eor	r3, r2
	cmp	r3, #0
	bge	.Ld07a
.Ld076:
	mov	r0, #1
	str	r0, [sp, #0xc]
.Ld07a:
	ldr	r2, [sp, #0xc]
	cmp	r2, #0
	beq	.Ld0b0
	ldr	r4, [sp, #4]
	ldrb	r3, [r4, #3]
	cmp	r3, #0
	beq	.Ld0a2
	ldr	r0, [r6, #0x38]
	str	r0, [sp, #0x1c]
	ldr	r2, [r6, #0x40]
	mov	r3, #0
	str	r3, [r6, #0x24]
	str	r2, [sp, #0x14]
	str	r3, [r6, #0x2c]
	ldrb	r3, [r4]
	cmp	r3, #0
	bne	.Ld0a2
	ldr	r4, [r6, #0x3c]
	str	r4, [sp, #0x18]
	str	r3, [r6, #0x28]
.Ld0a2:
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r6, #0x38]
	str	r3, [r6, #0x3c]
	str	r3, [r6, #0x40]
	mov	r3, #0
	strb	r3, [r1]
.Ld0b0:
	ldr	r0, [sp, #0x1c]
	str	r0, [r6, #8]
	ldr	r1, [sp, #0x18]
	str	r1, [r6, #0xc]
	ldr	r2, [sp, #0x14]
	str	r2, [r6, #0x10]
	ldr	r3, [sp, #4]
	ldrb	r2, [r3, #5]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Ld0fe
	ldr	r4, [r6, #0x24]
	str	r4, [sp, #0x1c]
	ldr	r0, [r6, #0x2c]
	str	r0, [sp, #0x14]
	cmp	r4, #0
	bne	.Ld0d8
	cmp	r0, #0
	beq	.Ld0fe
.Ld0d8:
	ldr	r0, [sp, #0x14]
	ldr	r1, [sp, #0x1c]
	bl	Func_44d0
	ldrh	r3, [r6, #6]
	sub	r0, r3
	lsl	r0, #16
	mov	r2, #0x80
	asr	r0, #16
	lsl	r2, #5
	cmp	r0, r2
	ble	.Ld0f2
	mov	r0, r2
.Ld0f2:
	ldr	r2, =0xfffff000
	cmp	r0, r2
	bge	.Ld0fa
	mov	r0, r2
.Ld0fa:
	add	r3, r0
	strh	r3, [r6, #6]
.Ld0fe:
	ldr	r1, [sp, #0x20]
	ldr	r2, [sp, #4]
	ldr	r3, [sp]
	sub	r1, #1
	add	r2, #0x70
	add	r3, #0x70
	str	r1, [sp, #0x20]
	str	r2, [sp, #4]
	str	r3, [sp]
	add	r6, #0x70
	cmp	r1, #0
	blt	.Ld118
	b	.Lcaf0
.Ld118:
	add	sp, #0x30
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_cacc

@ SetEntityPosition
@ r0=entity, r1=x, r2=y, r3=z (all 16.16). Teleports the entity: writes the
@ position to +0x08/+0x0C/+0x10, clears the velocity at +0x24/+0x28/+0x2C and
@ resets all three movement targets to the 0x80000000 "none" sentinel, so any
@ in-flight move is abandoned.
.thumb_func_start Func_d130
	str	r3, [r0, #0x10]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r0, #0x38]
	str	r3, [r0, #0x3c]
	str	r3, [r0, #0x40]
	mov	r3, #0
	str	r1, [r0, #8]
	str	r2, [r0, #0xc]
	str	r3, [r0, #0x24]
	str	r3, [r0, #0x28]
	str	r3, [r0, #0x2c]
	bx	lr
.func_end Func_d130

@ SetEntityMoveTarget
@ r0=entity, r1=target x, r2=target y, r3=target z (16.16). Starts a move
@ toward the given point.
@ Measures the distance with Func_948, refining it through Func_888/Func_45d4
@ when it is under 0x100000. A distance below 1.0 (0x10000) degenerates to a
@ teleport: the position is written straight to +0x08/+0x0C/+0x10 and the
@ targets are left cleared. Otherwise, unless +0x58 requests an exact stop, the
@ target is extended by a braking allowance derived from the max speed at +0x30
@ and the acceleration at +0x34, so the entity decelerates onto the point
@ instead of overshooting.
@ Writes the (possibly extended) target to +0x38/+0x3C/+0x40 and records which
@ axis the arrival test should watch in +0x56: 0x10 (x) by default, 0x12 (z)
@ when |dz| exceeds |dx|, and 0x11 (y) when vertical motion is enabled
@ (+0x55 == 0) and |dy| exceeds the dominant horizontal component.
.thumb_func_start Func_d14c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r0
	mov	r9, r3
	ldr	r3, [r7, #8]
	sub	r0, r1, r3
	mov	r8, r1
	mov	r10, r2
	cmp	r0, #0
	bge	.Ld16a
	ldr	r2, =0xffff
	add	r0, r2
.Ld16a:
	ldr	r3, [r7, #0xc]
	mov	r2, r10
	asr	r1, r0, #16
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.Ld17a
	ldr	r3, =0xffff
	add	r0, r3
.Ld17a:
	ldr	r3, [r7, #0x10]
	mov	r2, r9
	asr	r5, r0, #16
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.Ld18a
	ldr	r3, =0xffff
	add	r0, r3
.Ld18a:
	mov	r3, r5
	mul	r3, r5
	asr	r6, r0, #16
	mov	r0, r1
	mul	r0, r1
	mov	r2, r6
	mul	r2, r6
	add	r0, r3
	add	r0, r2
	ldr	r3, =Func_948
	bl	_call_via_r3
	mov	r2, #0x80
	lsl	r5, r0, #16
	lsl	r2, #13
	cmp	r5, r2
	bge	.Ld1ec
	ldr	r3, [r7, #8]
	mov	r2, r8
	sub	r1, r2, r3
	ldr	r3, [r7, #0xc]
	mov	r2, r10
	sub	r5, r2, r3
	ldr	r3, [r7, #0x10]
	mov	r2, r9
	sub	r6, r2, r3
	mov	r0, r1
	ldr	r3, =Func_888
	.call_via r3
	mov	r4, r0
	mov	r1, r5
	mov	r0, r5
	.call_via r3
	mov	r5, r0
	mov	r1, r6
	mov	r0, r6
	.call_via r3
	add	r4, r5
	add	r4, r0
	mov	r0, r4
	bl	Func_45d4
	mov	r5, r0
.Ld1ec:
	mov	r3, #0x80
	lsl	r3, #9
	cmp	r5, r3
	bge	.Ld20c
	mov	r3, r10
	mov	r2, r8
	str	r3, [r7, #0xc]
	mov	r3, #0x80
	lsl	r3, #24
	str	r2, [r7, #8]
	mov	r2, r9
	str	r2, [r7, #0x10]
	str	r3, [r7, #0x38]
	str	r3, [r7, #0x3c]
	str	r3, [r7, #0x40]
	b	.Ld2e4
.Ld20c:
	mov	r3, r7
	add	r3, #0x58
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.Ld27c
	ldr	r1, [r7, #0x30]
	ldr	r3, =Func_888
	mov	r0, r1
	.call_via r3
	mov	r1, r0
	ldr	r3, =Func_8ac
	ldr	r0, [r7, #0x34]
	bl	_call_via_r3
	mov	r1, r0
	cmp	r5, r1
	ble	.Ld23a
	lsr	r3, r1, #31
	add	r3, r1, r3
	asr	r3, #1
	sub	r1, r5, r3
	b	.Ld240
.Ld23a:
	lsr	r3, r5, #31
	add	r3, r5, r3
	asr	r1, r3, #1
.Ld240:
	ldr	r3, =Func_8ac
	mov	r0, r5
	bl	_call_via_r3
	ldr	r4, [r7, #8]
	mov	r5, r0
	mov	r2, r8
	ldr	r3, =Func_888
	sub	r0, r2, r4
	mov	r1, r5
	.call_via r3
	add	r4, r0
	mov	r8, r4
	ldr	r4, [r7, #0xc]
	mov	r2, r10
	sub	r0, r2, r4
	mov	r1, r5
	.call_via r3
	add	r4, r0
	mov	r10, r4
	ldr	r4, [r7, #0x10]
	mov	r2, r9
	sub	r0, r2, r4
	mov	r1, r5
	.call_via r3
	add	r4, r0
	mov	r9, r4
.Ld27c:
	mov	r3, r8
	str	r3, [r7, #0x38]
	mov	r3, r9
	mov	r2, r10
	str	r3, [r7, #0x40]
	ldr	r3, [r7, #8]
	str	r2, [r7, #0x3c]
	mov	r2, r8
	sub	r1, r2, r3
	ldr	r3, [r7, #0xc]
	mov	r2, r10
	sub	r5, r2, r3
	ldr	r3, [r7, #0x10]
	mov	r2, r9
	sub	r6, r2, r3
	mov	r3, #0x56
	add	r3, r7
	mov	r12, r3
	mov	r2, r12
	mov	r3, #0x10
	strb	r3, [r2]
	mov	r2, r1
	cmp	r1, #0
	bge	.Ld2ae
	neg	r2, r1
.Ld2ae:
	mov	r3, r6
	cmp	r6, #0
	bge	.Ld2b6
	neg	r3, r6
.Ld2b6:
	cmp	r2, r3
	bge	.Ld2c2
	mov	r3, #0x12
	mov	r2, r12
	strb	r3, [r2]
	mov	r1, r6
.Ld2c2:
	mov	r3, r7
	add	r3, #0x55
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.Ld2e4
	cmp	r1, #0
	bge	.Ld2d2
	neg	r1, r1
.Ld2d2:
	mov	r0, r5
	cmp	r0, #0
	bge	.Ld2da
	neg	r0, r0
.Ld2da:
	cmp	r1, r0
	bge	.Ld2e4
	mov	r3, #0x11
	mov	r2, r12
	strb	r3, [r2]
.Ld2e4:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_d14c

@ RunEntityUpdateFromRam
@ Takes no arguments. Allocates a 0x4E8-byte buffer (Func_4938), DMA-copies the
@ ARM entity update Func_a494 (rom_92b8.s) into it, calls the RAM copy, then
@ frees the buffer with Func_2df0. The copy exists purely so the hot ARM loop
@ executes out of RAM rather than ROM.
.thumb_func_start Func_d304
	push	{r5, r6, lr}
	ldr	r5, =0x4e8
	mov	r0, r5
	bl	Func_4938
	mov	r2, #0x84
	mov	r6, r0
	lsr	r5, #2
	lsl	r2, #24
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_a494
	mov	r1, r6
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bl	_call_via_r6
	mov	r0, r6
	bl	Func_2df0
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_d304

@ UpdateEntitiesXZ
@ Per-frame update over the first 14 entities at [iwram_1e64], stride 0x70.
@ Takes no arguments. A cut-down sibling of Func_cacc: target seeking is done
@ in the x/z plane only (the y axis is left to gravity), and the terrain-follow,
@ oscillation and collision steps are absent.
@ Per entity with a non-null script (+0x00):
@   - seek the target at +0x38/+0x40 with the acceleration at +0x34, clamped to
@     the max speed at +0x30; with no target, decay the velocity toward zero
@   - when bit 1 of +0x55 is set, apply the fall/landing step against the height
@     cached at +0x14 using the restitution at +0x44 and cutoff at +0x48
@   - integrate into +0x08/+0x0C/+0x10 and run the arrival test named by +0x56
@     (0x10 = x, 0x11 = y, 0x12 = z), clearing the targets on arrival and
@     optionally snapping to them per +0x58
@   - when bit 0 of +0x5A is set, turn the facing angle at +0x06 toward the
@     heading via Func_44d0, clamped to +/-0x1000 per frame
.thumb_func_start Func_d340
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1e64
	ldr	r5, [r3]
	mov	r2, r5
	sub	sp, #0x10
	mov	r0, #0xd
	add	r2, #0x55
	str	r0, [sp, #0xc]
	str	r2, [sp]
.Ld35e:
	ldr	r3, [r5]
	cmp	r3, #0
	bne	.Ld366
	b	.Ld62a
.Ld366:
	ldr	r4, [r5, #0xc]
	ldr	r3, [r5, #8]
	ldr	r2, [sp]
	str	r4, [sp, #8]
	mov	r11, r3
	ldr	r0, [r5, #0x10]
	ldrb	r3, [r2, #0xc]
	mov	r9, r0
	cmp	r3, #0
	beq	.Ld37c
	b	.Ld528
.Ld37c:
	mov	r3, #0
	str	r3, [sp, #4]
	mov	r4, #0x80
	ldr	r3, [r5, #0x38]
	lsl	r4, #24
	cmp	r3, r4
	bne	.Ld38c
	b	.Ld472
.Ld38c:
	mov	r2, r11
	sub	r0, r3, r2
	cmp	r0, #0
	bge	.Ld398
	ldr	r3, =0xffff
	add	r0, r3
.Ld398:
	ldr	r3, [r5, #0x40]
	mov	r4, r9
	asr	r6, r0, #16
	sub	r0, r3, r4
	cmp	r0, #0
	bge	.Ld3a8
	ldr	r2, =0xffff
	add	r0, r2
.Ld3a8:
	asr	r7, r0, #16
	mov	r3, r7
	mul	r3, r7
	mov	r0, r6
	mul	r0, r6
	add	r0, r3
	ldr	r3, =Func_948
	bl	_call_via_r3
	ldr	r4, =0xffffff
	lsl	r0, #16
	cmp	r0, r4
	bgt	.Ld3f0
	ldr	r3, [r5, #0x38]
	mov	r0, r11
	sub	r6, r3, r0
	ldr	r3, [r5, #0x40]
	mov	r2, r9
	sub	r7, r3, r2
	mov	r0, r6
	mov	r1, r6
	ldr	r3, =Func_888
	.call_via r3
	mov	r3, r0
	mov	r1, r7
	mov	r0, r7
	ldr	r4, =Func_888
	.call_via r4
	add	r3, r0
	mov	r0, r3
	ldr	r2, =Func_948
	bl	_call_via_r2
	lsl	r0, #8
.Ld3f0:
	cmp	r0, #0
	bne	.Ld3fe
	ldr	r3, [r5, #0x38]
	ldr	r4, [r5, #0x40]
	mov	r11, r3
	mov	r9, r4
	b	.Ld4dc
.Ld3fe:
	ldr	r1, [r5, #0x34]
	ldr	r2, =Func_8ac
	bl	_call_via_r2
	ldr	r3, =Func_888
	mov	r4, r0
	mov	r8, r3
	mov	r0, r6
	mov	r1, r4
	.call_via r8
	ldr	r3, [r5, #0x24]
	add	r3, r0
	mov	r10, r3
	str	r3, [r5, #0x24]
	mov	r0, r7
	mov	r1, r4
	.call_via r8
	ldr	r3, [r5, #0x2c]
	add	r6, r3, r0
	str	r6, [r5, #0x2c]
	mov	r0, r10
	mov	r1, r10
	.call_via r8
	mov	r3, r0
	mov	r1, r6
	mov	r0, r6
	.call_via r8
	add	r3, r0
	mov	r0, r3
	ldr	r4, =Func_948
	bl	_call_via_r4
	ldr	r1, [r5, #0x30]
	lsl	r0, #8
	cmp	r0, r1
	ble	.Ld4dc
	ldr	r2, =Func_8ac
	bl	_call_via_r2
	mov	r4, r0
	mov	r1, r4
	mov	r0, r10
	.call_via r8
	str	r0, [r5, #0x24]
	mov	r1, r4
	mov	r0, r6
	.call_via r8
	b	.Ld4da
.Ld472:
	ldr	r6, [r5, #0x24]
	ldr	r7, [r5, #0x2c]
	mov	r3, r6
	orr	r3, r7
	cmp	r3, #0
	beq	.Ld4dc
	ldr	r3, =Func_888
	mov	r0, r6
	mov	r8, r3
	mov	r1, r6
	.call_via r8
	mov	r3, r0
	mov	r1, r7
	mov	r0, r7
	.call_via r8
	add	r3, r0
	ldr	r2, =Func_948
	mov	r0, r3
	bl	_call_via_r2
	lsl	r0, #8
	cmp	r0, #0
	beq	.Ld4d6
	ldr	r3, [r5, #0x34]
	sub	r1, r0, r3
	cmp	r1, #0
	bge	.Ld4b8
	ldr	r4, [sp, #4]
	str	r4, [r5, #0x24]
	str	r4, [r5, #0x2c]
	b	.Ld4dc
.Ld4b8:
	ldr	r3, =Func_8ac
	bl	_call_via_r3
	mov	r4, r0
	mov	r1, r4
	mov	r0, r6
	.call_via r8
	str	r0, [r5, #0x24]
	mov	r1, r4
	mov	r0, r7
	.call_via r8
	b	.Ld4da
.Ld4d6:
	ldr	r0, [sp, #4]
	str	r0, [r5, #0x24]
.Ld4da:
	str	r0, [r5, #0x2c]
.Ld4dc:
	ldr	r3, [sp]
	ldrb	r2, [r3]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Ld528
	ldr	r3, [r5, #0x14]
	ldr	r4, [sp, #8]
	cmp	r4, r3
	ble	.Ld4fc
	ldr	r2, [r5, #0x28]
	ldr	r3, [r5, #0x48]
	sub	r2, r3
	str	r2, [r5, #0x28]
	mov	r0, r2
	b	.Ld52a
.Ld4fc:
	ldr	r0, [r5, #0x28]
	cmp	r0, #0
	bge	.Ld52a
	str	r3, [sp, #8]
	ldr	r3, =Func_888
	ldr	r1, [r5, #0x44]
	.call_via r3
	mov	r3, r0
	neg	r0, r3
	mov	r1, r0
	str	r0, [r5, #0x28]
	cmp	r1, #0
	bge	.Ld51a
	mov	r1, r3
.Ld51a:
	ldr	r3, [r5, #0x48]
	cmp	r1, r3
	bgt	.Ld52a
	mov	r3, #0
	str	r3, [r5, #0x28]
	mov	r0, #0
	b	.Ld52a
.Ld528:
	ldr	r0, [r5, #0x28]
.Ld52a:
	ldr	r4, [sp, #8]
	add	r4, r0
	ldr	r3, [r5, #0x24]
	str	r4, [sp, #8]
	add	r11, r3
	ldr	r0, [sp]
	ldr	r3, [r5, #0x2c]
	add	r9, r3
	ldrb	r3, [r0, #1]
	mov	r1, r5
	add	r1, #0x56
	cmp	r3, #0
	beq	.Ld5a4
	cmp	r3, #0x11
	beq	.Ld564
	cmp	r3, #0x11
	bgt	.Ld552
	cmp	r3, #0x10
	beq	.Ld558
	b	.Ld5a4
.Ld552:
	cmp	r3, #0x12
	beq	.Ld58c
	b	.Ld5a4
.Ld558:
	ldr	r2, [r5, #0x38]
	cmp	r11, r2
	beq	.Ld5a0
	ldr	r3, [r5, #8]
	mov	r4, r11
	b	.Ld596
.Ld564:
	ldr	r2, [r5, #0x3c]
	ldr	r3, [sp, #8]
	cmp	r3, r2
	beq	.Ld5a0
	ldr	r3, [r5, #0xc]
	ldr	r4, [sp, #8]
	b	.Ld596

	.pool_aligned

.Ld58c:
	ldr	r2, [r5, #0x40]
	cmp	r9, r2
	beq	.Ld5a0
	ldr	r3, [r5, #0x10]
	mov	r4, r9
.Ld596:
	sub	r3, r2
	sub	r2, r4, r2
	eor	r3, r2
	cmp	r3, #0
	bge	.Ld5a4
.Ld5a0:
	mov	r0, #1
	str	r0, [sp, #4]
.Ld5a4:
	ldr	r2, [sp, #4]
	cmp	r2, #0
	beq	.Ld5da
	ldr	r4, [sp]
	ldrb	r3, [r4, #3]
	cmp	r3, #0
	beq	.Ld5cc
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x2c]
	ldr	r0, [r5, #0x38]
	ldr	r2, [r5, #0x40]
	ldrb	r3, [r4]
	mov	r11, r0
	mov	r9, r2
	cmp	r3, #0
	bne	.Ld5cc
	ldr	r4, [r5, #0x3c]
	str	r4, [sp, #8]
	str	r3, [r5, #0x28]
.Ld5cc:
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x38]
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x40]
	mov	r3, #0
	strb	r3, [r1]
.Ld5da:
	mov	r0, r11
	str	r0, [r5, #8]
	ldr	r2, [sp, #8]
	mov	r3, r9
	str	r3, [r5, #0x10]
	str	r2, [r5, #0xc]
	ldr	r4, [sp]
	ldrb	r2, [r4, #5]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Ld62a
	ldr	r0, [r5, #0x24]
	ldr	r2, [r5, #0x2c]
	mov	r11, r0
	mov	r9, r2
	cmp	r0, #0
	bne	.Ld602
	cmp	r2, #0
	beq	.Ld62a
.Ld602:
	mov	r0, r9
	mov	r1, r11
	bl	Func_44d0
	ldrh	r3, [r5, #6]
	sub	r0, r3
	lsl	r0, #16
	mov	r4, #0x80
	asr	r0, #16
	lsl	r4, #5
	cmp	r0, r4
	ble	.Ld61e
	mov	r0, #0x80
	lsl	r0, #5
.Ld61e:
	ldr	r2, =0xfffff000
	cmp	r0, r2
	bge	.Ld626
	ldr	r0, =0xfffff000
.Ld626:
	add	r3, r0
	strh	r3, [r5, #6]
.Ld62a:
	ldr	r3, [sp, #0xc]
	ldr	r4, [sp]
	sub	r3, #1
	add	r4, #0x70
	str	r3, [sp, #0xc]
	str	r4, [sp]
	add	r5, #0x70
	cmp	r3, #0
	blt	.Ld63e
	b	.Ld35e
.Ld63e:
	add	sp, #0x10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_d340

	.section .rodata

@ .L131c0 -- 32-entry oscillation table (0x80 bytes of words) used by Func_cacc
@ for the bit-2 vertical bob: indexed by (+0x44 & 0x3F) >> 1 and scaled by the
@ amplitude at +0x48.
.L131c0:
	.incrom 0x131c0, 0x13240
