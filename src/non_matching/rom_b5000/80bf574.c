/* Func_80bf574 (0x080bf574) -- NON-MATCHING.
 * Blocker class: A ZERO-EXTENSION GCC KNOWS IT DOES NOT NEED.
 *
 * 24 lines against the ROM's 25 -- ONE SHORT -- with 16 differing. The twelfth
 * and last TickStatusCounter; its ten siblings in rom_bbb0c_a_c_c_a_a.c and its
 * eleventh in rom_bbb0c_a_c_c_a_b.c all matched this round and last.
 *
 * The batch-178 double-read lever IS working here -- the ROM's
 * `ldrb r2, [r1] / mov r3, r2 / cmp r3, #0` comes out right. What is missing is
 * one instruction further on:
 *
 *     rom    strb r3, [r1] / lsl r3, #0x18 / lsr r3, #0x18 / cmp r3, #0
 *            ... strb r3, [r2]          <- stores the ZERO-EXTENDED value
 *
 *     ours   lsl r3, r2, #0x18 / strb r2, [r1] / cmp r3, #0
 *            ... strb r2, [r3]          <- stores the raw value
 *
 * This function differs from its eleven siblings in one way: after the counter
 * reaches zero it writes that value into a companion byte at 0x147, rather than
 * writing a separately-materialised zero. The ROM therefore needs the counter
 * zero-extended -- `lsl`+`lsr` in place -- and both tests it and stores it.
 *
 * gcc will not produce the pair, because it is right not to: `strb` truncates
 * on its own, so the raw register serves the store, and a test for zero needs
 * only the three-operand `lsl` into a scratch. The ROM's compiler materialised
 * a value gcc can prove is unnecessary. There is no source spelling for "please
 * compute this redundantly".
 *
 * MEASURED (rom 25 lines):
 *   `if (*p != 0) goto fail; *(u+0x147) = *p;`        24 lines, 16 differing
 *   the positive form, `if (*p == 0) { store; return 1; }`
 *                                                     24, 16  (byte-identical)
 *   an `unsigned char v = *p` between store and test  24, 20
 *   an `int n = *p` between store and test            24, 20 -- and here gcc
 *     emits a fresh `ldrb` reload rather than the truncation, which is the
 *     same instruction count and a different wrong answer
 *
 * The two named-intermediate spellings are WORSE for the reason batch 178
 * established: naming collapses the double read that the first half of the
 * function depends on, and the diff moves from line 9 back to line 2.
 *
 * WHAT IS RIGHT: the `mov`+`lsl` construction of the even 0xa3<<1 offset, the
 * pooled 0x147 companion offset, the CSEd double read, the `add r3, #0xff`
 * decrement rather than `(*p)--`, the inverted `bne` guard that distinguishes
 * this sibling from the others, and the shared `mov r0, #0` exit.
 */
extern unsigned char *_GetUnit(void);

int Func_80bf574(void)
{
	unsigned char *u;
	unsigned char *p;

	u = _GetUnit();
	p = u + (0xa3 << 1);
	if (*p == 0)
		goto fail;
	*p = *p + 0xff;
	if (*p != 0)
		goto fail;
	*(u + 0x147) = *p;
	return 1;
fail:
	return 0;
}
