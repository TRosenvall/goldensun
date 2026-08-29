	.include "macros.inc"
	.include "gba.inc"

@ BuildPartyHeader
@ r0 = state block. Opens the party header window at (0, 0, 0xD, 5) into
@ state+0x10, attaches the menu cursor sprite through .gcc2_compiled.(-8, 0xB) and
@ stores it at state+0x14, starting it hidden (+0x05 = 0x0D). Also seeds
@ +0x1C = 0xFF and +0x1D = 0 -- no row and no column selected yet -- and sets
@ the cursor's +0x0F to 0xFE and the sprite at state+0x18's to 0xFF, which is
@ the OBJ sort order that keeps the cursor over the portraits.
.thumb_func_start Func_80a1814  @ 0x080a1814
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r5, r0
	mov	r3, #0
	sub	sp, #8
	mov	r8, r3
	str	r3, [r5, #0x10]
	mov	r6, r5
	mov	r3, #5
	str	r3, [sp]
	add	r6, #0x10
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r0, r6
	mov	r3, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	Func_80a10d0
	ldr	r6, [r6]
	mov	r1, #8
	neg	r1, r1
	mov	r0, r6
	mov	r2, #0xb
	bl	Func_80a1778
	mov	r3, #0xd
	strb	r3, [r0, #5]
	mov	r3, #0xff
	strb	r3, [r5, #0x1c]
	mov	r3, r8
	strb	r3, [r5, #0x1d]
	mov	r3, #0xfe
	str	r0, [r5, #0x14]
	strb	r3, [r0, #0xf]
	ldr	r2, [r5, #0x18]
	sub	r3, #0xff
	mov	r0, r6
	strb	r3, [r2, #0xf]
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80a1814

@ SpawnPartyActors
@ r0 = window, r1 = x offset, r2 = y offset, r3 = spacing.
@ Creates one animated actor per party member and lays them out along the top
@ of the screen. For each roster id from _Func_796c4 it remaps the resource
@ through _Func_8b398 (which is what makes the sprites follow story progress),
@ creates the actor with _Func_bc70, and files it in three parallel arrays:
@
@     state+0x114 + i*4   the actor pointer
@     state+0x134 + i*2   its x, from the window's tile column * 8 plus
@                         i * (spacing + 0x10)
@     state+0x144 + i*2   its y, from the window's tile row * 8 plus 0x10
@
@ It then sets +0x40 to 0x10000 (unit scale), clears bits 0 and 2 of the
@ actor's +0x09, zeroes +0x26 and starts animation 1 with _Func_ba30. Slots
@ past the party size are zeroed out to 8. Finally it registers Func_a19a0 at
@ sort key 0xC80 so the actors get drawn every frame.
@
@ The roster count also goes to state+0x1E.
.thumb_func_start Func_80a1870  @ 0x080a1870
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x2c
	str	r1, [sp, #0xc]
	str	r2, [sp, #8]
	str	r3, [sp, #4]
	ldr	r3, =iwram_3001f2c
	mov	r9, r0
	ldr	r3, [r3]
	mov	r0, sp
	add	r0, #0x10
	mov	r10, r3
	str	r0, [sp]
	bl	_Func_80796c4
	lsl	r0, #16
	lsr	r0, #16
	mov	r8, r0
	mov	r1, r8
	mov	r2, r10
	mov	r5, #0
	strb	r1, [r2, #0x1e]
	cmp	r5, r8
	bge	.La191c
	mov	r7, #0x9a
	mov	r6, #0x8a
	lsl	r7, #1
	lsl	r6, #1
	add	r7, r10
	add	r6, r10
	mov	r11, r5
.La18b8:
	ldr	r1, [sp]
	mov	r3, r11
	ldrh	r0, [r3, r1]
	bl	_Func_808b398
	bl	_CreateSprite
	cmp	r0, #0
	beq	.La190e
	str	r0, [r6]
	mov	r2, r9
	ldrh	r3, [r2, #0xc]
	ldr	r2, [sp, #4]
	add	r2, #0x10
	mul	r2, r5
	ldr	r1, [sp, #0xc]
	add	r3, r1, r3
	lsl	r3, #3
	add	r3, r2
	strh	r3, [r7]
	mov	r2, r9
	ldr	r1, [sp, #8]
	ldrh	r3, [r2, #0xe]
	add	r3, r1, r3
	lsl	r3, #3
	add	r3, #0x10
	strh	r3, [r7, #0x10]
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r6, #0x40]
	mov	r1, #0xd
	ldrb	r3, [r0, #9]
	neg	r1, r1
	mov	r2, r1
	and	r3, r2
	mov	r2, r0
	strb	r3, [r0, #9]
	add	r2, #0x26
	mov	r3, #0
	strb	r3, [r2]
	mov	r1, #1
	bl	_Sprite_SetAnim
.La190e:
	mov	r2, #2
	add	r5, #1
	add	r7, #2
	add	r6, #4
	add	r11, r2
	cmp	r5, r8
	blt	.La18b8
.La191c:
	cmp	r5, #7
	bgt	.La1938
	lsl	r3, r5, #2
	mov	r0, #0x8a
	add	r3, r10
	lsl	r0, #1
	add	r2, r3, r0
	mov	r3, #8
	mov	r1, #0
	sub	r5, r3, r5
.La1930:
	sub	r5, #1
	stmia	r2!, {r1}
	cmp	r5, #0
	bne	.La1930
.La1938:
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_80a19a0
	bl	StartTask
	add	sp, #0x2c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a1870

@ DestroyPartyActors
@ Takes no arguments. Walks the same state+0x114 array for as many entries as
@ _Func_796c4 reports, destroying each with _Func_bdd4, then unregisters
@ Func_a19a0. Null slots are skipped, so calling it twice is safe.
.thumb_func_start Func_80a195c  @ 0x080a195c
	push	{r5, r6, lr}
	sub	sp, #0x1c
	ldr	r3, =iwram_3001f2c
	mov	r0, sp
	ldr	r5, [r3]
	bl	_Func_80796c4
	lsl	r0, #16
	lsr	r0, #16
	cmp	r0, #0
	beq	.La198a
	mov	r3, #0x8a
	lsl	r3, #1
	add	r6, r5, r3
	mov	r5, r0
.La197a:
	ldmia	r6!, {r0}
	cmp	r0, #0
	beq	.La1984
	bl	_DeleteSprite
.La1984:
	sub	r5, #1
	cmp	r5, #0
	bne	.La197a
.La198a:
	ldr	r0, =Func_80a19a0
	bl	StopTask
	add	sp, #0x1c
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80a195c
