/* Cluster OvlFunc_958_2008d20..OvlFunc_958_2008d20 extracted from goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a.s.
 *
 * Split out of that .s; the sibling part stays as assembly and keeps its slot
 * in goldensun/overlays/rom_7e636c/overlay.ld, so the ROM layout does not move.
 *
 * GetEntrances, four-way form -- but NOT the plain family shape. Two things
 * differ, and the family sweep matched it anyway because it counts three
 * compares and four returns:
 *
 *   1. The first arm checks a story flag and picks between two more tables,
 *      so this is really a five-way selector.
 *   2. One arm returns gScript_970__02009a4c, a named global, where every
 *      other member returns a local `.L` data label.
 *
 * 0x96f is left as a literal: it is larger than an eight-bit `mov` can build,
 * so gcc pools it either way and it carries no pool-tell evidence about
 * whether it was a symbol. The compared ids DO carry that evidence and are
 * symbols -- see src/overlays/rom_79aad8/ovl_314_a.c.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_98;
extern int _ID_9d;
extern int _ID_9e;
extern int __GetFlag(int id);
extern unsigned char gScript_970__02009a4c[];
extern unsigned char L19d4[] __asm__(".L19d4");
extern unsigned char L1974[] __asm__(".L1974");
extern unsigned char L1aac[] __asm__(".L1aac");
extern unsigned char L195c[] __asm__(".L195c");

unsigned char *OvlFunc_958_2008d20(void)
{
    unsigned int base;
    unsigned int off;
    short v;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_ID_98)) {
        if (__GetFlag(0x96f))
            return L19d4;
        return L1974;
    }
    if (v == (int)(&_ID_9d))
        return gScript_970__02009a4c;
    if (v == (int)(&_ID_9e))
        return L1aac;
    return L195c;
}
