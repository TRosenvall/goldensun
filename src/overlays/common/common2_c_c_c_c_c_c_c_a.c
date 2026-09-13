/* Cluster OvlFunc_common2_41c..OvlFunc_common2_44c extracted from
 * goldensun/asm/overlays/common/common2_c_c_c_c_c_c_c_a.s.  The file converts
 * WHOLE: both of its functions match, so no split is needed and the linker line
 *
 *     asm/overlays/common/common2_c_c_c_c_c_c_c_a.o(.text)
 *
 * stays verbatim in overlays/rom_7bf5a8/overlay.ld and
 * overlays/rom_7e7574/overlay.ld.
 *
 * Total .text for this TU = 460 bytes (48 + 412).  Built with COMMON2_CFLAGS
 * via the existing `asm/overlays/common/common2_%.o` pattern -- no new Makefile
 * rule.  objcmp reports "(built with: call-saved-r4, no-interwork)" for both.
 *
 * WHAT THESE TWO ARE.  The back half of the overlay's software-float library.
 * OvlFunc_common2_618 (still parked) decomposes a double into the five-word
 * record {kind, sign, exp, lo, hi} that src/overlays/common/common2_a_a.c
 * describes; these two put one back together.
 *
 *   OvlFunc_common2_41c is the 64-bit LOGICAL RIGHT SHIFT the rest of the
 *   library calls instead of writing `>>` (which would emit a reference to
 *   __lshrdi3, a symbol this overlay does not carry).  Split at the word
 *   boundary, with the `32 - n <= 0` arm reached by `neg`.
 *
 *   OvlFunc_common2_44c is the encoder: classify, denormalise or overflow,
 *   round to nearest with ties to even at bit 8, then drop the eight guard bits
 *   and lay sign/exponent/mantissa into an 8-byte stack image.  ARM's legacy
 *   double layout puts the HIGH word first -- the same reading that
 *   src/non_matching/ovl_common/common2_618.c records for the decoder -- which
 *   is why the assembled high word is built in word 1 and then swapped into
 *   word 0.
 *
 * ------------------------------------------------------------------
 * ROUTE, by aligned residue (tools/objcmp.py, of 191 encodings):
 *
 *     first draft, `if (x != 0x80) round_up; else if ...`       130
 *     + the tie arm written as the THEN arm, so the compare
 *       branches away on `!=` and 0xff stays live to become
 *       0x100 by `add r3, #1`                                    48
 *     + `f.v = d->u.v;` BEFORE `sign = d->sign;`                  6
 *     + the 8-byte stack image declared `volatile`                0
 *
 * Two of those are worth keeping hold of.
 *
 * THE ROUNDING ARMS.  The ROM's `cmp r1, #0x80 / bne .L500` falls through into
 * the tie case, and inside it reaches 0x100 as `add r3, #1` on the 0xff that is
 * still in r3.  Spelling the test `!=` with the increment in the ELSE arm
 * inverts the fallthrough, gcc loses the live 0xff and pays `mov r3, #0x80 /
 * lsl r3, #1` for a constant Thumb cannot materialise in one insn -- and the
 * two extra instructions move every block boundary after them.  82 of the 130.
 *
 * THE `volatile` ON THE STACK IMAGE.  This is the residue that
 * src/non_matching/ovl_common/common2_618.c parks on and calls unreachable:
 * "the offset-0 store folds to `[sp, #0]` where the ROM stores through
 * `mov r2, sp`".  It is reachable.  The fold is done by CSE2 (pass 09; it is
 * absent from .00.rtl and .03.cse and present in .09.cse2), which replaces a
 * bare-REG address by its known frame equivalent but leaves `(plus REG 4)`
 * alone -- so offset 0 comes out `[sp]` while every other offset keeps the
 * pointer.  `volatile` on the OBJECT blocks it; the already-recorded sub-lever
 * "The `volatile` belongs on the OBJECT, not the pointer" under the DMA-helper
 * signature is the same mechanism in a different costume, and a pointer local
 * is not needed here at all.
 *
 * Measured and eliminated on this residue, all at 3 differing:  a pointer local
 * to the union; a pointer local assigned late; `*(u32 *)o`; writing word 0 as
 * a bitfield member; swapping through `o->b.frac_lo`; adding a u64 member for
 * 8-byte alignment; a second addressable local; `return *(double *)o`.  A
 * pointer assigned in the ENTRY block does fix it -- different CSE2 extended
 * basic block -- but costs a seventh callee-saved register and an extra push.
 *
 * THE BITFIELD ACCESS WIDTHS are not a lever, they are a consequence: once the
 * image is memory-resident, get_best_mode picks the narrowest mode containing
 * each field, so the 20-bit mantissa is a word at +4, the 11-bit exponent a
 * HALFWORD at +6 and the sign a BYTE at +7 -- and Thumb has no SP-relative
 * ldrh/ldrb, which is where the ROM's `mov r0, sp` comes from.  Declaring the
 * exponent `unsigned short` and the sign `unsigned char` changes nothing
 * (measured, identical); leaving the image in registers makes all three SImode.
 */

