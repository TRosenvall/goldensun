/* OvlFunc_881_200837c  --  0x0200837c
 *
 * Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a.s.
 *
 * Chooses the innkeeper's script by area, with two areas gated on save bits and
 * a fallback that also sets save bit 0x235.
 *
 * `break` INSIDE A CASE REACHES THE FALLBACK. The two gated arms
 * (0x31 and 0x40) drop out of the switch when their flag test fails, and the
 * ROM sends them to the same `.L456` block the default uses. A `break` in C
 * says exactly that, which is why the fallback is written after the switch
 * rather than in a `default:` arm.
 *
 * Case order off the blocks: 0x31, 0x40, 0x41/0x46, 0x47, 0x48, 0x49,
 * 0x42-0x45/0x4b, 0x50. The six-value group sits between 0x49 and 0x50 rather
 * than in numeric position.
 *
 * Matched on the first screen.
 */
extern unsigned char gState[];
extern unsigned char L5b84[] __asm__(".L5b84");
extern unsigned char L604c[] __asm__(".L604c");
extern unsigned char L6154[] __asm__(".L6154");
extern unsigned char L61e4[] __asm__(".L61e4");
extern unsigned char L625c[] __asm__(".L625c");
extern unsigned char L628c[] __asm__(".L628c");
extern unsigned char L62ec[] __asm__(".L62ec");
extern unsigned char L6394[] __asm__(".L6394");
extern unsigned char L63c4[] __asm__(".L63c4");

extern int __GetFlag(int id);
extern void __SetFlag(int id);

unsigned char *OvlFunc_881_200837c(void)
{
    unsigned char *g;

    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 0x31:
        if (__GetFlag(0x94f))
            break;
        if (__GetFlag(0x941) == 0)
            break;
        return L6154;
    case 0x40:
        if (__GetFlag(0x85a))
            break;
        return L604c;
    case 0x41:
    case 0x46:
        return L61e4;
    case 0x47:
        return L628c;
    case 0x48:
        return L6394;
    case 0x49:
        return L63c4;
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x4b:
        return L625c;
    case 0x50:
        return L62ec;
    }
    __SetFlag(0x235);
    return L5b84;
}
