/* OvlFunc_882_20090a4 (0x020090a4) -- 80 encodings, 176 bytes, EXACT.
 *
 * MATCHING, 0 differing encodings of 80.  Whole TU: 176 bytes, 80 encodings and
 * 8 relocations identical.  The piece holds this one function only, so this file
 * IS the translation unit.  Pins 0, devices 0, no flag group.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c_a.c asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_c_a.s --whole
 *
 * HOW IT CLOSED, from 8 of 80: ONE extra named local, `v = 0x37`, for the sixth
 * (second stack-passed) argument of the LAST call.  That is the stack-argument
 * materialisation lever -- two stack-destined arguments must be two separate
 * named locals so both are built before either is stored:
 *
 *     rom    mov r2, r10 / mov r3, #0x37 / str r2, [sp] / str r3, [sp, #0x4]
 *     before mov r3, r10 / str r3, [sp]  / mov r3, #0x37 / str r3, [sp, #0x4]
 *
 * AND IT CLOSED THE OTHER TWO RUNS TOO, which is the finding.  The park's
 * residue was three runs: that pair at the last call, and at calls 1 and 5 a
 * bare scratch-register choice (`mov r2, #0xf` / `mov r2, r8` against our r3).
 * The park's closing line read "NEXT: the r2/r3 choice.  Nothing in the
 * recorded levers addresses which scratch register gcc picks."  Nothing needed
 * to: the scratch choice was not an independent blocker.  One more live pseudo
 * re-ordered local-alloc's quantities for the WHOLE straight-line block, and the
 * r2/r3 picks at calls 1 and 5 -- both EARLIER than the edit -- came out right
 * as a consequence.  ARM's REG_ALLOC_ORDER opens 3, 2, 1, 0, so our uniform r3
 * was simply gcc's first free choice and the ROM's r2 meant r3 was taken; the
 * way to take it was to create a quantity that wanted it.
 *
 * GENERALISABLE: a scratch-register residue in a single-basic-block function is
 * evidence about the QUANTITY SET, not about the differing site.  Do not look
 * for a lever at the instruction that is wrong.
 *
 * THE PARK'S lever 1 IS WITHDRAWN, not merely unused.  It claimed -ffixed-r7
 * (FIXEDR7_CFLAGS) was load-bearing.  The Makefile has no FIXEDR7 rule for
 * rom_77dd1c, objcmp derives its flags from the Makefile and printed no
 * "built with" line, so the park's own figure of 8 was measured WITHOUT the
 * flag -- and this exact match is at the production flags.  Compare the caution
 * at Makefile:400-409, which says -ffixed-r7 is not the general cure for
 * "the ROM spends r8 where gcc reaches for r7" and that the cure is a live-range
 * reading.  It was, here.
 *
 * KEPT FROM THE PARK, both still load-bearing: the four constants the ROM holds
 * in callee-saved registers are named (0xf, 0xe, 0xd and the 0x35/0x36 pair) and
 * the two it materialises fresh immediately before their stores -- 0x34 and
 * 0x37 -- were literals.  0x37 is now a local; naming 0x34 as well is also exact
 * (measured), so that half of the reading is free either way.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_882_20090a4(void)
{
    int a;
    int b;
    int c;
    int t;
    int u;
    int v;

    a = 0xf;
    t = 0x35;
    __Func_8010704(0x1d, 0x17, 1, 1, a, t);
    b = 0xe;
    __Func_8010704(0x1d, 0x17, 1, 1, b, t);
    c = 0xd;
    __Func_8010704(0x1d, 0x17, 1, 1, c, t);
    __Func_8010704(0x1a, 0x14, 2, 1, b, 0x34);
    u = 0x36;
    __Func_8010704(0x19, 0x15, 1, 1, c, u);
    __Func_8010704(0x19, 0x15, 1, 1, a, u);
    __Func_8010704(0xe, 0x35, 1, 1, b, u);
    v = 0x37;
    __Func_8010704(0xd, 0x37, 1, 1, a, v);
}
