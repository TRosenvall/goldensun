/* OvlFunc_927_200a1b0 (0x0200a1b0) -- NON-MATCHING.
 *
 * NON-MATCHING, 6 of 110 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b4558/200a1b0.c \
 *     asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_c_c.s --func OvlFunc_927_200a1b0
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: ARGUMENT INTERLEAVE, STRAIGHT-LINE variant (not reachable).
 *
 * 108 lines against the ROM's 108, SIX differing, and all six are the same
 * shape at three call sites -- `mov r0, #0x12` has to land BEFORE the `lsl`
 * that completes r1's split build, and ours lands after:
 *
 *     rom   mov r1,#0x88 / mov r2,#0xb4 / lsl r2,#17 / mov r0,#0x12 / lsl r1,#16
 *     ours  mov r1,#0x88 / mov r2,#0xb4 / lsl r2,#17 / lsl r1,#16 / mov r0,#0x12
 *
 * The three sites are __MapActor_SetPos(0x12, 0x88<<16, 0xb4<<17),
 * __Func_8092adc(0x12, 0xc0<<8, 0x28) and __MapActor_Surprise(0x12, 0x81<<1).
 * Everything else -- 102 of 108 instructions, including seven other calls with
 * split-constant arguments and the eight-argument OvlFunc_927_2008ae8 with its
 * four stack slots -- is exact on the FIRST screen.
 *
 * WHY IT IS NOT REACHABLE.  This function has no conditional branch at all.
 * docs/elevation.md's argument-interleave note is explicit that the guard is
 * load-bearing: naming the split builds works because gcc will not hold the
 * constants across a branch and so rematerialises them at the call, and doing
 * the same in a straight-line function makes gcc HOLD them instead. That is
 * what tools/guarded_interleave.py was written to separate, and this function
 * is on the wrong side of it. It should not have passed the filter -- the
 * filter counts calls and instructions, not guards.
 *
 * MEASURED, all at exactly 108 lines and 6 differing unless noted:
 *   - naming both split builds in locals immediately before the call, the
 *     spelling that works on guarded sites                            6
 *   - naming the PRE-SHIFT base (`b = 0x88; f(0x12, b << 16, ...)`)   6
 *   - the same with `volatile int b` to block the fold        112 lines, 105
 *   - declaring all three callees `int` instead of `void` (the return-type
 *     lever, which is the documented control over r0's position)      6
 *   - withholding the prototypes, all three and Surprise alone        6
 *   - -fno-schedule-insns, -fno-rerun-cse-after-loop, -fno-gcse,
 *     -fno-strict-aliasing, -fno-defer-pop, -fno-expensive-optimizations
 *                                                                     6
 *   - -fno-schedule-insns2                              first diff at 1, 52
 *
 * A READING WORTH KEEPING, because it narrows the shape. The ROM does NOT put
 * r0 early everywhere: the four FOUR-argument OvlFunc_927_2008d90 calls emit
 * `mov r0, #0x12` last, after every shift, and those all match. Only the two-
 * and three-argument calls interleave. So the shape is not "this ROM likes r0
 * early" -- it is specific to calls that leave r3 free, which is consistent
 * with the interleave being an argument-loading order and not a scheduling
 * artifact. That also explains why no scheduler flag touches it.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT.  The constant 0xc0 << 10 is built across
 * __CutsceneWait -- `mov r5, #0xc0` before the call, `lsl r5, #10` after -- and
 * a plain named `int k` assigned before the call reproduces that exactly,
 * including r5 being the register. The four stack arguments of
 * OvlFunc_927_2008ae8 come out right from bare literals; the ROM's separate
 * `mov r4, #0` for the three stack zeros and `mov r3, #0` for the register zero
 * needs no naming at all.
 *
 * NEXT: see the batch-331 section below.  The line that used to stand here,
 * calling this a specimen and not a candidate, is REFUTED -- it is a pass-3
 * candidate, EXACT at two register pins.
 *
 * ================= BATCH 331 BRIEF A =================
 *
 * THE PIN-FREE FIGURE ABOVE IS CONFIRMED.  Re-measured; exact length both ways,
 * relocations clean, no SIZE line.  aligncmp resolves the residue into three
 * insert/delete PAIRS and nothing else, so it is three pure reorderings: at each
 * of the three sites our `lsl r1` sits one slot early and the ROM's
 * `mov r0, #0x12` one slot late.  Everything else is aligned-equal.
 *
 * ***** EXACT AT TWO REGISTER PINS.  THIS IS A PASS-3 CANDIDATE. *****
 * Parked rather than landed ONLY on the pin policy, the same way its
 * overlay-mate ovl_7b4558/2009818.c is -- see that header, and
 * docs/owner-decisions.md standing standard 3.  The figure claimed at the top
 * of this file is the PIN-FREE figure; the pinned figure is nothing at all,
 * objcmp reporting 272 bytes, every encoding and all 25 relocations identical.
 * The pinned body is two declarations and three assignments away from the body
 * below; book it in reports/pass3-depin.md alongside 2009818, which has the
 * same shape and the same pin count.
 *
 * THE PINNED BODY: two pins declared ONCE at function scope and reused at all
 * three sites -- `register int q0 __asm__("r0")` and
 * `register int q1 __asm__("r1")` -- with each of the three failing calls
 * written as `q0 = 0x12; q1 = <the split build>; f(q0, q1, ...)`.
 * shimcount.py reports two register pins and flags the missing fakematch row,
 * as it should for a body nobody is shipping.
 *
 * THE PIN LADDER, measured, because the minimum was not obvious:
 *   r0 pinned once at function scope, used at all three sites   two differing
 *   r0 pinned separately at each site (three pins)              two differing
 *   r0 and r1 pinned separately at each site (six pins)         EXACT
 *   r0 and r1 pinned ONCE at function scope (two pins)          EXACT  <- minimal
 * So r1 carries half the residue, and the pins DO NOT need to be per-site.
 * Nobody had checked the second point; it is what keeps the count at two rather
 * than six, and it is the finding most likely to transfer to the other
 * straight-line interleave parks.
 *
 * EIGHT MORE PIN-FREE NEGATIVES, including the two dimensions this park had
 * never varied.  All at exact length unless marked:
 *   the SLOT constant 0x12 named in a local at the top of the function and
 *     used at the three failing sites                           inert
 *   the same, crossed with the split builds named just before each call  inert
 *   the slot assigned immediately before each of the three sites         inert
 *   the slot substituted for EVERY appearance of 0x12                    worse
 *   naming ONLY the first split argument at the three-argument site      inert
 *   naming ONLY the second split argument at that site                   worse
 *   all four split builds named at the TOP of the function          badly worse
 *   the slot assigned late and used at the two-argument site only        worse
 * The last-but-three and last-but-two answer the brief's "do not search the
 * diagonal of a square": the three-argument site's two split arguments were
 * varied INDEPENDENTLY for the first time here, and the asymmetry does not pay.
 *
 * THE MECHANISM, READ FROM THE COMPILER ON DISK rather than inferred, which
 * tightens the sibling park's account of the same chain:
 *   - precompute_register_parameters, calls.c:813-829, loops over the arguments
 *     in ASCENDING order and calls expand_expr for each register argument right
 *     there -- so an argument whose value must be COMPUTED gets the lowest
 *     INSN_LUIDs, in argument order.
 *   - its copy-to-pseudo gate is calls.c:849-856, and its FIRST clause is that
 *     the value must NOT already be a REG or a SUBREG of a REG.  Only then are
 *     BLKmode, `rtx_cost (args[i].value, SET) > 2` and
 *     `SMALL_REGISTER_CLASSES && *reg_parm_seen` consulted.
 * Those two facts close the pin-free route, and they are worth stating because
 * at first glance they look like an opening.  0x12 is an INTEGER_CST, so
 * expand_expr emits NO insn for it and rtx_cost keeps it out of the pseudo
 * path, leaving load_register_parameters to emit `mov r0, #0x12` after every
 * precompute -- the highest LUID in the block, which is exactly what loses the
 * sched2 tie the sibling park documents.  Naming it does not help either: a
 * named local's value IS already a REG, so the first clause excludes it too and
 * its materialisation stays wherever reload rewrites the pseudo's own set,
 * which is EARLIER than the call rather than between the two split builds.
 * There is no source position between "before the call" and "inside the
 * argument setup".  That gap is what a hard-register local fills, and it is why
 * the pin works and no spelling does.
 *
 * SPLIT SHAPE, for whoever lands this at pass 3: datacheck.py on the reference
 * .s is CLEAN, and split_s.py --dry-run cuts it TWO ways, not three -- the .s
 * holds two functions and this one is the first, so the target goes to the _b
 * stem at 115 lines and the other to the _c stem at 200, with
 * overlays/rom_7b4558/overlay.ld rewritten.
 *
 * NOT A DUPLICATE.  dupfuncs.py reports seven duplicate groups covering
 * fourteen functions and this is not a member, so landing it is worth one.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int a, int b, int c);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_927_200a1b0(void)
{
    unsigned char *e;
    int k;

    e = __MapActor_GetActor(0x12);
    __CutsceneStart();
    __MapActor_SetPos(0x12, 0x88 << 16, 0xb4 << 17);
    OvlFunc_927_2008ea8(0x12, 1);
    OvlFunc_927_2008d90(0x12, 0x88, 0xcc << 1, 0x80 << 12);
    __CutsceneWait(0xa);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 11), 0,
                        0, 0, 1, 0);
    __Func_8092adc(0x12, 0xc0 << 8, 0x28);
    __MapActor_Surprise(0x12, 0x81 << 1);
    __Func_80925cc(0x12, 2);
    __SetCameraTarget(0x12, 1);
    OvlFunc_927_2008d90(0x12, 0x88, 0xdc << 1, 0xc0 << 11);
    __Func_809280c(0, 0x12, 0);
    k = 0xc0 << 10;
    __CutsceneWait(0xa);
    OvlFunc_927_2008d90(0x12, 0x88, 0xec << 1, k);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0x12, 0x88, 0xfc << 1, k);
    __Func_809280c(0, 0x12, 0);
    __CutsceneWait(6);
    __SetCameraTarget(0, 1);
    __MapActor_SetPos(0x12, 0, 0);
    __CutsceneWait(0x3c);
    __SetFlag(0x89d);
    __CutsceneEnd();
}
