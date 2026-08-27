/* OvlFunc_948_2009fd8 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c.s
 * Best screen: 97 instructions against the ROM's 97, 12 differing.
 *
 * BLOCKER CLASS: the r0-against-a-shift rotation, four times over.
 *
 *     rom    mov r1, #0xe8 / mov r2, #0xda / mov r0, #8 / lsl r1, #16 / lsl r2, #18
 *     ours   mov r1, #0xe8 / mov r2, #0xda / lsl r1, #16 / lsl r2, #18 / mov r0, #8
 *
 * The ROM sets the slot number BETWEEN the two constant bases and their two
 * shifts; gcc puts it after both shifts. Four __MapActor_SetPos calls, three
 * lines out of place each.
 *
 * This is the fourth function to end on exactly this shape (see
 * src/non_matching/ovl_79e5c0/2008304.c and ovl_7892c8/20085cc.c). Two levers
 * were tried here and neither moves it: an unprototyped
 * `extern void __MapActor_SetPos();` reproduces the same 12, and the
 * return-type lever (int with a trailing return 0) makes it worse at 15.
 *
 * Note what DOES match, because it constrains the source. The counter at
 * .L2f80 is incremented, compared against 0x10 in the SAME expression that
 * wrote it, then RE-READ for the switch -- three separate accesses, which is
 * why it is written as a plain global rather than cached in a local. The
 * eleven-slot table starting at 2 comes out of case 2/4/6/8/10/12 with the odd
 * numbers falling to the default, and the case bodies are in DESCENDING order
 * (12 first, 2 last), which is what the block layout says.
 */
extern int L2f80 __asm__(".L2f80");
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_948_200a0c4(int a, int b);

void OvlFunc_948_2009fd8(void)
{
    L2f80++;
    if (L2f80 > 0x10)
        L2f80 = 0;
    switch (L2f80) {
    case 12:
        if (__GetFlag(0xee7) == 0)
            __MapActor_SetPos(8, 0xe8 << 16, 0xda << 18);
        if (__GetFlag(0xee8) == 0)
            __MapActor_SetPos(9, 0x94 << 17, 0xce << 18);
        if (__GetFlag(0xee9) == 0)
            __MapActor_SetPos(0xa, 0xa4 << 17, 0xbe << 18);
        if (__GetFlag(0xeea) == 0)
            __MapActor_SetPos(0xb, 0xb4 << 17, 0xda << 18);
        break;
    case 10: OvlFunc_948_200a0c4(8, 0); break;
    case 8:  OvlFunc_948_200a0c4(9, 0); break;
    case 6:  OvlFunc_948_200a0c4(0xa, 0); break;
    case 4:  OvlFunc_948_200a0c4(0xb, 0); break;
    case 2:  OvlFunc_948_200a0c4(0xc, 1); break;
    }
}
