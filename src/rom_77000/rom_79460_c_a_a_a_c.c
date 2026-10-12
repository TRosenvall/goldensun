/* Func_8079664  (RemovePartyMember) -- asm/rom_77000/rom_79460_c_a_a_a_c.s
 *
 * LANDS.  BYTE-IDENTICAL: 96 bytes, 44 encodings, 4 relocations, 39/39 16-bit
 * instructions.  Measured --whole as well as --func.  The park's 14 of 44 was
 * re-measured at the head of batch 338 and CONFIRMED (44 encodings both sides,
 * 39 instructions both sides, so it was a true distance, as batch 332 D said).
 *
 * PINS: 0.  shimcount.py prints no rows.  No device, no dead store, no flag
 * group, no fakematch row.  datacheck.py on the reference: clean.  The piece
 * holds this function alone (tools/piece.py), so NO SPLIT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_79460_c_a_a_a_c.c \
 *     asm/rom_77000/rom_79460_c_a_a_a_c.s --whole
 *
 * ===========================================================================
 * TWO LEVERS, AND THE SECOND ONE IS A GENERAL RESULT ABOUT allocno_compare
 *
 * Batch 332 D split the 14 correctly: "roughly eight" the id/size register
 * swap, "roughly six" the tail's instruction order.  Both numbers were exact.
 *
 * ---------------------------------------------------------------------------
 * LEVER 1 (the six): WRITE THE SHIFT-DOWN AS AN INDEX LOOP AND LET gcc DO THE
 * STRENGTH REDUCTION.  14 -> 8, and the eight that remain are ALL the swap.
 *
 *     last = size - 1;
 *     while (i < last) {
 *         gState[0x1f8 + i] = gState[0x1f9 + i];
 *         i++;
 *     }
 *
 * The park wrote the walking pointer and the down-counter BY HAND
 * (`p = (g + i) + 0x1f8; n = last - i; do { *p = p[1]; n--; p++; } ...`) and
 * then spent a round on where to put the `n = last - i` statement -- three
 * placements inert at 14, one at 18, one at 21.  That whole dimension was the
 * wrong dimension.  Handed the index loop, `loop.c`'s strength reduction emits
 * the preheader itself and gets all six right at once, INCLUDING the one the
 * park could not reach: the 0xfc constant lands in **r4** (call-clobbered in
 * this object, so free) instead of stealing i's register, and the count
 * subtract comes LAST, because the loop pass emits the biv init before the
 * trip-count calculation rather than in source order.
 *
 * GENERAL: when a reference tail is a strength-reduced loop, SPELL THE LOOP,
 * NOT THE RESULT OF THE REDUCTION.  Hand-reducing moves the statements into
 * the programmer's order and no placement sweep can recover the pass's order.
 *
 * ---------------------------------------------------------------------------
 * LEVER 2 (the eight): ONE EXTRA INSN IN THE BLOCK WHERE ONLY `size` IS LIVE.
 *
 *     j = i;              <-- this line, and it must precede the next one
 *     last = size - 1;
 *     while (j < last) { ... }
 *
 * THE NUMBERS, which the park asked for and which are now exact.  From
 * `-da`'s `.17.lreg` (`dump_flow_info`, flow.c:5743-5756) on the parked body:
 *
 *     Register 32 (id)    used 5 times across 17 insns; crosses 2 calls
 *     Register 35 (size)  used 5 times across 16 insns; crosses 1 call
 *
 * `allocno_compare` (global.c:598-620) is
 * `floor_log2 (n_refs) * n_refs / live_length * 10000 * size`, truncated to
 * int, and it sorts DESCENDING:
 *
 *     pri(id)   = (int)(floor_log2(5)*5 / 17 * 10000) = 2*5/17*10000 = 5882
 *     pri(size) = (int)(floor_log2(5)*5 / 16 * 10000) = 2*5/16*10000 = 6250
 *
 * so `size` is allocated first and takes the first call-preserved register
 * REG_ALLOC_ORDER reaches (arm.h:989-995: 3, 2, 1, 0, 12, 14, 4, 5, 6, 7 --
 * r4 is call-used in this object, so r5), and `id` gets r6.  The reference
 * wants the opposite.  Verified directly, not inferred: `dump_conflicts`
 * (global.c:1862) prints `allocno_order` into `.18.greg` as
 * `;; 9 regs to allocate: 34 38 37 54 36 35 32 48 47` -- 35 ahead of 32.
 *
 * **THE PARK SAID "an instruction or two".  IT IS EXACTLY ONE, BECAUSE A TIE
 * IS A WIN.**  On equal priority `allocno_compare` falls through to
 * `return v1 - v2` on the ALLOCNO NUMBERS (global.c:617-619), and allocnos are
 * numbered by ascending pseudo (global.c:421-451), so reg 32 beats reg 35.
 * L_id == L_size is enough; nothing has to be made strictly shorter.
 *
 * WHERE THE ONE INSN HAS TO GO.  Counting the `.17.lreg` blocks, `id` is live
 * over exactly two insns that `size` is not -- its own set and the
 * `GetPartySize` call -- and `size` over exactly one that `id` is not, the
 * `last = size - 1` in the loop's exit block.  17 = 16 + 2 - 1.  Neither of
 * id's two can be removed, so the only reachable square is **one more insn in
 * the exit block, ahead of `last = size - 1`**, which is why `j = i;` AFTER
 * the subtract measures 8 and BEFORE it measures 0.
 *
 * AND THE INSN IS FREE.  `j` and `i` get the same hard register, so after
 * reload the copy is `mov r1, r1` and flow2 deletes it:
 * `life_analysis (insns, dump, PROP_FINAL)` (toplev.c:3417) calls
 * `delete_noop_moves` unconditionally (flow.c:2622-2624, "Always remove no-op
 * moves").  39 instructions out, 39 in the reference.
 *
 * WHY IT ONLY WORKS WITH LEVER 1.  Crossed the other way the copy costs an
 * instruction.  On the park's hand-reduced tail, `j` carries four references
 * (compare, subtract, address, and its set) over a short range, so
 * `allocno_compare` puts it AHEAD of `i`; `i` has no hard register yet when
 * `find_reg` runs for `j`, so the copy preference at global.c:1067-1098 has
 * nothing to match and `j` takes r2.  Result: the sort flips (`... 36 32 35
 * ...`), the first 25 encodings go EXACT, and a stray `adds r2, r1, #0`
 * appears at index 25 -- 20 of 44 on 40 instructions.  With the index loop `j`
 * IS the loop's biv, sorts after `i`, inherits r1, and vanishes.  **Two levers
 * that are each worth nothing alone and byte-identical together** -- this is
 * the crossing the brief template keeps asking for.
 *
 * ---------------------------------------------------------------------------
 * MEASURED, all at 44 encodings and 39 instructions unless noted
 *
 *    0  THIS BODY                                                     LANDS
 *    0  the same with `i = 0;` after the copy (a dead store)   not shipped
 *    0  the same as a `for (; j < last; j++)` after `j = i;`
 *    0  the search loop on `j` and the tail on `i` (copy reversed)
 *    0  `gState[0x1f8 + j + 1]` for `gState[0x1f9 + j]`
 *    8  index-loop tail, no copy                      <- lever 1 alone
 *    8  ... `while (i < size - 1)`, no `last` at all
 *    8  ... `for (; i < size - 1; i++)`
 *    8  ... `last = size; while (i < last - 1)`
 *    8  ... copy placed AFTER `last = size - 1`        <- the ordering proof
 *    8  ... `for (j = i; j < last; j++)` (the init lands in the preheader,
 *           i.e. after the subtract -- same failure as the line above)
 *   14  THE PARKED BODY
 *   14  hand-reduced tail + copy with only 2 later uses of j (cse2 unifies
 *       j with i and the copy dies)
 *   14  hand-reduced tail + `off = 0x1f8;` named before the if
 *   14  hand-reduced tail + a copy of `size` instead of `i`
 *   18  hand-reduced tail with the count subtract last
 *   20  hand-reduced tail + the copy instrument (40 instructions; first 25
 *       encodings EXACT -- this is the row that proved lever 2)
 *   21  hand-reduced tail with the subtract hoisted out of the if
 *   24  `while (i != last)` for the index loop (38 instructions)
 *   25  `while (i + 1 < size)` (100 bytes)
 *   26  hand-reduced tail + the pool load hoisted into the exit block
 *       (40 instructions; also flips the sort -- the first instrument)
 *   28  `g = gState + 0x1f8; p = g + i;` (36 instructions)
 *   40  the peeled test written `*(gState + 0x1f8 + i)` (36 instructions)
 *   42  `g = gState;` hoisted ahead of the search loop (38 instructions)
 *
 * AUTOSWEEP (batch 338, recorded negatives on the PARKED body -- do not
 * re-screen): flags -fno-gcse 14, -ffixed-r7 14, -fno-strict-aliasing 14,
 * -fno-rerun-loop-opt 14 (inert); -fno-schedule-insns2 16, -O1 22,
 * -fno-rerun-cse-after-loop 37, -fno-strength-reduce 40.  Anchor axis, 12
 * boundaries: all inert at 14.  The `-fno-strength-reduce` row at 40 is worth
 * keeping: it is the same pass lever 1 is buying from, measured from the other
 * side.
 *
 * STILL TRUE FROM THE PARK: the search loop needs no help at all -- gcc
 * rotates the counted loop into the reference's guard / peeled test / body
 * shape and forms the walking pointer only after the peeled test.  Batch 332
 * D's 720-permutation declaration sweep stays closed and is not re-run here.
 */
extern unsigned char gState[];
extern int GetPartySize(void);
extern void ClearFlag(int id);

int Func_8079664(int id)
{
    int size;
    int i;
    int last;
    int j;

    size = GetPartySize();
    ClearFlag(id);
    for (i = 0; i < size; i++) {
        if (gState[0x1f8 + i] == id)
            break;
    }
    j = i;
    last = size - 1;
    while (j < last) {
        gState[0x1f8 + j] = gState[0x1f9 + j];
        j++;
    }
    return GetPartySize();
}
