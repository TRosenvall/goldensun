/* OvlFunc_common2_254  [overlays/common]
 *
 * Source asm: goldensun/asm/overlays/common/common2_a_a.s
 *
 * BLOCKER CLASS: adjacent-store scheduling. TWO instructions of 23, and they
 * are the same two instructions in the opposite order.
 *
 *     rom    str r2, [r5, #0x0] / str r3, [r5, #0x4]
 *     ours   str r3, [r5, #0x4] / str r2, [r5, #0x0]
 *
 * REWRITTEN batch 152. The previous note put this at "twenty-three against
 * twenty-three, diverging from the first" and named the blocker as register
 * birth order plus addressing form. All of that is now solved -- see the
 * elevated sibling src/overlays/common/common2_a_b.c, which is this exact
 * function plus a sign flip and matches byte for byte. What is left here is
 * one adjacent pair of stores that gcc schedules the other way round.
 *
 * The three shapes that took it from 21 differing to 2, all carried over from
 * the sibling: the two 8-byte operands are subobjects of ONE struct with a
 * pointer local to each; the second operand's pointer is assigned AFTER the
 * first operand's stores, which is what fixes the r5/r6 naming; and the frame
 * is laid out by declaration order, last-declared lowest.
 *
 * WHY THE SIBLING MATCHES AND THIS DOES NOT. OvlFunc_common2_28c is identical
 * except that it flips the second operand's sign word between the two decode
 * calls. That extra work changes the scheduler's decision about the store
 * pair, and it comes out in the ROM's order. So the residue here is genuinely
 * a scheduling coin flip, not a missing source shape -- the same source, with
 * three more instructions after it, schedules correctly.
 *
 * MEASURED, all at 2 differing or worse:
 *   swapping the two assignments in the source            2  (gcc renormalises)
 *   writing the pair as a struct assignment `*pb = tmp`  23  (much worse)
 *   assigning both pointers up front                     12
 *   storing the second operand's pair first              12
 *   -fno-schedule-insns2                                 12
 *   -fno-schedule-insns                                   2  (no effect)
 *   -fno-schedule-insns -fno-schedule-insns2             12
 *
 * Note the two scheduler flags make it WORSE, which is the useful negative:
 * the pass that orders these two stores is not either -fschedule-insns pass.
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

double OvlFunc_common2_254(int alo, int ahi, int blo, int bhi) {
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
    return OvlFunc_common2_44c(OvlFunc_common2_0(&ra, &rb, &third));
}
