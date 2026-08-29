/* OvlFunc_933_2008344 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7bc690/ovl_314_a_c_a.s
 * Best screen: 6 differing of 45, streams the same length.
 *
 * BLOCKER CLASS: the r2/r3 exchange, on two masks of the same word.
 *
 *     rom    ldr r3, [r5] / mov r2, #7  / and r3, r2   <- VALUE in the destination
 *            ldr r6, [r5] / mov r3, #0xf / and r6, r3  <- VALUE in the destination
 *     ours   ldr r2, [r5] / mov r3, #7  / and r3, r2   <- mask in the destination
 *            mov r6, #0xf / ... / and r6, r2           <- mask in the destination
 *
 * Batch 97 found that a named constant of the FIELD's type puts the CONSTANT in
 * the destination. Here the ROM wants the opposite -- the loaded value there --
 * and naming the loaded word into an `int` local to try to force it makes it
 * WORSE (46 lines, 43 differing), because the extra local changes the prologue.
 *
 * So the lever runs one way only: it can put a constant in the destination, but
 * there is no counterpart that puts the value there. Worth knowing before
 * anyone tries the mirror image on the other members of the class.
 *
 * WHAT IS RIGHT AND SHOULD NOT BE RE-DERIVED: the parameter block is a plain
 * stack struct passed by address -- the ROM's `sub sp, #0x38` is 0x10 of
 * outgoing stack arguments (the call takes eight) plus a 0x28 struct, and
 * `add r5, sp, #0x10` is gcc taking its address. The `lsl #12 / lsr #16` pair
 * is `(unsigned short)(rand >> 4)`, the 16-bit-extract-at-an-offset shape from
 * batch 93.
 */
struct P {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x22 - 0x10];
    unsigned short f22;
    unsigned char pad24[4];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern int iwram_3001e40;
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

int OvlFunc_933_2008344(struct A *src)
{
    struct P t;
    int v;

    if ((iwram_3001e40 & 7) == 0)
        __PlaySound(0x76);
    v = iwram_3001e40 & 0xf;
    if (v != 0)
        return 0;
    t.f8 = 0xcccc;
    t.fc = 0xcccc;
    t.f22 = (__Random() << 12 >> 16) + (0xf8 << 8);
    OvlFunc_common0_10c(src->f8, src->fc, src->f10, 0, v, v, 0x880001, &t);
    return 0;
}
