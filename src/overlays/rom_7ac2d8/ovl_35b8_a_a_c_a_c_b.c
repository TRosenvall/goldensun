/* OvlFunc_924_200bbd4  --  0x0200bbd4
 *
 * Cut from goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c.s (fifth of
 * ten). All the file's data -- three jump tables -- sits well after the target
 * inside a later function, so the head piece needed no data handling. Split
 * verified byte-neutral before this landed.
 *
 * Scatters a particle at a randomised offset from a given point and hands a
 * small parameter block to the shared spawner.
 *
 * A CONSTANT BIAS IN A FIXED-POINT OFFSET HAS TWO PLACEMENTS, AND THE ROM SAYS
 * WHICH. The corpus's inherited spelling folds the bias in BEFORE the shift and
 * emits a subtract-then-shift pair. This ROM instead materialises the biased
 * constant from the pool and adds it AFTER the shift, as a second accumulate.
 * Splitting it into two compound assignments on the same lvalue -- accumulate
 * the shifted random, then subtract the shifted bias -- is the whole fix, 13 to
 * 0. That is the recorded every-lever-has-a-placement rule extended to a
 * CONSTANT ADDEND: the bias was present in the inherited spelling and only its
 * position was wrong.
 *
 * ACCUMULATE INTO THE PARAMETER, DO NOT REBUILD IT. Writing the same arithmetic
 * as one non-compound expression is strictly worse, 20 against 0: gcc stops
 * accumulating destructively into the parameter's own register and emits
 * three-operand adds, which raises pressure enough to widen the push set and
 * rotate all three argument registers. That confirms the recorded lever on a
 * function with NO LOADS AT ALL, which widens it -- the tell is not a
 * destructive LOAD, it is a destructive ADD on the parameter's register.
 *
 * Worth keeping as a diagnostic caution: a register rotation AND a widened push
 * appeared together here, and both were caused by an arithmetic spelling rather
 * than by allocation order. Reaching for the allocation-priority test on that
 * evidence would have been wrong.
 *
 * STRUCT-MEMBER INITIALISER ORDER IS DIRECT, NOT INVERTING, and this scopes a
 * recorded note. That note says two initialisers come out in the OPPOSITE order
 * to their assignments -- but it is about constants loaded into CALLEE-SAVED
 * REGISTERS before any branch. For stores through a named struct base the
 * relationship is the plain one: source order is emission order. Writing the
 * fields in the ROM's store order matches; the natural ascending order costs 4.
 * Only the first two fields are order-sensitive -- the other two share a value
 * and are pinned by CSE regardless.
 *
 * The greps supplied everything structural: searching the callee set and one
 * distinctive constant, never the stem, landed two CROSS-BANK solved files that
 * handed over the parameter block's layout, the eight-argument prototype and the
 * fixed-point random idiom verbatim. No stem-sibling would have given any of it.
 * Third consecutive round where callee-set identity beat filename adjacency.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_924_200bbd4(int x, int y, int z)
{
    struct P t;

    t.f4 = 7;
    t.f0 = 1;
    t.f8 = 0xb333;
    t.fc = 0xb333;
    x += (__Random() * 16 >> 16) << 16;
    x -= 8 << 16;
    z += (__Random() * 8 >> 16) << 16;
    z -= 4 << 16;
    OvlFunc_common0_10c(x, y, z, 0, 0, 0, 0xb0000, &t);
}
