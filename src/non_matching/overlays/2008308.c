/* OvlFunc_967_2008308 -- NOT MATCHING.  Body REPLACED by batch 344 brief C.
 *
 * CURRENT FIGURE, in stream terms: 2 of 89 stream encodings (ref 89, ours 89),
 * first at index 19.  ref 60 16-bit instructions and ours 60, SIZE 236 both,
 * 29/29 32-bit entries, relocations IDENTICAL.  *** THE COUNTS NOW AGREE, SO
 * THIS 2 IS A REAL DISTANCE -- the park's 56 was PHASE. ***  Pin-free.
 *
 * ONE PIN TAKES IT TO ZERO.  `{ PIN1; q0 = 0xe; __MapActor_Emote(q0, 0x81 << 1,
 * 0x32); }` with `#define PIN1 register int q0 __asm__("r0")` is
 * BYTE-IDENTICAL.  Per the pin policy this ships PIN-FREE at 2 and the one-pin
 * 0 is recorded here for pass 3.  r0 ALONE is enough; PIN3 also gives 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008308.c \
 *     asm/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_c.s --func OvlFunc_967_2008308
 *
 * ========== THE PARK'S OWN QUESTION, ANSWERED ============================
 *
 * The held park located the missing instruction exactly -- a `b` over a
 * MID-FUNCTION LITERAL POOL at the reference's lines 38-43 -- and asked:
 * "WHICH REFERENCE REACHES THE ROM'S FIRST POOL ENTRY WITH A NARROW
 * `pool_range`?"  Answer: THE MASK, once the AND is HImode and the mask is
 * SIGNED.  One token:
 *
 *     m &= 0xffffc000;  w = m;        ->        w = m & ~0x3fff;
 *
 * - Dropping the `int` intermediate narrows the AND to HImode, so the constant
 *   load becomes `*thumb_movhi_insn` alternative 1 (arm.md:4318-4353) at
 *   `pool_range` 64 instead of `*thumb_movsi_insn` alternative 6's 1020
 *   (arm.md:3838-3857).  `-da` shows it directly: before, ";; SImode fixup for
 *   i23; addr 18, range (0,1020): 0xffffc000"; after, an HImode fixup at 64.
 * - 18 + 64 = 82, the end-of-function barrier sits at ~232, so `arm_reorg`
 *   (arm.c:5550-5572) finds `last_barrier == NULL` and `create_fix_barrier`
 *   (arm.c:5243-5318) manufactures the `b`.  Pool ORDER is still 0xffffc000
 *   then 0x26ec because the SImode entry's `max_address` is larger
 *   (`add_minipool_forward_ref`, arm.c:4820-4860).
 * - THE SIGN IS LOAD-BEARING and is why the park's prose said "gcc pools
 *   0xc000": `0xffffc000` has type `unsigned int` in C, so the narrowed HImode
 *   constant is 49152 and the figure is 3.  `~0x3fff` and `-0x4000` are `int`,
 *   the constant is -16384, the pool word is the ROM's 0xffffc000, figure 2.
 * - gcc prints `ldrh r3, .L4`; gas assembles the ROM's `ldr r3,[pc,#60]`
 *   (`4b0f`) because Thumb-1 has no pc-relative `ldrh`.  Batch 79's finding,
 *   and the same mechanism dma.h's DMA3_FILL16 comment documents.
 *
 * ========== THE REMAINING 2: AN INSN_LUID TIE, WITH ITS CAUSE ============
 *
 *     rom   mov r1,#0x81 / mov r0,#0xe / lsl r1,#1 / mov r2,#0x32 / bl Emote
 *     ours  mov r1,#0x81 / lsl r1,#1   / mov r0,#0xe / mov r2,#0x32 / bl Emote
 *
 * From `-fsched-verbose=6` (which writes into `.23.sched2`, NOT stderr): at
 * t=206 the ready list is `29 61 57 219` and insn 219 (`mov r1,#129`) wins on
 * priority 19 against 18.  At t=207 the list is `29 61 57 220` and insn 220
 * (`lsl`), 57 (`mov r0,#14`) and 61 (`mov r2,#50`) are all priority 18,
 * class 3 and three dependents each, so every rung of `rank_for_schedule` is
 * dead down to INSN_LUID (haifa-sched.c:4111-4114).
 *
 * WHY OUR LUIDS ARE IN THE WRONG ORDER.  `precompute_register_parameters`
 * (calls.c:845-857) forces an argument into a pseudo when
 * `rtx_cost (value, SET) > 2 && SMALL_REGISTER_CLASSES && reg_parm_seen`.
 * SMALL_REGISTER_CLASSES is TARGET_THUMB (arm.h:1061), and arm_rtx_costs for a
 * Thumb CONST_INT under SET is 0 below 256 and COSTS_N_INSNS(2) = 6 for a
 * shiftable constant (arm.c:2077-2084, COSTS_N_INSNS at arm.c:2029).  So 0x102
 * is PRECOMPUTED -- emitted before `load_register_parameters` ever writes r0 --
 * while 0xe and 0x32 are not.  The split of `(set r1 258)` into mov+lsl
 * (arm.md:3859-3870) happens in place, so the `lsl` inherits a LUID below r0's.
 *
 * The tree already holds the proof that the ROM's order needs r0 FIRST:
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_a_b.s:132-136 is a
 * GENERATED, byte-matching file with the identical window, and the `.c` beside
 * it reaches it with a pinned argument block -- its own header says
 * "UN-PINNING q1 ... so the mov/lsl pair stays expanded and sched2 can slot the
 * pinned `mov r0` BETWEEN the mov and the lsl."
 *
 * THE DEVICE-FREE ROUTE, WITH ITS PRECONDITION.  A named local
 * (`int e = 0x81 << 1;`) does skip precompute -- `args[i].value` is then a REG
 * -- and is still 2, because the pseudo is COALESCED INTO r1 AT ITS OWN
 * DEFINITION SITE, which is above r0.  What would move the constant down to the
 * parameter-load slot is `update_equiv_regs` (local-alloc.c:845-862), which
 * substitutes a single-use constant pseudo at its use and deletes the def --
 * gated on `REG_N_REFS == 2 && REG_BASIC_BLOCK (regno) < 0`.  This function is
 * straight-line up to the Emote call, so the pseudo is block-local and
 * REG_BASIC_BLOCK >= 0.  (Same gate batch 341 brief E hit.)  **So the open
 * question is sharp: a carrier for the 0x102 argument whose definition is in a
 * DIFFERENT BASIC BLOCK from the call.**
 *
 * GAP ARITHMETIC, per region (the only branch is the `if` at the end):
 *   park body   REF pre-if 48 / if-body 9 / post-if 3 = 60
 *               OURS pre-if 47 / if-body 9 / post-if 3 = 59   -1, ALL PRE-IF
 *   this body   OURS 48 / 9 / 3 = 60                           0
 *
 * ========== MEASURED INERT / WORSE (batch 344 brief C) ===================
 *
 * EMOTE SITE, all EXACTLY 2 -- the park's negative reproduces at a REAL
 * distance, which it could not before:
 *   `0x102` literal; `(0x40<<2)|2`; `0x81*2`; `0x81+0x81`;
 *   `(unsigned short)(0x81<<1)`; `0xe` as a named local used at EVERY call
 *   site; `int e = 0x81<<1;` in a block immediately before the call
 *   `do { } while (0);` immediately before the call            40  WORSE
 * MASK SPELLINGS:
 *   `w = m & 0xffffc000`                                        3
 *   `w = (short)(m & ~0x3fff)` / `w = m & -0x4000`               2
 *   `m &= ~0x3fff; w = m;` / `m &= -0x4000; w = m;`             56, 59 insns
 *   `w = m; w &= ~0x3fff;`                                      81, SIZE 232/236
 * FLAG AXIS, re-run on THIS body (the park's was run on the old one):
 *   -fno-gcse, -fno-strict-aliasing, -ffixed-r7, -fno-rerun-loop-opt,
 *   -fno-rerun-cse-after-loop                        all EXACTLY 2 (inert)
 *   -fcall-saved-r4 11; -fno-schedule-insns2 75 (91 encodings); -O1 72
 *
 * The park's two earlier headers and its original prose are preserved below.
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
    w = m & ~0x3fff;
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

/* ===== PREVIOUS PARK HEADER (batch 339 brief C), PRESERVED =====
(* OvlFunc_967_2008308 -- NOT MATCHING.  HELD by batch 339 brief C.
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
 *)
*/

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
