/* OvlFunc_927_2009818 -- asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s
 *
 * NON-MATCHING PIN-FREE, 3 differing encodings of 38.  RE-DERIVED batch 328,
 * --func AND --whole; exact length, relocations clean, no SIZE line.
 * PIN COUNT of the body below: 0.
 *
 * ***** 0 of 38 IS IN HAND AT TWO PINS.  SEE "THE LANDING" BELOW. *****
 * Parked rather than landed ONLY on the pin policy (owner-decisions.md
 * standing standard 3, and the Func_80979a4 precedent of 2026-10-04: 0 of 47
 * with ONE pin was parked at its pin-free figure for pass 3).  Figure 3 here
 * is the pin-free figure; the pinned figure is 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7b4558/2009818.c asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s --func OvlFunc_927_2009818
 *
 * SPLIT: NONE NEEDED.  datacheck.py CLEAN; split_s.py: holds only
 * OvlFunc_927_2009818 and no data, convert it directly.  No frame at all
 * (`push {lr}` / `pop {r0}` / `bx r0`); the two pool words are indices 36-37,
 * after the last instruction, and both match.
 *
 * ========== THE RESIDUE, indices 29-31 of 38 ==========
 *     idx  REF                    | OURS
 *     29   2011 mov r0, #0x11     | 0449 lsl r1, #17
 *     30   0449 lsl r1, #17       | 0452 lsl r2, #17
 *     31   0452 lsl r2, #17       | 2011 mov r0, #0x11
 * ROM:  mov r1,#0xd4 | mov r2,#0xf0 | mov r0,#0x11 | lsl r1 | lsl r2 | bl
 * ours: mov r1,#0xd4 | mov r2,#0xf0 | lsl r1 | lsl r2 | mov r0,#0x11 | bl
 *
 * ========== THE CHAIN, RE-READ IN BATCH 328 ==========
 *
 * The park's reading of the sched2 TIE is correct and its conclusion was aimed
 * one step too far downstream.  Confirmed first: there is NO sched1 in this
 * build at all -- the -da dump list goes .13.combine then .23.sched2 -- so
 * rank_for_schedule's register-pressure rung (guarded `!reload_completed`,
 * haifa-sched.c:4029) never runs and cannot be the cause.
 *
 * 1. precompute_register_parameters (calls.c:805) forces the two out-of-range
 *    constant arguments into PSEUDOS, because its only gate is
 *    `rtx_cost (value, SET) > 2 && SMALL_REGISTER_CLASSES && reg_parm_seen`.
 *    0x11 is a Thumb `mov #imm8` and costs less, so argument 0 is NEVER
 *    precomputed and its load is emitted by load_register_parameters AFTER all
 *    precompute insns.  .00.rtl, verbatim:
 *        insn 68  (set (reg:SI 37) (const_int 27787264 [0x1a80000]))
 *        insn 70  (set (reg:SI 38) (const_int 31457280 [0x1e00000]))
 *        insn 72  (set (reg:SI 0 r0) (const_int 17 [0x11]))
 *        insn 74  (set (reg:SI 1 r1) (reg:SI 37))
 *        insn 76  (set (reg:SI 2 r2) (reg:SI 38))
 * 2. .17.lreg gives pseudos 37/38 REG_EQUIV, and the allocator takes the r1/r2
 *    preference off the copies, so reload rewrites insns 68/70 IN PLACE as the
 *    mov/lsl pairs and deletes the copies.  .19.flow2 is therefore
 *        91 r1=0xd4 | 92 lsl r1 | 93 r2=0xf0 | 94 lsl r2 | 72 r0=17
 *        74, 76 -> NOTE_INSN_DELETED
 *    -- i.e. `mov r0,#0x11` ends up with the HIGHEST LUID in the block.
 * 3. .23.sched2's table, VERBATIM:
 *        91  prio 66  core : 77 92
 *        93  prio 66  core : 77 94
 *        92  prio 65  core : 98 79 77
 *        94  prio 65  core : 98 79 77
 *        72  prio 65  core : 98 79 77
 *    Priority picks 91 then 93.  The remaining three tie on priority, on the
 *    last-scheduled-insn class, AND on dependent count (3 each, identical sets
 *    {98,79,77}), so INSN_LUID decides and 72's highest LUID issues it LAST.
 * SO THE FREE VARIABLE IS THE LUID, NOT A FOURTH DEPENDENT.  The park went
 * looking for an insn after the call that writes r0 and not r1/r2, found none
 * in the ROM's 36-instruction stream, and recorded a bound.  The bound is true
 * and it is not the only route.
 *
 * ========== THE LANDING: 0 of 38, TWO PINS ==========
 * scratch_elev/b328/A/t9818/v6.c, which is this body with the call written
 *
 *     { PIN0; PIN1; q0 = 0x11; q1 = 0xd4 << 17;
 *       __MapActor_SetPos(q0, q1, 0xf0 << 17); }
 *     #define PIN0 register int q0 __asm__("r0")
 *     #define PIN1 register int q1 __asm__("r1")
 *
 *   objcmp --whole: OK whole file -- 104 bytes, 38 encodings and 12 relocations
 *   identical.  shimcount.py: 2 register pins (PIN0, PIN1), no fakematch row.
 *   The mechanism is exactly step 3: a hard-register local makes the r0 fill an
 *   ORDINARY STATEMENT, which puts it ahead of the two materialisations in the
 *   stream, so it holds the LOWEST LUID and wins the three-way tie.
 *
 * CONTROLS, each one change against v6/v1 (this is what proves the mechanism):
 *   v1 PIN0+PIN1+PIN2, q0 assigned FIRST                             0
 *   v9 PIN0+PIN1+PIN2, two-step `q1 = 0xd4; q1 <<= 17;` fills        0
 *   v6 PIN0+PIN1 only, q0 first                                      0  <- minimal
 *   v3 SAME THREE PINS but q0 assigned LAST                          3  <- order is the lever
 *   v5 PIN1+PIN2 only (no r0 pin)                                    3
 *   v4 pin-free named locals x,y for the two shifted arguments        3
 *   v8 PIN0 + pin-free locals for the other two                      3
 *   v2 PIN0 only, arguments 1/2 left bare                            4 (first moves to 27)
 *   v7 PIN0+PIN2                                                     4 (first moves to 27)
 * So {r0, r1} is the MINIMAL pin set and the ASSIGNMENT ORDER is load-bearing.
 *
 * MEASURED, ALL EXACTLY INERT AT 3 (earlier batches, 5 edits at depth 3, 22
 * live subsets): callee without a prototype; __MapActor_SetPos declared to
 * return a value; __CutsceneEnd declared to return a value; slot in a named
 * local; coordinates cast at the call; every pair and triple of those; varargs
 * declaration; unsigned coordinate parameters; unsigned char slot parameter;
 * coordinates in named locals immediately before the call; __CutsceneEnd with
 * no prototype.  -fno-schedule-insns2 is REJECTED: 9 of 38, first at index 6.
 *
 * SUPERSEDES the park's "re-attack it if a way is found to make gcc
 * rematerialise without inventing locals".  Rematerialisation was never the
 * question; stream position was.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_927_2009818(void)
{
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xe, 1);
    OvlFunc_927_2008d90(0xe, 0xd4 << 1, 0xf0 << 1, 0x79999);
    __CutsceneWait(2);
    OvlFunc_927_2008e18(0xe);
    __Func_8092950(0xe, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x305);
    __MapActor_SetPos(0x11, 0xd4 << 17, 0xf0 << 17);
    __CutsceneEnd();
}
