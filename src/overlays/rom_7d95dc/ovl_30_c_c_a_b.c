/* Cluster OvlFunc_953_2008238..OvlFunc_953_2008238 extracted from goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_a.s.
 *
 * The first function stays in the .s, which keeps its name and its slot; this
 * piece is added after it in goldensun/overlays/rom_7d95dc/overlay.ld.
 *
 * Selects a table from the AREA ID at gState+0x1C0, with a sub-test on the
 * halfword at +0x1C2 inside the 0x8c arm.
 *
 * `_AREA_8d` did not exist before batch 67 -- 0x8d was defined only in
 * file_table.sym -- and was added on area.sym's own criterion, that the value is
 * compared against gState+0x1C0. `_AREA_8c` and `_AREA_8e` already existed,
 * which is itself mild support: the three consecutive ids are used by one
 * selector, and only the middle one was missing.
 *
 * The sub-test is an EQUALITY (`cmp r3, #0xc / bne`), not a range, so this
 * function avoids the signed lower-bound floor that blocks its neighbours.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_8c;
extern int _AREA_8d;
extern int _AREA_8e;
extern unsigned char L3e70[] __asm__(".L3e70");
extern unsigned char L4110[] __asm__(".L4110");
extern unsigned char L3e94[] __asm__(".L3e94");
extern unsigned char L3f60[] __asm__(".L3f60");
extern unsigned char L3e64[] __asm__(".L3e64");

void *OvlFunc_953_2008238(void)
{
    unsigned char *b;
    unsigned char *g;
    unsigned int k;
    int v;
    int w;

    b = (unsigned char *)&gState;
    k = 0xe0 << 1;
    g = b + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_8d))
        return L3e70;
    if (v == (int)(&_AREA_8c)) {
        k = 0xe1 << 1;
        g = b + k;
        w = *(short *)(g + (unsigned int)0);
        if (w == 0xc)
            return L4110;
        return L3e94;
    }
    if (v == (int)(&_AREA_8e))
        return L3f60;
    return L3e64;
}
