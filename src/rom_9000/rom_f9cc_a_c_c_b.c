/* Func_800fa8c -- MATCHING, 0 of 28 encodings, 60 bytes, PIN-FREE.
 *
 * PINS: 0.  FLAG GROUP: none.
 *
 * SPLIT: asm/rom_9000/rom_f9cc_a_c_c.s holds THREE functions (Func_800fa8c,
 * UnpackTilemap, LoadMapData) and this is the FIRST.
 * tools/split_s.py --dry-run:
 *     would write asm/rom_9000/rom_f9cc_a_c_c_b.s  (1 function(s),  36 lines)
 *     would write asm/rom_9000/rom_f9cc_a_c_c_c.s  (2 function(s), 431 lines)
 *     would REMOVE asm/rom_9000/rom_f9cc_a_c_c.s
 *     would rewrite stage1.ld
 * tools/datacheck.py on the source .s is SILENT (exit 0): no data requirement,
 * and stage1.ld names the object ONCE (line 219, `.text`).  exports: [].
 *
 * REPOINT: src/non_matching/rom_9000/LoadMapData.c is a park for a function
 * that stays in the _c half and its recipe names the pre-split path.  Run
 * tools/repoint_parks.py after the split.
 *
 * NOT the HeightTile_* family.  The brief flagged rom_9000's near-identical
 * `HeightTile_*` members and their duplicate park pair; checked, and neither
 * this function nor its .s file mentions the family (that family is
 * asm/rom_9000/rom_11ce0_*.s).  Nothing here is shared with it.
 *
 * Verify with (INSTALLED path, after the split):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_9000/rom_f9cc_a_c_c_b.c \
 *     asm/rom_9000/rom_f9cc_a_c_c_b.s --func Func_800fa8c
 *   -> OK Func_800fa8c -- 60 bytes, 28 encodings and 1 relocations identical
 *
 * ===================================================================
 * THE PARK READ 20 of 28 AND CALLED IT "register allocation", WHICH IS
 * THE SYMPTOM.  IT WAS TWO CAUSES AND BOTH ARE SOURCE-LEVEL.
 * ===================================================================
 *
 * The park was right that "the structure is EXACT -- every mnemonic, every
 * immediate, every branch -- and the registers are shifted by one throughout",
 * and right that the ROM uses SEVEN registers (`push {r5, r6, lr}` plus r4)
 * where the park's body used six (`push {r5, lr}`).  It was wrong that
 * "everything expressible in C is already right".  Both causes are the SAME
 * mistake made twice: a value the ROM keeps as its own quantity, folded into
 * an existing name.
 *
 * (A) THE TWO ARITHMETIC RESULTS ARE TWO QUANTITIES, worth 11 of the 20.  The
 *     ROM is
 *         add r3, r1, r0   @ t1 = v + acc        (a FRESH register)
 *         sub r1, r3, r2   @      t1 - m         (into v's register)
 *     The park wrote `t = v + acc; t = t - m;`, one name, which gcc keeps in
 *     one register (`add r3,r3,r1 / sub r3,r3,r2`) -- six registers, so
 *     `push {r5, lr}`.  Writing it as ONE EXPRESSION, `*(p - 1) = v + acc - m`,
 *     makes t1 a distinct pseudo, and the whole outer frame falls into place at
 *     once: the push set, n->r4, acc->r0, p->r5, mask->r6 all become the ROM's.
 *     20 -> 9.
 *
 *     NOTE, because it is the kind of thing that costs a round: spelling the
 *     same split with TWO EXPLICIT NAMES (`t = v + acc; u = t - m;`) is
 *     EXACTLY INERT at 20.  The one-expression form and the two-name form are
 *     not interchangeable here; only the former gets the second quantity.
 *
 * (B) THE RESULT IS WRITTEN BACK INTO `v`, worth the last 9.  At 9 the
 *     remaining difference is `v` and `m` swapped against the ROM (ours r3/r1,
 *     ROM r1/r2) and the store address in r2 where the ROM has r3.  From the
 *     `-da` dumps, with the single expression:
 *         .17.lreg  38 [t1] in 3   39 [addr] in 2      <- two LOCAL qtys
 *         .18.greg  36 [v]  in 3   37 [m]    in 1
 *     t1 and the second subtraction are two local qtys that take r3 between
 *     them, so `addr` is pushed to r2, and `v` then inherits r3 (it has
 *     `;; 36 preferences: 3`) while m is barred from r3 and r2 and lands in r1.
 *
 *     `v = v + acc - m; *(p - 1) = v;` deletes the second result as a separate
 *     quantity -- it is a new value in `v`, which is a GLOBAL allocno -- so
 *     only ONE local qty remains besides the address, t1 dies before `addr` is
 *     born, and the two SHARE r3.  Measured:
 *         .17.lreg  38 [t1] in 3   39 [addr] in 3      <- now sharing
 *         .18.greg  36 [v]  in 1   37 [m]    in 2
 *     which is the ROM exactly: t1 r3, addr r3, v r1, m r2, acc r0, n r4,
 *     p r5, mask r6.  Every one of the nine remaining encodings closes.
 *
 *     So the "registers shifted by one throughout" was never an allocator
 *     difference to be worked around.  It was ONE local quantity too many,
 *     displacing the address out of r3 and rotating every global allocno one
 *     place down REG_ALLOC_ORDER.
 *
 * CROSSED (tools/crossfire.py, depth 2, 10 edits, base 20):
 *   MEASURED EXACTLY INERT at 20, alone and in every pair: `m` unsigned, `v`
 *   unsigned, `p[-1] = t` for `*(p - 1) = t`, `if (m != acc)` for
 *   `if (acc != m)`, declaring `t` first, declaring a spare `u`, declaring a
 *   spare `q`, and a named store address (`q = p - 1; *q = t;`).
 *   ONLY "one expression" moved the figure, and it moved it with every one of
 *   those crossed on top -- so none of them is a prerequisite either.
 *   Nothing measured WORSE than the base.
 *
 * The park's other observations all survive and are kept verbatim: the walk is
 * `*p++` so it is `ldmia r5!, {r1}`; the store goes back to `p - 1`; the mask
 * is a named local so `m = v; m &= mask;` keeps `mov r2,r1 / and r2,r6`; the
 * count is built as `n = 0x80; n <<= 7;`; and `acc = 1; acc = -acc;` is what
 * gives `mov r0,#1 / neg r0,r0`.
 */
extern unsigned int gBuffer[];

void Func_800fa8c(void)
{
    unsigned int *p;
    int mask;
    int n;
    int acc;
    int v;
    int m;

    n = 0x80;
    acc = 1;
    p = gBuffer;
    mask = 0xfff;
    n <<= 7;
    acc = -acc;
    do {
        v = *p++;
        m = v;
        m &= mask;
        if (m == mask) {
            if (acc != m)
                acc++;
            v = v + acc - m;
            *(p - 1) = v;
        }
        n--;
    } while (n != 0);
}
