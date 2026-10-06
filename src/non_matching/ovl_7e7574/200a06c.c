/* OvlFunc_959_200a06c (0x0200a06c) -- NON-MATCHING.
 *
 * NON-MATCHING, 17 of 38 encodings  (MEASURED, batch 319 recipe backfill).
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e7574/200a06c.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_a_c.s --func OvlFunc_959_200a06c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: DUPLICATE-CONSTANT CSE, straight-line variant. THIRD instance
 * this batch, after ovl_7d30e0/200938c.c (a pair) and ovl_7e7574/200a1c4.c.
 *
 * 38 lines against the ROM's 37, and the extra line is the prologue:
 * `push {r5, r14}` where the ROM has `push {r14}`.
 *
 * A NEW PRESENTATION OF THE CLASS, and the reason this one is worth its own
 * file. The three __MapActor_SetPos calls pass 0xac<<18, 0xb0<<15; 0xb0<<18,
 * 0xb0<<15; 0xb4<<18, 0xc0<<15. gcc does not common the shifted VALUE -- it
 * commons the PRE-SHIFT BASE, hoisting `mov r5, #0xb0` and shifting copies of
 * it at each site, where the ROM writes `mov r1, #0xb0` and `mov r2, #0xb0`
 * separately.
 *
 * So the shared operand does not have to be the argument. `0xb0 << 15` and
 * `0xb0 << 18` are different values and neither is repeated in a way the
 * earlier two parks would describe; what is repeated is the eight-bit
 * immediate underneath both of them.
 *
 * This is straight-line from __CutsceneStart to __MapTransitionIn, so the
 * dominating-boundary remedy has nothing to attach to, exactly as in the other
 * two parks. Not re-measured here: the flag table in ovl_7d30e0/200938c.c
 * (-fno-rerun-cse-after-loop, -fno-gcse, -fno-cse-follow-jumps, and per-site
 * named locals) was inert on both earlier instances and this is the same
 * mechanism.
 *
 * NOT COPIED FROM ITS EXEMPLAR, deliberately. tools/fuzzy_solved.py offered
 * OvlFunc_925_200aeb8 at ratio 0.865, and that file is marked `// fakematch`:
 * it forces its register allocation with `__asm__ volatile ("" : "+r" (x))`
 * on thirteen locals. Copying that would have produced a match and propagated
 * a hack into a function that does not need one. The C below is written
 * plainly from the disassembly.
 *
 * NEXT: nothing source-level. Three specimens of this class now sit in the
 * tree; the useful next step is a tool that flags a repeated 8-bit immediate
 * across argument sites BEFORE the C is written, since all three were only
 * discovered from the prologue after screening.
  *
 * ================================================================
 * BATCH 332 -- RE-MEASURED, THE FIGURE CONFIRMED, THE MECHANISM CORRECTED,
 * THE FLAG TABLE MEASURED HERE RATHER THAN INHERITED, AND THE ONLY KNOWN
 * ROUTE SHOWN TO BE A PINNED ONE.
 * ================================================================
 *
 * THE FIGURE ABOVE STANDS at production flags, pin-free: seventeen of
 * thirty-eight in stream terms, with the two streams 27 against 28 sixteen-bit
 * instructions and ten 32-bit entries each, size 96 against 96 (an alignment
 * pad absorbs the instruction difference, which is why the size agrees).
 *
 * DIRECTION: OURS IS LONGER.  We emit an extra instruction; this is not a
 * missing statement.
 *
 * WHICH instruction, corrected.  The header above says "the extra line is the
 * prologue".  It is not: both sides push exactly one register list, and the
 * extra encoding is the EPILOGUE's `pop {r5}`.  The arithmetic is
 *   we save two instructions at the first two call sites (one `mov` and one
 *   `lsl` each become a single `mov r2, r5`), we spend two setting r5 up
 *   (`mov r5, #0xb0` before the first call and `lsl r5, #15` after it), and we
 *   spend one more restoring it.  Net one.
 *
 * WHAT IS COMMONED, corrected.  The header above says gcc commons the
 * PRE-SHIFT BASE and shifts copies at each site.  It does not: it commons the
 * SHIFTED VALUE.  There is exactly one `lsl r5, #15` in our output and the two
 * sharing sites both read `mov r2, r5`; the third site, whose value differs,
 * builds its own `mov`+`lsl` normally.  So the duplicate is the whole third
 * argument of the first two __MapActor_SetPos calls, not an eight-bit
 * immediate underneath two different values.
 *
 * WHY NO SPELLING CAN REACH IT -- read in the dumps and then in cse.c.
 * In the .00.rtl dump the two sharing sites' third arguments are ONE insn
 * each, `(set (reg) (const_int 5767168))`, identical rtx.  cse.c's own
 * cost macro at cse.c:509-514 scores a pseudo REG at 1 and sends anything
 * else to notreg_cost, and a Thumb two-instruction constant costs more than
 * that, so an available equivalent register ALWAYS wins and the unification is
 * unconditional.  The value is then live across a call, r4 is call-used under
 * this tree's flags, and every remaining low register is call-saved -- hence
 * the save.  Since fold collapses any equal-valued expression to the same
 * const_int before expansion, two spellings of the same number cannot present
 * as two rtxes.  MEASURED: writing the two values as different-looking but
 * equal expressions is byte-identical to writing them plainly.
 *
 * MEASURED HERE AND INERT (the header above declined to re-measure these; they
 * were measured, with tools/flagcmp.py, and all five leave the instruction
 * count at 27 against 28):
 *   the gcse-disabling flag                       -- inert
 *   the cse-rerun-after-loop flag                 -- inert
 *   the cse-follow-jumps flag                     -- inert
 *   the post-reload scheduling flag               -- changes the residue, not the length
 *   the pre-reload scheduling flag                -- inert
 * So there is no per-file flag row that lands this, and none should be
 * written.
 *
 * ALSO MEASURED AND WORSE OR INERT, pin-free:
 *   per-site named locals for both coordinates           -- inert
 *   per-site named local for the shared coordinate only  -- worse
 *   one reused local assigned before each call            -- much worse, and
 *                                                           four bytes longer
 *   an identity written into the second occurrence        -- inert
 *
 * THE ONLY ROUTE THAT REACHES ZERO IS PINS, AND IT IS RECORDED AS A DEVICE,
 * NOT AS A RESULT.  Forcing all three arguments of each call into r0/r1/r2
 * with `register ... __asm__` declarations -- nine declarations, three per
 * site -- gives a byte-identical object, relocations included.  That is the
 * same construction the LANDED src/overlays/rom_7b9cb4/ovl_30_a_c_c_c_a_a.c
 * already ships for OvlFunc_932_200b028, which has the very same duplicate
 * (`0xb0 << 15` twice across two __MapActor_SetPos calls) and is booked as a
 * fakematch.  Under this tree's pin policy that is NOT a landing: it belongs
 * to pass 3.  The device-free body below is what ships, and the number above
 * is its honest distance.
 *
 * WHAT THIS MAKES THE CLASS.  Three specimens were recorded as
 * "duplicate-constant CSE" needing a dominating boundary.  The boundary is a
 * red herring: the unification is a cost comparison with no boundary in it,
 * and the discriminator for the whole class is simply whether the duplicated
 * value's live range crosses a call.  Where it does, the only known source
 * -level answer is to keep the value out of a pseudo altogether, which from C
 * means a hard-register declaration.  A tool that flags a repeated LARGE
 * CONSTANT (not a repeated eight-bit immediate) across argument sites, and
 * reports whether a call sits between them, would have priced all three
 * before any C was written.
*/
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __MapTransitionIn(void);

void OvlFunc_959_200a06c(void)
{
    __CutsceneStart();
    __MapActor_SetPos(0xc, 0xac << 18, 0xb0 << 15);
    __MapActor_SetPos(0xd, 0xb0 << 18, 0xb0 << 15);
    __MapActor_SetPos(0xe, 0xb4 << 18, 0xc0 << 15);
    __MapActor_SetAnim(0xc, 5);
    __MapActor_SetAnim(0xd, 5);
    __MapActor_SetAnim(0xe, 5);
    __Func_809280c(0, 0xd, 0);
    __CutsceneEnd();
    __MapTransitionIn();
}
