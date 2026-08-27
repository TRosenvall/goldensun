/* OvlFunc_888_200814c  --  0x0200814c
 *
 * The whole of goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a_a_b.s.
 *
 * Returns the behaviour script the sanctum attendant should run, chosen by area
 * and -- for anything not listed -- by two save bits.
 *
 * READ THE TABLE TO GET THE CASE ORDER. Nine scripts, six of them selected by
 * area through a 41-slot table. The ROM emits the case bodies in the order
 * 0xa/0xc, 0xb, 0x14/0x15/0x32, 0x20, 0x1d, 0x23 -- note 0x20 BEFORE 0x1d,
 * which is not numeric order and is not something you would guess. Writing the
 * cases in that order matched on the first screen; writing them in numeric
 * order would not have.
 *
 * That is the practical form of batch 101's rule: with a jump table, the source
 * order is recoverable from the assembly, so transcribe the block order rather
 * than tidying the cases.
 *
 * The default falls out of the switch into two flag tests, which is why they
 * are written after the switch rather than in a `default:` arm.
 */
extern unsigned char gState[];
extern unsigned char L3e34[] __asm__(".L3e34");
extern unsigned char L3e70[] __asm__(".L3e70");
extern unsigned char L3ec4[] __asm__(".L3ec4");
extern unsigned char L3f0c[] __asm__(".L3f0c");
extern unsigned char L3f78[] __asm__(".L3f78");
extern unsigned char L3fd8[] __asm__(".L3fd8");
extern unsigned char L4038[] __asm__(".L4038");
extern unsigned char L4080[] __asm__(".L4080");
extern unsigned char L40ec[] __asm__(".L40ec");

extern int __GetFlag(int id);

unsigned char *OvlFunc_888_200814c(void)
{
    unsigned char *g;
    int area;

    g = gState;
    area = *(short *)(g + (0xe1 << 1));
    switch (area) {
    case 0xa:
    case 0xc:
        return L3e70;
    case 0xb:
        return L3ec4;
    case 0x14:
    case 0x15:
    case 0x32:
        return L3f0c;
    case 0x20:
        return L40ec;
    case 0x1d:
        return L4038;
    case 0x23:
        return L4080;
    }
    if (__GetFlag(0x87a))
        return L3fd8;
    if (__GetFlag(0x815))
        return L3f78;
    return L3e34;
}
