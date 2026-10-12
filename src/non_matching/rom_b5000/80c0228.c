/* Func_80c0228 -- asm/rom_b5000/rom_bffb8_a_a_c_c.s
 *   (the old subject line named rom_bffb8_a_a_c.s, WHICH DOES NOT EXIST; the
 *   Verify recipe below was already right and tools/piece.py agrees)
 *
 * NON-MATCHING, 21 of 53 encodings.  RE-MEASURED batch 342, brief B: unmoved.
 *   STILL A PHASE FIGURE.  Stream slots 53 both and size identical, but the
 *   INSTRUCTIONS ARE 50 (ref) against 49 (ours) and a trailing 2-byte pad
 *   absorbs the difference.  Relocations identical.
 *   PINS: 0.  Device-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c0228.c \
 *     asm/rom_b5000/rom_bffb8_a_a_c_c.s --func Func_80c0228
 *   and then, because the figure above is phase:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_b5000/80c0228.c \
 *     asm/rom_b5000/rom_bffb8_a_a_c_c.s Func_80c0228 -v
 *
 * SPLIT SHAPE: none.  tools/split_s.py --dry-run reports the reference "holds
 * only Func_80c0228 and no data; convert it directly, no split needed".
 *
 * AUTOSWEPT AT FULL DEPTH, batch 342 (tools/autosweep.py --max 200).  FLAG
 * AXIS: -fno-gcse, -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce and
 * -fno-rerun-loop-opt all inert at 21; -fno-rerun-cse-after-loop 22;
 * -fno-schedule-insns2 25; -O1 45.  ANCHOR AXIS: all 20 of 20 statement
 * boundaries inert.  BOTH CHEAP AXES ARE CLOSED.
 *
 * === BATCH 342: THE MISSING INSTRUCTION IS A COALESCING FACT, NOT PRESSURE ===
 *
 * Batch 333 located the one missing encoding (the reference's `pop {r5}`) and
 * called what remains "one coupled allocation decision".  The full stream diff
 * says it is one COALESCING decision, and that is a different search.
 *
 * Both sides compute the same chain.  The reference keeps its four values in
 * four places; ours ties three of them into one register:
 *
 *   reference                        ours
 *     asr r3, #3        (TIED)         asr r2, r3, #3     (not tied)
 *     mov r2, #0xd                     mov r3, #0xd
 *     sub r2, r3        row -> r2      sub r3, r2         row -> r3
 *     ldr r5, =0x6006000  base -> r5   ldr r2, =0x6006000 base -> r2
 *     lsl r3, r2, #6    shift -> r3    lsl r3, #6         shift TIED to row
 *     mov r1, #0                       mov r1, #0
 *     add r2, r3, r5    ptr  -> r2     add r3, r2         ptr   TIED to shift
 *
 * So the reference's `row`, `shift` and `ptr` are three quantities in two
 * registers (row and ptr share r2 because they do not overlap), which leaves
 * r0/r1/r2/r3/r4 all occupied when the pooled base is born and forces it onto
 * r5 -- hence `push {r5, lr}` and the extra `pop {r5}`.  Ours collapses
 * row -> shift -> ptr into r3, so the base fits in r2 and no sixth register is
 * ever wanted.  THE ROM IS NOT UNDER MORE PRESSURE; it simply did not
 * coalesce.  Block 2 is the same story with the registers exchanged (idx 36-39
 * is a pure r2/r3 swap) and the base there lands in r0 because `v` is dead.
 *
 * WHERE THE TIE IS MADE, AND THE GATE THAT WOULD REFUSE IT.  local-alloc's
 * `combine_regs` ties a two-operand insn's output to a dying input, and its
 * FIRST refusal is exactly the one that matters here:
 *
 *     if ((ureg >= FIRST_PSEUDO_REGISTER && reg_qty[ureg] < 0)
 *   -- local-alloc.c:1633-1636, "it means that it is not local to this block
 *      or dies more than once.  In either event, we can't do anything with it"
 *
 * So the next question is NOT "which declaration order", "which operand order"
 * or "where does i = 0 go" -- all of which are now swept wide and flat -- but
 * "what spelling makes the row/shift temp non-block-local or kill it twice, so
 * that combine_regs declines and the base is pushed onto r5".  Note the
 * obvious move has been tried and fails: making `r`, `i` and `d` block-local
 * by splitting them per block is INERT OR WORSE (below), because it changes
 * the wrong end of the chain.
 *
 * MEASURED IN BATCH 342 -- 14 NEW BODIES, ALL BYTE-IDENTICAL AT 21:
 *   the pooled 0x6006000 named in ONE local shared by both blocks, as an
 *     `unsigned short *` with pointer arithmetic `base + (row << 5)`, and as
 *     an `int` with `(... << 6) + base`
 *   the pooled base named in a SEPARATE local per block, and the same with the
 *     base assignment placed before `i = 0;` instead of after
 *   the base assigned inside the outer guard rather than above it
 *   base AND offset both named, per block
 *     -> all six base spellings are byte-identical, so the pool constant is
 *        rematerialised per block whatever the source says; naming it does not
 *        create a long-lived value and cannot buy the sixth register
 *   splitting the reused locals, all eight combinations of {r, i, d}:
 *     i split (alone or with d)                      26, WORSE
 *     r split (any combination)                      25, WORSE
 *     d split alone                                  21, inert
 *     -> THE BATCH-339 BLOCK-LOCALITY LEVER DOES NOT REACH THIS PARK.  That is
 *        worth recording because the park's shape invites it: three locals are
 *        reused across the two fill blocks and the obvious reading is that one
 *        of them is holding the allocator back.  It is not.
 *
 * --- THE BATCH-333 RECORD FOLLOWS; ITS LEVER AND ITS NEGATIVES STAND ---
 *
 * WHERE THE ONE INSTRUCTION IS.  The reference spends ONE MORE CALLEE-SAVED
 * REGISTER: `push {r5, lr}` and a separate `pop {r5}` before the `pop {r0} /
 * bx r0`.  Note r4 is NOT pushed on either side -- the build is
 * -fcall-used-r4, so the ROM's whole callee-saved budget here is r5 alone.
 * The missing encoding is that `pop {r5}`; ours carries a trailing 2-byte pad
 * instead, which is why the stream lengths agree while the instruction counts
 * do not.  Everything else is a register rename.
 *
 * THE LEVER: `i = 0;` MUST PRECEDE THE WHOLE ADDRESS COMPUTATION IN SOURCE
 * ORDER, in BOTH blocks.  Written
 *
 *     i = 0;
 *     d = (unsigned short *)(((0xd - r) << 6) + 0x6006000);
 *
 * the parameter `v` lands in r0 and `tile` in r4, exactly as the ROM.  Written
 * the other way round -- which is how this park stood -- they are r1 and r0 and
 * the rename propagates through every instruction of both fill loops.  That one
 * reordering is worth fifteen aligned encodings on its own.
 *
 * AND THE ORDERING REQUIREMENT IS STRICT: `i = 0;` must dominate the ENTIRE
 * address computation.  Putting a named `row = 0xd - r;` ABOVE the `i = 0;`
 * loses the whole gain and returns the park to thirty-five differing.  The
 * counter initialisation has to be the first statement of the block.
 *
 * MEASURED IN BATCH 333, ALL BYTE-IDENTICAL TO EACH OTHER at 20 aligned
 * differing -- a thirty-six-variant clean negative:
 *   six declaration orders (p,v,r,row,tile,i,d; row before r; fully reversed;
 *     tile before r; d before i; i,d early) x three block-1 forms (the
 *     expression inline; a named `row = 0xd - r`; the pool constant written
 *     first) x two block-2 forms (`(row << 6) + 0x6006000` and the reverse)
 *   a named `off = (0xd - r) << 6` in one block and in both
 *   pointer arithmetic `(unsigned short *)0x6006000 + ((0xd - r) << 5)`
 *   `-(r - 0xd)` against a halved base
 * So DECLARATION ORDER IS NOT THE DIMENSION HERE and the two operand orders of
 * the final add are not either.  The park's older note that naming the 0x7 mask
 * "fixed nothing" is confirmed and is the same class of negative.
 *
 * The copy-then-modify reading that produced the `r = v / 8; row = r + 0xd;`
 * split is sound and is kept: a copy before an add in the ROM is a tell that
 * two named values exist where we wrote one expression.  Batch 342 adds the
 * converse, which is the whole finding above -- a THREE-OPERAND shift or add in
 * the ROM where ours is two-address is a tell that the ROM DID NOT COALESCE,
 * and that is a local-alloc question, not a naming one.
 *
 * This function paints two rows of a 0x20-wide VRAM tilemap at 0x6006000 from
 * a tile id built out of a slot value read through iwram_3001ef8.
 */
extern int iwram_3001ef8;

void Func_80c0228(void)
{
    int *p;
    int v;
    int r;
    int row;
    int tile;
    int i;
    unsigned short *d;

    p = (int *)iwram_3001ef8;
    v = p[0];
    if ((unsigned int)v <= 0x4f) {
        tile = (v & 7) + 0xf081;
        r = v / 8;
        i = 0;
        d = (unsigned short *)(((0xd - r) << 6) + 0x6006000);
        do {
            i++;
            *d = tile;
            d++;
        } while (i != 0x20);
        tile |= 0x80 << 4;
        r = v / 8;
        row = r + 0xd;
        if ((unsigned int)row <= 0x14) {
            i = 0;
            d = (unsigned short *)((row << 6) + 0x6006000);
            do {
                i++;
                *d = tile;
                d++;
            } while (i != 0x20);
        }
    }
}
