/* Func_80a7440 (0x080a7440) -- NON-MATCHING.
 * Blocker class: A NAMED ZERO THAT CSEs WITH A CALL ARGUMENT.
 *
 * 23 lines against the ROM's 23, 16 differing -- or 21 against 23 with the one
 * lever that fixes the biggest difference. Both results are recorded because
 * neither dominates.
 *
 * Clears the menu block's halfword at +0x174, asks Func_80a77a4 for a
 * selection, and returns either that selection's -1 or the byte at +0x21a.
 *
 * THE PROBLEM IS A POOLED ZERO. `*(unsigned short *)(s + 0x174) = 0` gets the
 * zero POOLED -- `ldr r3, =0x0` -- where the ROM has `mov r2, #0x0`. That is
 * the recorded halfword-store pooling rule and its recorded fix is to name the
 * value in a local first, which is exactly what worked on Func_801ce48 in this
 * same batch.
 *
 * HERE THE FIX COSTS MORE THAN IT BUYS. The call two lines later is
 * `Func_80a77a4(0)`, and once the stored zero has a name gcc CSEs the two
 * zeroes into one register and drops two instructions -- the body goes to 21
 * lines against the ROM's 23, and 17 differing. The ROM materialises the two
 * zeroes separately.
 *
 * That is the batch-178 finding running in reverse and biting: naming a value
 * collapses repeated occurrences, and here the repetition is what the ROM
 * depends on. Two names for one value do not help -- gcc CSEs on the value,
 * not the spelling, which docs/elevation.md already records under the
 * duplicate-constant class.
 *
 * MEASURED (rom 23 lines):
 *   zero inline at the store                          23 lines, 16 differing
 *   the offset written as (0xba << 1) rather than 0x174
 *                                                     23, 16  (byte-identical)
 *   the result copied into a join variable before the
 *     comparison, so the ROM's `mov r2, r0` appears    23, 16  (byte-identical)
 *   the zero named in a local first                    21, 17
 *   named zero AND the join variable                   21, 17
 *
 * The 23-line version is kept: it has the ROM's length and its residue is the
 * pooled zero plus a register rotation, where the 21-line version is missing
 * two instructions outright.
 *
 * NEXT: the question worth one screen is whether the stored zero can be made
 * unshareable with the call argument -- a volatile-qualified local, or a value
 * derived at runtime that gcc cannot fold to zero. Everything spelling-level
 * has been tried.
 */
extern unsigned char *iwram_3001f2c;
extern int Func_80a77a4(int a);

int Func_80a7440(void)
{
	unsigned char *s;
	int v;

	s = iwram_3001f2c;
	*(unsigned short *)(s + (0xba << 1)) = 0;
	v = Func_80a77a4(0);
	if (v != -1)
		v = s[0x21a];
	return v;
}
