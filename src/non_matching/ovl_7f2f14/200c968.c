/* OvlFunc_968_200c968 (0x0200c968) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER SELECTION.
 *
 * 90 lines against the ROM's 90, 22 differing, and every difference is which
 * scratch register holds a value -- not which instructions exist, not their
 * order, and not the callee-saved allocation.
 *
 *     rom    add r2, sp, #0x10 ... mov r10, r2      struct built in r2, held in r10
 *     ours   add r7, sp, #0x10 ...                  built straight into r7
 *
 *     rom    ldr r2, =0x3001e40 / ldr r7, [r2] ... ldr r3, [r2]
 *     ours   ldr r3, =0x3001e40 / ldr r2, [r3]      one deref, no held address
 *
 * MEASURED, all 90 lines and 22 differing, byte-identical to each other:
 *   the global's ADDRESS named (`pg = &iwram_3001e40;` with two `*pg` reads),
 *     which is the recorded lever for exactly the ROM's two-reload shape
 *   an explicit `struct P *tp = &t;` with every field written through it and
 *     `tp` passed to the call, which is the shape `add r2, sp, #0x10` reads as
 *
 * WHAT IS RIGHT: the body. The three coordinate jitters (`Random() * 49 >> 16`
 * minus 0x18, shifted 16), the fourth Random scaled by 4 and shifted 15, the
 * 0x8f << 1 halfword and the .L52cc pointer into the stack struct, the eight
 * argument positions and the `iwram_3001e40 & 3` guard with the `& 7` sound
 * test nested inside it -- all exact, and all of it came from the exemplar
 * OvlFunc_947_200a2d8 with the constants changed.
 *
 * NEXT: nothing source-level. See src/non_matching/rom_15000/80208e4.c, parked
 * the same round on the same class, for the pattern.
 */
struct P {
    int f0;
    int f4;
    unsigned char pad8[0x18 - 8];
    unsigned short f18;
    unsigned char pad1a[2];
    void *f1c;
    unsigned char pad20[0x28 - 0x20];
};

struct A { unsigned char pad00[8]; int f8; int fc; int f10; };

extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern unsigned char L52cc[] __asm__(".L52cc");
extern void OvlFunc_968_2008118(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

int OvlFunc_968_200c968(struct A *src)
{
    struct P t;
    int v;
    int x;
    int y;
    int z;
    int n;

    t.f0 = 1;
    t.f4 = 5;
    t.f18 = 0x8f << 1;
    t.f1c = L52cc;
    v = iwram_3001e40 & 3;
    if (v != 0)
        return 0;
    if ((iwram_3001e40 & 7) == 0)
        __PlaySound(0xf6);
    x = src->f8 + (((__Random() * 49 >> 16) - 0x18) << 16);
    y = src->fc + (((__Random() * 49 >> 16) - 0x18) << 16);
    z = src->f10 + (((__Random() * 49 >> 16) - 0x18) << 16);
    n = ((__Random() * 4 >> 16) << 15) + (0x80 << 8);
    OvlFunc_968_2008118(x, y, z, 0, n, v, 0xcc << 14, &t);
    return 0;
}
