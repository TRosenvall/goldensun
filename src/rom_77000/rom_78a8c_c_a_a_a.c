/* Func_8078aa0  [rom_77000]  --  MATCHING, byte-identical, PIN-FREE.
 *
 * LANDS.  0 differing encodings of 23; size 48 bytes against 48; 21
 * instructions against 21; 1 relocation, identical.  MEASURED batch 340 brief
 * C with tools/objcmp.py at production flags, BOTH --func AND --whole (the
 * reference .s holds this function alone, so --whole is meaningful and agrees).
 * PINS 0, shims 0, devices 0 -- tools/shimcount.py reports nothing.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_78a8c_c_a_a_a.c \
 *     asm/rom_77000/rom_78a8c_c_a_a_a.s --func Func_8078aa0
 *
 * SPLIT SHAPE: none.  tools/datacheck.py is clean on the reference and it holds
 * exactly one .thumb_func_start, so no split_s.py step applies.
 *
 * =================== WHAT THE PARK HAD, AND WHAT IT MISSED ===================
 *
 * The park read 17 of 23 and its reading of the RESIDUE was exactly right: the
 * ROM keeps the return accumulator in r0 for the whole function and copies the
 * index parameter out of r0 into r2 (`adds r2, r0, #0`), while the parked body
 * did the reverse and paid one `adds r0, r2, #0` at the tail.  Its mechanism was
 * right too -- `allocno_compare` (global.c:597-620) sorts allocnos by
 *
 *     (double) (floor_log2 (n_refs) * n_refs) / live_length * 10000 * size
 *
 * descending, ties by allocno number, and the .17.lreg inputs it quotes are
 * index 4 refs / 16 insns against accumulator 4 refs / 26 insns.
 *
 * ITS ERROR WAS WHICH TERM TO MOVE.  With n_refs EQUAL at 4 and size EQUAL at 1,
 * that comparison reduces to LIVE LENGTH ALONE: 16 beats 26, so the index sorts
 * first, takes r0 off its incoming-argument copy preference
 * (global.c:1067-1090), and the accumulator settles for r2.  The park spent all
 * twenty-eight of its measured bodies on the n_refs term -- eleven trying to
 * give the accumulator seven references, which gcc folds back to four every
 * time, and the rest proving the index cannot drop to three without losing an
 * instruction the ROM has.  Both of those findings stand.  Neither is the lever.
 *
 * THE LEVER IS ONE DELETED INITIALISER.  `int r = 0;` at the top of the function
 * makes the accumulator live across the guard, which is what costs it the
 * twenty-six-insn range.  Remove it -- let the guard path return a literal and
 * let every arm of the clamp define `r` -- and the accumulator's range collapses
 * to the arms, its priority beats the index's, it is allocated first, and its
 * own copy preference for r0 (the return copy) hands it r0.  Byte-identical.
 *
 * THE PARK CONFLATED TWO EDITS, AND THE CONTROL IS RECORDED.  Its header credits
 * "SINGLE EXIT" with closing the length gap and warns that the early-return form
 * "makes gcc materialise zero TWICE ... and pay a `b` to the epilogue".  That is
 * true of the form it measured and ONLY because of the initialiser:
 *     int r = 0; ... if (i > 0x7f) return r;   -- 24 of 23, relocations dirty,
 *                                                 four bytes long: the park's own
 *                                                 dead predecessor figure, re-
 *                                                 measured in batch 340
 *     int r = 0; ... if (i > 0x7f) return 0;   -- 24 of 23, same
 *     int r;     ... if (i > 0x7f) return 0;   -- ZERO
 * So single exit was never the requirement; dropping the initialiser is.
 *
 * FOURTEEN SPELLINGS REACH ZERO, which is worth knowing because it says the
 * match is not balanced on a detail.  All four declaration orders of
 * {tb, v, r} x three guard spellings (`return 0;`, a braced `return 0;`, and
 * `r = 0; return r;`) are byte-identical, and so are two SINGLE-EXIT bodies that
 * keep `int r = 0;` but give the clamp arms their own accumulator and copy it out
 * at the end (`r = w;`).  Declaration order is INERT here -- all six
 * permutations tried read zero -- so this function is not a slot-order specimen.
 *
 * ALSO MEASURED IN BATCH 340, pin-free, and all WORSE or inert:
 *   a named pointer for the element, `q = &tb[i]` then `*q`      11 of 23
 *   the same with `q = tb + i`                                   12 of 23
 *   the pointer formed outside the guard                         11 of 23
 *   a pointer for the STORE only                                 17 of 23, inert
 *   an explicit `else { r = 0; }` with `int r;`                   15 of 23, dirty
 *   one assignment `r = v;` after the clamp instead of per arm    20 of 23, short
 *   the table base taken inside the guard                        18 of 23
 *   `i = 0;` after the store (batch 336's shift-in-place lever)   17 of 23, inert
 *   BOTH CHEAP AXES CLOSED on the parked body by tools/autosweep.py: all eight
 *   per-file flag groups and all ten anchor placements read 17 of 23.
 *
 * The pointer rows are the interesting negatives: they DO take the index's
 * reference count from four to two, which is what the park said was
 * unreachable, and the allocation still does not flip -- the pointer's own
 * allocno takes the register instead.  So n_refs on the index is now an open
 * axis that is simply the wrong one.
 */
extern unsigned char ewram_2000380[];

int Func_8078aa0(int i, int d)
{
    unsigned char *tb = ewram_2000380;
    int v;
    int r;

    if (i > 0x7f)
        return 0;
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
    return r;
}
