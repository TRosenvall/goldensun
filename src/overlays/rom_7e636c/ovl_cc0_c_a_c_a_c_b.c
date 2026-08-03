/* Cluster OvlFunc_958_2008d88..OvlFunc_958_2008d88 extracted from goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in goldensun/overlays/rom_7e636c/overlay.ld, so the ROM layout does
 * not move.
 *
 * GetEntrances, five-way form -- the same variant as its neighbour
 * OvlFunc_958_2008d20: the 0x98 arm checks a story flag and picks between two
 * further tables.
 *
 * The two are NOT interchangeable, and the difference is easy to miss. Both
 * return one named global among four `.L` tables, but from DIFFERENT arms:
 *
 *     OvlFunc_958_2008d20   0x9d -> gScript_970__02009a4c
 *     OvlFunc_958_2008d88   0x9e -> gScript_885__02009ce0
 *
 * Deriving this file from its neighbour by substituting labels put the global
 * in the wrong arm; it compiled, and the screen caught it at 29 instructions
 * against 31.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_98;
extern int _ID_9d;
extern int _ID_9e;
extern int __GetFlag(int id);
extern unsigned char gScript_885__02009ce0[];
extern unsigned char L1bcc[] __asm__(".L1bcc");
extern unsigned char L1b48[] __asm__(".L1b48");
extern unsigned char L1c80[] __asm__(".L1c80");
extern unsigned char L1b3c[] __asm__(".L1b3c");

unsigned char *OvlFunc_958_2008d88(void)
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
            return L1bcc;
        return L1b48;
    }
    if (v == (int)(&_ID_9d))
        return L1c80;
    if (v == (int)(&_ID_9e))
        return gScript_885__02009ce0;
    return L1b3c;
}
