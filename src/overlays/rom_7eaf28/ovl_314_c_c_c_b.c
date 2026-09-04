/* OvlFunc_960_2008e8c -- 0x02008e8c
 *
 * The sanctum's per-frame entry: arm the map flags, start the attendant task if
 * the save byte is set, then branch on which of two areas the party is in --
 * capture a palette word and run the area's own handler, or play the refusal
 * sound. Finally clear a bit in the map flags halfword unless the sub-area is
 * non-zero.
 *
 * THREE SEPARATE gState FETCHES, and that is the whole residue. Written with
 * ONE named base the function comes out a line SHORT at 26 differing, because
 * gcc keeps gState in r5 across the body and indexes off it. The ROM fetches it
 * fresh three times -- `ldr r3, =gState` inside the flag-byte branch, `ldr r5,
 * =gState` for the two area tests, and `ldr r3, =gState` again for the
 * sub-area test -- so the source needs a local PER SITE. That is the ordinary
 * rematerialisation lever, applied to a base pointer rather than a constant.
 *
 * The base still has to be a NAMED local at each site rather than inlined:
 * inlining folds base and offset into a single `=gState+448` pool word, which
 * is the gState array idiom already on file.
 *
 * THE TWO AREA IDS ARE SYMBOLS. `ldr r6, =0xa4` and `ldr r3, =0xa5` pool values
 * an eight-bit `mov` would build, and they are compared against the area
 * halfword at gState+0x1c0 -- the id space area.sym covers. `_AREA_a4` and
 * `_AREA_a5` were already there. 0xa4 is kept in a callee-saved register across
 * the calls because it is compared twice; 0xa5 is rebuilt at each of its two
 * sites, and writing both as plain symbol references gets that split for free.
 *
 * `t = 0xfdff; t &= v;` is the named-constant form -- the AND's destination is
 * the CONSTANT, matching `ldr r3, =0xfdff / and r3, r2`.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern int _AREA_a4;
extern int _AREA_a5;
extern unsigned char L1a00[] __asm__(".L1a00");
extern int __GetFlagByte(int id);
extern void __StartTask(void (*f)(void), int prio);
extern void __PlaySound(int id);
extern void OvlFunc_960_2008400(void);
extern void OvlFunc_960_2008d24(void);
extern void OvlFunc_960_2008f50(void);
extern void OvlFunc_960_2009094(void);

int OvlFunc_960_2008e8c(void)
{
    unsigned char *b;
    unsigned char *c;
    unsigned char *g;
    unsigned char *g1;
    unsigned char *g2;
    int v;
    int t;

    b = iwram_3001e70[0];
    c = iwram_3001e70[0x13];
    *(int *)(c + (0xe0 << 1)) = 0x201;
    if (__GetFlagByte(0x84 << 2) != 0) {
        g1 = gState;
        g1[0xf9 << 1] = 2;
        __StartTask(OvlFunc_960_2008400, 0xc8 << 4);
    }
    g = gState;
    v = *(short *)(g + (0xe0 << 1));
    if (v == (int)(&_AREA_a4) || v == (int)(&_AREA_a5)) {
        *(unsigned short *)L1a00 = *(unsigned short *)0x500019e;
        OvlFunc_960_2008d24();
    }
    v = *(short *)(g + (0xe0 << 1));
    if (v == (int)(&_AREA_a4))
        OvlFunc_960_2008f50();
    else if (v == (int)(&_AREA_a5))
        OvlFunc_960_2009094();
    else
        __PlaySound(0x90 << 1);
    g2 = gState;
    if (*(short *)(g2 + (0xe1 << 1)) == 0) {
        t = 0xfdff;
        t &= *(unsigned short *)(b + 0x14);
        *(unsigned short *)(b + 0x14) = t;
    }
    return 0;
}
