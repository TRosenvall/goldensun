/* Func_80c1054 -- 0x080c1054, split out of asm/rom_b5000/rom_bffb8_c_c_a.s.
 *
 * MATCHING -- 48 bytes, 22 encodings, 2 relocations identical.
 * The park read 17 of 22; the figure was MISALIGNMENT, not distance.
 *
 * Verify with (BEFORE the split -- runnable as the tree stands now):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b323/B/p3_candidate.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a.s --func Func_80c1054
 *
 * and AFTER the split, against the INSTALLED path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_bffb8_c_c_a_b.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a_b.s --func Func_80c1054
 *
 * SPLIT SHAPE.  tools/datacheck.py on rom_bffb8_c_c_a.s is SILENT (no data
 * section), and tools/split_s.py --dry-run reports:
 *     would write asm/rom_b5000/rom_bffb8_c_c_a_a.s  (3 functions, 195 lines)
 *     would write asm/rom_b5000/rom_bffb8_c_c_a_b.s  (1 function,   29 lines)
 *     would REMOVE asm/rom_b5000/rom_bffb8_c_c_a.s ; would rewrite stage1.ld
 * Func_80c1054 is the LAST of four, so there is no _c part.
 * Pins: 0.  No exports needed.  No per-file flag group.
 *
 * WHAT THE PARK GOT WRONG, AND WHERE IT ALREADY HELD THE ANSWER.
 *
 * The park diagnosed "THE COUNT IS SAVED BEFORE THE TEST, plus the addressing-
 * base choice that follows from it", and reported `19 differing` with the
 * streams "three lines short".  The observation was right and the verdict was
 * not: the ROM is 22 instructions and the park's body 21, so its positional
 * figure measured the one-instruction shift, not a distance.  Every index from
 * the call onward was identical text at a shifted position.
 *
 * The park then named its own fix and walked past it.  It says Func_808c2dc
 * "has the SAME shape -- a count from a call, tested, then a walk -- and fails
 * the same way at 19 of 22.  One fix would take both."  Func_808c2dc is no
 * longer a park: it landed EXACT in src/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_b.c,
 * and its header is the recipe:
 *
 *     "THE NEGATIVE IS THE INTERESTING HALF ... the honest do-while over
 *      `unsigned char *p` with `--n` measures 18 differing of 21 and is four
 *      bytes short.  The pointer in the ROM is strength_reduce's, not the
 *      source's.  The source indexes; loop optimisation turns the index into
 *      the pointer and the trip count into a countdown, and writing the OUTPUT
 *      of that pass back into C hands the optimiser something it cannot
 *      re-derive."
 *
 * That is exactly what the park's body did -- a `do/while` over an explicit
 * `off` byte offset with `n--` -- and it is why it came out a line short.  An
 * ascending indexed `for` matched on the first candidate.
 *
 * SO EVERY FEATURE THE PARK CALLED A BLOCKER IS A PASS OUTPUT:
 *   - `mov r7, r5` (the buffer copy the park said gcc "never needs") is the
 *     giv's base register, born in the loop preheader INSIDE the guard;
 *   - `mov r5, r0` after the `ble` is check_dbra_loop's trip counter, which is
 *     why the count is committed to a callee-saved register only inside the
 *     guarded block and the test is on the raw r0;
 *   - `ldrsh r0, [r6, r7]` with OFFSET as the addressing base -- which the park
 *     tried and failed to flip by writing `off + (char *)base` -- falls out of
 *     strength reduction, so nothing in the source has to ask for it.
 *
 * This is the batch-322 rule paying again: a landed header records what the TU
 * actually wanted; a park records what somebody could not make work.
 */
extern int Func_80b6c08(int kind, void *buf);
extern void Func_80c0f98(int id, int flag);

void Func_80c1054(void)
{
    short buf[0xe];
    int n;
    int i;

    n = Func_80b6c08(3, buf);
    for (i = 0; i < n; i++)
        Func_80c0f98(buf[i], 0);
}
