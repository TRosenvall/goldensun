/* Cluster OvlFunc_924_200d1b0..OvlFunc_924_200d1b0 extracted from
 * goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c.s.
 *
 * Total .text for this TU = 66 bytes (= 0x42).
 * Slotted between the _a and _c pieces in goldensun/overlays/rom_7ac2d8/overlay.ld.
 *
 * BYTE-IDENTICAL TWIN of OvlFunc_923_2009c20 in overlays/rom_7aa430; this C is
 * that file's verbatim, with only the symbol changed. See it for why the step
 * counter is incremented at int width and narrowed only where it is tested.
 */

struct B {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    unsigned short f64;
    unsigned char pad66[2];
    struct B *f68;
};

extern int __sin(int a);

int OvlFunc_924_200d1b0(struct A *a)
{
    struct B *b;
    int t;
    int v;

    t = a->f64 + 1;
    b = a->f68;
    a->f64 = t;
    v = (short)t;
    if (v > 0x1f)
        return 0;
    a->f18 = __sin(v << 10);
    a->f1c = a->f18;
    a->f8 = b->f8;
    a->fc += 0x80 << 9;
    a->f10 = b->f10;
    return 1;
}
