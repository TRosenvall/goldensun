/* OvlFunc_common2_618  [overlays/common]
 *
 * Source asm: goldensun/asm/overlays/common/common2_c_c_c_c_c_c_c_c_c_c.s
 *
 * BLOCKER CLASS: three separate gcc-is-cleverer residues. 38 differing of a
 * 102-line ROM, ours 98. Reachable at all only under COMMON2_CFLAGS with
 * -fcall-saved-r4 (batch 151); the whole register allocation -- r4, r5, r6,
 * r7 and the five-register push -- now matches the ROM exactly, as do the
 * frame, the block order and every branch sense.
 *
 * WHAT THIS FUNCTION IS. IEEE-754 double decomposition, the front half of the
 * overlay's software-float library. It reads a double as two words, extracts
 * sign/exponent/mantissa, and fills the five-word record (kind, sign, exp, lo,
 * hi) that OvlFunc_common2_0 and OvlFunc_common2_44c consume. kind is 2 for
 * zero, 4 for infinity, 1/0 for quiet/signalling NaN, 3 for a finite value.
 *
 * TWO READINGS THAT ARE SETTLED AND SHOULD NOT BE RE-DERIVED:
 *
 *   1. WORD 0 OF THE INPUT IS THE HIGH WORD. `lsl r3, #12 / lsr r6, r3, #12`
 *      takes the low 20 bits of [r0, #0] as the mantissa's top, and the
 *      exponent and sign come out of the same word. That is ARM's legacy
 *      mixed-endian double layout, and it agrees with OvlFunc_common2_304,
 *      whose INT_MIN path loads r0 = 0xc1e00000 / r1 = 0.
 *   2. THE STACK SCRATCH IS A UNION, READ AT DIFFERENT WIDTHS. The ROM copies
 *      both words to an 8-byte stack buffer and then reads `ldrh [r2, #6]` and
 *      `ldrb [r2, #7]`. Writing this as a union of two u32, a u16[4] and a
 *      u8[8] gives `(u.h[3] >> 4) & 0x7ff` and `u.b[7] >> 7`, and gcc emits
 *      the ROM's exact `lsl #17 / lsr #21` and `lsr #7`. It needs a POINTER
 *      LOCAL to the union: without one gcc scalarises the buffer away and
 *      computes both fields from the register, 19 instructions short.
 *   3. THE MANTISSA IS ONE unsigned long long. The ROM's shifts compute into
 *      temporaries and then `mov` into a fixed register pair -- the signature
 *      of a DImode shift, not of two u32s shifted in place. Modelling it as
 *      two u32 locals is 14 instructions short; as a union of u64 and two u32
 *      halves it is within four, and the register allocation snaps to the
 *      ROM's. The implicit mantissa bit is likewise a 64-BIT or: the ROM loads
 *      the constant as a PAIR, `ldr r1, =0x0` beside `ldr r2, =0x10000000`,
 *      and the low half's load is left dead.
 *
 * WHAT IS LEFT, all three of them gcc doing something smarter than the ROM:
 *
 *   a. The offset-0 store folds to `[sp, #0]` where the ROM stores through
 *      `mov r2, sp`. The batch-151/152 fixes do not apply: making the union a
 *      subobject of a wrapper struct changes nothing here (measured, identical
 *      38), because a single pointer at offset 0 is what the ROM has too.
 *   b. The quiet-NaN test. The ROM tests bit 51 of the 64-bit mantissa and
 *      builds the boolean with cmp/beq/mov #1; gcc folds the whole thing to
 *      `(hi >> 19) & 1`. Writing it as an explicit `k = 0; if (...) k = 1;`
 *      restores the branch but costs eight lines elsewhere (107 vs 102).
 *   c. gcc CROSS-JUMPS the normal path's two stores into the shared tail that
 *      the denormal and NaN paths use; the ROM duplicates them inline. Giving
 *      the normal path its own union variable, so the stores come from
 *      different registers, does not stop it (measured, still 38).
 *
 * Note (b) and (c) pull against each other: every spelling that restores one
 * costs more than it saves on the others. That is why this is parked at 38
 * rather than at a single identified instruction.
 */
struct Dec {
    int kind;
    int sign;
    int exp;
    unsigned int lo;
    unsigned int hi;
};

struct Pair {
    unsigned int hi;
    unsigned int lo;
};

union Bits {
    struct { unsigned int lo; unsigned int hi; } w;
    unsigned short h[4];
    unsigned char b[8];
};

union M {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } w;
};

void OvlFunc_common2_618(struct Pair *in, struct Dec *out)
{
    union Bits u;
    union Bits *p;
    union M m;
    unsigned int low;
    unsigned int hi;
    unsigned int e;

    p = &u;
    low = in->lo;
    p->w.lo = low;
    hi = in->hi;
    p->w.hi = hi;
    m.w.hi = (hi << 12) >> 12;
    e = (p->h[3] >> 4) & 0x7ff;
    m.w.lo = low;
    out->sign = p->b[7] >> 7;
    if (e == 0) {
        if ((low | m.w.hi) == 0) {
            out->kind = 2;
            return;
        }
        out->exp = -1022;
        m.q = m.q << 8;
        out->kind = 3;
        while (m.w.hi <= 0xfffffff) {
            m.q = m.q << 1;
            out->exp = out->exp - 1;
        }
    } else if (e == 0x7ff) {
        if ((low | m.w.hi) == 0) {
            out->kind = 4;
            return;
        }
        out->kind = (m.q & 0x0008000000000000ULL) != 0;
    } else {
        out->exp = e - 1023;
        m.q = (m.q << 8) | 0x1000000000000000ULL;
        out->kind = 3;
        out->lo = m.w.lo;
        out->hi = m.w.hi;
        return;
    }
    out->lo = m.w.lo;
    out->hi = m.w.hi;
}
