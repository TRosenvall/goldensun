/* Cluster OvlFunc_895_200892c..OvlFunc_895_200892c extracted from goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c.s.
 *
 * Total .text for this TU = 248 bytes (= 0x00f8).
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c.o in goldensun/overlays/rom_78dee8/overlay.ld.
 * The target was the FIRST of six functions, so there is no _a part.
 *
 * Area-entry fixups behind three independent flag guards.
 *
 * TWO SPELLINGS ARE LOAD-BEARING:
 *
 *   1. z (the stored zero) is assigned BEFORE q (the destination address).
 *      Written as the single statement *(int *)L269c = 0, gcc puts the address
 *      in r2 and the zero in r3; the ROM has them the other way round.  The
 *      order of the two assignments is what picks the registers -- see "source
 *      order of two loads" in docs/elevation.md.
 *
 *   2. x and y (the two split-build arguments of __MapActor_SetPos) are named
 *      in the block DOMINATING the guarded call, not inside the arm.  That is
 *      what lets gcc place mov r0,#8 between the two movs and the two lsls.
 *      Naming them inside the arm instead was screened and changes nothing --
 *      the dominance precondition is real, not incidental.
 *
 * The 0x100 stored through iwram_3001ebc is left as a plain literal: gcc
 * derives it from the offset register (sub r2, #0xc0 off the 0x1c0 it just
 * built) without help, matching the ROM.
 */
extern char *iwram_3001ebc;
extern unsigned char L269c[] __asm__(".L269c");

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __StartTask(void (*fn)(void), int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_895_2009ac8(void);

void OvlFunc_895_200892c(void)
{
    char *p;
    int s;
    int z;
    int *q;
    int x, y;

    __SetFlag(0xa2 << 1);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x100;
    if (__GetFlag(0x814)) {
        z = 0;
        q = (int *)L269c;
        *q = z;
        __StartTask(OvlFunc_895_2009ac8, 0xc8 << 4);
    }
    if (__GetFlag(0x879)) {
        s = 6;
        __Func_8010704(5, 6, 1, 1, s, s);
        __Func_8010704(5, 6, 1, 1, 7, s);
        __Func_8010704(5, 6, 1, 1, 8, s);
        __Func_8010704(0, 1, 3, 1, s, 5);
    }
    x = 0xf0 << 15;
    y = 0xe8 << 16;
    if (__GetFlag(0x815)) {
        __MapActor_SetPos(8, x, y);
        s = 0xe;
        __Func_8010704(2, 0xa, 1, 1, 6, s);
        __Func_8010704(2, 0xa, 1, 1, 7, s);
        __Func_8010704(2, 0xa, 1, 1, 8, s);
    }
}
