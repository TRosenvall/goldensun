/* Func_80b2720 -- asm/rom_b0000/rom_b0070_c_a_a_c.s
 *
 * MATCHING, 0 of 32 encodings  (MEASURED, batch 342 brief A)
 *   *** REQUIRES GCSE_CFLAGS (-fno-gcse).  See "THE FLAG IS NOT OPTIONAL". ***
 *   Instruction counts EQUAL at 30/30; relocations ok; size 68/68.
 *   The park's figure at the head of this batch was 23 of 32 with instructions
 *   30/30, i.e. the ONE TRUE DISTANCE of brief A's four targets.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-gcse goldensun-build python3 tools/objcmp.py \
 *     src/rom_b0000/rom_b0070_c_a_a_c.c \
 *     asm/rom_b0000/rom_b0070_c_a_a_c.s --func Func_80b2720
 *
 * PIECE: tools/piece.py reports ONE function in rom_b0070_c_a_a_c.s, so this is
 * a WHOLE-FILE conversion -- no tools/split_s.py cut is needed.
 * EXPORTS: none.  `.Lb41ac` is ALREADY `.global` at
 * asm/rom_b0000/rom_b0070_c_c_c_c_dat.s:10, and two landed files in the same
 * bank (src/rom_b0000/rom_b0070_c_a_a_b.c, rom_b0070_c_a_b.c) already spell it
 * with the same `__asm__(".Lb41ac")` extern.
 * PINS: tools/shimcount.py reports 0.  Device-free body.
 *
 * WHAT THE RESIDUE WAS.  The park said "SCRATCH-REGISTER SELECTION plus a
 * peeled first load" and "NEXT: nothing source-level".  The first half is
 * right, the second is wrong, and the two halves are TWO SEPARATE LEVERS that
 * are each INERT OR WORSE ALONE:
 *
 *   -fno-gcse alone, park body untouched         23 -> 11 of 32
 *   the statement order alone, at -O2            23 -> 30 of 32   (WORSE)
 *   the two together                             0
 *
 * LEVER 1 -- `-fno-gcse`, straight off tools/autosweep.py's flag axis with no
 * hypothesis applied.  At -O2 gcse hoists the loop's first `ldrh` above the
 * loop and then has to rebuild the halfword sentinel test inside it as
 * `ldrh / add r2, r3, #0 / lsl r3, r2, #16 / cmp r3, #0` -- four instructions
 * where the ROM has `mov r1, #0 / ldrsh r3, [r2, r1] / cmp r3, #0`.  No source
 * spelling defeats that hoist: naming the loaded value (the park's own
 * measured negative), an anchor in the loop, an anchor before the guard, a
 * separately named `short` sentinel, `p[0]`, and an offset-first cast ALL read
 * 29-30 of 32 at -O2 on top of lever 2.  The flag is the only route.
 *
 * LEVER 2 -- `n = 0;` MUST SIT ABOVE THE STATEMENT THAT CONSUMES `id`, and the
 * reason is a HARD-REGISTER CONFLICT THAT PRUNES A PREFERENCE.  At 11 of 32 the
 * only residue was a two-way register swap plus the one instruction it costs:
 *
 *     rom   n in r4, d in r0, and a closing `mov r0, r4`   (30 insns)
 *     ours  n in r0, d in r4, and no closing copy          (29 insns)
 *
 * `.18.greg` prints the whole decision, and the two dumps differ in exactly
 * three lines (scratch_elev/b342/A/d_b2720/ vs d_b2720_land/):
 *
 *     park     ;; 38 conflicts: 33 34 35 36 38 43 45 47 49 52 54 2 3 13
 *              ;; 38 preferences: 0
 *              32 in 0  ...  36 in 4  38 in 0
 *     landing  ;; 38 conflicts: ... 0 2 3 13          <- HARD REG 0 APPEARS
 *              (no `38 preferences:` line at all)     <- THE PREFERENCE IS GONE
 *              32 in 0  ...  36 in 0  38 in 4
 *
 * Allocno 38 is `n`, 36 is `d`.  The chain, with citations:
 *   1. `return n` expands to `(set (reg:SI 0) (reg:SI 38))`, and
 *      set_preference (global.c:1630-1642) gives 38 a preference for r0.
 *   2. prune_preferences ORs an allocno's hard_reg_conflicts into `temp` and
 *      then does `AND_COMPL_HARD_REG_SET (allocno[num].hard_reg_preferences,
 *      temp)` (global.c:897-909).  So A HARD CONFLICT WITH r0 DELETES THE
 *      PREFERENCE FOR r0.
 *   3. Moving `n = 0;` above `off = (id * 32 + id) * 2;` makes `n` live across
 *      the insns that read the incoming `id` in hard reg r0, which is what puts
 *      r0 into 38's hard_reg_conflicts in the first place.
 *   4. find_reg's pass 0 does `IOR_HARD_REG_SET (used,
 *      allocno[num].regs_someone_prefers)` (global.c:1016), and
 *      regs_someone_prefers is the union of the full preferences of
 *      LOWER-PRIORITY CONFLICTING allocnos (global.c:912-944).  With 38's
 *      preference pruned, r0 leaves 36's `used`, and REG_ALLOC_ORDER
 *      (arm.h:989) is `3, 2, 1, 0, 12, 14, 4, 5, ...` -- r3 is a hard conflict,
 *      r2 is taken by allocno 35, r1 is preferred by allocno 33 (`dst`), so 36
 *      takes r0.
 *   5. `n` then falls to r4 and gcc must emit the ROM's `mov r0, r4`.
 *
 * SO THIS IS A FOURTH ROUTE INTO THE ALLOCATOR, and it is not any of the three
 * the brief lists (block-local via local-alloc; shorten live_length; split the
 * range so the value leaves the race).  All three of those act on
 * allocno_compare's PRIORITY.  This one does not touch the priority order at
 * all -- `;; 6 regs to allocate: 35 36 38 43 34 33` is IDENTICAL in both dumps.
 * It acts on find_reg's PASS-0 EXCLUSION SET by deleting a competitor's
 * preference, and the handle is LENGTHENING a live range to overlap an
 * incoming-parameter hard register -- the opposite of every priority lever.
 * Measured: `base, off, n` reads 11 and `base, n, off` reads 0, so the
 * requirement is precisely "before the `id` use", not "first in the function".
 * Every priority lever WAS tried on top of -fno-gcse and all were inert at 11:
 * a second name for the returned value (before and after the terminator
 * store), an extra reference on `d`, `d` defined after `p`, a redundant
 * `n = 0` inside the guard, `m = n + 1; n = m;` (the batch-340 split), a
 * `while (1)` loop, a goto loop, dropping `d` for `dst[n]`, and anchors at
 * five boundaries.
 *
 * AUTOSWEEP (batch 342): flag axis -- `-fno-gcse` 11, `-O1` 18, everything else
 * inert at 23; anchor axis -- 14 of 14 boundaries, best 22.  BOTH AXES PAID
 * HERE, which the tool's own docstring says has never happened on the flag
 * axis; the six parks that rate was measured on had all been hand-swept first,
 * and this one was a batch-319 recipe backfill nobody had ever swept.
 */
extern unsigned char Lb41ac[] __asm__(".Lb41ac");

int Func_80b2720(int id, unsigned short *dst)
{
    unsigned char *base;
    unsigned short *p;
    unsigned short *d;
    int off;
    int n;

    n = 0;
    off = (id * 32 + id) * 2;
    base = Lb41ac;
    if (*(short *)(base + off) != 0) {
        d = dst;
        p = (unsigned short *)(off + (int)base);
        do {
            *d = *p;
            n++;
            p++;
            d++;
            if (n > 0x17)
                break;
        } while (*(short *)p != 0);
    }
    *(unsigned short *)(n * 2 + (int)dst) = 0;
    return n;
}
