/* Func_801fd34 (StepOverlayAnimation)  --  0x0801fd34   [rom_15000]
 *
 * *** LANDING.  BYTE-IDENTICAL.  PIN-FREE, NO INLINE ASM, NO REGISTER PIN.
 * WAS PARKED AT 10 of 36. ***
 *   `objcmp --func`: `OK Func_801fd34 -- 80 bytes, 36 encodings and 2
 *   relocations identical`.  `--whole` agrees: `OK whole file -- 80 bytes,
 *   36 encodings and 2 relocations identical`.  32 instructions + 4 pool words
 *   on both sides.  `tools/shimcount.py`: no rows.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.c \
 *     asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.s --whole
 *
 * SPLIT SHAPE: none.  `tools/piece.py` prints
 *   asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.s -- 1 function(s)
 * and `tools/datacheck.py` on that reference exits 0 silently.  Install path
 * src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.c, no `split_s.py` step, no
 * Makefile flag group (generic `asm/%.o: src/%.c`).
 *
 * ===========================================================================
 * WHAT CLOSED IT: *** ONE `do { } while (0)` AROUND THE PACK-AND-STORE. ***
 * ===========================================================================
 *
 * The park's residue was ten encodings in two groups, both inside the one basic
 * block that is the loop body:
 *
 *   idx 16-22  a seven-encoding PERMUTATION of the same seven insns
 *              ROM:  lsl r1,r3,#1 / mov r2,r3 / add r1,#22 / add r2,#16 /
 *                    add r3,#20 / lsl r3,#10 / lsl r2,#5
 *              ours: mov r2,r3 / lsl r1,r3,#1 / add r2,#16 / add r3,#20 /
 *                    lsl r3,#10 / lsl r2,#5 / add r1,#22
 *   idx 25-27  ROM:  strh / add r6,#2 / add r5,#1
 *              ours: add r5,#1 / strh / add r6,#2
 *
 * THE PARK'S ARITHMETIC WAS ENTIRELY CORRECT AND ITS VERDICT WAS WRONG, and the
 * verdict is worth quoting because it is the costly kind -- a BOUND:
 *   "The ROM's block 3 therefore cannot be the output of a priority-ordered list
 *    schedule at all, whatever the expression is spelled like ... Both standing
 *    routes are closed ... no amount of further term respelling can reach it
 *    either ... WHAT IS LEFT, honestly stated: nothing source-reachable that
 *    this batch can name."
 * Every number behind that is right: `add r5,#1` feeds `cbranchsi4` so its
 * priority cannot fall below 2, `strh`'s cannot rise above 1, and the ROM puts
 * the former BELOW the latter.  What the park then concluded -- that nothing in
 * C can reach it -- skipped the third regime.  THE TWO REGIMES THE PARK
 * ENUMERATED ("the two regimes available from C are exactly those two", chain
 * order and priority order) ARE NOT THE ONLY TWO: a **scheduling anchor** gives
 * priority order *within each span it delimits*, and that is a third regime,
 * reachable from C and already documented in this tree.
 *
 * docs/elevation.md, "`do { } while (0)` IS A SCHEDULING BARRIER", states the
 * mechanism and even states this exact diagnostic:
 *   >> "If a sched2 residue is NOT two adjacent independent instructions but a
 *   >>  FIXED PRIORITY GAP, look for a missing barrier -- a `do/while(0)` macro
 *   >>  -- not for a source-order swap."
 * The park had *proved* the fixed priority gap and then looked for a
 * source-order swap anyway.  That is the lesson to carry: **a proof that no
 * ordering of the existing statements works is a proof that an anchor is
 * needed, not a proof that the park is closed.**
 *
 * THE MECHANISM, from the compiler, as elevation.md records it: in
 * `sched_analyze_insn` a `NOTE_INSN_LOOP_BEG` / `LOOP_END` collected before an
 * insn fires `schedule_barrier_found` (`haifa-sched.c:3714`, arming notes at
 * `:3727-3731`), and the insn then takes `REG_DEP_ANTI` on every
 * `reg_last_uses[i]` (`:3744`) and a **true** dependence on every
 * `reg_last_sets[i]` (`:3748`) and `reg_last_clobbers[i]` (`:3751`); `:3754`
 * sets `reg_pending_sets_all` and `:3756` flushes the memory lists.  A
 * `do { } while (0)` therefore totally orders the block at that point and emits
 * nothing -- confirmed here: 32 instructions against 32, 80 bytes against 80.
 *
 * ---------------------------------------------------------------------------
 * THE EDIT, AND WHY IT IS PLAUSIBLE SOURCE RATHER THAN AN ARTIFACT
 * ---------------------------------------------------------------------------
 *
 *     c = t + 0x14;
 *     do {
 *         c <<= 10;
 *         b <<= 5;
 *         *p = c | b | a;
 *     } while (0);
 *     p++;
 *
 * Two things had to change together and neither works alone:
 *   1. the two shifts must be their own STATEMENTS (`c <<= 10; b <<= 5;`)
 *      instead of living inside `(c << 10) | (b << 5) | a`, or there is no
 *      point between them for the anchor to sit;
 *   2. the anchor must span statements 4..6 inclusive -- the two shifts AND the
 *      store.  Its position is the whole lever, exactly as elevation.md's
 *      batch-329 amendment says ("the anchor has a POSITION, and the position
 *      is the lever").
 * `do { } while (0)` around a shift-pack-and-store triple is what this tree's
 * own `SET_IO` / `SET_PALETTE` macros look like (elevation.md names them), so
 * this reads as a macro invocation in the original source rather than as a
 * matching artifact.  It is nonetheless worth a pass-3 look for the macro's
 * real name and body.
 *
 * ---------------------------------------------------------------------------
 * MEASURED THIS BATCH -- 2,578 bodies, and the table is the finding
 * ---------------------------------------------------------------------------
 * Every row is ref 36 encodings / 32 instructions, ours the same unless noted,
 * so every figure below is a distance.
 *
 *   ** 0 **  ONE `do { } while (0)` around `c <<= 10; b <<= 5; *p = ...;`
 *       0    the same anchor extended over `p++` as well  (`dw_4-8`)
 *       0    two anchors, 69 further placements (all redundant)
 *       0    three bare `__asm__ volatile ("")` barriers, several placements
 *       2    one anchor around statements 3..6 or 3..7
 *       2    two bare `__asm__ volatile ("")` barriers, best of 845 crossings
 *       3    one bare barrier, best of 176 placements
 *       9    NO anchor and NO barrier, best of 384 crossed term spellings
 *            (a-term x b-term x c-term x all six statement orders) -- so the
 *            BARRIER-FREE figure is **9**, not the park's 10: splitting only
 *            `b <<= 5;` out of the OR is worth one encoding on its own.
 *      10    the park's shipped body
 *
 * SO THE RESIDUE DECOMPOSES EXACTLY: splitting the shifts into statements is
 * worth 1, the anchor is worth the other 9, and the anchor needs the split to
 * have somewhere to go.  Neither half is visible on its own -- the park measured
 * "60 legal permutations ... 32 crossed variants, then 24 more" of the FIRST
 * dimension with the second absent, and correctly reported them all flat at 10.
 *
 * ALSO CORRECTED: the park's `-fno-schedule-insns2` instrument reads 4 and was
 * used to argue "the ROM needs sched2 to split the reload pair and not to sort
 * the rest, and no flag offers that".  True, and irrelevant now -- the anchor
 * gives precisely that, because sched2 still runs on both sides of it.  This
 * file must still NOT be proposed for SCHED2_CFLAGS; that part of the park's
 * ruling stands and is now moot.
 *
 * WHAT WAS INERT HERE AND IS KEPT FROM THE PARK, all three still reproducing:
 * `g = &iwram_3001800;` as the first statement (this is what fixed the pool-word
 * order and the relocations, park's cause A); the signed divide `/ 0x4000` as
 * the idiom; and the `<< 10` term written FIRST in the OR, since gcc's OR
 * accumulator is the first operand and the ROM's two `orr` encodings
 * (`4313 orr r3,r2`, `430b orr r3,r1`) pin it.
 */
extern int iwram_3001800;
extern int sin(int a);

void Func_801fd34(void)
{
    unsigned short *p;
    int *g;
    int i;
    int t, a, b, c;

    g = &iwram_3001800;
    p = (unsigned short *)0x50001d0;
    i = 0;
    do {
        t = sin((*g + i * 8) * 3 << 8) / 0x4000;
        a = t * 2 + 0x16;
        b = t + 0x10;
        c = t + 0x14;
        do {
            c <<= 10;
            b <<= 5;
            *p = c | b | a;
        } while (0);
        p++;
        i++;
    } while (i <= 3);
}
