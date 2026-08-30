/* Cluster OvlFunc_common2_28c..OvlFunc_common2_28c extracted from goldensun/asm/overlays/common/common2_a.s.
 *
 * Total .text for this TU = 64 bytes (= 0x40).
 * Preserves the original ROM layout when slotted after
 * asm/overlays/common/common2_a_a.o in goldensun/overlays/rom_7bf5a8/overlay.ld
 * (and identically in rom_7e7574).
 *
 * Built with COMMON2_CFLAGS -- no -mthumb-interwork, and -fcall-saved-r4
 * rather than the tree-wide -fcall-used-r4. The Makefile pattern was widened
 * from common2_c% to common2_% for this file; common2_a is the same TU.
 *
 * This is subtraction in the overlay's software-float library: both operands
 * are decoded by OvlFunc_common2_618 into a five-word record, the second
 * operand's sign word is flipped, and OvlFunc_common2_0 adds them.
 *
 * THREE SOURCE SHAPES were needed, and each was measured:
 *
 *   - The two 8-byte operands are SUBOBJECTS of one struct, with a pointer
 *     local to each. Two separate locals leave both offset-0 stores folded to
 *     `[sp, #N]` where the ROM stores through the address register (8 of 23
 *     differing); as members of one struct, `&v.a` and `&v.b` are real
 *     subobject addresses and both pairs store through them. This is the
 *     batch-151 rule, but note it needs TWO pointers here, not one: a single
 *     base register with offsets 0/4/8/0xc is what the one-struct form gives
 *     if you point at the struct itself, and that is NOT what the ROM does.
 *   - The POINTER BIRTH ORDER decides the register names. Assigning both
 *     pointers up front gives &rb the lower register and swaps r5/r6 through
 *     the whole body (8 differing). The ROM's order is &a, &ra, &b, so the
 *     second operand's pointer must be assigned AFTER the first operand's
 *     stores. That one move is 8 differing -> 3.
 *   - The frame is laid out by DECLARATION ORDER, last-declared lowest.
 *     Offsets run v=0x00, third=0x10, rb=0x24, ra=0x38, so the declarations
 *     run in exactly the opposite order.
 *
 * Its sibling OvlFunc_common2_254 is the same function without the sign flip
 * and does NOT match -- see src/non_matching/ovl_common/common2_254.c. The
 * flip is what makes this one land: it changes the scheduling of the second
 * operand's store pair, which is the single thing 254 cannot reach.
 */

struct Dec {
    int kind;
    int sign;
    int exp;
    int lo;
    int hi;
};

struct Pair {
    int lo;
    int hi;
};

struct Args {
    struct Pair b;
    struct Pair a;
};

extern void OvlFunc_common2_618(struct Pair *v, struct Dec *d);
extern struct Dec *OvlFunc_common2_0(struct Dec *a, struct Dec *b, struct Dec *out);
extern double OvlFunc_common2_44c(struct Dec *d);

double OvlFunc_common2_28c(int alo, int ahi, int blo, int bhi) {
    struct Dec ra;
    struct Dec rb;
    struct Dec third;
    struct Args v;
    struct Pair *pa;
    struct Pair *pb;

    pa = &v.a;
    pa->lo = alo;
    pa->hi = ahi;
    pb = &v.b;
    pb->lo = blo;
    pb->hi = bhi;
    OvlFunc_common2_618(pa, &ra);
    OvlFunc_common2_618(pb, &rb);
    rb.sign = rb.sign ^ 1;
    return OvlFunc_common2_44c(OvlFunc_common2_0(&ra, &rb, &third));
}
