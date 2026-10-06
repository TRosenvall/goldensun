/* Func_80f3858 (BeginPaletteFade) -- NON-MATCHING.
 *
 * NON-MATCHING, 2 differing encodings of 29  (MEASURED, batch 329 brief J;
 * was 8 of 29).  Counts now AGREE (ref 29, ours 29) and objcmp prints no SIZE
 * and no POOL WORD line, so the inherited 4-byte size excess and the surplus
 * pool word are both GONE.  First differing index 9.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_f2000/80f3858.c asm/rom_f2000/rom_f2028_c_c_c_a_a.s --func Func_80f3858
 *
 * ----- everything below to the next ===== heading is the INHERITED park text,
 * ----- kept verbatim; its figure line, its MISALIGNMENT reading and its recipe
 * ----- are all superseded by the two lines above.
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: CONSTANT DERIVATION, and it is the MIRROR IMAGE of
 * src/non_matching/ovl_7ac2d8/200adcc.c.
 *
 * 28 lines against the ROM's 28, 7 differing, all in the two byte stores.
 *
 *     rom   ldr r1, =0x3001 / add r3, r4, r1 / add r1, #1  ... add r3, r4, r1
 *     ours  ldr r2, =0x3001 / add r3, r4, r2 ... ldr r3, =0x3002 / add r2, r4, r3
 *
 * The ROM keeps 0x3001 in a register and DERIVES 0x3002 by adding one.  gcc
 * takes a second pool entry instead.
 *
 * Worth pairing with 200adcc, where the same class runs the other way: there
 * gcc derived 0x50000c4 as 0x50000ce - 0xa and the ROM took the second pool
 * entry.  So gcc-2.96 is not uniformly more or less willing to derive one
 * near constant from another than the ROM's compiler was; the two disagree in
 * both directions, on the same kind of address pair, a few hundred bytes
 * apart in the same game.  That rules out a single flag as the explanation
 * for either, and it is why neither park proposes one.
 *
 * Tried:
 *   - `p[0x3001]` and `p[0x3002]` as plain subscripts:  7 differing
 *   - a named `off` variable incremented between the stores, which is the
 *     "name the OFFSET, not the base" lever and is exactly the ROM's shape:
 *     20 differing and a stream two lines SHORT.  gcc constant-folds `off++`
 *     before it ever reaches register allocation, so the lever cannot express
 *     a runtime increment of a compile-time offset.  This is the useful
 *     negative: that lever moves which VALUE is named, never whether the
 *     addition survives to runtime.
 *   - --no-rerun-cse (7, unchanged), --O1 (18), --no-sched2 (18).
 *
 * The rest of the function -- the three scaled base additions feeding
 * Func_80f2ebc -- is byte-exact, including the interleaved lsl/add pairs.
  *
 * BATCH 205 -- WRITING THE DERIVATION EXPLICITLY IS MUCH WORSE, not better.
 *
 * This park calls itself the mirror image of ovl_7ac2d8/200adcc.c: there the
 * ROM loads two pool constants and gcc derives one from the other, here the
 * ROM derives (`add r1, #1`) and gcc loads two. The natural move is therefore
 * to write the derivation into the source, and docs/elevation.md records that
 * deriving is reachable when the first value is genuinely CONSUMED before the
 * increment -- which it is here, `add r3, r4, r1` precedes `add r1, #1`.
 *
 * It does not work. Both spellings measured:
 *
 *     off = 0x3001; p[off] = frames; off += 1; p[off] = 0;   26 lines, 20 differing
 *     the same with `off++`                                  26 lines, 20 differing
 *
 * against 28 lines and 7 differing for the two plain literal indices kept
 * below. The function comes out TWO INSTRUCTIONS SHORT, so gcc is not merely
 * ordering things differently -- naming the offset lets it fold both stores
 * through one address computation and drop work the ROM does.
 *
 * So the mirror framing does not carry a lever with it. The reachable-derivation
 * rule is about a value that must survive as a VARIABLE across its uses; here
 * naming it gives gcc a strength-reduction opportunity it takes, and the ROM's
 * `add r1, #1` is a consequence of how it held the offset rather than something
 * the source asked for.
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        p[0x3002] = 0;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
 * ===== TWO THINGS THE PARK ASSERTED, BOTH REFUTED. =====
 *
 * (1) "COUNT DIFFERS (ref 29, ours 30) -- so this positional figure measures
 * MISALIGNMENT, not distance."  It did not.  objcmp on the park body reports
 * `POOL WORD COUNT ref 3, ours 4 (instructions AGREE at 26)` and says in terms
 * that the instruction streams are the same length and the difference is pool
 * CONTENT.  The 8 was a distance all along.  This is another sighting of the
 * pool/padding trap INSIDE a standing park header; read objcmp unfiltered.
 *
 * (2) "BATCH 205 -- WRITING THE DERIVATION EXPLICITLY IS MUCH WORSE."  True, and
 * beside the point: the derivation did not need writing.  The park's blocker was
 *
 *   rom   ldr r1,=0x3001 / add r3,r4,r1 / add r1,#1 / mov r2,#0
 *         / strb r5,[r3] / add r3,r4,r1 / strb r2,[r3]
 *   park  ldr r2,=0x3001 / add r3,r4,r2 / strb r5,[r3] / ldr r3,=0x3002
 *         / add r2,r4,r3 / mov r3,#0 / strb r3,[r2]
 *
 * and it is now exact as far as index 8 -- `add r1,#0x1` included -- with the
 * two offsets still written as the plain literals `p[0x3001]` and `p[0x3002]`.
 * gcc DERIVES the second constant from the first on its own.  The one change is
 * a named `int zero` whose assignment sits BETWEEN the two stores.
 *
 * So the park's "mirror image of ovl_7ac2d8/200adcc.c" framing led the search
 * at the constant, and the constant was never the free variable.  Whether gcc
 * derives 0x3002 from 0x3001 or takes a second pool entry is decided by what
 * else is live across the first store, not by how the offsets are spelled.
 * Same lever, same batch, same shape as OvlFunc_common1_15b8 (6 -> 0).
 *
 * ===== THE RESIDUE AT 2 IS ONE ADJACENT SWAP, AND IT IS A TIE-BREAK. =====
 *
 *   rom   ... add r1,#0x1 / mov r2,#0x0 / strb r5,[r3,#0x0] ...
 *   ours  ... add r1,#0x1 / strb r5,[r3,#0x0] / mov r2,#0x0 ...
 *
 * Both insns sit at scheduling priority 0 -- arm_adjust_cost returns 0 for
 * anti/output deps (arm.c), and a store whose only dependent is an anti dep has
 * priority 0 -- so sched2 falls through to INSN_LUID, i.e. source order.  The
 * ROM's order therefore needs `zero = 0;` BEFORE the first store.
 *
 * AND THAT IS THE TENSION, measured both ways:
 *   zero = 0 BEFORE the first store  -> store block EXACT (indices 0..13 all
 *       match, `mov r2,#0` included) but the Func_80f2ebc argument block breaks:
 *       7 of 29, first index 14.  The ROM interleaves two shift chains
 *       (r3 for 0x1000, r2 for 0x400); with `zero` live into that region gcc
 *       serialises all three through one temp.
 *   zero = 0 BETWEEN the stores      -> argument block EXACT, store block one
 *       swap short: 2 of 29.  THIS BODY.
 * Naming the three argument pointers in the ROM's computation order (b, a, c)
 * recovers one of the seven: 6 of 29.  Still worse than 2.
 *
 * MEASURED, zero's PLACEMENT is the whole lever and its TYPE is irrelevant:
 *   int/char/unsigned char zero, assigned inside the if before the stores: all
 *   7 of 29, first index 14, identical.
 *   `int zero = 0;` at the declaration, or assigned before `p = ...`, or
 *   between `p = ...` and the if: all 7 of 29 but first index 6 -- i.e. the
 *   derivation is NOT recovered.  It has to be inside the if.
 *   Declaration order (zero before p, or zero in an inner scope) and
 *   `register int zero`: all 2 of 29, byte-identical to this body.
 *   --no-rerun-cse 2 (no change); --no-sched2 13 of 29.
 * MEASURED WORSE: pointer increment (`q = p + 0x3001; *q = frames; q++;`)
 *   27 lines, 23 differ; `off = 0x3001; ... off++;` with a named zero 26 lines,
 *   20 differ (the park's own result, reproduced); the two stores swapped in
 *   source 12 of 29.
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;
    int zero;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        zero = 0;
        p[0x3002] = zero;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
 */
extern char *iwram_3001ed0;
extern void Func_80f2ebc(void *a, void *b, void *c, int n);

void Func_80f3858(int frames)
{
    char *p;
    int zero;

    p = iwram_3001ed0;
    if (p != 0) {
        p[0x3001] = frames;
        zero = 0;
        p[0x3002] = zero;
        Func_80f2ebc(p + 0x400, p + 0x1000, p + 0x1c00, frames);
    }
}
