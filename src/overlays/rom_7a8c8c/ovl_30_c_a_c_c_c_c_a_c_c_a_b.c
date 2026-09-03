/* OvlFunc_922_2009948  --  0x02009948
 *
 * The tail of goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c_a.s;
 * the function ahead of it stays in _a.s. No data in the file.
 *
 * A per-area dispatch: repaint one attribute rectangle and run that area's
 * script, with the fourth area splitting again on a sub-area value.
 *
 * A STACK-ARGUMENT SLOT FED BY A LITERAL IS ONE LIVE PSEUDO; FED BY A NAMED
 * LOCAL IT IS TWO. The ROM's shape at every one of the five call sites is
 *
 *      mov r3, #e / mov r2, #f / str r3, [sp] / str r2, [sp, #4]
 *
 * With literals in the call gcc materialises and stores each in turn, reusing
 * one register for both -- `mov r3,#e / str / mov r3,#f / str` -- which is one
 * instruction longer per site. Naming both stack arguments makes their live
 * ranges overlap the two stores, which forces two registers and the ROM's
 * two-movs-then-two-strs. Five sites times three instructions is fifteen of the
 * twenty differing.
 *
 * SPLITTING INTO PER-ARM LOCALS IS NOT WHAT DID IT, and the measurement is the
 * point: five disjoint pairs and ONE SHARED PAIR compile BYTE-IDENTICALLY. Both
 * screened OK. The five sites sit in mutually exclusive arms of an else-if
 * chain, so a single source pair already decomposes into five non-overlapping
 * webs at allocation time and never spans a join -- no long live range, no
 * allocno_compare demotion, no r10. THE BATCH-182 SPLIT NEEDS THE SHARED RANGE
 * TO SPAN A JOIN; SIBLING ARMS DO NOT COUNT. The shared pair is kept here
 * because it is the simpler source and the output is identical.
 *
 * `sub rN, #1` AGAINST `add rN, #0xffff` ON A ldrh VALUE IS DECIDED BY WHERE THE
 * CAST SITS, not by the comparison. Written
 * `(unsigned short)(*(unsigned short *)p - 1) <= 1`, gcc sinks the truncation
 * into the subtraction and emits `ldr r0, =0xffff / add r3, r0`, burning a pool
 * slot. Assigning through an INT TEMPORARY first pins the subtract at int width
 * and leaves the truncation to the compare, which gcc then does as
 * `lsl r3,#16 / cmp r3, 0x10000 / bhi`. The two obvious rewrites both fail
 * identically -- `v == 1 || v == 2` and an `unsigned short` temp each give the
 * `add 0xffff` form back. The int temp is load-bearing.
 *
 * The four area ids are the pooled-small-constant tell and all four were already
 * in area.sym. gState needs the local-pointer form or gcc folds the base and the
 * offset into one pool word.
 */
extern unsigned char gState[];
extern int _AREA_3e;
extern int _AREA_3f;
extern int _AREA_40;
extern int _AREA_41;
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_922_2009050(void);
extern void OvlFunc_922_2009154(void);
extern void OvlFunc_922_20092cc(void);
extern void OvlFunc_922_20095dc(void);

void OvlFunc_922_2009948(void)
{
    unsigned char *gp;
    int a, b;
    int d;

    gp = gState;
    if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_3e) {
        a = 8;
        b = 0x2a;
        __Func_8010704(8, 0x1d, 0xf, 5, a, b);
        OvlFunc_922_2009050();
    } else if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_3f) {
        a = 0;
        b = 0x1c;
        __Func_8010704(0xc, 8, 0xa, 0x12, a, b);
        OvlFunc_922_2009154();
    } else if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_40
               && *(short *)(gp + (0xe1 << 1)) != 1) {
        a = 0xc;
        b = 3;
        __Func_8010704(0xc, 0x15, 9, 0x10, a, b);
        OvlFunc_922_20092cc();
    } else if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_41) {
        d = *(unsigned short *)(gp + (0xe1 << 1)) - 1;
        if ((unsigned short)d <= 1) {
            a = 0x16;
            b = 0x14;
            __Func_8010704(0xe, 0xa, 9, 8, a, b);
        } else {
            a = 0x14;
            b = 0x2d;
            __Func_8010704(7, 0x2d, 0xb, 4, a, b);
        }
        OvlFunc_922_20095dc();
    }
}
