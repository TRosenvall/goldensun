/* Func_8029274 -- asm/rom_15000/rom_23178_a_c_c_c_a.s
 *
 * STILL NON-MATCHING, 6 of 40 encodings -- RE-MEASURED batch 322A.  The park's
 * figure is right and its two-cluster anatomy is right; its DENOMINATOR is not.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8029274.c \
 *     asm/rom_15000/rom_23178_a_c_c_c_a.s --func Func_8029274
 *
 * *** THE DENOMINATOR IS 40, NOT 47. ***  The park's prose says "6 of 47" in
 * four places while its own recipe line says 40.  objcmp: `(ref 40, ours 40)`,
 * size identical.  47 appears nowhere in any measurement and should not be
 * quoted again.
 *
 * --whole: the reference holds TWO functions, ['Func_8029274', 'Func_80292c4'],
 *   so landing needs a two-way split.  SPLIT SHAPE (both re-run this batch):
 *     tools/datacheck.py asm/.../rom_23178_a_c_c_c_a.s -> NO OUTPUT, exit 0
 *     tools/split_s.py ... Func_8029274 --dry-run ->
 *       rom_23178_a_c_c_c_a_b.s (1 function, 52 lines)   [Func_8029274]
 *       rom_23178_a_c_c_c_a_c.s (1 function, 98 lines)   [Func_80292c4]
 *       removes rom_23178_a_c_c_c_a.s, rewrites stage1.ld
 *       install path on a landing: src/rom_15000/rom_23178_a_c_c_c_a_b.c
 * PINS: 0.  No shims of any kind, no per-file flag, no fakematch row.
 *
 * ===================== WHAT THE 6 ARE (re-measured per index) ===============
 *
 * All six are REAL INSTRUCTIONS -- no pool words anywhere in this function.
 *
 *   cluster 1 (2 encodings) -- the digit store against the index increment
 *     XX  18  ref 7023 strb r3,[r4]  | ours 3201 adds r2,#1
 *     XX  19  ref 3201 adds r2,#1    | ours 7023 strb r3,[r4]
 *
 *   cluster 2 (4 encodings) -- ONE register decision with four consequences
 *     XX  28  ref 18d1 adds r1,r2,r3 | ours 189c adds r4,r3,r2
 *     XX  30  ref 780b ldrb r3,[r1]  | ours 7823 ldrb r3,[r4]
 *     XX  31  ref 3901 subs r1,#1    | ours 3c01 subs r4,#1
 *     XX  34  ref 4561 cmp  r1,ip    | ours 4564 cmp  r4,ip
 *
 * The copy-back pointer is **r1** in the ROM and **r4** in ours, and r1 is the
 * register the digit count `n` arrived in and is dead in by then.  Note idx 28
 * also differs in OPERAND ORDER: `adds r1,r2,r3` is index-then-base, ours is
 * base-then-index.
 *
 * ===================== A FLAT CROSS, WHICH IS THE FINDING ===================
 *
 * Landed sibling src/rom_15000/rom_20198_c_c_c_a_a_c_a_b.c documents that a
 * reg+reg form whose FIRST register is the scaled index is the tell for a
 * SUBSCRIPT, and that base-first is what naive pointer arithmetic gives -- which
 * is exactly the idx-28 difference.  **It is inert here.**  Measured, one
 * container, every row size-exact and relocation-clean:
 *
 *     v00_base   `p = buf + i;`                        6
 *     v01        `p = &buf[i];`           (subscript)  6
 *     v02        `p = (char *)(i + (int)buf);`         6
 *     v04        `p = (char *)((int)buf + i);`         6
 *     v03        `p = buf; p += i;`                    8   WORSE
 *     v06        `p++` before `i++`                    6
 *     v07        `*p++ = d;`                           6
 *     v08        `val >>= 4` before `i++`              6
 *     v09        `i++` last in the body                6
 *
 * **Nine spellings, two independent dimensions (how the pointer is FORMED and
 * the order of the four loop-body statements), and the figure never moves off 6
 * except to get worse.**  Per the batch-322 brief, a flat cross is itself the
 * result: the lever is in neither dimension, and the next move is the SIGNATURE,
 * the TYPE or the TU shape -- not another cell. In particular the index-first
 * spelling does NOT reach idx 28 here, so whatever decides operand order on this
 * insn is downstream of the source form, and the landed sibling's rule does not
 * generalise to a stack-array base.
 *
 * The park's own two backfires still bound the obvious moves and are kept: the
 * copy-back loop in int arithmetic is 26 and one short, and a SECOND pointer
 * variable for the copy-back loop is 21 and two short.  The second is worth
 * re-reading, because the ROM genuinely uses two different registers for the two
 * loops (r4 then r1), which one pseudo cannot do -- yet two pseudos lose the
 * second `mov rN, sp`.  **That tension is unresolved and is the most promising
 * thing left here:** a shape that gives two pointers but materialises the buffer
 * address twice.
 */
void Func_8029274(unsigned int val, unsigned int n, char *out)
{
    char buf[8];
    char *p;
    int i;
    unsigned int d;
    int mask;

    if (n > 5)
        n = 5;
    i = 0;
    if (n != 0) {
        mask = 0xf;
        p = buf;
        do {
            d = val & mask;
            if (d <= 9)
                d += 0x30;
            else
                d += 0x37;
            *p = d;
            i++;
            val >>= 4;
            p++;
        } while (i != n);
    }
    i = n - 1;
    if (i >= 0) {
        p = buf + i;
        do {
            *out = *p;
            p--;
            out++;
        } while ((int)p >= (int)buf);
    }
}
