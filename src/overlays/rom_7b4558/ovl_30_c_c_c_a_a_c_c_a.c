// fakematch
/* OvlFunc_927_200a004  --  0x0200a004
 *
 * Cut out of goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_c_a.s,
 * which held this function alone.
 *
 * A cutscene script: pan the camera to a mark, run slot 0x12 through a move,
 * clear its sprite flags, wait, set flag 0x30a, then place slot 0x16 at the
 * same coordinates the camera used.
 *
 * FAKEMATCH. The clean best is 13 of 43, and the residue is the straight-line
 * repeated-constant blocker: 0xba << 18 and 0xfc << 17 are each used at two
 * call sites, and the function is ONE basic block across twelve `bl`s, so the
 * dominating-branch test fails and cprop can never rebuild them.
 *
 * NO FLAG CAN REACH THIS CLASS, and that is now READ rather than swept. The
 * responsible pass is the FIRST cse_main, not gcse and not the rerun:
 * `.02.jump` still carries four const_int occurrences, one per use, and
 * `.03.cse` has already rewritten the second site to
 * `(set (reg:SI 1 r1) (reg:SI 32))` with the constant demoted to a REG_EQUAL
 * note; `.07.gcse` changes nothing further. In toplev.c:2917 that first
 * cse_main sits inside the plain `optimize > 0` block with NO -f flag gating
 * it at all -- flag_rerun_cse_after_loop guards only the SECOND call, at line
 * 3095. So the earlier six- and seven-flag sweeps on this class were not
 * looking at the wrong flags; there is no flag to find. Stop sweeping.
 *
 * THE LAUNDER IS PER CALL SITE, NOT PER CONSTANT -- the counterintuitive part,
 * and the reason there are six of them rather than the two the CSE problem
 * strictly needs. MEASURED: laundering only the two repeated constants at both
 * sites leaves 4 of 43. The CSE residue is gone, but a SCHED2 residue replaces
 * it, because gcc's natural interleave was already the ROM's -- the third call
 * site comes out exact with no help at all -- and anchoring SOME arguments of a
 * call perturbs it, leaving the un-anchored `neg r1` and `mov r3, #1` floating
 * above the anchored shifts. Adding launders to that call's other two
 * arguments, which are not repeated and are not a CSE problem, takes it to 0.
 * Half-measures: laundering the `1` alone gives 2, the `-1` alone gives 2.
 *
 * RULE: anchor EVERY argument of any call you anchor any argument of. A
 * partially laundered argument list is strictly worse than none. `--no-sched2`
 * on the partial forms gives 14, which confirms sched2 is the reorderer and
 * that suppressing it is not the fix.
 *
 * LAUNDER THE FIRST OCCURRENCE, NEVER THE SECOND. Laundering only the
 * MapActor_SetPos site leaves 13 -- bit-identical to no launder at all.
 * From `.03.cse`: CSE substitutes into the launder's own INITIALISER
 * (`set (reg _t) (const_int ...)` becomes `set (reg _t) (reg 32)`) before the
 * asm ever sees the value. The asm makes the value opaque downstream of the
 * assignment, which is too late. Anchor the first site and every later
 * occurrence rebuilds naturally.
 *
 * WHAT THIS DOES NOT ESTABLISH: the original toolchain plainly did not use
 * inline asm. The fakematch stands in for whatever it did differently, exactly
 * as in the register-allocation-order class. It is a match, not an explanation.
 *
 * Verified beyond the screen: assembled both sides with -I include,
 * 116 of 116 bytes, cmp clean.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __SetFlag(int id);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *actor, int flags);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);

void OvlFunc_927_200a004(void)
{
    int x, d, y, n, x2, y2;

    __CutsceneStart();
    OvlFunc_927_2008ea8(0x12, 1);
    x = 0xba << 18;  __asm__ ("" : "+r" (x));
    d = -1;          __asm__ ("" : "+r" (d));
    y = 0xfc << 17;  __asm__ ("" : "+r" (y));
    n = 1;           __asm__ ("" : "+r" (n));
    __Func_80933f8(x, d, y, n);
    OvlFunc_927_2008d90(0x12, 0xba << 2, 0xfc << 1, 0x90 << 12);
    OvlFunc_927_2008e18(0x12);
    __Func_8092950(0x12, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x30a);
    x2 = 0xba << 18;  __asm__ ("" : "+r" (x2));
    y2 = 0xfc << 17;  __asm__ ("" : "+r" (y2));
    __MapActor_SetPos(0x16, x2, y2);
    __CutsceneEnd();
}
