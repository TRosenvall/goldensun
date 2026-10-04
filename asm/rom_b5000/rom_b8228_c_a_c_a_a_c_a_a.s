	.include "macros.inc"
	.include "gba.inc"

@ PlaceCombatantParts
@ r0 = combatant id. Projects the parts and writes their positions back through
@ Func_c23c0, so the drawn parts follow the logical position. Exported.
.thumb_func_start Func_80b84c0  @ 0x080b84c0
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r1
	mov	r8, r0
	bl	GetBattleActor
	ldr	r5, [r0]
	mov	r1, #0
	mov	r0, r5
	bl	Func_80b7f70
	add	r5, #8
	mov	r6, r0
	bl	Func_80b7ed8
	mov	r1, r7
	mov	r0, r5
	bl	PhysMove
	ldr	r5, =Func_8000888
	ldr	r1, [r6, #0x18]
	.call_via r5
	mov	r6, r0
	mov	r0, r8
	bl	_GetUnit
	mov	r3, #0x94
	lsl	r3, #1
	add	r0, r3
	ldrb	r0, [r0]
	bl	Func_80c23c0
	cmp	r0, #0
	beq	.Lb850e
	mov	r0, r6
	mov	r1, #0x18
	b	.Lb8512
.Lb850e:
	mov	r0, r6
	mov	r1, #0x30
.Lb8512:
	.call_via r5
	ldr	r3, [r7, #4]
	sub	r3, r0
	str	r3, [r7, #4]
	mov	r0, #0
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b84c0
