/* Cluster OvlFunc_common2_380..OvlFunc_common2_380 extracted from goldensun/asm/overlays/common/common2_c_c_c_c_a.s.
 *
 * Total .text for this TU = 108 bytes (= 0x6c), literal pool included.
 * Preserves the original ROM layout when slotted between
 * asm/overlays/common/common2_c_c_c_c_a_a.o and asm/overlays/common/common2_c_c_c_c_b.o in
 * goldensun/overlays/rom_7bf5a8/overlay.ld (and identically in rom_7e7574).
 *
 * Built with -fcall-saved-r4, NOT the tree-wide -fcall-used-r4: this function
 * opens `push {r4, lr}`, which -fcall-used-r4 makes unreachable. See
 * COMMON2_CFLAGS in the Makefile.
 *
 * Two source shapes matter here and both were needed for the byte match:
 *   - the 8-byte input block and the 20-byte decoded record are ONE struct,
 *     not two locals. As two locals gcc folds the first store to `[sp, #0]`;
 *     as one struct, `&s.in` is a real address and both stores go through it,
 *     matching the ROM's `mov r3, sp` / `str r0, [r3]` / `str r1, [r3, #4]`.
 *   - the `goto` puts the saturating return first and the scaling block last,
 *     which is the ROM's block order (`ble` to the scale block, fall through
 *     to saturation). Written structurally the two blocks come out swapped.
 */

struct DecodeIn {
    int lo;
    int hi;
};

struct Decoded {
    int kind;
    int sign;
    int scale;
    int lo;
    int hi;
};

struct DecodeFrame {
    struct DecodeIn in;
    struct Decoded out;
};

extern void OvlFunc_common2_618(struct DecodeIn *in, struct Decoded *out);
extern int OvlFunc_common2_40c(struct Decoded *d);
extern int OvlFunc_common2_3ec(struct Decoded *d);
extern int OvlFunc_common2_3fc(struct Decoded *d);
extern int OvlFunc_common2_41c(int lo, int hi, int shift);

int OvlFunc_common2_380(int lo, int hi) {
    struct DecodeFrame s;
    struct DecodeIn *in;
    struct Decoded *d;
    int v;

    in = &s.in;
    d = &s.out;
    in->lo = lo;
    in->hi = hi;
    OvlFunc_common2_618(in, d);
    if (OvlFunc_common2_40c(d) != 0)
        return 0;
    if (OvlFunc_common2_3ec(d) != 0)
        return 0;
    if (OvlFunc_common2_3fc(d) == 0) {
        if (d->scale < 0)
            return 0;
        if (d->scale <= 0x1e)
            goto scale;
    }
    /* saturate to INT_MAX, or INT_MIN when the sign word is set */
    return (d->sign != 0) + 0x7fffffff;
scale:
    v = OvlFunc_common2_41c(d->lo, d->hi, 0x3c - d->scale);
    if (d->sign == 0)
        return v;
    return -v;
}
