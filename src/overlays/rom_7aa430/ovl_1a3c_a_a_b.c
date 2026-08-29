/* Cluster OvlFunc_923_2009c20..OvlFunc_923_2009c20 extracted from
 * goldensun/asm/overlays/rom_7aa430/ovl_1a3c_a_a.s.
 *
 * Total .text for this TU = 66 bytes (= 0x42).
 * Placed in the run in goldensun/overlays/rom_7aa430/overlay.ld.
 *
 * One frame of a 32-step animation: bumps the step counter at +0x64, and while
 * it is still in range takes a sine of it into two scale words, copies two
 * coordinates from the parent at +0x68, and advances a third by 0x10000.
 * Returns 1 while running, 0 once the counter passes 0x1f.
 *
 * THE COUNTER IS STORED AS A HALFWORD AND TESTED AS A SIGNED ONE.
 * `lsl r3, #16 / asr r0, r3, #16` is a sign-extension of the value just
 * stored -- so the source keeps the incremented value at INT width, stores it,
 * and then reads it back as `(short)`. Writing the test against the member
 * instead makes gcc reload; writing the increment in halfword width makes it a
 * masked add. Same rule as OvlFunc_911_20080cc in batch 76: do the arithmetic
 * at int width, put the narrowing only where the value is TESTED.
 *
 * The parent pointer is read BEFORE the counter is stored -- that ordering is
 * in the ROM and is what the source says; moving it after costs a register.
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

int OvlFunc_923_2009c20(struct A *a)
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
