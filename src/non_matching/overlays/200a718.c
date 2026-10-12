/* CLAIM LINE CORRECTED (batch 343): this park measures 17, not 55.
 *
 * Batch 336 brief G took this park 55 -> 17 and the improvement WAS carried into
 * the body, but this top claim line was never updated -- and the top line is what
 * parkcheck and piece.py parse, so the park has ranked as 55 for seven batches
 * while measuring 17.  Found by a parkcheck sweep of the 15-60 band in batch 343,
 * prompted by brief B finding the identical defect on OvlFunc_959_200a69c (claim
 * line 29, body 10).  A STALE TOP LINE HIDES A CLOSE PARK FROM TARGET SELECTION.
 */
/* OvlFunc_959_200a718 -- 0x0200a718,
 *
 * NON-MATCHING, 17 of 63 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 63, ours 59) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 152 bytes, ours 144.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/200a718.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s --func OvlFunc_959_200a718
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c.s
 *
 * A cutscene beat: give slots 2, 3 and 1 the same speed and send each to its
 * own mark, wait for each to arrive and turn it to 0xc000, then turn slot 0 as
 * well.
 *
 * 55 of 62. Not a new blocker -- it is the straight-line repeated-constant
 * class at a scale that makes the usual remedy uneconomic, and it is recorded
 * for that reason rather than for anything novel.
 *
 * THE SHAPE. The function has NO BRANCH anywhere, and it uses three
 * multi-instruction constants over and over:
 *
 *     0x80 << 9   three times (the speed x)
 *     0x80 << 8   three times (the speed y)
 *     0xc0 << 8   four times  (the facing angle)
 *
 * With no branch, nothing dominates the repeats, so cse1 commons every one of
 * them into a register and gcse cprop -- strictly cross-block -- cannot undo
 * it. That is the recorded rule, and it predicts exactly what the screen shows.
 *
 * WHY IT IS PARKED RATHER THAN FAKEMATCHED. The idiom would work; the sibling
 * OvlFunc_959_200c704 in this same overlay took it and matched. But that
 * function had ONE repeated constant and needed three pin blocks plus a
 * barrier. This one has TEN repeats across three distinct constants, and the
 * "anchor every argument of any call you anchor any argument of" rule means
 * pinning whole argument lists, so it would carry roughly ten pin blocks in 62
 * instructions. That is within house norms by raw count -- the heaviest
 * existing fakematch carries seventeen launders -- but the cost is in building
 * and then tearing each one down to prove it load-bearing, which is the
 * discipline that keeps a fakematch honest.
 *
 * So this is a judgement call, not a wall: a competent fakematch is available
 * and was declined as poor value against the ~1,000 functions still open. If
 * the fakematch backlog is ever worked deliberately, this is a good candidate
 * -- the structure is regular, the three constants are independent, and the
 * sibling next door is a working template for the idiom.
 *
 * NOT TRIED, because the plain floor was measured first and the decision
 * followed from it: any pin, any barrier, any two-step spelling.

 * ========================================================================
 * BATCH 336 BRIEF G -- FIFTY-FIVE TO SEVENTEEN.  SAME BLOCKER AS ITS PIECE
 * MATE OvlFunc_959_200a69c, AND THE SAME DOOR IN cse.c.
 * ========================================================================
 *
 * CURRENT FIGURE: 17 of 63 encodings in STREAM terms (ref 63, ours 63),
 * 49 instructions against 49, and the thirteen `bl` relocations now line up.
 * Superseded figure: fifty-five, measured on a stream four encodings short.
 * Barrier-free the body still reads FIFTY-FIVE (ref 63, ours 59).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/200a718.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s --func OvlFunc_959_200a718
 *
 * WHAT THE BLOCKER ACTUALLY IS, which this park did not say.  It is NOT
 * misalignment and it is NOT a missing construct: it is DUPLICATE-CONSTANT
 * CSE, the same class as the piece mate `src/non_matching/ovl_7e7574/200a69c.c`
 * -- and it runs in the SHORTER direction here, which is why it reads as
 * length loss.  The ROM pushes `{lr}`; the old body pushed `{r5, r6, lr}`,
 * because cse pass one unified `0x80 << 9` (three sites), `0x80 << 8` (three
 * sites) and `0xc0 << 8` (four sites) and parked two of them in callee-saved
 * registers.  That SAVED more materialisations than the extra `pop` cost, so
 * our stream came out three instructions SHORT rather than long.  Read the
 * push list before reading the length.
 *
 * THE LEVER, written up in full in the piece mate's header: cse.c:5741-5745
 * flushes cse's whole value table at an insn whose PATTERN is an ASM_OPERANDS
 * with MEM_VOLATILE_P.  `__asm__ volatile ("" :: "i"(0))` -- zero outputs,
 * zero clobbers, one IMMEDIATE input, no code emitted -- is that shape.
 * A bare `__asm__ volatile ("")` is ASM_INPUT and does nothing here
 * (measured: 56 of 63 with `::: "memory"`, which is a PARALLEL and also fails
 * the test).  One before each call carrying a repeated shifted constant:
 * 55 -> 17, and the stream lengths become equal.
 *
 * WHAT THE REMAINING 17 IS: argument-setup ORDER inside the six constant-
 * carrying sites, every instruction present.  `mov r0, #slot` sits two or
 * three slots later than the ROM's and the `lsl` one slot earlier, at each
 * site.  `-fno-schedule-insns2` is WORSE (26 of 63), so the production
 * schedule is right.  One further byte: the two-byte tail pad reads
 * `.short 0x0000` in the reference and `nop` (0x46c0) from gcc, because this
 * function has no literal pool for the `.align 2, 0` to sit in front of.
 *
 * PIECE MEMBERSHIP, CORRECTED.  `tools/piece.py` reports FOUR functions in
 * this reference, not two: 200a69c (now 10 of 48), this one, 200a7b0
 * (753 of 835) and 200b054 (no park body, 2,123 instructions).  This is the
 * SECOND member, so `split_s.py --dry-run` writes `_a.s` (200a69c, stays
 * assembly), `_b.s` (this function) and `_c.s` (the other two).
 * >> IT SPLITS OUT ALONE.  It is not hostage to the 753 or to 200b054.
 *
 * MEASURED (production flags, stream terms):
 *   park body, no barrier                                   55 of 63, 46 insns
 *   `:: "i"(0)` before the six constant sites               17 of 63  <- kept
 *   the same, skipping the first site                       17 of 63
 *   `:: "i"(0)` before EVERY call                           23 of 63
 *   `:: "i"(0)` AFTER each call instead of before           23 of 63
 *   `::: "memory"` before the six sites                     56 of 63, 46 insns
 *   one named local PER USE for every shifted constant       74 of 63, 63 insns
 *     -- the `OvlFunc_953_200a820` model the piece mate's header recommends;
 *        measured badly WORSE here, as that header predicted it would be with
 *        few repeated values
 *   `-fno-schedule-insns2` on the 17 body                   26 of 63
 *
 * SHIMS.  Six `__asm__ volatile ("" :: "i"(0))`, no pins.
 * `tools/shimcount.py` reports ZERO; its BARRIER regex matches only the bare
 * `__asm__ volatile ("")`, so this spelling is invisible to the audit. */
