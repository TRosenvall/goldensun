/* Func_800bfa4 -- 0x0800bfa4  (asm/rom_9000/rom_be70_c_c.s)
 *
 * NON-MATCHING, 8 of 44 encodings, NO PINS  (MEASURED, batch 323 brief J).
 * This REPLACES the installed park body, which measures 12 of 44.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/rom_bfa4.c \
 *     asm/rom_9000/rom_be70_c_c.s --func Func_800bfa4
 *
 * (The park's first header line said rom_be70_c.s; the working path is
 * rom_be70_c_c.s, which is what its own recipe used.  Corrected here.)
 *
 * NOT A DUPLICATE PARK.  The brief flagged `rom_bfa4.c` as a source-file-stem
 * name of the shape that hides a second park.  Checked:
 * `find src -name 'rom_bfa4*' -o -name '800bfa4*'` returns this file only, and
 * `grep -rln Func_800bfa4 src/` returns this park, src/rom_9000/exports.s, and
 * src/non_matching/rom_9000/800c62c.c (a CALLER, not a second park).  The 12 was
 * not a two-park race.
 *
 * SPLIT SHAPE, for whenever this lands -- NOTE IT NEEDS A TEXT/DATA SPLIT:
 *   datacheck.py asm/rom_9000/rom_be70_c_c.s FIRES:
 *     data sections : .rodata
 *     EXPORTS       : .L1314c  (already global -- NOT the set a split needs)
 *     Func_800bfa4 reads no data label -> split needs NO new export
 *   split_s.py --dry-run ... Func_800bfa4 ->
 *     rom_be70_c_c_b.s (the function, 50 lines)  -> src/rom_9000/rom_be70_c_c_b.c
 *     rom_be70_c_c_c.s (the .rodata, 6 lines)    -> stays as asm
 *
 * WHAT MOVED, AND IT IS A CROSSING RESULT.  The park recorded two edits as a
 * closed dilemma: "assign cx first -> loads in ROM order, registers swapped,
 * 14" and "assign cy first -> registers closer, loads swapped, 12", concluding
 * "there is no spelling that gets both".  Crossed, they are worth 4:
 *
 *     cam[0] assigned first              14   (WORSE than the park's 12)
 *     dy computed before dx              10
 *     BOTH                                8   <-- this body
 *
 * This is the "a rejected-because-worse edit is HALF of a two-part fix" shape
 * from tools/crossfire.py's own list, in its purest form: one half had been
 * measured and REJECTED for being worse than the base, and the two were never
 * crossed.
 *
 * WHAT THE 8 BOUGHT.  At 12 the residue included the register assignment of
 * both deltas; at 8 the assignment is ROM-EXACT and so is the whole tail:
 *     dx -> r1, dy -> r2, out -> r5, cam[0] -> r1, mask -> r4
 * and indices 6, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 and everything
 * after now agree, including `add r3, r1, r0`, both `cmp r2, ...` and both
 * `asr r3, rN, #16`.  The prologue is `push {r5, lr}` as the ROM has it.
 *
 * WHAT REMAINS is eight contiguous encodings, indices 7..14, and it reduces to
 * ONE local-alloc decision -- cam[1]'s register:
 *
 *     rom    ldr r2, [r3, #4]   cam[1] -> r2 ; then o[2] and o[4] both -> r3
 *     ours   ldr r3, [r3, #4]   cam[1] -> r3 ; then o[4] -> r2, o[2] -> r3
 *
 * VERIFIED IN THE DUMP, not asserted: `.17.lreg` for this body prints
 *   ;; Register 36 in 1.   (cam[0])
 *   ;; Register 37 in 3.   (cam[1])     <-- the ROM wants 2
 *   ;; Register 43 in 2.   (o[4])
 *   ;; Register 45 in 3.   (o[2])
 * and `.18.greg` gives `;; 3 regs to allocate: 39 33 38` with 38 (dx) in r1 and
 * 39 (dy) in r2, i.e. the GLOBAL half is already right.
 *
 * AND THE EIGHT ARE NOT EIGHT DECISIONS, THEY ARE ONE.  Because cam[1] shares
 * r3 with o[2], the mask-and-subtract of cam[1] must precede `ldr r3, [r0,#8]`
 * -- a real anti-dependence, visible in .23.sched2's dependence table -- so the
 * dy chain is FORCED ahead of the dx chain, which is the other six differences.
 * In the ROM cam[1] is in r2, nothing blocks r3, and the dx chain goes first.
 * Fix cam[1]'s register and the chain order follows; nothing else is wrong.
 *
 * Why cam[1] takes r3: it is a local-alloc quantity born at the insn where the
 * cam pointer (also r3) dies, and death/birth are separate slots
 * (local-alloc.c:1415, two per insn), so r3 is free and is REG_ALLOC_ORDER's
 * head (arm.h:989 -- `3, 2, 1, 0, ...`).  For r2 to win, o[2]'s quantity has to
 * be allocated BEFORE cam[1]'s, and by QTY_CMP_PRI
 * (floor_log2(n_refs) * n_refs * size / (death - birth)) it is not close:
 * cam[1] has four references (load, and-use, and-set, minus-use) against o[2]'s
 * two, over a comparable span.  Every one of those four references is an
 * emitted instruction, so the reference count cannot be lowered from source --
 * the same wall as HeightTile_A's, and the reason this stops at 8.
 *
 * MEASURED THIS ROUND.  Two crossfire.py runs (depth 2, 8 edits each, 37
 * subsets each) plus a ten-variant sweep; ref 44 encodings and ours 44
 * everywhere except where noted, so these are distances:
 *
 *   EXACTLY INERT at 8 (candidate prerequisites, all of them):
 *     a dead `cam = 0` after the second camera read
 *     `cx = cam[0] & mask; cy = cam[1] & mask;` (premask at the read)
 *     `dy = -(cy & mask) + o[4]` (operand order on the subtraction)
 *     `mask` declared after `cam` instead of before
 *     a brace-only reshape of the guard
 *     an extra `*out = *out;` ahead of everything
 *     named `cxm` / `cym` locals for the masked values
 *   WORSE, with figures:
 *     dx chain written first (ROM's order)                   14
 *     cam[1] read first again                                14
 *     `cx = cam[0] & mask` with dy first                     25  (RELOCDIFF, -4 bytes)
 *     `cam[0]`/`cam[1]` read through `*cam++`                43  (RELOCDIFF, +4 bytes)
 *     accumulator form `dx = o[2]; dx -= cx & mask;`         17
 *     named `ox`/`oy` for the two object reads               23
 *     the guard inverted with the reject arm first           17
 *     `out[0] =` / `out[1] =` instead of `*out++ =`          10  (MEM: the ROM's
 *         two stores become one stmia pair plus a str; flagged, not believed)
 *   CROSSED, the useful rows:
 *     cam[0]-first x dy-first                                 8  <-- this body
 *     cam[0]-first x any of the seven inert edits            14  (the inert
 *         edits do not unlock the dx-chain-first form)
 * Nothing below 8 was found in 84 measured subsets.
 */
extern int iwram_3001e70;

int Func_800bfa4(int *o, int *out)
{
    int *cam;
    int mask;
    int cx;
    int cy;
    int dx;
    int dy;

    cam = (int *)(iwram_3001e70 + 0xe4);
    mask = 0xffff0000;
    cx = cam[0];
    cy = cam[1];
    dy = o[4] - (cy & mask);
    dx = o[2] - (cx & mask);
    if (dx > -0x200000 && dx < 0x1100000 && dy > 0 && dy < 0xe00000) {
        *out++ = dx >> 16;
        *out = dy >> 16;
        return 0;
    }
    *out++ = 0;
    *out = 0;
    return -1;
}
