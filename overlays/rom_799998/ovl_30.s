	.include "macros.inc"

@ ============================================================================
@ Overlay 0x799998 -- four constant tables and a single conditional map-load
@ fix-up.
@
@ Slot 0  OvlFunc_54  map-load entry
@ Slot 1  OvlFunc_30  edge transitions   -> .L108
@ Slot 2  OvlFunc_3c  map event list     -> .L180
@ Slot 3  OvlFunc_44  read after slot 4  -> .L194
@ Slot 4  OvlFunc_4c  map objects        -> .L1c4
@ Slot 5  OvlFunc_38  interactions       -> none (returns 0)
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.L108
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L180
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: read after slot 4.
.thumb_func_start OvlFunc_44
	ldr	r0, =.L194
	bx	lr
.func_end OvlFunc_44

@ Slot 4: map object table.
.thumb_func_start OvlFunc_4c
	ldr	r0, =.L1c4
	bx	lr
.func_end OvlFunc_4c

@ Map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204 and the message
@ delay at +0x1C8 to 0x18 -- slower than the usual 0x10, so this room's text
@ reads at a deliberate pace.
@
@ The rest runs only when save bit 0x300 is set, and stages one map object that
@ the object table cannot describe on its own:
@   - Func_923e4 places slot 8 at (0xD80000, 0x880000),
@   - animation 2, then Func_c528 with argument 0 on its entity,
@   - byte +0x23 set to 2 -- the draw-kind field -- and byte +0x59 cleared,
@     which is the flag OvlFunc_6c-style lookups test to decide whether an
@     entity blocks a push,
@   - Func_10704 repaints a 5x5 attribute block from (0xB, 0x24) to (0xB, 6),
@     so the collision under the object matches where it now stands.

.thumb_func_start OvlFunc_54
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	add	r3, #0x44
	str	r3, [r2]
	sub	r3, #0x3c
	add	r2, r1, r3
	mov	r0, #0xc0
	mov	r3, #0x18
	str	r3, [r2]
	lsl	r0, #2
	sub	sp, #8
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lc8
	mov	r1, #0xd8
	mov	r2, #0x88
	lsl	r2, #16
	mov	r0, #8
	lsl	r1, #16
	bl	__Func_923e4
	mov	r1, #2
	mov	r0, #8
	bl	__Func_924d4
	mov	r0, #8
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #8
	bl	__Func_92054
	mov	r3, #2
	add	r0, #0x23
	strb	r3, [r0]
	mov	r0, #8
	bl	__Func_92054
	mov	r5, #0
	add	r0, #0x59
	mov	r3, #0xb
	mov	r2, #6
	strb	r5, [r0]
	mov	r1, #0x24
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xb
	mov	r2, #5
	mov	r3, #5
	bl	__Func_10704
.Lc8:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_54

	.section .data

.L108:
	.incbin "overlays/rom_799998/orig.bin", 0x108, (0x180-0x108)
.L180:
	.incbin "overlays/rom_799998/orig.bin", 0x180, (0x194-0x180)
.L194:
	.incbin "overlays/rom_799998/orig.bin", 0x194, (0x1c4-0x194)
.L1c4:
	.incbin "overlays/rom_799998/orig.bin", 0x1c4
