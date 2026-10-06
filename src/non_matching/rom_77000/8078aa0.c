/* Func_8078aa0  [rom_77000]  --  NOT MATCHING
 *
 * NON-MATCHING, 17 of 23 encodings  (MEASURED batch 330, objcmp --func,
 *   production flags, PIN-FREE).  THIS ONE IS A TRUE DISTANCE:
 *     INSTRUCTION COUNT  ref 21, ours 21     POOL WORD COUNT  ref 1, ours 1
 *     SIZE equal at 48 bytes, RELOCATIONS IDENTICAL
 *   objcmp prints no SIZE line and no RELOCATIONS line, only the encodings
 *   count, which is what makes the number a distance rather than a reading.
 *
 *   THE PREDECESSOR FIGURE OF TWENTY-FOUR OF TWENTY-THREE IS DEAD.  It was a
 *   misalignment reading over streams two instructions apart -- the old body
 *   ran four bytes long and its relocation sat at a shifted offset, which the
 *   old header had already noticed without drawing the consequence.  The
 *   "guard inversion plus three instructions of extra control flow" diagnosis
 *   is also dead: the extra control flow was TWO instructions, and the single
 *   edit that removes both is below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078aa0.c \
 *     asm/rom_77000/rom_78a8c_c_a_a_a.s --func Func_8078aa0
 *
 * Source asm: goldensun/asm/rom_77000/rom_78a8c_c_a_a_a.s
 * The reference .s holds this function ALONE, so --whole is meaningful here and
 * AGREES: same figure, no SIZE line, no RELOCATIONS line, no section-tail
 * difference.  The figure is not a --func artifact.
 *
 * Split shape: none.  The function already sits alone in the reference .s, so
 * no datacheck.py / split_s.py step applies.  Pins: ZERO.
 *
 * WHAT CLOSED THE LENGTH GAP.  Two edits, both plain C:
 *   1. SINGLE EXIT.  `if (i <= 0x7f) { ...body... } return r;` instead of
 *      `if (i > 0x7f) return r;` followed by the body.  The early-return form
 *      makes gcc materialise zero TWICE -- once for the accumulator and once
 *      for the return register on the guard path -- and pay a `b` to the
 *      epilogue.  The guarded form emits the ROM's `bgt` straight to the pop.
 *      This alone took the stream from twenty-three instructions to the ROM's
 *      twenty-one and made the relocation offsets agree.
 *   2. SYMMETRIC CLAMP ARMS.  Each arm sets the stored value and then the
 *      returned value, in that order -- the order the ROM's `movs r3, #0x63`
 *      / `movs r0, #0x63` pair is in.  Swapping the two statements inside the
 *      arm scores one lower positionally but is NOT the ROM's order, and it
 *      does not reach zero under the pin below; do not be tempted by it.
 *
 * WHAT THE RESIDUE IS: ONE HARD-REGISTER SWAP, AND NOTHING ELSE.  The ROM
 * keeps the return accumulator in r0 for the whole function and copies the
 * index parameter out of r0 into r2 (`adds r2, r0, #0`).  This body does the
 * reverse -- index stays in r0, accumulator goes to r2 -- so it pays one
 * `adds r0, r2, #0` at the tail, and the ROM's `adds r2, r0, #0` / `movs r0, #0`
 * pair comes out in the other arrangement.  Every branch, every compare, both
 * clamp arms, the table load, the pool word and the relocation already agree.
 *
 * PROOF THAT THIS IS THE WHOLE RESIDUE.  ONE register pin closes it to zero,
 * TWO INDEPENDENT WAYS: `register int r __asm__("r0")` on the accumulator, or
 * `register int i __asm__("r2")` on a copy of the index parameter.  Both read
 * byte-identical, same size, same relocations.  Parked rather than landed under
 * the prefer-pin-free policy (owner decisions, batch 319); this is a one-pin
 * pass-3 item and reports/pass3-depin.md is where it belongs.
 *
 * THE MECHANISM, READ OUT OF THE COMPILER AND OUT OF ITS OWN DUMPS.
 *   * `allocno_compare` (global.c:598-621) sorts allocnos by
 *     `floor_log2 (n_refs) * n_refs / live_length * 10000 * size`, descending,
 *     ties broken by allocno number ascending.
 *   * The `.17.lreg` dump prints the inputs: the index pseudo is "used 4 times
 *     across 16 insns", the accumulator pseudo "used 4 times across 26 insns".
 *     That is 8/16 against 8/26, so the index sorts first, and the `.18.greg`
 *     dump's own allocation order confirms it exactly.
 *   * `find_reg` then picks by REG_ALLOC_ORDER (arm.h:989-995, which is
 *     3,2,1,0,12,14,4,5,...) and would hand the index r2 -- but the
 *     copy-preference override at global.c:1067-1090 moves it onto r0, because
 *     the incoming-argument copy gives the index pseudo a copy preference for
 *     r0 and r0 is still free when its turn comes.  The accumulator is
 *     allocated afterwards, finds r0 taken, and settles for r2.
 *   * So the pass that decides this is the SORT, not the search.  Flipping it
 *     needs the accumulator's priority to beat 8/16, i.e.
 *     floor_log2 (n) * n > 13 at live_length 26, i.e. SEVEN references on the
 *     accumulator -- or at most THREE on the index.
 *
 * MEASURED, batch 330 -- twenty-eight bodies, none of which moves the swap:
 *   * the index given three references: impossible without losing an
 *     instruction the ROM has.  Its three uses are the compare, the
 *     register-offset `ldrb` and the register-offset `strb`, all present in the
 *     reference, so four references is its floor.
 *   * the accumulator given more references: eleven spellings -- a redundant
 *     zero store in the negative arm, `r = v` where v is a known zero, a
 *     self-assignment, assigning through the accumulator in both arms, and an
 *     `else` branch that re-zeroes it -- and gcc folds every one of them back
 *     to four references, so the priority never moves.
 *   * splitting the index into a second local, both before and inside the
 *     guard, and with the load and the store taking different copies: gcc
 *     coalesces the copy away in all three placements.
 *   * `goto` to a trailing label instead of the guarded block: identical.
 *   * a block-scoped temporary for the loaded byte: same figure, worse
 *     alignment.
 *   * storing the accumulator instead of the value (they are provably equal at
 *     that point): loses four instructions and breaks the relocation.
 *
 * WHAT WOULD BE NEW.  A lever that makes a pseudo CONFLICT with a hard register
 * it is copied from, which is the only other way global.c:907 prunes a copy
 * preference.  Hard r0 is live only at the entry copy and at the return copy,
 * and the index is dead at both, so nothing in plain C reaches it here.  Do not
 * spend another round on spellings of the arms; the arms are correct.
 */
extern unsigned char ewram_2000380[];

int Func_8078aa0(int i, int d)
{
    int r = 0;
    unsigned char *tb = ewram_2000380;
    int v;

    if (i <= 0x7f) {
        v = tb[i];
        v += d;
        if (v < 0) {
            v = 0;
            r = 0;
        } else if (v > 0x63) {
            v = 0x63;
            r = 0x63;
        } else {
            r = v;
        }
        tb[i] = v;
    }
    return r;
}
