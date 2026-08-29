/* OvlFunc_945_200854c  --  0x0200854c
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a_a.s.
 *
 * Chooses which script the innkeeper runs, by area and then by save bits.
 *
 * A 23-SLOT TABLE WITH SIX TARGETS, and the case groups are read straight off
 * it: areas 1 and 2 share one arm, 4 and 23 another, and 15, 17 and 19 a third.
 * The 4/23 pairing in particular is not a pattern anyone would guess.
 *
 * Block order gives the source order: 1/2, 4/23, 5, 15/17/19, 21, then the
 * fallback after the switch.
 *
 * The two-flag ladder in the 1/2 arm is one short-circuit condition --
 * `if (__GetFlag(0x928) && __GetFlag(0x93e) == 0)` -- because the ROM jumps to
 * the same block from both tests rather than nesting.
 *
 * Matched on the first screen.
 */
extern unsigned char gState[];
extern unsigned char L7420[] __asm__(".L7420");
extern unsigned char L7444[] __asm__(".L7444");
extern unsigned char L7570[] __asm__(".L7570");
extern unsigned char L76fc[] __asm__(".L76fc");
extern unsigned char L781c[] __asm__(".L781c");
extern unsigned char L7930[] __asm__(".L7930");
extern unsigned char L7984[] __asm__(".L7984");
extern unsigned char L79c0[] __asm__(".L79c0");
extern unsigned char L7b58[] __asm__(".L7b58");
extern unsigned char L7d44[] __asm__(".L7d44");
extern unsigned char L7edc[] __asm__(".L7edc");

extern int __GetFlag(int id);

unsigned char *OvlFunc_945_200854c(void)
{
    unsigned char *g;

    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 1:
    case 2:
        if (__GetFlag(0x8a << 4))
            return L76fc;
        if (__GetFlag(0x928) && __GetFlag(0x93e) == 0)
            return L7570;
        return L7444;
    case 4:
    case 23:
        if (__GetFlag(0x93e))
            return L7edc;
        return L79c0;
    case 5:
        if (__GetFlag(0x8a << 4))
            return L7930;
        if (__GetFlag(0x93e))
            return L7984;
        return L781c;
    case 15:
    case 17:
    case 19:
        return L7b58;
    case 21:
        return L7d44;
    }
    return L7420;
}
