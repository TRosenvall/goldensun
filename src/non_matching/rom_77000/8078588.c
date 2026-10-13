/* GiveItemTo (0x08078588) -- NON-MATCHING.
 *
 * NON-MATCHING, 58 of 68 encodings  (RE-MEASURED, batch 345 brief A).
 *   ref 68 encoding rows / 144 bytes; ours 68 / 144, dsize 0, relocations
 *   IDENTICAL.  The 16-bit INSTRUCTION counts differ by one (ours is one
 *   SHORT and carries one extra pool word plus a 2-byte pad), so the 58 is
 *   PARTLY PHASE.  ALIGNED residue 33 equal / 35 differing on the body below,
 *   against 32/36 for the body batch 319 measured.  Roughly HALF THE FUNCTION
 *   differs once aligned, so this is the most distant of brief A's four and it
 *   is NOT a near-miss however its figure reads.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078588.c \
 *     asm/rom_77000/rom_78414_c_c_a_a_c_a_c.s --func GiveItemTo
 *
 * ===== BATCH 345 BRIEF A: THE PARK'S "EVERY REMAINING DIFFERENCE IS WHICH OF
 * ===== r1/r2/r3/r4 CARRIES A VALUE" IS WRONG.  THERE ARE FOUR DEFECT CLASSES
 * ===== HERE AND TWO OF THEM ARE SIGNEDNESS, WHICH IS NOT ALLOCATION AT ALL.
 *
 * Per-region aligned split of the 35 (counted from an LCS-aligned listing):
 *   prologue + GetUnit + GetItemInfo + the info[3] & 0x10 test   0
 *   the stackable search loop (first loop)                       9
 *   the quantity update block                                   12
 *   the free-slot search loop (second loop)                     11
 *   epilogue                                                     0
 *   pool + pad                                                   3
 *
 * DEFECT 1 -- SIGNEDNESS, two instructions, and nothing had noticed:
 *     rom    lsrs r3, r1, #11        ... cmp r2, #29 / bhi
 *     ours   asrs r3, r1, #11        ... cmp r4, #29 / bgt
 * The ROM shifts LOGICALLY and compares UNSIGNED.  `v` and `q` are declared
 * `int` in the park; declaring both `unsigned int` produces `lsrs` and `bhi`.
 * MEASURED: `v` unsigned alone takes the aligned residue 36 -> 35; `q`
 * unsigned alone is EXACTLY INERT (the `bhi` needs the unsignedness to reach
 * the compare through `q = (v >> 11) + 1`, so it follows `v`, not `q`); both
 * together 35; `mask` unsigned as well changes nothing further.  The body
 * below carries both.  The ndiff does not move (58 either way) because this is
 * inside the out-of-phase region -- THE ALIGNED NUMBER IS THE ONLY ONE THAT
 * SHOWS IT, which is why six batches of figure-reading missed it.
 *
 * DEFECT 2 -- THE LITERAL POOL IS DUMPED MID-FUNCTION AND IN THE WRONG ORDER.
 *     rom    ... two `.word`s at the very END: 0x1ff then 0x7ff
 *     ours   ... both `.word`s dumped after the `b.n` in the middle, in the
 *                order 0x7ff then 0x1ff, plus a trailing `.short 0x0000` pad
 * The ROM reaches its pool with `ldr r2, [pc, #104]`; we reach ours with
 * `[pc, #68]` and `[pc, #12]`.  So this is a minipool-placement defect
 * (arm.c's `create_fix_barrier` taking the free barrier at the `b.n` rather
 * than carrying the pool to the end), and the extra word plus the pad is where
 * the -1 instruction / +1 pool-row bookkeeping comes from.  This is the thing
 * to attack first: it is what makes the two streams out of phase, and nothing
 * downstream can be read until it is fixed.
 *
 * DEFECT 3 -- the park's "ONE REAL FIX" DOES NOT HOLD, and the park
 * contradicts itself about it.  The park says naming the offset gives the
 * ROM's register-offset load for slot 0 and calls the body below "the
 * exact-length version"; the body below does NOT use a named offset in the
 * first loop.  MEASURED THIS BATCH: writing the first loop with a named `off`
 * (`off = 0xd8; ... *(unsigned short *)(u + off) ... off += 2;`) gives
 * ni 66 / dsize -4, i.e. TWO ENCODING ROWS SHORT, with aligned residue 38 --
 * WORSE than 35 and further out of phase.  So the "one real fix worth keeping"
 * is a regression on this body, and the park's own progression table (73 lines
 * 54 differing vs 72 lines 58) is the version that is NOT shipped.
 *
 * DEFECT 4 -- the two-address tie, the same shape found on Func_80c0eec this
 * batch.  In the quantity-update block:
 *     rom    ldr r3, =0x7ff / lsls r2, r2, #11 / ands r3, r1 / orrs r3, r2
 *     ours   ldr r3, =0x7ff / ands r1, r3 / lsls r3, r4, #11 / orrs r1, r3
 * The ROM ties the `and`'s output to the MASK register and shifts BEFORE the
 * and; we tie it to the value and shift after.  On Func_80c0eec the same tie
 * turned out to be decided by whether one operand is live past the insn, and
 * the lever there was the operand's liveness, not its spelling -- that is the
 * reading to carry here.
 *
 * MEASURED INERT OR WORSE on this body:
 *   - the eight per-file flag groups: -fno-gcse 58 at aligned 34/34 (the only
 *     one that improves anything, by one row); -fno-schedule-insns2, -ffixed-r7
 *     and -fno-rerun-loop-opt exactly inert at 35; -fno-strength-reduce 57 but
 *     ni 70 / dsize +4 and aligned 45 (much worse); -O1 63 at ni 63 / dsize -12.
 *   - *** -fno-rerun-cse-after-loop IS A FALSE WIN AND autosweep RANKS IT AS A
 *     CANDIDATE.  It reads 49 of 68 against a base of 58, but ni 70 / dsize +4
 *     (TWO ROWS LONG) and aligned 37 -- it RAISES the real residue from 35.
 *     autosweep flags it "lower count but INSTRUCTION COUNT MOVED"; it does not
 *     say it is worse, and it is.  Do not spend a round on this row.
 *   - the anchor axis on the parked body (autosweep, 21 boundaries, 4 of which
 *     did not compile): all inert at 58.
 *
 * NEXT: the minipool placement, not the registers.  Everything the park and
 * every later reading called "which of r1/r2/r3/r4 carries a value" is being
 * read through a phase offset caused by defect 2.
 *
 * --------------------- the park's header, carried forward ---------------------
 *
 * NON-MATCHING, 58 of 68 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078588.c \
 *     asm/rom_77000/rom_78414_c_c_a_a_c_a_c.s --func GiveItemTo
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: SCRATCH-REGISTER SELECTION (see docs/elevation.md).
 *
 * 73 lines against the ROM's 73, 54 differing, and after the one real fix
 * below every remaining difference is which of r1/r2/r3/r4 carries a value.
 *
 * THE ONE REAL FIX, and it is worth keeping. The ROM reads inventory slot 0
 * with a REGISTER offset -- `mov r3, #0xd8 / ldrh r3, [r6, r3]` -- where
 * `u + 0xd8 + i * 2` with `i` folded to zero gives pointer arithmetic and an
 * immediate-offset load (`mov r1, r6 / add r1, #0xd8 / ldrh r3, [r1]`).
 * Naming the offset in an int local produces the ROM's form. That is the
 * register-offset rule applied to a constant index, which is a case the
 * recorded note does not cover -- it is written for a computed one.
 *
 * With that fix the loads match and the residue is purely allocation:
 *
 *     rom    ldr r2, =0x1ff ... and r3, r2 ... mov r1, r2
 *     ours   ldr r4, =0x1ff ... and r3, r4
 *
 * MEASURED:
 *   pointer arithmetic for slot 0 (as first written)     73 lines, 54 differing
 *   the offset named, giving the register-offset load     72 lines, 58
 *
 * The named-offset version is one line SHORT because the ROM's `mov r1, r2`
 * copy of the mask has nowhere to come from -- the same
 * copy-into-a-register shape recorded three times already, and inert to source
 * spelling in all of them.
 *
 * The C below is the exact-length version. Per the recognition rule added in
 * batch 171, this got one probe past the point where the diff became a column
 * of `mov rN` against `mov rM`, and then stopped.
 *
 * WHAT IS RIGHT: the two search loops, the stackable-flag test on info[3] & 0x10,
 * the quantity field packed at bits 11..15 with its 0x1d ceiling, the 0x1ff id
 * mask, and the shared `-1` exit reached from both the overflow check and the
 * full-inventory case.
 *
 * NEXT: nothing source-level.
 */
extern unsigned char *GetUnit(int who);
extern unsigned char *GetItemInfo(int item);

int GiveItemTo(int who, int item)
{
    unsigned char *u;
    unsigned char *info;
    unsigned short *p;
    int i;
    int off;
    int mask;
    unsigned int v;
    unsigned int q;

    u = GetUnit(who);
    info = GetItemInfo(item);
    if ((info[3] & 0x10) != 0) {
        mask = 0x1ff;
        for (i = 0; i <= 0xe; i++) {
            if (((*(unsigned short *)(u + 0xd8 + i * 2) ^ item) & mask) == 0)
                break;
        }
        if (i != 0xf) {
            off = i * 2 + 0xd8;
            v = *(unsigned short *)(u + off);
            q = (v >> 11) + 1;
            if (q > 0x1d)
                return -1;
            *(unsigned short *)(u + off) = (v & 0x7ff) | (q << 11);
            return i;
        }
    }
    p = (unsigned short *)(u + 0xd8);
    off = 0xd8;
    for (i = 0; i <= 0xe; i++) {
        if (*p++ == 0) {
            *(unsigned short *)(u + off) = item;
            return i;
        }
        off += 2;
    }
    return -1;
}
