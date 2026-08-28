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

@ 232 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   RunTextBoxModal, GetStatusSlot x7, AddStatusEntry x7, GetStatusSlot x7
@   AddStatusEntry x7, GetStatusSlot x7, AddStatusEntry x7, GetStatusSlot x6
@   AddStatusEntry x6, BuildCharacterSummary x4
.thumb_func_start OvlFunc_974_20088c4
	push	{lr}
	ldr	r0, =0xc1d
	mov	r1, #1
	sub	sp, #0x100
	bl	__Func_801776c
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #1
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #2
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #3
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #4
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #5
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #6
	mov	r0, #0
	bl	__GiveDjinni
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #0
	mov	r2, #1
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #0
	mov	r2, #2
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #0
	mov	r2, #3
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #0
	mov	r2, #4
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #0
	mov	r2, #5
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #0
	mov	r2, #6
	mov	r0, #0
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #0
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #1
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #2
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #3
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #4
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #5
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #6
	mov	r0, #1
	bl	__GiveDjinni
	mov	r1, #2
	mov	r2, #0
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #1
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #2
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #3
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #4
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #5
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #2
	mov	r2, #6
	mov	r0, #1
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #0
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #1
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #2
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #3
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #4
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #5
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #6
	mov	r0, #3
	bl	__GiveDjinni
	mov	r1, #1
	mov	r2, #0
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #1
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #2
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #3
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #4
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #5
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #1
	mov	r2, #6
	mov	r0, #3
	bl	__SetDjinni
	mov	r1, #3
	mov	r2, #0
	mov	r0, #2
	bl	__GiveDjinni
	mov	r1, #3
	mov	r2, #1
	mov	r0, #2
	bl	__GiveDjinni
	mov	r1, #3
	mov	r2, #2
	mov	r0, #2
	bl	__GiveDjinni
	mov	r1, #3
	mov	r2, #3
	mov	r0, #2
	bl	__GiveDjinni
	mov	r1, #3
	mov	r2, #4
	mov	r0, #2
	bl	__GiveDjinni
	mov	r1, #3
	mov	r2, #5
	mov	r0, #2
	bl	__GiveDjinni
	mov	r1, #3
	mov	r2, #0
	mov	r0, #2
	bl	__SetDjinni
	mov	r1, #3
	mov	r2, #1
	mov	r0, #2
	bl	__SetDjinni
	mov	r1, #3
	mov	r2, #2
	mov	r0, #2
	bl	__SetDjinni
	mov	r1, #3
	mov	r2, #3
	mov	r0, #2
	bl	__SetDjinni
	mov	r1, #3
	mov	r2, #4
	mov	r0, #2
	bl	__SetDjinni
	mov	r1, #3
	mov	r2, #5
	mov	r0, #2
	bl	__SetDjinni
	mov	r0, #0
	bl	__CalcStats
	mov	r0, #1
	bl	__CalcStats
	mov	r0, #3
	bl	__CalcStats
	mov	r0, #2
	bl	__CalcStats
	add	sp, #0x100
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_20088c4

@ 64 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   RunTextBoxModal, ChangeHp x4, ChangePp x4, GetCombatantRecord x2
@   BuildCharacterSummary x4
.thumb_func_start OvlFunc_974_2008b10
	push	{r5, r6, lr}
	ldr	r0, =0xc1b
	mov	r1, #1
	bl	__Func_801776c
	mov	r1, #0x64
	neg	r1, r1
	mov	r0, #0
	bl	__ModifyHP
	mov	r1, #0x64
	neg	r1, r1
	mov	r0, #1
	bl	__ModifyHP
	mov	r1, #0x21
	neg	r1, r1
	mov	r0, #2
	bl	__ModifyHP
	mov	r1, #0x64
	neg	r1, r1
	mov	r0, #3
	bl	__ModifyHP
	mov	r1, #0x32
	neg	r1, r1
	mov	r0, #0
	bl	__ModifyPP
	mov	r1, #0x28
	neg	r1, r1
	mov	r0, #1
	bl	__ModifyPP
	mov	r1, #0x23
	neg	r1, r1
	mov	r0, #2
	bl	__ModifyPP
	mov	r1, #0x14
	neg	r1, r1
	mov	r0, #3
	bl	__ModifyPP
	mov	r0, #0
	bl	__GetUnit
	ldr	r6, =0x131
	mov	r2, #0xa0
	mov	r5, #1
	lsl	r2, #1
	strb	r5, [r0, r6]
	add	r0, r2
	strb	r5, [r0]
	mov	r0, #1
	bl	__GetUnit
	mov	r2, #0x98
	lsl	r2, #1
	add	r3, r0, r2
	strb	r5, [r3]
	mov	r3, #2
	strb	r3, [r0, r6]
	mov	r0, #0
	bl	__CalcStats
	mov	r0, #1
	bl	__CalcStats
	mov	r0, #3
	bl	__CalcStats
	mov	r0, #2
	bl	__CalcStats
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_974_2008b10
