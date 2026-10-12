/* Func_80c0228 -- asm/rom_b5000/rom_bffb8_a_a_c_c.s
 *
 * *** LANDS.  BYTE-IDENTICAL.  0 pins, device-free, no split. ***
 *   objcmp: `OK Func_80c0228 -- 112 bytes, 53 encodings and 1 relocations
 *   identical`.  The park stood at 21 of 53 with ref 50 / ours 49 16-bit
 *   instructions -- a PHASE figure, the missing instruction being the
 *   reference's `pop {r5}`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_bffb8_a_a_c_c.c \
 *     asm/rom_b5000/rom_bffb8_a_a_c_c.s --func Func_80c0228
 *
 * SPLIT SHAPE: none.  tools/split_s.py --dry-run: "holds only Func_80c0228 and
 * no data; convert it directly, no split needed".  tools/datacheck.py: no rows.
 * tools/shimcount.py: 0.
 *
 * ================= WHAT CLOSED IT, AND THE PARK'S OWN QUESTION =================
 *
 * The park (batch 342, brief B) had the mechanism exactly right and named the
 * gate itself:
 *
 *     "what spelling makes the row/shift temp non-block-local or kill it twice,
 *      so that combine_regs declines and the base is pushed onto r5"
 *      -- local-alloc.c:1633-1636
 *
 * THE ANSWER IS `row`, AND IT IS TWO INDEPENDENT HALVES.  Both are required and
 * each is worth a different part of the stream:
 *
 *   1. THE DIVIDE RESULT MUST BE BLOCK-LOCAL IN EACH BLOCK.  The park shared one
 *      `int r` across both fill blocks, so its pseudo spanned two basic blocks,
 *      `reg_qty[r] < 0`, and combine_regs declined at local-alloc.c:1633-1636 --
 *      giving `asrs r2, r3, #3` (untied) where the ROM has `asrs r3, r3, #3`
 *      (tied to the dying dividend temp).  Name the divide ONCE (block 1) and
 *      leave block 2's as an anonymous expression and BOTH asr's tie.  That is
 *      also what frees the register the ROM spends: with `r` tied into the
 *      dividend's r3, the freed register at the base's birth is no longer r2,
 *      r0/r1/r2/r3/r4 are all occupied, and the pooled 0x6006000 is forced onto
 *      r5 -- `push {r5, lr}` and the extra `pop {r5}` appear.  THIS is the
 *      missing instruction.  Alone it takes aligned-equal 34 -> 40 of 53.
 *
 *   2. `row` MUST BE NON-BLOCK-LOCAL, i.e. ASSIGNED IN BOTH BLOCKS.  With a
 *      block-local `row`, combine_regs ties block 2's `row` to its dying divide
 *      result and the ROM's copy `adds r2, r3, #0` disappears -- one instruction
 *      SHORT in block 2 even after half 1 has added one.  Writing block 1's
 *      offset through the SAME `row` gives the pseudo two deaths in two blocks,
 *      combine_regs declines, and the copy comes back.  Alone this is worth
 *      nothing; crossed with half 1 it is the landing.
 *
 * SO IT IS A CROSSING, AND THE PARK HAD MEASURED BOTH HALVES SEPARATELY AND
 * RECORDED BOTH AS NEGATIVES.  "splitting the reused locals, all eight
 * combinations of {r, i, d}: r split (any combination) 25, WORSE" is half 1 on
 * its own; "three block-1 forms (... a named `row = 0xd - r` ...)" is half 2 on
 * its own.  Neither moves the figure; together they are byte-identical.  This is
 * the brief's "cross the lists" obstacle in its purest measured form -- the
 * variable to split and the variable to SHARE are different variables, and the
 * park's eight-combination sweep varied only {r, i, d} and never touched `row`.
 *
 * THE OTHER DIRECTION, KEPT: the park's reading of the residue was right and is
 * confirmed.  A THREE-OPERAND shift or add in the ROM where ours is two-address
 * is a tell that the ROM DID NOT COALESCE; it is a local-alloc question, not a
 * naming one.  And `i = 0;` need no longer be the first statement of the block
 * (the park's "ORDERING REQUIREMENT IS STRICT") -- with `row` shared, p3_v_g
 * below puts `row = 0xd - r;` ABOVE `i = 0;` and is also byte-identical.
 *
 * FOUR SPELLINGS ARE ALL BYTE-IDENTICAL (scratch_elev/b344/A/v/):
 *   t3_j  one named `r` in block 1, block 2's divide inline, `row` shared  <- THIS FILE
 *   t3_k  block 1's divide inline, one named `r` in block 2, `row` shared
 *   t3_f  `r` and `s` both named (per block), `row` shared, `i = 0;` first
 *   t3_g  as t3_f with `row = 0xd - r;` before `i = 0;`
 * MEASURED AND SHORT OF IT (aligned-equal of 53):
 *   t3_a  `r`/`s` split per block, `row` block-local ................ 40, -1 insn
 *   t3_d  the same with the two names exchanged ..................... 40, -1 insn
 *   t3_e  both divides inline, `row` shared ......................... 51, 3 differing
 *   t3_b  `r` and `d` split ......................................... 37
 *   t3_c  `r`, `d` and `i` split .................................... 33
 *   park  base ...................................................... 34
 * FLAG AXIS: closed by the park (all eight groups) and not re-run; the landing
 * is at production flags.
 *
 * This function paints two rows of a 0x20-wide VRAM tilemap at 0x6006000 from a
 * tile id built out of a slot value read through iwram_3001ef8.
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
        row = 0xd - r;
        i = 0;
        d = (unsigned short *)((row << 6) + 0x6006000);
        do {
            i++;
            *d = tile;
            d++;
        } while (i != 0x20);
        tile |= 0x80 << 4;
        row = v / 8 + 0xd;
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
