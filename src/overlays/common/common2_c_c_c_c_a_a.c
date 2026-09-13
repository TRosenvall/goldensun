/* OvlFunc_common2_304 extracted from goldensun/asm/overlays/common/common2_c_c_c_c_a_a.s.
 *
 * The whole TU is this one function. Total .text = 124 bytes (= 0x7c),
 * including its three literal-pool words. Preserves the original ROM layout
 * with the linker lines unchanged:
 *
 *     overlays/rom_7bf5a8/overlay.ld:65   asm/overlays/common/common2_c_c_c_c_a_a.o(.text)
 *     overlays/rom_7e7574/overlay.ld:133  asm/overlays/common/common2_c_c_c_c_a_a.o(.text)
 *
 * Built with COMMON2_CFLAGS -- no -mthumb-interwork, and -fcall-saved-r4
 * rather than the tree-wide -fcall-used-r4. Already covered by the existing
 * asm/overlays/common/common2_%.o pattern; no new Makefile rule is needed.
 *
 * This is int -> double in the overlay's software-float library. It fills the
 * five-word record that OvlFunc_common2_618 produces and OvlFunc_common2_44c
 * packs: kind, sign, exponent, and a 64-bit mantissa. INT_MIN cannot be
 * negated in 32 bits, so it returns the packed double directly.
 *
 * THREE THINGS MAKE IT MATCH, and each was measured against the 52-differing
 * park it replaces (src/non_matching/ovl_common/common2_304.c):
 *
 *   - THE MANTISSA IS ONE 64-BIT OBJECT, READ BACK THROUGH A UNION. The loop
 *     body is gcc's inline DImode `<<= 1` -- two loads, lsr/lsl/mov/lsl/orr,
 *     two stores -- which only appears if the source shifts a single 8-byte
 *     object. The loop CONDITION tests the high word alone, so the union's
 *     w.hi is what reads it; gcc cannot forward a DImode store to an SImode
 *     load, and that is the ROM's `ldr r3, [r0, #0x10]` re-read after the
 *     store pair. As two separate `int lo, hi` members the park was six
 *     instructions SHORT. 52 differing -> 30.
 *
 *   - THE ENTRY TEST IS AN `if` AROUND A `do`/`while`, NOT A `while`. Written
 *     as a plain `while`, gcc gives the loop body and the loop-exit test two
 *     separate address pseudos and copies between them inside the loop. The
 *     `if` + `do` form produces the ROM's single preheader pair
 *     `mov r0, r5 / mov r12, r2` and one base register for the whole loop.
 *     30 differing -> 29, and it is what frees r12 for the loop bound.
 *
 *   - `exp` IS `volatile`. The ROM re-loads, decrements and re-stores the
 *     exponent from its stack slot on every iteration; gcc's loop pass
 *     register-promotes that MEM into a callee-saved register, hoisting the
 *     load into the preheader and sinking the store past the loop -- which
 *     costs the extra `push {r6}` and renames the frame pointer r5 -> r6.
 *     This is the documented "a `volatile` local reproduces a value the ROM
 *     keeps in a stack slot" signature, and it is an INFERENCE: a plain local
 *     that the original compiler simply declined to promote gives the same
 *     output. Stated as a reading, not as a claim about the original source.
 *     29 differing -> EXACT.
 *
 * WHY THE RETURN TYPE IS `long long` AND NOT `double`. This gcc build cannot
 * emit a floating literal: every DFmode constant except 0.0 and 1.0 comes out
 * of the constant pool as the uninitialised pattern 0xafafafaf (see the
 * elevation note). `-2147483648.0` is 0xc1e00000_00000000, and a 64-bit
 * INTEGER constant of that bit pattern pools correctly and loads with the
 * ROM's exact `ldr r1, .L+4 / ldr r0, .L` pair -- the word order works out
 * because ARM's legacy mixed-endian double puts the high word first, which is
 * word 0, which is also a little-endian long long's LOW word. The declared
 * type is therefore the library's 64-bit value rather than `double`; the
 * calling convention (r0/r1) is identical either way, so the declaration of
 * OvlFunc_common2_44c here differs only in spelling from the `double` one in
 * common2_a_b.c.
 *
 * MEASURED AND REJECTED (all with the corrected COMMON2_CFLAGS screen):
 *   park's two `int lo, hi` mantissa members                   52
 *   64-bit mantissa, `double` return (garbage pool word)       37
 *   64-bit mantissa, `long long` return, plain `while`         30
 *   `if` + `do`/`while`, exp not volatile                      29
 *   ... plus an `int` member in the union (alias lever)        29
 *   ... plus a `char c[8]` member in the union                 29
 *   ... plus signed `int lo, hi` union words                   29
 *   ... plus exp in its own `union { int; char[4]; }`          29
 *   ... plus exp decremented first in the loop body            29
 *   a `struct Dec *p` pointer local for the loop               52 / 60
 *   `volatile int *ep = &d.exp;` (use-site volatile)           51
 *   `*(volatile int *)&d.exp` at both accesses                 56
 *   `*(volatile int *)&d.exp` in the loop only                 25
 *
 * The alias lever does NOT reach this promotion: gcc's loop MEM promotion
 * compares the constant offsets off one known base, so no amount of
 * type-punning in the union makes the mantissa stores look like they might
 * hit `exp`. Only `volatile` does.
 */

struct Dec {
    int kind;
    int sign;
    volatile int exp;
    union {
        unsigned long long q;
        struct {
            unsigned int lo;
            unsigned int hi;
        } w;
    } m;
};

extern long long OvlFunc_common2_44c(struct Dec *d);

long long OvlFunc_common2_304(int x)
{
    struct Dec d;
    unsigned int sign;

    d.kind = 3;
    sign = (unsigned int)x >> 31;
    d.sign = sign;
    if (x == 0) {
        d.kind = 2;
        goto done;
    }
    d.exp = 0x3c;
    if (sign != 0) {
        if (x == (int)0x80000000)
            return 0xc1e00000LL;    /* -2147483648.0 */
        d.m.q = (long long)(-x);
    } else {
        d.m.q = (long long)x;
    }
    if (d.m.w.hi <= 0xfffffff) {
        do {
            d.m.q <<= 1;
            d.exp--;
        } while (d.m.w.hi <= 0xfffffff);
    }
done:
    return OvlFunc_common2_44c(&d);
}
