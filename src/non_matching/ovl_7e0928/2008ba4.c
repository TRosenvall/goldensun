/* OvlFunc_956_2008ba4  --  0x02008ba4 -- NOT MATCHING.  2 of 75 encodings,
 * SIZE and RELOCATIONS both silent, AND IT NEEDS GCSE_CFLAGS TO GET THERE.
 *   [asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a_a.s -- the WHOLE file: ONE
 *   function, no data.  overlays/rom_7e0928/overlay.ld:45 names the object
 *   once, `asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a_a.o(.text)`, and that is
 *   the only line in any .ld naming this full path -- the .data list (lines
 *   97-106) and the .bss list (108-113) name only ovl_30_c_c_c_c_c_a/_b/_c and
 *   common1_c_c_b.  So the landing is one line, asm/ -> src/, with NO SPLIT --
 *   if the last two encodings are ever closed.]
 *
 * SAY IT LOUDLY: THE 2-OF-75 RESULT IS UNDER -fno-gcse (GCSE_CFLAGS).  At the
 * tree default it is 72 of 75 with the relocation list four bytes short, and no
 * spelling recovers that without the flag.  If this is ever landed it needs the
 * object added to GCSE_CFLAGS in the Makefile, and that must be stated in the
 * commit -- it is not a default-flags match.
 *
 *   default -O2                 XX ENCODINGS differ in 72 place(s) (ref 75, ours 73)
 *                               XX RELOCATIONS differ
 *   -O2 -fno-gcse               XX ENCODINGS differ in 2 place(s) (ref 75, ours 75)
 *                                  first at index 21: ref 0073  ours 4f21
 *
 * Same verdict against the real asm/ path and against a scratch copy of the .s.
 *
 * WHAT THE FUNCTION IS.  71 instructions.  One area-guarded three-message
 * prompt, a sibling of the seven-member OvlFunc_962 family solved in
 * src/overlays/rom_7ec19c/ovl_30_c_a_c_b.c: say the opening line, run
 * __Func_8091c7c, then deliver the follow-up at base+1 (with a map transition
 * around OvlFunc_common1_78) or base+2.  The whole body sits inside
 * `if (*(short *)(gState + (0xe1 << 1)) == 2)`.
 *
 * THE MESSAGE BASE IS A LIVE PSEUDO, AND THAT IS THE ONLY REASON FOR THE FLAG.
 * The ROM holds 0x2073 in r7 across every call and derives the other two ids
 * from it -- `add r0, r5, r7`, then `add r0, r7, #1 / add r0, r5, r0` and
 * `add r0, r7, #2 / add r0, r5, r0`, with r5 = a * 3.  A three-operand add off
 * a held pool constant is the recorded signature of a LIVE pseudo (see
 * src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_b.c): cse.c
 * relates two CONST values only through get_related_value, which needs a
 * SYMBOL_REF, so two independent literals can never produce that `add`.
 *
 * TWO SPELLING DETAILS ARE LOAD-BEARING and both come from that same family:
 *
 *   - the derived ids must go through a SECOND named local assigned in the arm
 *     (`id = base + 1; __MessageID(a * 3 + id);`).  Written inline as
 *     `a * 3 + (base + 1)` gcc reassociates to `(a * 3 + base) + 1`, keeps the
 *     first site's sum in a callee-saved register and emits ONE `add r0, r5, #1`
 *     -- three instructions short of the ROM.  With the second local it emits
 *     the ROM's pair.
 *   - `__Func_8092c40` must NOT be declared.  The ROM fills its r0 LAST
 *     (`mov r1, #0 / mov r0, r6`), which is what gcc does for an implicitly
 *     declared callee; declaring it flips the pair.  Same finding as
 *     OvlFunc_962_20081d4.
 *
 * 0x2073 IS A LITERAL, NOT A SYMBOL, AND ONLY objcmp CAN SAY SO.  The reference
 * object carries exactly two R_ARM_ABS32 records for this function,
 * iwram_3001ebc and gState, so the `(int)&_MSG_2073` spelling that unlocked the
 * OvlFunc_962 family is WRONG here -- it would add a relocation the reference
 * does not have.  message.sym has no 0x2073 entry either.  This is the same
 * judgement the two rom_780898 siblings had to make in opposite directions.
 *
 * THE RESIDUE IS THE "POOL LOADS COME FIRST" ORDERING CLASS, EXACTLY TWO
 * ENCODINGS:
 *
 *     rom    bl __CutsceneStart / lsl r3, r6, #1 / ldr r7, =0x2073 /
 *            add r5, r3, r6 / add r0, r5, r7
 *     ours   bl __CutsceneStart / ldr r7, =0x2073 / lsl r3, r6, #1 /
 *            add r5, r3, r6 / add r0, r5, r7
 *
 * One adjacent swap.  Everything else -- the prologue push list and its
 * interleave, all five callee-saved assignments (r5 = a*3, r6 = a, r7 = 0x2073,
 * r8 = gState[0xfa<<1], r10 = iwram_3001ebc), both `add r0, r7, #K` pairs, both
 * __ActorMessage orders, the two iwram stores, the epilogue and the three-word
 * pool -- is byte-identical.
 *
 * THE SCHEDULER IS DOING IT, AND -fno-schedule-insns2 IS NOT THE ANSWER.  With
 * `h = a << 1;` written as its own statement BEFORE the base assignment the
 * pre-schedule order is already the ROM's, and sched2 hoists the pool load back
 * over the shift; -fno-schedule-insns2 pins it correctly and then costs 18
 * elsewhere, because the ROM's ENTRY block is itself heavily scheduled
 * (`mov r1,#0xfa / lsl r1,#1 / mov r10,r3 / add r3,r2,r1 / ldr r3,[r3] /
 * sub r1,#0x32 / mov r8,r3`).  sched2 has to stay on, and with it on the pool
 * load always leads.
 *
 * MEASURED WORSE (all under -fno-gcse unless noted; against 75 encodings):
 *
 *   spelling                                                  differing
 *   -------------------------------------------------------  ---------
 *   any spelling at the tree default -O2                       72 (+RELOC)
 *   derived ids inline as `a * 3 + (base + 1)`                 (-3 insns)
 *   `base` pinned to r7 / r5 / r6 / r4 / r8 / r9 / r10      68 / 13 / 11 / 72 / 64 / 66 / 65
 *   `a * 3` named as its own plain local (`prod`)               73 (+SIZE)
 *   `prod` pinned to r5, base plain                              8 (+RELOC)
 *   a copy `id = base;` at the first site as well               13
 *   base assigned in the dominating entry block                  5 (+RELOC)
 *   -fno-schedule-insns2 (fixes the swap, breaks the entry)      18
 *   -O1 / -O1 -fno-gcse                                         44 (+SIZE)
 *
 *   INERT AT 2 (the residue is immovable under all of these):
 *     -fno-rerun-cse-after-loop, -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 *     -fno-force-mem, -fno-peephole, -fno-caller-saves, -fno-delayed-branch,
 *     -fno-function-cse, -fno-defer-pop, -fno-strength-reduce,
 *     -fno-thread-jumps, -fno-regmove, -fno-optimize-register-move,
 *     -fschedule-insns, -fno-sched-interblock, -fno-sched-spec (all on top of
 *     -fno-gcse);
 *     dropping the prototype of ANY of the nine declared callees;
 *     `(a << 1) + a` or `a + a * 2` for `a * 3`;
 *     `h = a << 1;` as a separate statement, with or without an r0..r4 pin;
 *     `unsigned` for `base`; `base + a * 3` for `a * 3 + base`;
 *     the literal at the first site with `base` assigned after it;
 *     an r0 pin on the __MessageID argument;
 *     an early `return` for the guard instead of the `if`.
 *
 * BLOCKER CLASS: "Pool loads come first, but the basic-block lever moves them"
 * -- and this function has no usable boundary.  The lever's own correction
 * (batch 105) needs a block that DOMINATES the call and in which the constant
 * can be named; here the only boundary above the site is the area guard, and
 * naming the base there costs 5 and reorders the pool.  The other recorded
 * remedy for the straight-line members of the class is register pinning with
 * inline asm; all seven registers were tried and every one is worse than the
 * unpinned form, because gcc-2.96 will CSE a local register variable against
 * another pseudo that happens to be in the same hard register (r7 gives
 * `add r0, r7, r7` -- a miscompile, not just a mismatch).
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_common1_78(int a);

void OvlFunc_956_2008ba4(int a)
{
    unsigned char *p;
    unsigned char *g;
    int t;
    int id;
    int base;

    p = iwram_3001ebc;
    g = gState;
    t = *(int *)(g + (0xfa << 1));
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        __CutsceneStart();
        base = 0x2073;
        __MessageID(a * 3 + base);
        __Func_8092c40(a, 0);
        if (__Func_8091c7c(t, 0) == 0) {
            id = base + 1;
            __MessageID(a * 3 + id);
            __ActorMessage(a, 0);
            *(int *)(p + (0xe0 << 1)) = 0x80 << 2;
            *(int *)(p + (0xe4 << 1)) = 0xf;
            __MapTransitionOut();
            __WaitMapTransition();
            OvlFunc_common1_78(a);
            __MapTransitionIn();
            __WaitMapTransition();
        } else {
            id = base + 2;
            __MessageID(a * 3 + id);
            __ActorMessage(a, 0);
        }
        __CutsceneEnd();
    }
}
