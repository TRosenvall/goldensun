// fakematch
/* OvlFunc_884_2008780  --  0x02008780
 *
 * Was goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_a.s, which held
 * it alone.
 *
 * A cutscene beat: a sound, two map-tile rewrites ten frames apart, then two
 * walks with a draw-priority change between them.
 *
 * FAKEMATCH, but a MILD one and NOT the usual shape: two bare
 * `register __asm__` pins and NO inline-asm barrier at all. The plain-C floor
 * is 9 of 44, the straight-line repeated-constant blocker on 0xb0 << 1.
 *
 * THE BARE PIN REACHES THIS ONE, AND ADDING THE BARRIER MAKES IT WORSE. That
 * contradicts nothing in the notebook but it does bound an earlier measurement.
 * On OvlFunc_938_2009450 the pin ALONE was inert -- 14, identical to plain
 * literals -- and the volatile barrier was what defeated cse1. Here the barrier
 * is a regression, and not locally: it perturbed the scheduler three
 * instructions UPSTREAM, inside a CopyMapTiles call that plain literals had
 * already matched (8 and 6 differing against 4 without it).
 *
 * The difference is WHAT THE PIN DOES TO THE PSEUDO, READ from the -da dumps.
 * With plain literal arguments, expand puts any multi-insn constant into a
 * PSEUDO already at .00.rtl -- `(set (reg:SI 33) (const_int 352))` -- and
 * .03.cse rewrites the second site to that pseudo. A hard-register declaration
 * REMOVES the pseudo, so there is nothing left to common and no barrier is
 * needed; it also moves the insn to the declaration point. Where the constant
 * still flows through a pseudo, the pin cannot help and the barrier is what
 * works. Try the bare pin FIRST; reach for the barrier only if it is inert.
 *
 * DECLARATION ORDER IS ARGUMENT-SETUP ORDER. The last single-instruction
 * residue was a rank_for_schedule tie, READ from .23.sched2: `mov r0,#0`,
 * `lsl r1,#1` and `lsl r2,#1` all carry priority 70; `lsl r2` wins on
 * INSN_DEPEND count (4 against 3), and `mov r0` against `lsl r1` is 3-against-3
 * and falls through to INSN_LUID -- the RTL chain order. So the order the
 * pinned register variables are DECLARED in is the order the argument setup
 * comes out in. Declaring r0, r1, r2 in that order gave the ROM's chain.
 *
 * THE RETURN-TYPE LEVER'S MECHANISM, now read rather than inferred. An
 * implicitly-declared (int-returning) callee carries
 * `(set (reg:SI 0 r0) (call ...))`. That set is the next REAL write of r0, so
 * it TRUNCATES the dependent list of the `mov r0,#0` feeding it -- two
 * dependents instead of three -- and it loses the tie against `lsl r1`.
 * Declaring the callee `void` restores the third dependent, which is the next
 * call's own `mov r0,#0`, and hands the decision back to LUID, which the
 * declaration order had already fixed.
 *
 * Two callees needed `void` FOR TWO DIFFERENT REASONS: __Func_8092b08 for its
 * own argument order, and __Func_80921c4 for the r0 dependent count at the
 * PREVIOUS call site. All 32 return-type combinations over the other five
 * callees were swept; the rest are irrelevant.
 *
 * WHEN TWO ROM CALL SITES WITH IDENTICAL SOURCE SHAPE SCHEDULE DIFFERENTLY, do
 * not hunt for a source difference. Call 1 here is
 * `mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1` and call 3 is
 * `mov r1 / mov r2 / lsl r1 / lsl r2 / mov r0`, from the same source shape. It
 * is the dependent-count/LUID tie resolving differently because of what FOLLOWS
 * each call. Reproduce one and the other usually falls out.
 *
 * Padding, checked and not a problem: the function is 0x6a bytes and the TU
 * occupies 0x6c. orig.bin has 0000 in the gap where raw gcc output would pad
 * Thumb with 46c0; the generic rule's trailing `.align 2, 0` forces the zero
 * fill, and with it the .text is byte-identical at 108 bytes.
 */

extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_884_2008714(int a);

void OvlFunc_884_2008780(void)
{
	int t;

	__PlaySound(0xbc);
	t = 2;
	__CopyMapTiles(0, 0x3f, 0x33, 8, t, t);
	__WaitFrames(0xa);
	__CopyMapTiles(2, 0x3f, 0x33, 8, t, t);
	__WaitFrames(0xa);
	{
		register unsigned int z __asm__("r0") = 0;
		register unsigned int a __asm__("r1") = 0xb0 << 1;
		__Func_80921c4(z, a, 0x99 << 1);
	}
	__Func_8092b08(0, 3);
	__Func_80921c4(0, 0xb0 << 1, 0x94 << 1);
	OvlFunc_884_2008714(2);
}
