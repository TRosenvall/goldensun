/* Cluster OvlFunc_907_2008890..OvlFunc_907_2008890 extracted from goldensun/asm/overlays/rom_79b154/ovl_30_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_79b154/ovl_30_c_a_c_a.o and the rest of the overlay in
 * goldensun/overlays/rom_79b154/overlay.ld.
 *
 * Writes an interaction word, then dispatches on the area id.
 *
 * TWO THINGS SHARE ONE VARIABLE HERE, deliberately. `off` is 0xe0 << 1 and is
 * used BOTH as the byte offset into the iwram block and as the halfword index
 * into gState -- the ROM keeps it in r2 across both (`str r3,[r1,r2]` then
 * `ldrsh r2,[r3,r2]`). Two separate locals give two materialisations.
 *
 * `t = 0xc8 << 4;` is the basic-block lever: assigned at the top, used inside
 * the second arm of the dispatch, so gcc rematerialises it at the call and
 * splits the pair around `ldr r0, =OvlFunc_907_2008ed8`. That is the
 * pool-load-first half of the class -- see reports/arg-interleave.md.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_1e;
extern int _AREA_20;
extern int _AREA_23;

extern void OvlFunc_907_20088f0(void);
extern void OvlFunc_907_2008ae0(void);
extern void OvlFunc_907_2008d10(void);
extern void OvlFunc_907_2008ed8(void);
extern void __StartTask(void *fn, int n);

int OvlFunc_907_2008890(void)
{
    unsigned char *base;
    unsigned int off;
    short area;
    int v;
    int t;

    t = 0xc8 << 4;
    base = (unsigned char *)iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    v = 0x80 << 1;
    *(int *)(base + off) = v;
    area = *(short *)((unsigned char *)&gState + off);
    if (area == (int)(&_AREA_1e)) {
        OvlFunc_907_20088f0();
    } else if (area == (int)(&_AREA_23)) {
        OvlFunc_907_2008ae0();
        __StartTask(OvlFunc_907_2008ed8, t);
    } else if (area == (int)(&_AREA_20)) {
        OvlFunc_907_2008d10();
    }
    return 0;
}
