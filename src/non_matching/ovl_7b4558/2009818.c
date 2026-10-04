/* OvlFunc_927_2009818 -- asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s
 *
 * NON-MATCHING, 3 of 38 encodings.  MEASURED THIS BATCH, --func AND --whole.
 * PIN COUNT: 0 (tools/shimcount.py reports no shims).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b4558/2009818.c \
 *     asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s --func OvlFunc_927_2009818
 *
 *   --func  : XX ENCODINGS differ in 3 place(s) (ref 38, ours 38), first at index 29
 *   --whole : OvlFunc_927_2009818  3 of 38 differ (ours 38), first at index 29
 *   relocations clean, no SIZE line, exact length.
 *
 * SPLIT: NONE NEEDED.  datacheck.py CLEAN; split_s.py says "holds only
 * OvlFunc_927_2009818 and no data; convert it directly".
 *
 * THE FRAME QUESTION IS VACUOUS HERE, checked with all five greps over the
 * extracted reference: no `sub sp,#imm`, no `(add|sub) sp, rN`, no `mov rX,sp`
 * and no `add rX,sp,#K`, no `str rX,[sp...]`, no two-operand `add rX, sp`.
 * The prologue is `push {lr}` and the epilogue `pop {r0} / bx r0`.  There is no
 * frame at all, so none of the seven aggregate-resolution classes applies.
 * THE POOL IS NOT MID-FUNCTION EITHER: the two pool words are indices 36-37,
 * after the last instruction, and BOTH MATCH -- so the Func_80b09fc "tu-pool"
 * precedent does not apply.  Checked, not assumed.
 *
 * ===== THE RESIDUE, AND THE EXACT RUNG -- WHICH IS NOT THE ONE THE PARK NAMED
 *
 * All three differing indices are REAL INSTRUCTIONS; the only pool words
 * (36, 37) match.
 *
 *     idx  REF                    | OURS
 *     29   2011 mov r0, #0x11     | 0449 lsl r1, #17
 *     30   0449 lsl r1, #17       | 0452 lsl r2, #17
 *     31   0452 lsl r2, #17       | 2011 mov r0, #0x11
 *
 * The park called this "argument emission interleave ... scheduler, probably
 * unreachable from this function's shape".  The first half is right, the
 * mechanism was not read.  It is:
 *
 * (1) gcc PRECOMPUTES the two out-of-range constant arguments into pseudos
 *     BEFORE any hard-register load.  .00.rtl, verbatim:
 *         insn 68  (set (reg:SI 37) (const_int 27787264 [0x1a80000]))
 *         insn 70  (set (reg:SI 38) (const_int 31457280 [0x1e00000]))
 *         insn 72  (set (reg:SI 0 r0) (const_int 17 [0x11]))
 *         insn 74  (set (reg:SI 1 r1) (reg:SI 37))
 *         insn 76  (set (reg:SI 2 r2) (reg:SI 38))
 *     This is precompute_register_parameters forcing the CONSTANT_P but not
 *     LEGITIMATE_CONSTANT_P arguments into registers.  0x11 fits a Thumb
 *     `mov #imm8` and is therefore NOT precomputed, so it is emitted LAST and
 *     gets the HIGHEST LUID of the three.  Reload then materialises the two
 *     pseudos in place as mov+lsl pairs, still ahead of insn 72 (.19.flow2:
 *     91, 92, 93, 94, 72, call 77).
 *
 * (2) sched2 then cannot fix it, because the ladder ties all the way down.
 *     From .23.sched2's own table for this block:
 *         insn 91 `r1=0xd4`   prio 66   dependents {77, 92}
 *         insn 93 `r2=0xf0`   prio 66   dependents {77, 94}
 *         insn 92 `r1<<=17`   prio 65   dependents {98, 79, 77}
 *         insn 94 `r2<<=17`   prio 65   dependents {98, 79, 77}
 *         insn 72 `r0=0x11`   prio 65   dependents {98, 79, 77}
 *     The two movs win the priority rung (two-insn chain to the call) and are
 *     emitted first, which we match.  The remaining three TIE on priority (65)
 *     AND on dependent count (3) -- identical successor sets, because the
 *     trailing `bl __CutsceneEnd` clobbers r0, r1 and r2 alike and the
 *     epilogue insn depends on all three.  So INSN_LUID decides, and insn 72
 *     has the highest.  The observed ready lists are exactly that:
 *         t=340:  33 ... 94 92   -> picks 92
 *         t=341:  72  94         -> picks 94
 *         t=342:  72             -> picks 72
 *     insn 72 sits at the HEAD (= lowest rank) of every list it appears in.
 *
 * ===== WHAT WOULD CLOSE IT, FROM A MATCHING FUNCTION THAT ALREADY DOES =====
 *
 * 221 matching functions in this tree emit the ROM's exact shape
 * (`mov r0,#imm / lsl rA / lsl rB / bl`), so IT IS REACHABLE.  Read one:
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_a.c writes the call as a plain
 * literal -- `__MapActor_SetSpeed(9, 0x80 << 8, 0x80 << 7);` -- with the same
 * precompute order and the same LUIDs, and gets the ROM's order anyway.  Its
 * block-0 visualization:
 *         107  371 r1=0x80        109  43  r0=0x9
 *         108  373 r2=0x80        110  372 r1=r1<<0x8   111  374 r2=r2<<0x7
 * It wins on the DEPENDENT-COUNT rung, before LUID is consulted, because the
 * statement AFTER the call is `if (arg != 0)`, which emits `r0 = r9` in the
 * same basic block.  That insn gives `mov r0,#9` an OUTPUT dependence the two
 * shifts do not have.
 *
 * > BOUND, with its evidence attached: in THIS function the three argument
 * > insns have IDENTICAL successor sets {call, __CutsceneEnd call, epilogue},
 * > measured in .23.sched2, so there is no rung above LUID to break.  Closing
 * > it needs an insn after `bl __MapActor_SetPos` that writes r0 and not
 * > r1/r2 -- and the ROM's 36-instruction stream contains no such insn
 * > (`bl __CutsceneEnd / pop {r0} / bx r0`).  That is a statement about what
 * > was measured here, not a claim that the class is closed: the 221 corpus
 * > hits say the shape is ordinary wherever the tie-break exists.
 *
 * MEASURED, ALL EXACTLY INERT AT 3 (crossfire, 5 edits at depth 3 -- 22 live
 * subsets, every one a candidate prerequisite, nothing better, nothing worse):
 *     callee without a prototype (the no-prototype lever)          3
 *     __MapActor_SetPos declared to return a value                 3
 *     __CutsceneEnd declared to return a value                     3
 *     slot in a named local                                        3
 *     coordinates cast at the call                                 3
 *     every pair and triple of the above                           3
 * And probed singly, with the same result: varargs declaration (3), unsigned
 * coordinate parameters (3), unsigned char slot parameter (3), coordinates in
 * named locals immediately before the call (3), __CutsceneEnd with no
 * prototype (3).  -fno-schedule-insns2 is REJECTED: 9 of 38, first at index 6.
 *
 * A FLAT SWEEP IS THE FINDING.  Neither the declaration dimension nor the
 * statement dimension touches this, because the decision is made in
 * precompute_register_parameters and then confirmed by a three-way tie.
 *
 * SUPERSEDES the park's "re-attack it if a way is found to make gcc
 * rematerialise without inventing locals" -- rematerialisation is not the
 * question.  The question is one extra dependent on the r0 move.
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
