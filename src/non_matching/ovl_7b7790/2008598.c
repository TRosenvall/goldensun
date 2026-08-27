/* OvlFunc_929_2008598 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7b7790/ovl_314_c_c_c_c_c_c.s
 * Best screen: 4 differing of 55, streams the same length.
 *
 * BLOCKER CLASS: the interleaved shifted-constant argument.
 *
 *     rom    mov r0, #0xe6 / mov r1, #0 / mov r2, r5 / mov r3, #0x14 / lsl r0, #17
 *     ours   mov r0, #0xe6 / lsl r0, #17 / mov r1, #0 / mov r2, r5 / mov r3, #0x14
 *
 * The ROM defers the `lsl` past the other three argument moves; we keep the
 * `mov`/`lsl` pair together. Only the FIRST of the two calls in that arm does
 * this -- the second has the pair adjacent in the ROM too -- so it is the
 * scheduler, not a property of the argument.
 *
 * This is the blocker catalogued in
 * src/non_matching/overlays/interleaved_arg_setup.c, and the note there that a
 * named local KEEPS the pair together is confirmed: naming the shared
 * 0x8e << 18 changes nothing (it is already shared through r5), and
 * -fno-schedule-insns2 makes it worse, 9 of 55.
 *
 * Everything else is right, including two pieces of gcc's own arithmetic that
 * look like source decisions and are not: the stored 0x209 is derived from the
 * 0x1c0 offset already in the register, and the 0x1c2 gState offset is then
 * derived from THAT by `sub r2, #0x47`. The source just writes the three
 * constants.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void __ClearFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);

int OvlFunc_929_2008598(void)
{
    char *p;
    unsigned char *g;
    int area;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    area = *(short *)(g + 0x1c2);
    if (area == 4 || area == 7) {
        OvlFunc_common0_70(0xf8 << 16, 0, 0x1a10000, 0x14);
    } else if (area == 6) {
        OvlFunc_common0_70(0xe6 << 17, 0, 0x8e << 18, 0x14);
        OvlFunc_common0_70(0xf2 << 17, 0, 0x8e << 18, 0x14);
    } else if (area == 8) {
        __ClearFlag(0x12f);
        __MapActor_SetAnim(0xa, 6);
    }
    return 0;
}
