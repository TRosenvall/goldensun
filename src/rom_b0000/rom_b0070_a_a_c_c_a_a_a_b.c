/* Cluster Func_80b0958..Func_80b0958 extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s.
 *
 * Total .text for this TU = 164 bytes (= 0xa4).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_a_a_a.o and asm/rom_b0000/rom_b0070_a_a_c_c_a_a_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 273. No pins, no flags.
 *
 * NAMING A LOADED VALUE DECIDED IT, AND DECLARATION ORDER WAS INERT. The first
 * candidate was 6 differing, an r1/r2 swap through the 0x16 bit-field block. With the
 * `b->f16` load left ANONYMOUS, no declaration order of the two mask locals reached the
 * ROM -- five differing at best. Once the load was given its own local, ALL SIX
 * permutations of the three locals matched.
 *
 * That is worth keeping beside the declaration-order tie-break recorded in batch 272:
 * there the order was the lever and the naming was not; here the naming is the lever
 * and the order is inert. The discriminator is whether the competing value is a NAMED
 * pseudo at all -- an anonymous load has no allocno to tie with, so there is no tie for
 * an order to break.
 *
 * The structs and the `t = 0xfffffe00; t &= ...` mask spelling came verbatim from
 * src/non_matching/rom_b0000/80b0a20.c -- a PARK, which is worth noting as a file-mate
 * source in its own right: a park carries a candidate and its measurements even when
 * its own function is unsolved.
 */
struct B {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[0xa];
    unsigned char f14;
    unsigned char pad15;
    unsigned short f16;
};

struct A {
    struct B *p;
    unsigned char pad04[4];
    short f8;
    short fa;
};

void Func_80b0958(struct A *a)
{
    struct B *b;
    int d;
    int s;
    int m;
    int t;
    int u;

    b = a->p;
    if (b == 0)
        return;

    d = b->f6 - a->f8;
    s = d / 4;
    if (s < 0)
        s = -s;
    if (d > 0) {
        if (s != 0)
            b->f6 = b->f6 - s;
        else
            b->f6 = b->f6 - 1;
    } else if (d < 0) {
        if (s != 0)
            b->f6 = b->f6 + s;
        else
            b->f6 = b->f6 + 1;
    } else
        goto second;
    m = 0x1ff;
    m &= b->f6;
    u = b->f16;
    t = 0xfffffe00;
    t &= u;
    t |= m;
    b->f16 = t;
second:
    d = b->f8 - a->fa;
    s = d / 4;
    if (s < 0)
        s = -s;
    if (d > 0) {
        if (s != 0)
            b->f8 = b->f8 - s;
        else
            b->f8 = b->f8 - 1;
    } else if (d < 0) {
        if (s != 0)
            b->f8 = b->f8 + s;
        else
            b->f8 = b->f8 + 1;
    } else
        return;
    b->f14 = b->f8;
}
