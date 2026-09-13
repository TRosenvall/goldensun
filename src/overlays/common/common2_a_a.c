/* Cluster OvlFunc_common2_0..OvlFunc_common2_254 extracted from
 * goldensun/asm/overlays/common/common2_a_a.s.  The file converts WHOLE:
 * both of its functions match, so no split is needed and the linker line
 *
 *     asm/overlays/common/common2_a_a.o(.text)
 *
 * stays verbatim in overlays/rom_7bf5a8/overlay.ld and
 * overlays/rom_7e7574/overlay.ld.
 *
 * Total .text for this TU = 652 bytes.  Built with COMMON2_CFLAGS via the
 * existing `asm/overlays/common/common2_%.o` pattern -- no new Makefile rule.
 * MEASURED on the whole object:
 *
 *     no-interwork + -fcall-saved-r4 (COMMON2_CFLAGS)   EXACT
 *     + -mthumb-interwork                                33 differ
 *     no-interwork, tree-default -fcall-used-r4         310 differ
 *
 * so the common2_% wildcard is CORRECT for this TU, both halves of it.
 *
 * This is the overlay software-float library: OvlFunc_common2_0 is the add on
 * the five-word decoded record that OvlFunc_common2_618 produces, and
 * OvlFunc_common2_254 is the double(double,double) entry point that decodes
 * both operands, adds, and re-encodes through OvlFunc_common2_44c.  Its
 * sibling OvlFunc_common2_28c (subtraction, src/overlays/common/common2_a_b.c)
 * is the same entry point with a sign flip.
 *
 * ------------------------------------------------------------------
 * OvlFunc_common2_254 -- was parked at 2 differing since batch 152.
 *
 * The park was right about the residue (one adjacent store pair, the ROM's
 * `str r2,[r5] / str r3,[r5,#4]` emitted the other way round) and wrong about
 * it being unreachable.  It is not a scheduling coin flip: reading the
 * .23.sched2 dependence table, the two stores TIE on priority (133/133) and on
 * class, and rank_for_schedule falls through to DEPENDENT COUNT, where
 * `str r3` wins 6 to 5.  The asymmetry is that r2 is redefined by
 * `add r2, sp, #0x10` (the third argument of the inner call), which kills
 * reg_last_uses[r2] and with it the store's anti-dependence on the last two
 * calls; r3 is never redefined, so its store keeps an anti-dep on all four.
 * The matched sibling ties 5-5 because its sign flip contributes exactly one
 * insn setting each register -- and on a tie LUID picks the ROM's order.
 *
 * The fix removes the tie instead of winning it.  The two 8-byte operands are
 * 64-bit objects, so writing them with a 64-bit TYPE makes each pair ONE
 * *thumb_movdi_insn, which emits both halves in ROM order with nothing for
 * sched2 to reorder.  `double` and `long long` both reach EXACT; `double` is
 * kept because that is what the function's ABI actually is.
 *
 * ------------------------------------------------------------------
 * OvlFunc_common2_0 -- never attempted before.  Route, by aligned residue
 * (tryc --align, of 306 instructions):
 *
 *     first draft, struct with int lo/hi, early-return chain    220
 *     + `if (P) { ... } return a;` nesting instead of `if (!P) return a;`
 *       and the else-if form of the exponent-range test         119
 *     + union { u64 v; struct { u32 lo, hi; } w; } so the final
 *       overflow test reads ONLY the high word                  109
 *     + sticky shift written `(x & 1) | (x >> 1)`                26
 *     + the loop counter bumped BEFORE the shift                EXACT
 *
 * Two of those are worth keeping hold of.  The union matters because
 * `out->u.w.hi > 0x1fffffff` is a single ldr/cmp where any spelling that reads
 * the whole 64-bit value makes gcc hoist both words out of the branch.  And
 * the OR operand order is not a local scheduling nudge: flipping the two
 * commutative operands of the 64-bit sticky shift changed the number of low
 * registers the loop body needs, which pushed a pseudo into HI_REGS and
 * renamed ea/eb/the loop constant to r14/r12/r11 across the whole function --
 * 109 disagreeing instructions down to 26, none of them in the loops' own
 * arithmetic.
 */

typedef unsigned long long u64;
typedef long long s64;

struct Frac {
    unsigned int lo;
    unsigned int hi;
};

union Fu {
    u64 v;
    struct Frac w;
};

struct Dec {
    int kind;
    int sign;
    int exp;
    union Fu u;
};

extern int OvlFunc_common2_2d4(struct Dec *d);
extern int OvlFunc_common2_2e4(struct Dec *d);
extern int OvlFunc_common2_2f4(struct Dec *d);
extern struct Dec *OvlFunc_common2_2cc(struct Dec *a, struct Dec *b);
extern void OvlFunc_common2_618(double *v, struct Dec *d);
extern double OvlFunc_common2_44c(struct Dec *d);

struct Args {
    double b;
    double a;
};

struct Dec *OvlFunc_common2_0(struct Dec *a, struct Dec *b, struct Dec *out)
{
    int ea, eb, d;
    u64 fa, fb;
    s64 diff;

    if (OvlFunc_common2_2d4(a)) return a;
    if (OvlFunc_common2_2d4(b)) return b;

    if (OvlFunc_common2_2e4(a)) {
        if (OvlFunc_common2_2e4(b) && a->sign != b->sign)
            return OvlFunc_common2_2cc(a, b);
        return a;
    }
    if (OvlFunc_common2_2e4(b)) return b;

    if (OvlFunc_common2_2f4(b)) {
        if (OvlFunc_common2_2f4(a)) {
            *out = *a;
            out->sign = a->sign & b->sign;
            return out;
        }
        return a;
    }
    if (OvlFunc_common2_2f4(a)) return b;

    ea = a->exp;
    eb = b->exp;
    fa = a->u.v;
    fb = b->u.v;
    d = ea - eb;
    if (d < 0) d = -d;
    if (d <= 63) {
        while (ea > eb) {
            eb++;
            fb = (fb & 1) | (fb >> 1);
        }
        while (eb > ea) {
            ea++;
            fa = (fa & 1) | (fa >> 1);
        }
    } else if (ea > eb) {
        fb = 0;
    } else {
        ea = eb;
        fa = 0;
    }

    if (a->sign != b->sign) {
        if (a->sign) diff = fb - fa;
        else diff = fa - fb;
        if (diff >= 0) {
            out->sign = 0;
            out->exp = ea;
            out->u.v = diff;
        } else {
            out->sign = 1;
            out->exp = ea;
            out->u.v = -diff;
        }
        while (out->u.v - 1 < 0x0FFFFFFFFFFFFFFFULL) {
            out->u.v <<= 1;
            out->exp--;
        }
    } else {
        out->sign = a->sign;
        out->exp = ea;
        out->u.v = fa + fb;
    }

    out->kind = 3;
    if (out->u.w.hi > 0x1FFFFFFFU) {
        out->u.v = (out->u.v & 1) | (out->u.v >> 1);
        out->exp++;
    }
    return out;
}

double OvlFunc_common2_254(double a, double b) {
    struct Dec ra;
    struct Dec rb;
    struct Dec third;
    struct Args v;
    double *pa;
    double *pb;

    pa = &v.a;
    *pa = a;
    pb = &v.b;
    *pb = b;
    OvlFunc_common2_618(pa, &ra);
    OvlFunc_common2_618(pb, &rb);
    return OvlFunc_common2_44c(OvlFunc_common2_0(&ra, &rb, &third));
}
