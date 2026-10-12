/* OvlFunc_897_200a84c -- *** MATCHES BYTE-FOR-BYTE AT PRODUCTION FLAGS. ***
 *   OK OvlFunc_897_200a84c -- 144 bytes, 64 encodings and 8 relocations identical
 *
 * NO FLAG GROUP, NO PINS, NO DEVICE.  tools/shimcount.py reports no rows, so
 * there is nothing for fakematch.txt.
 *
 * PARK FIGURE RE-DERIVED AT THE HEAD OF BATCH 340, not inherited:
 *   16 differing STREAM encodings of 64; instructions 55/55 -- IN PHASE, so a
 *   true DISTANCE -- and objcmp reports RELOCATIONS IDENTICAL (8 of 8).  The
 *   recovered body's header said "16 of 64 ... 55/55 ... IN PHASE" and that
 *   survived verification exactly.
 *
 * THE FIX IS ONE DECLARATION PER ARM.  The park carried ONE `one` used in both
 * arms of the if; each arm gets its own carrier instead.  Three spellings of
 * that all measure byte-identical (0 of 64):
 *   - a second function-scope int used only in the else arm   (p4 as shipped is
 *     the block-scoped form, which is the same program)
 *   - `int one = 1;` declared INSIDE each arm                 *** shipped ***
 *   - the same with the declarations in the opposite order
 *
 * THE MECHANISM, and the park's diagnosis was RIGHT ABOUT THE SWAP AND WRONG
 * ABOUT THE ALLOCATOR.  The park wrote "BLOCKER: REGISTER ALLOCATION -- a clean
 * r5/r6 swap", measured three declaration-order spellings at 16 each, and
 * concluded "Nothing in the source reaches the allocator's priority order for
 * two values with these live ranges."  The observation is exact; the verdict
 * named the wrong allocator.  gcc's own `-da` dumps say so:
 *
 *   .17.lreg   Register 33 used 13 times across 172 insns; set 2 times;
 *                crosses 5 calls          <- `one`, live in blocks 1 AND 2
 *              Register 34 used 3 times across 18 insns IN BLOCK 1;
 *                crosses 1 call           <- `two`, live in ONE block
 *   .18.greg   ;; Register 34 in 5.       <- LOCAL-ALLOC already took r5
 *              ;; 1 regs to allocate: 33  <- global-alloc only ever sees `one`
 *              32 in 0  33 in 6  34 in 5
 *
 * So `two` is allocated by LOCAL-ALLOC, which runs first and hands out r5, and
 * global-alloc is left with r6 for `one`.  No priority lever inside global.c
 * can reach that, which is exactly why all three declaration-order spellings
 * the park tried measured 16: they were all arguing with the wrong pass.
 * local-alloc owns a pseudo whose references lie in ONE basic block
 * (local-alloc.c:1341-1420; the gate is REG_BASIC_BLOCK (regno) >= 0).
 *
 * Splitting `one` per arm makes BOTH carriers block-local, so local-alloc
 * allocates both, in its own order, and the ROM's r5/r6 assignment falls out:
 * r5 to the first-used carrier in each arm, r6 to `two`.  This is batch 338's
 * finding ("a value whose uses lie in ONE basic block is allocated by
 * local-alloc, so SPLITTING A REUSED LOCAL can reassign a callee-saved register
 * no priority lever can reach", OvlFunc_946_200add0 13 -> 0) applied in the
 * OPPOSITE DIRECTION: there a value was split OUT of global-alloc to change its
 * register, here a value is split out so that local-alloc gets BOTH and the
 * order of two global/local allocations stops mattering.
 *
 * The park's other two readings both held and are kept: the else arm's single
 * `2` really is a plain literal in the ROM (`mov r3, #2 / str r3, [sp]`), not
 * the carried r6, and that is reproduced; and `one` really is assigned in both
 * arms rather than hoisted -- hoisting it above the `if` measures 46 of 64 with
 * RELOCATIONS DIFFERING and -4 bytes, a different program shape, not a near miss.
 *
 * MEASURED, all at production flags (base = 16 of 64, 55/55 instructions):
 *   a separate carrier per arm                       *** 0, byte-identical ***
 *   `int one = 1;` block-scoped inside each arm       *** 0 ***  (shipped)
 *   the two per-arm carriers declared in the other order  *** 0 ***
 *   `two` carried into the else arm as well           6 of 64
 *   `two = 2;` moved above `one = 1;` in the then arm  16  (inert)
 *   `one = 1;` hoisted above the whole if             46 of 64, -4 bytes,
 *                                                     RELOCATIONS DIFFER
 * AUTOSWEPT on the parked body (tools/autosweep.py, batch 340): both cheap axes
 *   CLOSED.  8 flag groups -- -O1 44, -fno-schedule-insns2 44, the other six 16;
 *   12 anchor placements, all inert at 16.  Neither was needed.
 *
 * LANDING SHAPE -- nothing to do.  tools/piece.py: the reference holds ONE
 * function, this one.  tools/datacheck.py prints nothing (no data sections).
 * tools/split_s.py --dry-run says it outright:
 *     "asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.s holds only
 *      OvlFunc_897_200a84c and no data; convert it directly, no split needed"
 * So this converts straight to src/overlays/rom_791794/ with no `.global`
 * exports, no split and no Makefile flag rule.
 *
 * Verify with (INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.s --whole
 *
 * Verify NOW, from this scratch path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b340/B/p4_candidate.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.s --func OvlFunc_897_200a84c
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

void OvlFunc_897_200a84c(int f)
{
    if (f != 0) {
        int one = 1;
        int two;

        __CopyMapTiles(8, 0x2f, 0x40, 7, one, one);
        two = 2;
        __CopyMapTiles(7, 0x30, 0x3f, 8, two, one);
        __CopyMapTiles(7, 0x31, 0x3f, 9, two, one);
    } else {
        int one = 1;

        __CopyMapTiles(0x38, 0, 0x40, 7, one, one);
        __CopyMapTiles(0x38, 0, 0x3f, 8, one, one);
        __CopyMapTiles(0x38, 0, 0x3f, 9, 2, one);
        __CopyMapTiles(0x3a, 0x19, 0x40, 8, one, one);
    }
    __Func_800fe9c();
}
