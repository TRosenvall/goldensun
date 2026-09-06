/* PARKED -- OvlFunc_896_2009d04 -- 0x02009d04
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_c.s, the FIRST of the three
 *    functions left in that .s when _c_c_b.s was cut out for
 *    OvlFunc_896_200978c]
 *
 * 537 instructions of cutscene script.  BEST RESULT, against BOTH a scratch
 * copy of the reference and the REAL asm/ path, each re-run to confirm:
 *
 *   XX ENCODINGS differ in 8 place(s) (ref 537, ours 537)
 *      first at index 45: ref 2102  ours 4ea8
 *   XX RELOCATIONS differ
 *
 * SIZE IS SILENT -- 1400 bytes both -- and the relocation line differs in
 * EXACTLY TWO entries, both displaced by two bytes:
 *
 *   ref  00000076 __Func_80925cc / 0000007c __CutsceneWait
 *   ours 00000078 __Func_80925cc / 0000007e __CutsceneWait
 *
 * The same 8 and the same 2 under `-fno-gcse`, `-fno-rerun-cse-after-loop`,
 * `-ffixed-r7`, `-fno-strict-aliasing`, `-fno-cse-follow-jumps`,
 * `-fno-expensive-optimizations`, `-fno-strength-reduce` and `-fno-peephole`.
 * No flag group reaches it.
 *
 * ############################################################################
 * ## THE WHOLE RESIDUE IS TWO SCHEDULING DECISIONS.  Everything else -- 529  ##
 * ## of 537 encodings, the count, the size, the literal pool, the mid-       ##
 * ## function pool, the four-way control flow, all 149 other relocations --  ##
 * ## is byte-identical.                                                      ##
 * ############################################################################
 *
 * DEFECT 1 (6 of the 8): A POOL LOAD INTO A CALL-SAVED REGISTER IS HOISTED
 * ACROSS TWO CALLS, AND THE SAME LOAD INTO r0 IS NOT.
 *
 *   rom    bl __Func_8092adc / mov r1,#2 / mov r0,#0xe / bl __Func_80925cc
 *          mov r0,#0x14 / bl __CutsceneWait
 *          ldr r6, =0x10b6 / mov r0, r6 / bl __MessageID
 *   ours   bl __Func_8092adc / LDR R6, =0x10b6 / mov r1,#2 / mov r0,#0xe /
 *          bl __Func_80925cc / mov r0,#0x14 / bl __CutsceneWait /
 *          mov r0, r6 / bl __MessageID
 *
 * One instruction, moved five earlier; the two `bl`s it steps over are the two
 * displaced relocations.  READ FROM THE COMPILER RATHER THAN INFERRED:
 *
 *  * It is `-fschedule-insns2`.  Under `-fno-schedule-insns2` the load sits
 *    exactly where the ROM has it (and 162 other things break, see below).
 *  * haifa's `rank_for_schedule` (haifa-sched.c:4029) compares INSN_PRIORITY
 *    first and everything else only on a tie.  Priority is the critical path to
 *    the end of the block, so `movs r1, #2` (feeding the call two calls
 *    earlier) and `ldr r6` (feeding the call after next) differ only by the
 *    LOAD LATENCY the machine description gives `ldr`.  That latency is not
 *    reachable from C.
 *  * The value MUST be in a call-saved register, because the ROM derives
 *    `0x10ba` as `add r0, r6, #4` and `0x10bb` as `add r6, #5` -- cse's
 *    `related_value` off the live base.  Spelling all three as literals loses
 *    both derivations: 41 aligned differing.
 *  * And there is no third way.  A hybrid -- `__MessageID(0x10b6)` as a
 *    literal with `m = 0x10b6;` introduced only for the later uses -- is the
 *    IDENTICAL 4 aligned, because cse commons the two and rebuilds exactly the
 *    same RTL.  So does binding the local with
 *    `register int m __asm__("r6");`.  Six spellings, one output.
 *
 * NEW, and grepped by concept first (docs/elevation.md §"Pool loads come
 * first...", §"gcc hoists a POOL LOAD too", §"a pool load of a SYMBOL is not
 * hoisted, where a pool load of an int constant is"): those record hoisting as
 * a CSE/loop phenomenon and offer symbols as the counter-lever.  This is
 * SCHEDULER hoisting of a load whose value is already correctly commoned, over
 * a distance of two calls, and it is decided by whether the destination is
 * call-saved.  The recorded remedies do not apply: there is no second use to
 * perturb, and the value is an integer, not a symbol.
 *
 * DEFECT 2 (2 of the 8): ONE ARGUMENT-SETUP TRANSPOSITION IN THE `if` ARM OF
 * THE MESSAGE RE-ASK LOOP.
 *
 *   rom    bl __CutsceneWait / movs r1, #4 / movs r0, #0xe / bl DoAnim
 *   ours   bl __CutsceneWait / movs r0, #0xe / movs r1, #4 / bl DoAnim
 *
 * The IDENTICAL two instructions in the LOOP BODY (`.L2048`) come out right
 * from the descending pin.  In the `if` arm nothing moves them: ascending pin,
 * descending pin, reversed `register` DECLARATION order, a partial pin on r1
 * only, a partial pin on r0 only, no pin at all, the message id assigned before
 * the call, before the wait, and after it -- eleven spellings, all the same two
 * instructions in the same wrong order.  The blocks differ only in what FOLLOWS
 * the call (a pool load and a jump, versus another call), which is again a
 * priority input, not a source input.
 *
 * `-fno-schedule-insns2` IS AN ACTIVELY MISLEADING PROBE HERE, exactly as
 * docs/elevation.md §4941 says.  It fixes defect 1 and costs 162 aligned
 * differing, and the reason is structural, not tunable: this function's
 * argument setups are visibly INTERLEAVED -- `mov r1,#0xa0 / mov r2,#0xa /
 * mov r0,#0xe / lsl r1,#7` splits r1's `mov` from its `lsl` around two other
 * arguments -- and one `q1 = 0xa0 << 7;` statement emits its `mov`/`lsl` pair
 * adjacently.  No source order can produce that interleave without the
 * scheduler, so SCHED2_CFLAGS cannot be the landing shape for this function
 * however the pins are retuned.
 *
 * WHAT THE FUNCTION IS, and everything below here is CONFIRMED by the 529
 * matching encodings.
 *
 * THE PROLOGUE, BY CONTENT.  `push {r5, r6, r7, lr}` plus `mov r7, r8 /
 * push {r7}`: FOUR values live across calls, no r4 (-fcall-used-r4 in force).
 * A one-variable-per-register reading does not start:
 *
 *   r5  FOUR tenants -- the `0x200a` argument of two OvlFunc_896_200c248
 *       calls, then the `actor + 0x5a` byte pointer, then the 30-iteration
 *       counter, then the `0xc0 << 11` store value
 *   r6  TWO tenants -- the message base `m`, then the `actor + 0x55` byte
 *       pointer
 *   r7  the actor pointer, reassigned three times
 *   r8  a zero, set beside the `&= 0xfe` and read once, ~50 instructions later
 *
 * The zero is not optional: writing the literal `0` at its one store is 531
 * instructions, SIX SHORT, and 27 aligned.  One shared actor variable is
 * right; three (one per `__MapActor_GetActor`) is 535 instructions and 39
 * aligned.
 *
 * THE MESSAGE RE-ASK LOOP IS A ROTATED `while (1)` WITH A MID-BODY BREAK.
 * The ROM enters the loop at its MIDDLE (`b .L205e` past the re-ask preamble)
 * and gcc cross-jumps the two `__MessageID` tails into one.  The rotated
 * spelling reproduces it exactly:
 *
 *     v = 0x10c3;
 *     while (1) {
 *         __MessageID(v);  __Func_8092c40(0xe, 0);
 *         if (__Func_8091c7c(1, 0) != 0) break;
 *         ... ; v = 0x10c6;
 *     }
 *
 * and an explicit `goto` into the middle of a `do`/`while` TIES it, so the
 * natural spelling ships (docs/elevation.md §"BLOCK LAYOUT IS SOURCE ORDER,
 * AND `goto` LOSES IT").  The unrotated `while (cond == 0) { ... }` with the
 * preamble duplicated is 541 instructions, FOUR LONG, 22 aligned -- gcc does
 * not cross-jump it.
 *
 * THE `orr` DESTINATION LEVER IS REQUIRED AND THE `and` ONE IS NOT, which is
 * the sibling's finding reproduced in a second function:
 *
 *     rom   ldrb r2,[r5] / mov r3,#0xfe / and r3,r2      plain `*f &= 0xfe` OK
 *     rom   ldrb r2,[r5] / mov r3,#1    / orr r3,r2      needs the temp
 *
 * `t1 = 1; t1 |= *f; *f = t1;` at both OR sites, with SEPARATE temporaries.
 * Plain `*f |= 1` at both is 8 aligned; writing the AND through a temp as well
 * ties.
 *
 * THE DESCENDING-FILL CALLEE IS REACHED BY NAME.  Both `__Func_8092c40` sites
 * (slots 0xb and 0xe) want `mov r1 / mov r0`; dropping them from the DESC list
 * costs 10 aligned, and the one other descending site in the function
 * (`__MapActor_DoAnim(0xe, 4)` inside the re-ask loop) was found only from the
 * residue.
 *
 * THIRTY-ONE PINS, minimised greedily to a fixpoint FROM BOTH ENDS on the
 * sequence-aligned metric; both directions converge on the SAME set:
 * 7, 9, 11, 12, 17, 21, 26, 27, 28, 39, 42, 45, 54, 55, 59, 61, 62, 63, 66,
 * 74, 78, 79, 85, 88, 98, 99, 108, 114, 119, 124, 129.  No pins at all is 545
 * instructions and 144 aligned.  Pinning the two `OvlFunc_896_200c248(0x200a,
 * ...)` sites is what DESTROYS the shared `0x200a` register, exactly as the
 * sibling records for its message sites -- they are excluded, and the value
 * then CSEs into r5 unaided (a named local for it ties).
 *
 * MEASURED WORSE (one change at a time against the final candidate):
 *
 *   | change                                       | insns | aligned |
 *   |----------------------------------------------|-------|---------|
 *   | no pins at all                               |  545  | 144     |
 *   | message ids as three literals                |  537  |  41     |
 *   | unrotated `while` for the re-ask loop        |  541  |  22     |
 *   | one actor variable per GetActor (three)      |  535  |  39     |
 *   | literal `0` instead of the r8 zero           |  531  |  27     |
 *   | no DESC sites at all                         |  537  |  10     |
 *   | DESC on 85 only (the two 8092c40s dropped)   |  537  |   8     |
 *   | plain `*f \|= 1` at both OR sites             |  537  |   8     |
 *   | `i++` before the wait instead of after       |  537  |   6     |
 *   |----------------------------------------------|-------|---------|
 *   | `goto`-into-the-middle re-ask loop           |  537  |   4     |
 *   | `0x200a` in a named local                    |  537  |   4     |
 *   | hybrid literal/variable message base         |  537  |   4     |
 *   | `register int m __asm__("r6")`               |  537  |   4     |
 *   | DESC on 83 as well                           |  537  |   4     |
 *   | `while` / `goto` counting loop               |  537  |   4     |
 *   | `k = 0x80 << 9` hoisted out of the loop body |  537  |   4     |
 *   | AND written through a temp too               |  537  |   4     |
 *   | `f`/`b`/`c` inline instead of named          |  537  |   4     |
 *   | whole-value fill instead of `a << k`         |  537  |   4     |
 *
 * The named `f = a + 0x5a`, `b = a + 0x55` and `c = 0xc0 << 11` locals all TIE
 * with their inline forms and are kept anyway, because the ROM's own register
 * usage implies them -- each is held in a CALL-SAVED register across a call --
 * and docs/elevation.md §894's corollary says not to delete a local the ROM's
 * own code implies.
 *
 * IT DOES NOT BLOCK THE COLLAPSE ON FLAGS.  This candidate is the same 8
 * encodings under `-fno-gcse` as at the default, so if the residue is ever
 * closed the function is free to share a TU with OvlFunc_896_200978c.  What
 * blocks the collapse today is simply that this function does not match.
 *
 * Harness: scratch_elev/b243/f2009d04/{sweep,sdiff,minaligned,objcmp2}.py plus
 * d04/gen.py and d04/calls.txt (the transcription, with the ROM's emitted
 * register order per site).
 */
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __PlaySound(int id);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_TravelTo(unsigned char *a, int x, int b, int y);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_800fe9c(void);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_8078a08(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_896_200c248(int a, int b);
extern void OvlFunc_896_200c3bc(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define RIN2 register int q1 __asm__("r1"); \
             register int q0 __asm__("r0")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_896_2009d04(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *f;
    unsigned char *b;
    int m;
    int z;
    int c;
    int i;
    int v2;
    int t1;
    int t2;

    __Func_80925cc(0x1, 0x3);
    __CutsceneWait(0xa);
    __Func_8092adc(0x1, 0xc0 << 6, 0x0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0x0);
    __Func_8092950(0xe, 0xf);
    { PIN3; q0 = 0xe; q1 = 0xc4 << 17; q2 = 0xe3 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    OvlFunc_896_200c3bc();
    { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x1, 0x2);
    { PIN3; q0 = 0x1; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xe, 0x2);
    __CutsceneWait(0x14);
    m = 0x10b6;
    __MessageID(m);
    __ActorMessage(0xe, 0x0);
    { PIN3; q0 = 0xa; q1 = 0x1d50000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_896_200c248(0x200a, 0xa);
    OvlFunc_896_200c248(0x200a, 0x28);
    { PIN3; q0 = 0xa; q1 = 0x1fb0000; q2 = 0xae << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80925cc(0x1, 0x2);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x1; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x185; q2 = 0xea << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_896_200c248(0x1, 0x14);
    __Func_8019aa0(m + 4, 0x1, 0xa);
    __MapActor_SetSpeed(0x1, 0x80 << 8, 0x80 << 7);
    a = __MapActor_GetActor(0x1);
    f = a + 0x5a;
    *f &= 0xfe;
    z = 0x0;
    __Func_80921c4(0x1, 0xbc << 1, 0xeb << 1);
    __CutsceneWait(0x1e);
    t1 = 0x1; t1 |= *f; *f = t1;
    __MapActor_DoAnim(0xe, 0x4);
    m += 5;
    __CutsceneWait(0xa);
    __MessageID(m);
    OvlFunc_896_200c248(0xe, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xe, 0x3);
    OvlFunc_896_200c248(0xe, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0xe, 0x3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092950(0xe, 0x80 << 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0x0);
    a = __MapActor_GetActor(0xe);
    b = a + 0x55;
    *b = z;
    __PlaySound(0xdc);
    i = 0x0;
    do {
        *(int *) (a + 0xc) += 0x80 << 9;
        __CutsceneWait(0x1);
        i++;
    } while (i != 0x1e);
    *b = 0x5;
    __Func_809259c(0x1, 0x2);
    OvlFunc_896_200c248(0x1, 0xa);
    { PIN3; q0 = 0xe; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    OvlFunc_896_200c248(0x1, 0x14);
    __Func_80925cc(0xe, 0x1);
    OvlFunc_896_200c248(0xe, 0x14);
    { PIN3; q0 = 0x1; q1 = 0x103; q2 = 0x14;
      __MapActor_Emote(q0, q1, q2); }
    OvlFunc_896_200c248(0x1, 0x1e);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xd0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xa0 << 7; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0x1dd0000; q1 = -0x1; q2 = 0xa7 << 17; q3 = 0x0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(0x1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 0x4);
    OvlFunc_896_200c248(0xa, 0xa);
    { PIN2; q1 = 0x0; q0 = 0xb;
      __Func_8092c40(q0, q1); }
    __Func_80933d4(0x66666, 0xcccc);
    __Func_80933f8(0xbb << 17, -0x1, 0xeb << 17, 0x1);
    __Func_8093530();
    { PIN3; q0 = 0xe; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xe0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x1, 0x2);
    if (__Func_8091c7c(0x1, 0x0) != 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0xe, 0x4);
        v2 = 0x10c3;
        while (1) {
            __MessageID(v2);
            { PIN2; q1 = 0x0; q0 = 0xe;
              __Func_8092c40(q0, q1); }
            if (__Func_8091c7c(0x1, 0x0) != 0)
                break;
            __CutsceneWait(0x14);
            { PIN2; q1 = 0x4; q0 = 0xe;
              __MapActor_DoAnim(q0, q1); }
            __CutsceneWait(0xa);
            v2 = 0x10c6;
        }
    }
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xe, 0x3);
    __CutsceneWait(0x14);
    __MessageID(0x10c4);
    OvlFunc_896_200c248(0xe, 0x1e);
    __MapActor_DoAnim(0xe, 0x3);
    __CutsceneWait(0xa);
    OvlFunc_896_200c248(0xe, 0x1e);
    *b = 0x0;
    { PIN3; q0 = 0xe; q1 = 0x26666; q2 = 0x13333;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN4; q0 = (int) a; q1 = 0xe6 << 17; q2 = 0x0; q3 = 0xb4 << 17;
      __Actor_TravelTo((unsigned char *) q0, q1, q2, q3); }
    __MapActor_WaitMovement(0xe);
    __Func_8092950(0xe, 0x0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0x1);
    __CutsceneWait(0x1e);
    __SetCameraTarget(0x1, 0x1);
    __Func_8093530();
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x1; q1 = 0x103; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x1, 0x3);
    __CutsceneWait(0x14);
    a = __MapActor_GetActor(0x1);
    t2 = 0x1; t2 |= a[0x5a]; a[0x5a] = t2;
    *(int *) (a + 0x30) = 0xc0 << 10;
    *(int *) (a + 0x34) = 0x80 << 10;
    c = 0xc0 << 11;
    __PlaySound(0x99);
    *(int *) (a + 0x28) = c;
    __MapActor_SetAnim(0x1, 0x7);
    { PIN3; q0 = 0x1; q1 = 0xab << 1; q2 = 0xeb << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x1);
    __CutsceneWait(0x1e);
    __PlaySound(0x99);
    *(int *) (a + 0x28) = c;
    __MapActor_SetAnim(0x1, 0x7);
    { PIN3; q0 = 0x1; q1 = 0x9c << 1; q2 = 0xeb << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x1);
    __CutsceneWait(0x1e);
    __PlaySound(0x99);
    *(int *) (a + 0x28) = c;
    __MapActor_SetAnim(0x1, 0x7);
    { PIN3; q0 = 0x1; q1 = 0x8b << 1; q2 = 0xf0 << 1;
      __Func_8092158(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x1);
    __CutsceneWait(0x1e);
    __Func_80933d4(0x80 << 8, 0x80 << 5);
    __SetCameraTarget(0x0, 0x1);
    { PIN3; q0 = 0x1; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092848(0x0, 0x1, 0x0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0x1, 0x3);
    __MapActor_DoAnim(0x0, 0x4);
    __Func_80925cc(0x1, 0x2);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x1, 0x2);
    p = __MapActor_GetActor(0x0);
    if (p != 0)
        __MapActor_TravelTo(0x1, *(short *) (p + 0xa), *(short *) (p + 0x12));
    __MapActor_WaitMovement(0x1);
    __MapActor_SetPos(0x1, 0x0, 0x0);
    __Func_8078a08(0xdc);
    __Func_8078a08(0xdd);
    __Func_8078a08(0xdf);
}
