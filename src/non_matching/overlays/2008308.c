/* OvlFunc_967_2008308 -- NOT MATCHING.  HELD by batch 339 brief C.
 *
 * CURRENT FIGURE, in stream terms: 56 of 89 stream encodings (ref 89, ours 89),
 * first at index 8.  THE TWO SIDES ARE NOT THE SAME LENGTH -- ref 60 16-bit
 * instructions against ours 59 (-1) -- so that 56 is PHASE, NOT DISTANCE.  The
 * body is UNCHANGED from the batch-338 recovery; brief C found no improvement
 * on it, and its value this round is the located instruction plus the negatives.
 *
 * THE MISSING INSTRUCTION IS LOCATED, AND IT IS NOT THE EMOTE INTERLEAVE.
 * It is a `b` over a MID-FUNCTION LITERAL POOL.  The reference, at
 * asm/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_c.s:39-43, reads
 *
 *      bl __ActorMessage
 *      b  .L360
 *      .pool_aligned
 *   .L360:
 *      mov r0, #0xa
 *
 * -- two of its three pool words (0xffffc000 and 0x26ec) sit there, and only
 * ActorCmd_ARRAY_944__02009314 goes at the end.  We dump all three at the end,
 * so we never emit the `b`.  objcmp sees it at stream index 8:
 * ref 4b0f (`ldr r3, [pc, #60]`) against ours 4b33 (`ldr r3, [pc, #204]`).
 * The 32-bit entry counts are 29/29, so the POOL WORDS ARE ALREADY RIGHT; the
 * whole -1 is the manufactured branch.
 *
 * THE MECHANISM, with the preconditions, because the obvious one does not hold.
 * `arm_reorg` (config/arm/arm.c:5459) puts a pool at the LAST BARRIER WITHIN
 * REACH of the pool's first entry, and manufactures one with a `b` through
 * `create_fix_barrier` only when `last_barrier == NULL` (arm.c:5550-5572) --
 * i.e. when no natural barrier is in range.  THE RANGE IS NOT THE REASON HERE:
 * the Thumb pool load is `*thumb_movsi_insn` alternative 6, whose `pool_range`
 * is 1020 (config/arm/arm.md:3856), and the whole function is 236 bytes, so the
 * end-of-function barrier is comfortably in reach.  docs/elevation.md's entry
 * "A mid-function pool is REPRODUCIBLE, not a disqualification" names the
 * producing shape as a NARROW first pool entry (`pool_range` 32 or 60,
 * arm.md:3069 and 3155 -- the HImode/QImode loads), and the related entry "An
 * HImode constant >= 0x8000 costs a mid-function pool AND a branch" is the
 * worked case.  So the open question for the next agent is sharp and checkable:
 * WHICH REFERENCE REACHES THE ROM'S FIRST POOL ENTRY WITH A NARROW `pool_range`?
 * Ours reaches 0xffffc000 with a SImode `ldr` and so can wait for the end.
 *
 * SECOND RESIDUE, unchanged and still the park's own: the argument-setup
 * interleave at __MapActor_Emote,
 *      rom   mov r1,#0x81 / mov r0,#0xe / lsl r1,#1
 *      ours  mov r1,#0x81 / lsl r1,#1   / mov r0,#0xe
 * This is an INSN_LUID tie at the bottom of `rank_for_schedule`
 * (haifa-sched.c:4111-4114): `mov r1,#0x81` wins on priority (it feeds the
 * `lsl`, which feeds the call, so priority 2 against 1), and the remaining
 * three are all class 3 with one dependent each -- arm_adjust_cost gives the
 * data dependence cost 1, which `rank_for_schedule` at haifa-sched.c:4077-4078
 * reads as class 3, so the class rung is dead -- leaving original insn order to
 * decide.  The ROM's order therefore says `mov r0, #0xe` had the lower LUID.
 *
 * MEASURED THIS ROUND, all against ref 60 / ours 59:
 *   the eight per-file FLAG groups on the unchanged body   56, and EVERY ONE
 *       still 59 instructions.  -O1 is 58 instructions / 72 differing and
 *       -fno-schedule-insns2 is 59 / 74; both WORSE.  Flag axis CLOSED.
 *   32 ANCHOR placements (`do { } while (0);` at every statement boundary)
 *       best 56, worst 76, and EVERY ONE still 59 instructions.  An anchor
 *       cannot manufacture a pool barrier -- it emits no insn, so it changes
 *       nothing arm_reorg can see.  Anchor axis CLOSED.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008308.c \
 *     asm/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_c.s --func OvlFunc_967_2008308
 *
 * The original prose park is preserved below.  NOTE its claim that "of the 60
 * differing, only TWO are real ... the pool point is downstream of nothing we
 * control" -- the pool point is the ENTIRE length gap, and arm.c:5550 says what
 * controls it, so it is not downstream of nothing.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, void *b);
extern unsigned char ActorCmd_ARRAY_944__02009314[];

void OvlFunc_967_2008308(void)
{
    unsigned char *a;
    int m;
    short w;

    a = __MapActor_GetActor(0);
    m = *(unsigned short *)(a + 6) + (0x80 << 6);
    m &= 0xffffc000;
    w = m;
    __SetFlag(0xc0 << 2);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x26ec);
    __CutsceneWait(0x32);
    __MapActor_Emote(0xe, (0x81 << 1), 0x32);
    __Func_809280c(0xe, 0, 0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    if ((w << 16) == (0x80 << 24)) {
        __Func_8092304(0, 0, 0x10);
        __Func_8092adc(0, 0xc0 << 8, 0);
        __CutsceneWait(0x14);
    }
    __MapActor_SetBehavior(0xe, ActorCmd_ARRAY_944__02009314);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_967_2008308 -- 0x02008308,
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
 *)

(* RETRIED (batch 147).  Four more spellings, none moved it off 60 differing:
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
 *)

*/
#endif
