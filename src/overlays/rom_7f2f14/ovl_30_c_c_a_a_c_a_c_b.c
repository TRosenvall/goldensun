/* Cluster OvlFunc_968_200af8c..OvlFunc_968_200af8c extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_c.s.
 *
 * The first function stays in the .s; this piece is added after it in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * A six-way selector on the AREA ID at gState+0x1C0, using _AREA_b5 through
 * _AREA_ba. `_AREA_b6` was added in batch 67 on comparison evidence; note that
 * it routes to the DEFAULT rather than to a table of its own, which is why the
 * arm is `goto dflt;` and not a return. Five consecutive ids used by one
 * selector, with the added one sitting among them, is further support for the
 * addition.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_b5;
extern int _AREA_b6;
extern int _AREA_b7;
extern int _AREA_b8;
extern int _AREA_b9;
extern int _AREA_ba;
extern unsigned char L6e44[] __asm__(".L6e44");
extern unsigned char L7120[] __asm__(".L7120");
extern unsigned char L7300[] __asm__(".L7300");
extern unsigned char L73b4[] __asm__(".L73b4");
extern unsigned char L74f8[] __asm__(".L74f8");
extern unsigned char L6f1c[] __asm__(".L6f1c");

void *OvlFunc_968_200af8c(void)
{
    unsigned char *g;
    unsigned int k;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_b5))
        return L6e44;
    if (v == (int)(&_AREA_b6))
        goto dflt;
    if (v == (int)(&_AREA_b7))
        return L7120;
    if (v == (int)(&_AREA_b8))
        return L7300;
    if (v == (int)(&_AREA_b9))
        return L73b4;
    if (v == (int)(&_AREA_ba))
        return L74f8;
dflt:
    return L6f1c;
}
