/* OvlFunc_957_2008bc8  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.s
 * Best screen: 11 instructions in disagreeing regions, of 39 (rom 39, ours 36).
 *
 * BLOCKER CLASS: gcc CSEs two loads of the same address.
 *
 * The ROM reads the area field TWICE, once unsigned and once signed:
 *
 *      ldrh  r1, [r3, #0x0]        <- unsigned, used later via lsl/asr
 *      mov   r0, #0x0
 *      ldrsh r2, [r3, r0]          <- signed, compared immediately
 *
 * gcc keeps only the signed load and derives the other comparison from it,
 * dropping the `ldrh` and the `lsl #16 / asr #16` that sign-extends it -- three
 * instructions, and every register downstream is renamed.
 *
 * That is correct: the two loads read the same halfword and the sign-extension
 * of the unsigned one is the signed one. No source spelling separates them,
 * because nothing between the loads can change the value -- the only store in
 * between is to REG_BLDALPHA, a different object. `volatile` would defeat the
 * analysis and is a fakematch; see reports/fakematch-worklist.md.
 *
 * THE AREA IDS ARE SOLVED. `_AREA_92` and `_AREA_97` both reproduce their pool
 * loads exactly, `_AREA_92` being one of the eight added in batch 67. The
 * remaining defect is entirely in how the field is read, not in the constants.
 *
 * The `(short)h` cast is the right spelling for the ROM's `lsl #16 / asr #16`
 * and should not be re-derived; it is simply never reached because the load it
 * would sign-extend has been eliminated.
 */
#include "gba/io.h"

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_92;
extern int _AREA_97;
extern void __Func_8092950(int a, int b);

void OvlFunc_957_2008bc8(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned short h;
    int v;
    int t;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    h = *(unsigned short *)g;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_92)) {
        t = 0x80 << 5;
        *(unsigned short *)REG_ADDR_BLDALPHA = t;
    }
    if ((short)h == (int)(&_AREA_97)) {
        __Func_8092950(0x10, 1);
        __Func_8092950(0x11, 4);
        __Func_8092950(0x12, 0xb);
        __Func_8092950(0x13, 2);
        __Func_8092950(0x14, 3);
    }
}