extern void __MapActor_SetSpeed(int slot, int vx, int vy);
extern void __Func_809218c(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8092adc(int slot, int angle, int frames);

void OvlFunc_959_200a718(void)
{
    __asm__ volatile ("" :: "i"(0));
    __MapActor_SetSpeed(2, 0x80 << 9, 0x80 << 8);
    __asm__ volatile ("" :: "i"(0));
    __Func_809218c(2, 0xfc << 1, 0xd8);
    __asm__ volatile ("" :: "i"(0));
    __MapActor_SetSpeed(3, 0x80 << 9, 0x80 << 8);
    __asm__ volatile ("" :: "i"(0));
    __Func_809218c(3, 0xdc << 1, 0xe8);
    __asm__ volatile ("" :: "i"(0));
    __MapActor_SetSpeed(1, 0x80 << 9, 0x80 << 8);
    __asm__ volatile ("" :: "i"(0));
    __Func_809218c(1, 0xf0 << 1, 0xe0);
    __MapActor_WaitMovement(1);
    __asm__ volatile ("" :: "i"(0));
    __Func_8092adc(1, 0xc0 << 8, 0);
    __MapActor_WaitMovement(2);
    __asm__ volatile ("" :: "i"(0));
    __Func_8092adc(2, 0xc0 << 8, 0);
    __MapActor_WaitMovement(3);
    __asm__ volatile ("" :: "i"(0));
    __Func_8092adc(3, 0xc0 << 8, 0);
    __asm__ volatile ("" :: "i"(0));
    __Func_8092adc(0, 0xc0 << 8, 0);
}