typedef unsigned int u32;
typedef unsigned long long u64;

union Fu {
    u64 v;
    struct { u32 lo, hi; } w;
};

/* The five-word record OvlFunc_common2_618 fills in. */
struct Dec {
    int kind;
    int sign;
    int exp;
    union Fu u;
};

/* IEEE-754 double, in the order the fields sit once the two words are in
 * ascending memory order.  The finished value swaps them; see below. */
struct Bits {
    u32 frac_lo;
    u32 frac_hi : 20;
    u32 exp : 11;
    u32 sign : 1;
};

union Db {
    double value;
    struct Bits b;
    u32 w[2];
};

extern int OvlFunc_common2_5e8(struct Dec *d);   /* kind <= 1: NaN    */
extern int OvlFunc_common2_5f8(struct Dec *d);   /* kind == 4: infinity */
extern int OvlFunc_common2_608(struct Dec *d);   /* kind == 2: zero   */

u64 OvlFunc_common2_41c(u64 v, int n)
{
    union Fu in, out;
    int room;

    if (n == 0)
        return v;

    in.v = v;
    room = 32 - n;
    if (room <= 0) {
        out.w.hi = 0;
        out.w.lo = in.w.hi >> -room;
    } else {
        u32 carry = in.w.hi << room;
        out.w.hi = in.w.hi >> n;
        out.w.lo = (in.w.lo >> n) | carry;
    }
    return out.v;
}

double OvlFunc_common2_44c(struct Dec *d)
{
    volatile union Db out;
    union Fu f;
    int sign;
    int exp;
    u32 tmp;

    f.v = d->u.v;
    sign = d->sign;
    exp = 0;

    if (OvlFunc_common2_5e8(d)) {
        exp = 0x7ff;
        f.v |= 0x0008000000000000ULL;   /* make it quiet */
    } else if (OvlFunc_common2_5f8(d)) {
        exp = 0x7ff;
        f.v = 0;
    } else if (OvlFunc_common2_608(d)) {
        f.v = 0;
    } else if (f.v != 0) {
        int e = d->exp;
        if (e < -1022) {
            /* Denormal: shift right into place, keeping a sticky bit. */
            int shift = -1022 - e;
            if (shift > 56) {
                f.v = 0;
            } else {
                u32 sticky = 0;
                if (f.v & ((1 << shift) - 1))
                    sticky = 1;
                f.v = OvlFunc_common2_41c(f.v, shift) | sticky;
            }
            if ((f.v & 0xff) == 0x80) {
                if (f.v & 0x100)
                    f.v += 0x80;
            } else {
                f.v += 0x7f;
            }
            if (f.w.hi > 0x0fffffff)     /* rounded up out of denormal range */
                exp = 1;
            f.v >>= 8;
        } else if (e > 1023) {
            exp = 0x7ff;
            f.v = 0;
        } else {
            exp = e + 1023;
            if ((f.v & 0xff) == 0x80) {
                if (f.v & 0x100)
                    f.v += 0x80;
            } else {
                f.v += 0x7f;
            }
            if (f.w.hi > 0x1fffffff) {   /* rounding carried into bit 61 */
                f.v >>= 1;
                exp++;
            }
            f.v >>= 8;
        }
    }

    out.b.frac_hi = f.w.hi;
    out.b.exp = exp;
    out.b.sign = sign;

    /* ARM stores a double's HIGH word first, so put the two words that way. */
    tmp = out.w[1];
    out.w[1] = f.w.lo;
    out.w[0] = tmp;
    return out.value;
}
