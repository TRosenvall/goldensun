/* Cluster OvlFunc_958_2009394..OvlFunc_958_2009394 extracted from goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_c_c_c.s.
 *
 * Total .text for this TU = 240 bytes (= 0x00f0).
 * Slotted before asm/overlays/rom_7e636c/ovl_cc0_c_c_c_c_c.o in
 * goldensun/overlays/rom_7e636c/overlay.ld, in BOTH the .text and .data lists.
 *
 * SPLIT BY HAND, because tools/split_s.py correctly refused: the .s held ONE
 * function plus fifteen data blobs, and there was no second function to split
 * at, so converting the whole file would have deleted the data and broken the
 * link.  The boundary is clean -- text through .func_end, then .section .data --
 * so _b carries the function and _c the data, and the byte-neutral compare was
 * run with the function still in assembly before this file was written.
 *
 * TWO SPELLINGS ARE LOAD-BEARING:
 *
 *   1. The two area comparisons are SYMBOLS, not literals.  The ROM pool-loads
 *      0x98 and 0x9e where `cmp r2, #imm` would do, which is the pool tell that
 *      the operand was symbolic; _AREA_98 and _AREA_9e were already in area.sym.
 *      Written as literals the compare is a plain immediate and does not match.
 *
 *   2. x1/y1 and x2/y2 -- the split-build arguments of the two guarded
 *      __MapActor_SetPos calls -- are named in the block dominating them, which
 *      is what places the ROM's `mov r0, #8` and `mov r0, #0xa` between the
 *      constant movs and their shifts.  Inline, those two sites are the only
 *      difference, 4 of 100.
 *
 * The int return is read off the epilogue: it ends `pop {r1} / bx r1`, so r0 is
 * live and the function returns the 0 it sets.
 */
extern unsigned char gState[];
extern int _AREA_98;
extern int _AREA_9e;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_958_2009394(void)
{
    unsigned char *g;
    int s1, s2;
    int x1, y1, x2, y2;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_98)) {
        __SetFlag(0xa2 << 1);
        if (__GetFlag(0x9a << 4))
            __MapActor_SetPos(0xb, 0, 0);
    }
    x1 = 0xdc << 17;
    y1 = 0x9a << 17;
    x2 = 0xae << 18;
    y2 = 0x90 << 17;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_9e)) {
        if (*(short *)(g + (0xe1 << 1)) == 1) {
            s1 = 0x6b;
            s2 = 0x11;
            __Func_8010704(0x6c, 0x11, 1, 1, s1, s2);
        }
        if (__GetFlag(0x9a2)) {
            __MapActor_SetPos(8, x1, y1);
            __MapActor_SetAnim(8, 2);
            s1 = 0x1b;
            s2 = 0x13;
            __Func_8010704(0x1d, 0x13, 1, 1, s1, s2);
        }
        if (__GetFlag(0x9a5)) {
            __MapActor_SetPos(9, 0, 0);
            __MapActor_SetPos(0xa, x2, y2);
            __MapActor_SetAnim(0xa, 2);
        }
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
    }
    return 0;
}
