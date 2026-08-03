	.include "macros.inc"
	.include "gba.inc"

@ Wrapper: OvlFunc_7c(0xd21).
.thumb_func_start OvlFunc_974_2008180
	push	{lr}
	ldr	r0, =0xd21
	ldr	r1, =0xd4c
	sub	r1, r0
	bl	OvlFunc_974_200807c
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_2008180

@ Wrapper: OvlFunc_7c(0xd4c, 0xc9b).
.thumb_func_start OvlFunc_974_2008198
	push	{lr}
	ldr	r3, =0xc9b
	ldr	r1, =0xcc6
	ldr	r0, =0xd4c
	sub	r1, r3
	bl	OvlFunc_974_200807c
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_2008198

@ Wrapper: OvlFunc_7c(0xd77, 0xc9b).
.thumb_func_start OvlFunc_974_20081b8
	push	{lr}
	ldr	r3, =0xc9b
	ldr	r1, =0xcc6
	ldr	r0, =0xd77
	sub	r1, r3
	bl	OvlFunc_974_200807c
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_20081b8

@ Wrapper: OvlFunc_7c(0xda2, 0xc9b).
.thumb_func_start OvlFunc_974_20081d8
	push	{lr}
	ldr	r3, =0xc9b
	ldr	r1, =0xcc6
	ldr	r0, =0xda2
	sub	r1, r3
	bl	OvlFunc_974_200807c
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_20081d8
