	.include "macros.inc"
	.include "gba.inc"

@ 588 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   RunTextBoxModal, FindUsableItem x191, BuildCharacterSummary x4
.thumb_func_start OvlFunc_974_200829c
	push	{lr}
	ldr	r0, =0xc1e
	mov	r1, #1
	bl	__Func_801776c
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb4
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb5
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb6
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xb7
	mov	r0, #0
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xba
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbb
	mov	r0, #1
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbc
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbd
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	b	.L6a0

	.pool_aligned

.L6a0:
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xec
	mov	r0, #2
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xbf
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc0
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc1
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc2
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc3
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r1, #0xc4
	mov	r0, #3
	bl	__GiveItemTo
	mov	r0, #0
	bl	__CalcStats
	mov	r0, #1
	bl	__CalcStats
	mov	r0, #3
	bl	__CalcStats
	mov	r0, #2
	bl	__CalcStats
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_200829c
