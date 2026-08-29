/* Cluster OvlFunc_957_2008c2c..OvlFunc_957_2008c2c extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a.s.
 *
 * Slotted between the _a and _c pieces in
 * goldensun/overlays/rom_7e3e08/overlay.ld.
 *
 * Guards five identical calls on the AREA ID at gState+0x1C0. `_AREA_97`
 * already existed.
 *
 * The five `__Func_8092950(0x10..0x14, 6)` calls are written out rather than
 * looped: the ROM has them unrolled, and a loop would add an induction variable
 * and a branch.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001f30;
extern int _AREA_97;
extern int __GetFlag(int id);
extern void OvlFunc_957_2008b30(void);
extern void __Func_8092950(int a, int b);

void OvlFunc_957_2008c2c(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *g;
    unsigned int k;
    int v;
    int t;

    p = iwram_3001f30;
    if (__GetFlag(0x80 << 2) != 0) {
        OvlFunc_957_2008b30();
        q = p;
        q += 0x34;
        t = 1;
        *q = t;
    }
    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v != (int)(&_AREA_97))
        return;
    __Func_8092950(0x10, 6);
    __Func_8092950(0x11, 6);
    __Func_8092950(0x12, 6);
    __Func_8092950(0x13, 6);
    __Func_8092950(0x14, 6);
}
