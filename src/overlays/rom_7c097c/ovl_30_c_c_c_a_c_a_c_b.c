/* Cluster OvlFunc_936_200964c..OvlFunc_936_200964c extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c.s.
 *
 * Slotted between ovl_30_c_c_c_a_c_a_c_a.o and the rest of the overlay.
 *
 * A FIVE-ARM area dispatcher -- the same family as the two-arm members in batch
 * 47 and the three-arm one in batch 45, just longer, and it needed nothing the
 * shorter ones did not. Same gState+0x1c0 read with `off = 0` as a variable
 * (Thumb `ldrsh` has no immediate-offset form), same pool tell on all five
 * compared constants, same `pop {r1}` for `return 0`.
 *
 * The __SetFlag comes BEFORE the area read, which is what the ROM does.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_63;
extern int _AREA_66;
extern int _AREA_99;
extern int _AREA_9b;
extern int _AREA_9c;
extern void __SetFlag(int id);
extern void OvlFunc_936_20096bc(void);
extern void OvlFunc_936_20097e8(void);
extern void OvlFunc_936_2009858(void);
extern void OvlFunc_936_20098a4(void);
extern void OvlFunc_936_2009930(void);

int OvlFunc_936_200964c(void)
{
    unsigned int base;
    unsigned int off;
    short v;

    __SetFlag(0x87a);
    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_AREA_63))
        OvlFunc_936_20096bc();
    else if (v == (int)(&_AREA_66))
        OvlFunc_936_20097e8();
    else if (v == (int)(&_AREA_99))
        OvlFunc_936_2009858();
    else if (v == (int)(&_AREA_9b))
        OvlFunc_936_20098a4();
    else if (v == (int)(&_AREA_9c))
        OvlFunc_936_2009930();
    return 0;
}
