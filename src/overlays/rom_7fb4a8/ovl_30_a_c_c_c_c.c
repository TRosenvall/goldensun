/* Cluster OvlFunc_971_2008f30..OvlFunc_971_2008f30 extracted from goldensun/asm/overlays/rom_7fb4a8/ovl_30_a.s.
 *
 * Total .text for this TU = 88 bytes (= 0x58).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_b.o and asm/overlays/rom_7fb4a8/ovl_30_b.o in
 * goldensun/overlays/rom_7fb4a8/overlay.ld.
 */
extern unsigned int gState;
extern int __GetPartySize(void);
extern int __GetFlag(int id);

int OvlFunc_971_2008f30(int kind)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int g;
    unsigned int off;
    int n;
    int limit;
    int i;

    n = __GetPartySize();
    limit = 3;
    if (__GetFlag(0xb9 << 1) == 0)
        limit = 4;
    if (n > limit)
        n = limit;
    i = 0;
    if (i < n) {
        g = (unsigned int)&gState;
        off = 0xfc;
        off <<= 1;
        g += off;
        p = (unsigned char *)g;
        q = p;
        do {
            if (*q++ == 0xff)
                goto ret0;
            if (*p == kind)
                return 1;
            p++;
            i++;
        } while (i < n);
    }
ret0:
    return 0;
}
