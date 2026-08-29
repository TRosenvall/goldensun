/* Cluster OvlFunc_938_2008264..OvlFunc_938_2008264 extracted from goldensun/asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c.s.
 *
 * Total .text for this TU = 252 bytes (= 0x00fc).
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c.o in goldensun/overlays/rom_7c37ac/overlay.ld.
 * The target was the FIRST of five functions, so there is no _a part; the
 * remaining four functions and the trailing .data travel together in _c.
 *
 * Area-entry fixups: switches on the area halfword and repairs actor state.
 *
 * FOUR SEPARATE SPELLINGS ARE LEVER-CRITICAL HERE.  This function needed all
 * of them stacked, 70 differing lines down to zero; each is documented in
 * docs/elevation.md and none of them is cosmetic:
 *
 *   1. The two halfword stores go through an INT INTERMEDIATE (v), not a direct
 *      cast-store.  Written *(short *)(p + 6) = 0x80 << 5 gcc pools the folded
 *      constant; via an int local it builds mov r3,#0x80 / lsl r3,#5 as the ROM
 *      does.  An unsigned short destination -- the other HImode remedy -- does
 *      nothing here, because 0x1000 does not sign-extend.
 *   2. y = 0xd3 << 17 is NAMED in the block dominating its call, which is what
 *      places the lsl after the other argument setup.
 *   3. e = 0x38a0000, the pooled argument, is named in the same dominating
 *      block.  Naming y alone leaves the r0/r1 pair transposed.
 *   4. The two STACK arguments of both six-argument calls are named (s1, s2).
 *      Passed as literals gcc reuses one register -- load, store, load, store;
 *      the ROM materialises both into two registers and then stores both.
 *
 * Removing any one of the four reopens the difference, so do not "simplify"
 * them back into literals.
 */
extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_80118c0(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void OvlFunc_938_2009494(void);
extern void OvlFunc_938_2008360(void);

void OvlFunc_938_2008264(void)
{
    unsigned char *g;
    char *p;
    int v;
    int y;
    int e;
    int s1, s2;

    __Func_80118c0(1);
    __Func_80118c0(2);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 9:
        if (__GetFlag(0x941)) {
            p = __MapActor_GetActor(8);
            v = 0x80 << 5;
            *(unsigned short *)(p + 6) = v;
            if (__GetFlag(0x914) == 0)
                OvlFunc_938_2009494();
        } else {
            __MapActor_SetPos(9, 0, 0);
            y = 0xd3 << 17;
            e = 0x38a0000;
            if (__GetFlag(0x321)) {
                __MapActor_SetPos(8, e, y);
                p = __MapActor_GetActor(8);
                v = 0xd0 << 8;
                *(unsigned short *)(p + 6) = v;
            }
        }
        break;
    case 0xa:
    case 0xb:
        if (__GetFlag(0x915)) {
            s1 = 4;
            s2 = 3;
            __CopyMapTiles(0x3a, 0x46, 0x36, 0x46, s1, s2);
            s1 = 0x37;
            s2 = 8;
            __Func_8010704(0x37, 9, 2, 1, s1, s2);
            __Func_800fe9c();
            __WaitFrames(1);
        }
        break;
    case 0x14:
        __MapActor_SetPos(9, 0, 0);
        if (__GetFlag(0x109) == 0)
            OvlFunc_938_2008360();
        break;
    }
}
