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
 *
 * ---------------------------------------------------------------------------
 * BATCH 325 BRIEF D -- REOPENED AGAINST THE RELOAD-CURSOR READING. PARK HOLDS
 * AT 8 of 44.  Re-derived: objcmp --func 8 of 44 (ref 44, ours 44), first at
 * index 7, indices 7..14 contiguous, relocations identical, MEM ldr=9 str=2.
 * (`--whole` prints SIZE ref 164 / ours 96 because the .s still carries the
 * .rodata the split above would separate; the --func figure is the distance.)
 *
 * *** VERDICT: GENUINE ALLOCATION ORDER (local-alloc), NOT A RELOAD CURSOR. ***
 * `.18.greg` does print three reloads -- `Using reg 0` twice and `Using reg 3`
 * once, for insns 46/49/53 -- but those are the guard's pooled constants and
 * they are already byte-exact; none of them is in indices 7..14.  And the two
 * lines are `find_reg`'s (reload1.c:1664) and `find_reload_regs`' (:1729), not
 * `allocate_reload_reg`'s, which prints nothing; `choose_reload_regs_init`
 * (:5129) confines the round-robin cursor to `chain->used_spill_regs`, so on a
 * one-reload insn the cursor has no freedom.  See
 * src/non_matching/rom_15000/801f730.c for that reading in full.
 *
 * THE PARK'S OBSERVATION REPRODUCES EXACTLY.  `.17.lreg` for this body:
 *   ;; Register 36 in 1.  (cx = cam[0])      ;; Register 37 in 3.  (cy = cam[1])
 *   ;; Register 43 in 2.  (o[4])             ;; Register 45 in 3.  (o[2])
 * and `;; 3 regs to allocate: 39 33 38` with dx->r1, dy->r2.  The ROM's map is
 * cx->r1, cy->r2, and BOTH o-reads->r3.  One local-alloc decision, as claimed.
 *
 * *** BUT THE PARK'S "FIX cam[1]'s REGISTER AND THE CHAIN ORDER FOLLOWS;
 *     NOTHING ELSE IS WRONG" IS REFUTED BY MEASUREMENT. ***
 *
 * Reading cam[1] BEFORE cam[0] while keeping the dy chain first gives
 * `;; Register 37 in 2` -- cam[1] in r2, the ROM's register -- AND cx->r1,
 * o[2]->r3, o[4]->r3, dx->r1, dy->r2.  THE WHOLE REGISTER MAP IS THE ROM'S.
 * That body reads 10, not 0, and every one of the 10 is instruction ORDER: both
 * streams hold the same 41 instructions.  So fixing cam[1]'s register does NOT
 * bring the chain order with it.
 *
 *   int *cam; int mask; int cx, cy, dx, dy;
 *   cam = (int *)(iwram_3001e70 + 0xe4); mask = 0xffff0000;
 *   cy = cam[1]; cx = cam[0];
 *   dy = o[4] - (cy & mask); dx = o[2] - (cx & mask);        -> 10 of 44
 *   (scratch_elev/b325/D/v8/v09_dyfirst_cyread1st.c)
 *
 * WHY cam[1]'s REGISTER MOVES, read off the dumps: cam[1]'s qty is born at the
 * insn where the `cam` pointer qty dies, and local-alloc's death/birth are
 * separate slots (local-alloc.c:1415, two per insn), so it can reuse r3 --
 * exactly as the park says.  Reading cam[1] FIRST makes the pointer die at the
 * cam[0] read instead, i.e. AFTER cam[1]'s birth, a real overlap, and r3 is
 * barred.  That is the lever, and it is reachable from ordinary C.
 *
 * WHY THE TWO HALVES COLLIDE -- the mechanism this park now stops on.  Both
 * o-reads land in r3, so the two `ldr r3,[r0,#N]` insns carry a mutual
 * anti-dependence; `.23.sched2`'s dependence table for the fixed-map body shows
 * insn 24 (the cam[1] read) at prio 9 against insn 27's and insn 6's prio 8
 * precisely because the dx chain hangs off the dy chain through that r3 edge.
 * The chain that comes first in RTL therefore owns the longer path, wins the
 * ready list, and goes first -- so THE OUTPUT'S CHAIN ORDER IS THE SOURCE'S
 * CHAIN ORDER, and the sched2 ladder cannot be asked to invert it.
 *
 *   >> NAMED REMAINING CAUSE: the ROM needs the dx chain FIRST *and* cam[1] in
 *      r2, and those two come from opposite source orders.  dx-first requires
 *      cam[0] read first for the map, and cam[0]-first dx-first (v01) gives
 *      cx->r2 / cy->r1 and `push {r5, r6, lr}` -- an extra callee-saved register
 *      the ROM does not push.  Closing this needs a way to bar r3 over cam[1]'s
 *      range WITHOUT lengthening the `cam` pointer's live range, i.e. a third
 *      local-alloc qty of higher priority than cam[1]'s occupying r3 across it.
 *      Nothing in the program supplies one.
 *
 * MEASURED THIS ROUND -- 45 WHOLE BODIES, ref 44 / ours 44 and MEM ldr=9 str=2
 * everywhere except as noted.  NOTHING BELOW 8.
 *   8   this body (base)
 *   8   `cx = cam[0]; dy = o[4] - (cam[1] & mask); dx = o[2] - (cx & mask);`
 *         -- EXACTLY INERT, no `cy` local and NO volatile anywhere: a candidate
 *            prerequisite, and a strictly simpler body at the same figure
 *  10   cam[1] read first, dy chain first        <- THE ROM'S REGISTER MAP
 *  10   the same with either or both values pre-masked at the read
 *  10   the same with `mask = ~0xffff`
 *  10   the same with `mask` declared after `cam`
 *  10   the same with `dx = -(cx & mask) + o[2]`
 *  10   the same with `mask` assigned before `cam`
 *  10   `unsigned int cx, cy`
 *  12   cam[1] read first + dx chain first (three spellings)
 *  13   both values pre-masked + dx chain first
 *  14   dx chain first (the park's row, reproduced)
 *  14   cam[1] read inline late + dx chain first, via a struct
 *  14   masked values assigned into dx/dy then subtracted
 *  15   `mask` as an inline literal at both uses
 *  16   accumulator form with both o-reads hoisted
 *  17   two accumulator forms
 *  19   both camera reads inline in the expressions (3 spellings); all
 *         `push {r5, r6, lr}`
 *  26   both inline with the dy chain first
 *  31   cam[1] pre-masked + dx first -- 42 insns, -4 bytes, RELOCDIFF
 *  45   `cam2 = cam + 1` as a second pointer -- 46 insns, +4 bytes, RELOCDIFF
 * Struct spellings for the camera (`struct Cam { int x, y; }`) and for `o` both
 * reach 19 and need r6; they are not a route.
 *
 * Bodies live in scratch_elev/b325/D/v8/, the reasoning in
 * scratch_elev/b325/D/NOTES.md.
 *
 * ---------------------------------------------------------------------------
 * BATCH 327 BRIEF I -- RE-DERIVED, PARK HOLDS AT 8 of 44.
 *   objcmp --func: 8 differing encodings of 44 (ref 44, ours 44), first at
 *   index 7, indices 7..14 contiguous.  The figure and the residue reproduce.
 *   (`--whole` still prints SIZE ref 164 / ours 96 because the .s carries the
 *   .rodata the recorded split would separate; the --func figure is the distance.)
 *
 * READ OFF THE REFERENCE DIRECTLY, which pins down where the park's "four
 * references on one qty" comes from: the ROM runs the dx chain FIRST and reuses
 * one register for the whole chain --
 *     ldr r1,[r3] / ldr r2,[r3,#4] / ldr r3,[r0,#8] / and r1,r4 / sub r1,r3,r1
 * so cx, `cx & mask` and dx ALL live in r1, and cy, `cy & mask` and dy all live
 * in r2.  That is why cam[1]'s quantity carries four references (load, and-use,
 * and-set, minus-use) and why the count cannot be lowered from source.
 *
 * FOUR NEW CROSSES, none of them among the park's 45 bodies.  These cross the
 * park's own EXACTLY-INERT prerequisite (the simpler body with no `cy` local)
 * against its map lever (which camera word is read first) and against the ROM's
 * chain order -- the batch-326 pattern of crossing an inert row with a rejected
 * one.  All ref 44 / ours 44, so these are distances:
 *     the inert simpler body re-derived
 *       (`cx = cam[0]; dy = o[4] - (cam[1] & mask); dx = o[2] - (cx & mask);`)
 *       ................................................................ 8
 *     `cy = cam[1]` read FIRST, no `cx` local, dx chain first (ROM order)  12
 *     NEITHER camera read given a local, dx chain first ................. 19
 *     `cx` local read first, dx chain first, cam[1] inline .............. 19
 * Nothing below 8.  The park's contradiction stands and is now tested from the
 * inert side as well as the worse side: **dx-first and cy-in-r2 come from
 * opposite source orders**, and removing the `cy` local -- the one edit that is
 * free at 8 -- does not unlock the dx-first form either.
 *
 * So the NAMED REMAINING CAUSE above is unchanged and is the whole residue: a
 * third local-alloc quantity of higher priority than cam[1]'s would have to
 * occupy r3 across cam[1]'s range WITHOUT lengthening the `cam` pointer's live
 * range, and nothing in the program supplies one.
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
