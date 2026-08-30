/* OvlFunc_967_2008308 -- 0x02008308,
 * asm/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_c.s
 *
 * 88 vs 86 lines.  Candidate at scratch/L8308.c.
 *
 * SOLVED:
 *   - The mask needs an `int` intermediate; assigned straight into the `short`
 *     it narrows and gcc pools 0xc000 where the ROM has 0xffffc000.
 *   - The comparison is done in the HIGH HALF.  The ROM sign-extends the short
 *     (`lsl #16 / asr #16`) and then shifts LEFT AGAIN to compare against
 *     `0x80 << 24`.  Spelling it `(w << 16) == (0x80 << 24)` reproduces that;
 *     `w == (short)0x8000` gives a pooled 0xffff8000 and a direct compare.
 *     Together these moved the first difference from line 8 to line 19.
 *
 * BLOCKER: the argument-setup interleave at __MapActor_Emote --
 *      rom   mov r1,#0x81 / mov r0,#0xe / lsl r1,#1
 *      ours  mov r1,#0x81 / lsl r1,#1   / mov r0,#0xe
 * -- at a site that precedes every conditional branch in the function, so the
 * batch-127 lever has no dominating block to work from.  Naming the constants
 * at the top instead makes gcc keep them in callee-saved registers and adds two
 * pushes (85 lines, 81 differing), exactly as the straight-line parks show.
 *
 * Worth noting for the class: the interleaved single-instruction argument here
 * is `mov r0, #0xe`, not a zero.  The detector in docs/elevation.md only looks
 * for `mov r0, #0`, so the 248-function sizing undercounts.
 */

/* RETRIED (batch 147).  Four more spellings, none moved it off 60 differing:
 *      `em = 0x81 << 1;` at the very top, used at the Emote call only
 *          -- 87 lines, 78 differing, WORSE.  The dominating-block lever that
 *          works on OvlFunc_926_200a484 needs the def in a block the use does
 *          not share; here there is no branch before the call, so hoisting only
 *          lengthens the live range and gcc allocates instead of rematerialising.
 *      the actor slot 0xe as a named local used at every call site   60
 *      the actor slot 0xe as a named local used at the Emote only    60
 *      the emote constant written as the plain literal 0x102         60
 *
 * Three spellings leaving the count EXACTLY unchanged says the r0 position here
 * is decided below the source, same conclusion as ovl_7ddb88/20092f0.c reached
 * on six sites of the same shape.  The two parks are the same blocker.
 *
 * ALSO WORTH RECORDING: of the 60 differing, only TWO are real.  The rom emits
 * `b L0 / L0:` to jump over an early literal pool and we put the pool at the
 * end, which shifts every later line and inflates the count.  Do not read 60 as
 * sixty problems -- the whole function is right except the Emote interleave and
 * the pool dump point, and the pool point is downstream of nothing we control
 * from a single-function translation unit.
 */
