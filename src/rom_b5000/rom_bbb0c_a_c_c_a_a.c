/* Cluster Func_80bf250..Func_80bf524 extracted from
 * goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a_a.s -- the WHOLE file, so no split
 * was needed and the linker script is unchanged.
 *
 * TickStatusCounter, ten times. Each decrements one per-combatant status
 * counter in the persistent record and returns 1 on the turn it reaches zero,
 * clearing the companion field alongside it. They differ only in which record
 * offset they tick, which effect id they pass to Func_80bf208, and -- for four
 * of them -- in one extra field or one extra rule.
 *
 * THE LEVER FOR ALL TEN IS THAT NOTHING IS NAMED. The ROM opens every one of
 * them with
 *
 *     ldrb r2, [r5]
 *     mov  r3, r2
 *     cmp  r3, #0
 *
 * and r2 is dead immediately after -- the later read of the same byte is a
 * fresh `ldrb r1, [r5]`. That copy was read as a redundant `mov` with no source
 * spelling behind it, and six of these functions were parked on it for exactly
 * that reason. It is not redundant. THE SOURCE READS `*p` TWICE -- once for the
 * zero test and once for the decrement -- and gcc CSEs the second read into a
 * register copy. Written with a local (`n = *p; if (n == 0) ...; n += 0xff;`)
 * there is only one read, gcc emits one `ldrb`, and the body comes out ONE
 * INSTRUCTION SHORT with 26 of 32 lines differing. Written entirely through the
 * pointer it matches on the first screen.
 *
 * The three-`return 0`-paths-share-one-block rule (docs/elevation.md, multiple
 * exits) still applies and is why `goto fail` is here rather than three plain
 * `return 0` statements; and `*p = *p + 0xff` is deliberate -- `(*p)--` on an
 * unsigned char gives `lsl #24 / lsr #24` around the store instead of the ROM's
 * bare `add r3, #0xff`. Both were established by the park this file replaces.
 *
 * Func_80bf318, Func_80bf250 and Func_80bf2b4 also clear a companion signed
 * byte, and there the ZERO IS NAMED and assigned between the store and the
 * test, which is where the ROM materialises it (`mov r7, #0` sitting after
 * `lsl r3, #0x18`). Assigning it any earlier moves the instruction and costs
 * six differing lines.
 *
 * An offset that is even is built with `mov`+`lsl` and one that is odd is
 * pooled; writing `(0x9c << 1)` and `0x139` respectively reproduces both
 * without a lever.
 */
extern unsigned char *_GetUnit(void);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf250(int a)
{
	unsigned char *u;
	unsigned char *p;
	signed char *q;
	int z;

	u = _GetUnit();
	p = u + (0x99 << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	z = 0;
	if (*p == 0) {
		*(u + 0x133) = z;
		return 1;
	}
	q = (signed char *)(u + 0x133);
	if (*q >= 0)
		goto fail;
	if (Func_80bf208(a, *p, 0x1e) == 0)
		goto fail;
	*q = z;
	*p = z;
	return 1;
fail:
	return 0;
}

int Func_80bf2b4(int a)
{
	unsigned char *u;
	unsigned char *p;
	signed char *q;
	int z;

	u = _GetUnit();
	p = u + (0x9a << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	z = 0;
	if (*p == 0) {
		*(u + 0x135) = z;
		return 1;
	}
	q = (signed char *)(u + 0x135);
	if (*q >= 0)
		goto fail;
	if (Func_80bf208(a, *p, 0x14) == 0)
		goto fail;
	*q = z;
	*p = z;
	return 1;
fail:
	return 0;
}

int Func_80bf318(int a)
{
	unsigned char *u;
	unsigned char *p;
	signed char *q;
	int z;

	u = _GetUnit();
	p = u + (0x9b << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	z = 0;
	if (*p == 0) {
		*(u + 0x137) = z;
		return 1;
	}
	q = (signed char *)(u + 0x137);
	if (*q >= 0)
		goto fail;
	if (Func_80bf208(a, *p, 0x14) == 0)
		goto fail;
	*q = z;
	*p = z;
	return 1;
fail:
	return 0;
}

int Func_80bf37c(int a)
{
	unsigned char *p;

	p = _GetUnit() + (0x9c << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
	if (Func_80bf208(a, *p, 0x1e) == 0)
		goto fail;
	*p = 0;
	return 1;
fail:
	return 0;
}

int Func_80bf3bc(int a)
{
	unsigned char *p;

	p = _GetUnit() + 0x139;
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
	if (Func_80bf208(a, *p, 0x3c) == 0)
		goto fail;
	*p = 0;
	return 1;
fail:
	return 0;
}

int Func_80bf400(int a)
{
	unsigned char *p;

	p = _GetUnit() + (0x9d << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
	if (Func_80bf208(a, *p, 0x46) == 0)
		goto fail;
	*p = 0;
	return 1;
fail:
	return 0;
}

int Func_80bf440(int a)
{
	unsigned char *p;

	p = _GetUnit() + 0x13b;
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
	if (Func_80bf208(a, *p, 0x28) == 0)
		goto fail;
	*p = 0;
	return 1;
fail:
	return 0;
}

int Func_80bf484(int a)
{
	unsigned char *p;

	p = _GetUnit() + (0x9e << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
	if (Func_80bf208(a, *p, 0x32) == 0)
		goto fail;
	*p = 0;
	return 1;
fail:
	return 0;
}

/* The odd one out: this counter is a 3-bit field with a carry above it, so it
 * is first brought under 8, then decremented only when the low bits are set,
 * and a result above 7 fails rather than ticking. Same all-through-the-pointer
 * spelling; the `lsl #24 / lsr #24` pair the ROM shows at the join is gcc's
 * truncation of the CSEd value and needs nothing written for it. */
int Func_80bf4c4(int a)
{
	unsigned char *p;

	p = _GetUnit() + 0x13d;
	if (*p == 0)
		goto fail;
	if (*p > 7)
		*p = *p + 0xf8;
	if (*p & 7)
		*p = *p + 0xff;
	if (*p == 0)
		return 1;
	if (*p > 7)
		goto fail;
	if (Func_80bf208(a, *p, 0x1e) == 0)
		goto fail;
	*p = 0;
	return 1;
fail:
	return 0;
}

/* And this one only ticks -- no companion field, no effect call, so nothing
 * forces a callee-saved register and the ROM's prologue is a bare `push {lr}`.
 * It takes no argument; the combatant id its neighbours save in r6 is simply
 * passed through to _GetUnit in r0. */
int Func_80bf524(void)
{
	unsigned char *p;

	p = _GetUnit() + (0x9f << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p == 0)
		return 1;
fail:
	return 0;
}
