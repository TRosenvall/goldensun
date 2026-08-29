/* OvlFunc_891_20095d4 and OvlFunc_891_20095fc  [ovl_78c76c]
 * Source asm: goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_a.s
 *
 * Seventeen against seventeen, diverging at instruction 3 in the set-up for
 * the first call: the ROM interleaves TWO shifted constants and a plain
 * argument as
 *
 *     mov r1, #0xd0 / mov r2, #0xe0 / mov r0, #2 / lsl r1, #16 / lsl r2, #15
 *
 * This is the two-shifted-constants case that gcc DOES produce elsewhere --
 * probe q8 in the session notes emitted exactly that pattern from
 * f3(0xe, 0x102, 0x204). Here it does not, and the plain `mov r0, #2` in the
 * middle is the difference: gcc groups the two shift pairs and puts the
 * unrelated move outside them.
 *
 * So this is the arg-interleave class with a third argument involved, and
 * worth retrying if that class ever falls -- it is not a separate problem.
 *
 * RE-TESTED after the arg-fill-order fix (2026-08-03). Four formulations, all
 * producing the identical diff above:
 *
 *   1. Full prototypes on every callee, with return types. That is what fixed
 *      arg-fill-order -- an implicitly declared callee returns int, so gcc
 *      keeps r0 live across the call and fills the next call's r0 last. It
 *      does not move r0 here.
 *   2. The reverse: __Func_8012078 implicitly declared.
 *   3. The shifted values built into named locals before the call, so the
 *      shifts are statements rather than argument expressions.
 *   4. A narrower first parameter (unsigned char), so r0's argument needs a
 *      conversion the others do not.
 *
 * So r0 placement here is NOT driven by declaration state, which is what
 * separates arg-interleave from arg-fill-order. They looked like one class.
 *
 * The filter still does not catch this one: tools/elevation_candidates.py
 * looks two lines back from an `lsl` for the `mov` that starts it, and here
 * the pair is split by three. Same deliberate gap as OvlFunc_933_2009874 --
 * but that is now two functions it has cost.
 *
 * WHAT THE DIFF ACTUALLY IS (corrected 2026-08-03). It is NOT the interleave.
 * gcc reproduces `mov r1 / mov r2 / lsl r1 / lsl r2` here without any help.
 * The single residual instruction is where the PLAIN argument r0 sits: the
 * ROM wedges `mov r0, #2` between the two movs and the two lsls, and gcc puts
 * it after both lsls. Everything else in the function is identical.
 *
 * gcc DOES emit the wedged form naturally -- three sites in the honest
 * (non-fakematch) generated output, two of them siblings in this very
 * overlay. The clearest is OvlFunc_891_200966c in
 * src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_b.c, which calls the SAME
 * __Func_8012078 with the SAME two shifted constants and matches:
 *
 *     new_var = 0xd0 << 16;              <- assigned BEFORE the outer if
 *     if (...) {
 *         new_var2 = 0xe0 << 15;         <- assigned INSIDE it
 *         if (...) __Func_8012078(2, new_var, new_var2, 0xff);
 *     }
 *
 * The mechanism is that control flow separates each assignment from its use,
 * so gcc emits the `mov` early and defers the `lsl` to the use site, leaving
 * exactly the gap `mov r0, #2` fills.
 *
 * That cannot be transplanted here: this function has NOTHING between its
 * prologue and the call, so there is no branch to separate them. Mirroring
 * the sibling's declaration order and initialiser form changes nothing.
 *
 * So the open question is narrow and worth stating precisely: what else, other
 * than an intervening branch, makes gcc-2.96 defer an `lsl` to its use site?
 *
 * A CAUTION FOR WHOEVER TRIES NEXT. The first sweep for "does gcc ever emit
 * this shape" said yes, 335 sites -- and was WRONG, because it counted
 * fakematch TUs. Those force shapes with inline-asm barriers and register
 * variables, so their output is evidence about the barriers, not the
 * compiler. The tell is `.code 16` directives leaking into the listing.
 * Exclude fakematch.txt and the `// fakematch` first-line marker before
 * concluding anything from generated output.
 *
 * LATER: re-derived independently, which is how the park was found -- the
 * function was offered as a fresh candidate by a sweep of the multi-function
 * pool, and the note above was only noticed after the C had been rewritten.
 * The rediscovery reached the same diff and the same conclusion, so nothing
 * above changes. One thing it adds:
 *
 * THIS IS A COUNTEREXAMPLE TO HANDOFF.md's PREDICTIVE RULE. That rule says a
 * call misorders when its argument list mixes cheap constants with expensive
 * values and A CHEAP ONE IS NOT LAST, and that a call whose cheap constant IS
 * last will match. Here the ROM's final setup instruction is `mov r3, #0`, a
 * cheap constant, so the rule predicts a match. Both twins miss by three.
 *
 * That matters beyond this file. tools/census.py uses the rule to separate the
 * `precompute` class from `open`, so the open count is OPTIMISTIC -- it holds
 * functions that will fail on interleave grounds despite passing the filter.
 * The rule has false negatives as well as the false positives already
 * measured at 2.4%.
 *
 * Also re-confirmed, with the folded constants 0xd00000 and 0x700000 written
 * in place of the shifts: identical diff. That is a fifth formulation to add
 * to the four below.
 */
extern void __Func_8012078(int a, int b, int c, int d);
extern int  OvlFunc_891_2009be8(int a, int b, int c);
extern void OvlFunc_891_200a244(void);

void OvlFunc_891_20095d4(void)
{
    __Func_8012078(2, 0xd0 << 16, 0xe0 << 15, 0);
    if (OvlFunc_891_2009be8(0xa, 0xe, 7))
        OvlFunc_891_200a244();
}
