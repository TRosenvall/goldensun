/* OvlFunc_883_200806c  [ovl_780898]  --  0x0200806c
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_a_a_a_c.s
 *
 * FindEntityAtPosition. HEAD OF A 17-MEMBER FAMILY -- the largest unsolved
 * group in the overlays, so this is worth more than one function.
 *
 * Blocker: REGISTER BIRTH ORDER in the prologue. Forty instructions against
 * forty, with the whole loop body identical; only the set-up differs:
 *
 *     rom    mov r4, r0 / ldr r2, [r3] / ldr r3, [r4] / mov r1, r2
 *     ours   ldr r3, [r3] / mov r1, r0 / mov r4, r3 / ldr r3, [r1]
 *
 * The ROM puts the POSITION argument in r4 and the table pointer in r1; we do
 * the opposite. Under REG_ALLOC_ORDER {3,2,1,0} that means the ROM creates the
 * position pseudo first, so the argument has to be touched before the table is
 * loaded.
 *
 * TRIED, both still 40-vs-40:
 *   1. `px = pos->x >> 20` hoisted above the table load (diverges at 1)
 *   2. as 1 but the table base taken into its own local first (diverges at 2)
 *
 * Both read the argument first in the SOURCE and gcc still schedules the
 * memory load ahead of the register move. Worth trying next: making the table
 * pointer depend on something derived from pos, so the dependency forces the
 * order rather than the statement sequence suggesting it.
 *
 * THE ANNOTATION IS WRONG ABOUT THE SIGNATURE, and it matters here because the
 * whole family shares it. It reads:
 *
 *     r0 = an {x, y, z} triple, r1 = the entity to skip (the caller itself).
 *
 * r1 is overwritten by `mov r1, r2` before it is ever read, so there is no
 * second argument. This is the class of error docs/attribution.md records:
 * mechanism right, purpose wrong. The C below takes one argument.
 *
 * The comparison really is mismatched between axes, as the annotation says:
 * x and z at whole-tile resolution (asr #20), y at 1/16 (asr #16, with 0xffff
 * added first to round negatives toward zero).
 *
 * PROGRESS: THE INSTRUCTION SEQUENCE NOW MATCHES. Splitting the table's
 * address from its dereference -- taking `*(char **)iwram_3001ebc` into a
 * local, then adding 0x34 in a second statement -- reproduces the ROM's
 * prologue shape exactly:
 *
 *     rom    ldr r3,=0x3001ebc / mov r4,r0 / ldr r2,[r3] / ldr r3,[r4] / mov r1,r2
 *     ours   ldr r3,=0x3001ebc / mov r1,r0 / ldr r2,[r3] / ldr r3,[r1] / mov r4,r2
 *
 * Same instructions, same order. The ONLY difference left in the whole
 * function is that r4 and r1 are exchanged: the ROM holds the position
 * argument in r4 and the table pointer in r1, and we do the reverse.
 *
 * Both are live across the entire loop, so this is purely which pseudo the
 * allocator sees first. Four ways of changing that have failed:
 *
 *   3. `px = pos->x >> 20` hoisted above the table load (moves the pool load
 *      to after the argument, diverging at 1 instead of 2 -- worse)
 *   4. the table address split from the dereference (this one; diverges at 2)
 *   5. as 4 with `px` declared before `p`
 *   6. as 4 with the argument copied into its own local first
 *   7. declaring the second argument the annotation claims exists, unused, in
 *      case its presence changed the allocation. gcc drops it entirely; the
 *      output is identical.
 *
 * The declaration order does not reach the allocator, and neither does the
 * statement order once the shape is right.
 *
 * A NOTE ON WHY THIS IS ODD. gcc-2.96 allocates by priority, roughly usage
 * frequency over the live range, not by birth order -- "birth order" in
 * docs/elevation.md is a simplification that happens to hold for short
 * functions. Here `pos` is dereferenced two or three times per iteration and
 * the table pointer once, so `pos` should win the cheaper register, and in our
 * output it does: r1. The ROM gives the cheaper register to the table pointer
 * instead, which is what a HIGHER usage count for the table would produce.
 *
 * That suggests the original source touched the table more often than this
 * does -- for instance if the loop bound were expressed against the pointer
 * rather than the separate counter the ROM keeps in r5. Not yet tested,
 * because the ROM plainly does keep a counter.
 *
 * WHAT THIS IS WORTH. Seventeen functions share this body. Solving the
 * register exchange solves all of them, and the remaining distance is one
 * allocation decision -- not a codegen shape, not a missing symbol, not a
 * scheduling difference.
 */
struct Vec { int x, y, z; };
struct Ent { unsigned char pad_00[8]; int x, y, z; };

extern unsigned char iwram_3001ebc[];

struct Ent *OvlFunc_883_200806c(struct Vec *pos)
{
    struct Ent **p;
    struct Ent *e;
    int i;
    int a;
    int b;

    p = (struct Ent **)((char *)*(void **)iwram_3001ebc + 0x34);
    i = 8;
    do {
        e = *p++;
        if ((pos->x >> 20) == (e->x >> 20)) {
            a = pos->y;
            if (a < 0)
                a += 0xffff;
            b = e->y;
            if (b < 0)
                b += 0xffff;
            if ((a >> 16) == (b >> 16)) {
                if ((pos->z >> 20) == (e->z >> 20))
                    return e;
            }
        }
        i++;
    } while (i <= 0x41);
    return 0;
}
