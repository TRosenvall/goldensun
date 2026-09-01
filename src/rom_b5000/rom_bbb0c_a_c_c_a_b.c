/* Func_80bf54c -- 0x080bf54c, the whole of
 * goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a_b.s, so no split was needed and the
 * linker script is unchanged.
 *
 * The eleventh TickStatusCounter, and the simplest: decrement one status
 * counter and return 1 on the turn it reaches zero. No companion field and no
 * effect call, so nothing forces a callee-saved register and the prologue is a
 * bare `push {lr}`.
 *
 * Same lever as its ten siblings in rom_bbb0c_a_c_c_a_a.c: NOTHING IS NAMED.
 * The ROM's `ldrb r2, [r1] / mov r3, r2 / cmp r3, #0` is a second read of `*p`
 * that gcc CSEs into a register copy, not a redundant `mov`. Reading it into a
 * local emits one `ldrb` and comes out an instruction short.
 *
 * Its offset is odd (0x13f) so it is pooled rather than built with mov+lsl,
 * which `_GetUnit() + 0x13f` reproduces without a lever.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf54c(void)
{
	unsigned char *p;

	p = _GetUnit() + 0x13f;
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
fail:
	return 0;
}
