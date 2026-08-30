/* OvlFunc_common2_304  [overlays/common]
 *
 * Source asm: goldensun/asm/overlays/common/common2_c_c_c_c_a_a.s
 *
 * BLOCKER CLASS: register allocation plus store-forwarding. 52 differing
 * against a 62-line ROM, ours 56 -- six instructions SHORT, and the six are
 * work gcc optimises away rather than work we are missing.
 *
 * NEW in batch 152: reachable at all only because COMMON2_CFLAGS now carries
 * -fcall-saved-r4 (batch 151). The prologue, epilogue and the whole frame
 * layout are right; what follows is the residue.
 *
 * The function is int -> double in the overlay's software-float library. It
 * fills a five-word record on the stack -- kind, sign, exponent, and a 64-bit
 * mantissa as lo/hi -- normalises the mantissa left until the high word
 * exceeds 0xfffffff, decrementing the exponent, and hands the record to
 * OvlFunc_common2_44c to be packed. INT_MIN is special-cased to a pooled
 * double constant, which is how we know the return type: the pair is loaded
 * r0 = 0xc1e00000, r1 = 0, so the HIGH word is in r0. That is ARM's legacy
 * mixed-endian double layout (FLOAT_WORDS_BIG_ENDIAN), and it confirms the
 * record-to-double convention for the rest of this library.
 *
 * THREE THINGS DIFFER, and they are independent:
 *
 *   1. The ROM copies sp into a CALLEE-SAVED register (`mov r5, sp`) and keeps
 *      it; gcc uses r4 and rematerialises sp. Because the record is the whole
 *      frame, `&d == sp` exactly, so gcc can always rematerialise -- the same
 *      root cause as the offset-0 folding in batch 151, and here there is no
 *      second object to merge it with.
 *   2. The ROM holds the loop bound 0xfffffff in r12 across the loop
 *      (`ldr r2, =0xfffffff` / `mov r12, r2`); gcc hoists the pool load far
 *      earlier and keeps it in a low register.
 *   3. The ROM RE-READS the high word from memory to test the loop condition
 *      after storing it; gcc forwards the store and tests the value it just
 *      computed. That is two of the six missing instructions.
 *
 * MEASURED:
 *   `d.` field access, no pointer local                        52  (best)
 *   `struct Dec *p = &d;` and p-> throughout                   62
 *   a second pointer local for the loop only                   62
 *   computing lo/hi into temporaries and storing lo first      60
 *   -fno-schedule-insns2 on either shape                       no change
 *
 * The pointer local makes it WORSE here, which is the useful negative: the
 * batch-151 stack-vector rule does not apply to a record that IS the frame.
 */

struct Dec {
    int kind;
    int sign;
    int exp;
    int lo;
    int hi;
};
extern double OvlFunc_common2_44c(struct Dec *d);

double OvlFunc_common2_304(int x)
{
    struct Dec d;
    int lo;
    int hi;
    int sign;
    sign = (unsigned int)x >> 31;
    d.kind = 3;
    d.sign = sign;
    if (x == 0) {
        d.kind = 2;
        goto done;
    }
    d.exp = 0x3c;
    if (sign != 0) {
        if (x == (int)0x80000000)
            return -2147483648.0;
        lo = -x;
        hi = lo >> 31;
    } else {
        lo = x;
        hi = x >> 31;
    }
    d.lo = lo;
    d.hi = hi;
    while ((unsigned int)d.hi <= 0xfffffff) {
        d.hi = (d.hi << 1) | ((unsigned int)d.lo >> 31);
        d.lo = d.lo << 1;
        d.exp = d.exp - 1;
    }
done:
    return OvlFunc_common2_44c(&d);
}
