/* Cluster OvlFunc_967_2008084..OvlFunc_967_2008084 extracted from goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_a.s.
 *
 * Slotted between ovl_30_c_c_a_a.o and the rest of the overlay.
 *
 * A three-way script selector: area, then a flag inside the matching area.
 * `pop {r1}` is the return-value tell.
 *
 * The flag id is loaded ONCE here, so this is not the constant-CSE shape and
 * needs no Makefile rule -- unlike its neighbour OvlFunc_967_20084b0 in batch
 * 52, which loads the same id twice on mutually exclusive arms and also needed
 * no rule, for a different reason.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_b4;
extern int __GetFlag(int id);
extern unsigned char L1974[] __asm__(".L1974");
extern unsigned char L189c[] __asm__(".L189c");
extern unsigned char L1734[] __asm__(".L1734");

unsigned char *OvlFunc_967_2008084(void)
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
    if (v == (int)(&_AREA_b4)) {
        if (__GetFlag(0x9a7))
            return L1974;
        return L189c;
    }
    return L1734;
}
