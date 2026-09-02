/* Func_801cee0 -- 0x0801cee0, the whole of what remained of
 * goldensun/asm/rom_15000/rom_1ca1c_a_c.s after Func_801ce90 was split out.
 *
 * The increment mirror of Func_801ce90 next door: dispatch on a halfword, pick
 * one of three gState counters, and bump it unless it is already at that
 * counter's ceiling.
 *
 * I WROTE THE PARK THIS REPLACES AND ITS CENTRAL CLAIM WAS WRONG. It recorded
 * that the branch polarity in two of the three arms was "source-inert across
 * the plain `break`, an explicit `goto inc`, and an explicit `goto out`, and
 * three jump-optimisation flags", and concluded the 4-differing join form was
 * unreachable. All three of those spellings keep the RETURN as the `if`'s
 * then-body, which is the thing that decides the polarity.
 *
 * THE POLARITY IS DECIDED BY WHICH SIDE OF THE `if` CARRIES THE EXIT. gcc
 * expands `if (C) X;` as invert-C, jump past the then-body, and then threads
 * the one-jump block:
 *
 *   `if (*p > N) return; break;`
 *       bls Lafter / b Lret / Lafter: b Ljoin   ->  bls Ljoin / b Lret
 *
 *   `if (*p <= N) break; return;`
 *       bhi Lafter / b Ljoin / Lafter: b Lret   ->  bhi Lret / b Ljoin
 *
 * The second is the ROM. Swapping which arm of the `if` is the then-body costs
 * nothing else: registers, block order, the CSEd double read and the
 * cross-jumped tail all stay where they were.
 *
 * The two levers carried over from the park hold and are still load-bearing:
 * the double read of `*p` with nothing named, which gives
 * `ldrb r2, [r1] / mov r3, r2`; and `g = gState;` INSIDE each arm, which keeps
 * base and offset as two pool words instead of one folded entry.
 *
 * MEASURED (rom 48 lines):
 *   join form, arms `if (*p > N) return; break;`      48 lines, 4 differing
 *   the same via an explicit `inc:` label             48, 4
 *   store-and-return arms, inverted                   48, 17
 *   arms `if (*p <= N) break; return;`                48, MATCH
 *   the same via an `inc:` label and `goto inc`       48, MATCH
 *   the same with the last arm left in the > form     48, MATCH
 *   the same with `(*p)++`                            48, MATCH
 *
 * The last row matters for the neighbouring file: the increment's spelling is
 * free here, exactly as the decrement's was in Func_801ce90.
 */
extern unsigned char gState[];

void Func_801cee0(int a)
{
	unsigned char *g;
	unsigned char *p;

	switch (*(unsigned short *)(a + 0x574)) {
	case 0:
		g = gState;
		p = g + (0x83 << 2);
		if (*p <= 1)
			break;
		return;
	case 1:
		g = gState;
		p = g + 0x205;
		if (*p <= 0x17)
			break;
		return;
	case 2:
		g = gState;
		p = g + 0x206;
		if (*p <= 0xe)
			break;
		return;
	default:
		return;
	}
	*p = *p + 1;
}
