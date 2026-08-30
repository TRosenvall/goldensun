/* Cluster OvlFunc_929_2008598..OvlFunc_929_2008598 split out of goldensun/asm/overlays/rom_7b7790/ovl_314_c_c_c_c_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * THE DECLARATION LEVER HAS A THIRD FORM: `void` versus `int` RETURN on the
 * mismatching callee itself. The plain body is 4 differing -- `lsl r0, #0x11`
 * scheduled before the other three argument registers, where the ROM defers it
 * past `mov r3, #0x14`. Prototyping OvlFunc_common0_70 as `void` causes that;
 * declaring it `int`, or leaving it implicit, is byte-exact. Both directions
 * were screened. The existing `extern void` declaration elsewhere in the tree
 * is what makes the wrong one the obvious guess.
 *
 * The shared `mov r3, #0x14 / bl` tail cross-jumps on its own once the call is
 * written in both arms; no goto is needed.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void __ClearFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern int OvlFunc_common0_70(int a, int b, int c, int d);

int OvlFunc_929_2008598(void)
{
    char *p;
    unsigned char *g;
    int e;
    int z;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    e = *(short *)(g + (0xe1 << 1));
    if (e == 4 || e == 7) {
        OvlFunc_common0_70(0xf8 << 16, 0, 0x1a10000, 0x14);
    } else if (e == 6) {
        z = 0x8e << 18;
        OvlFunc_common0_70(0xe6 << 17, 0, z, 0x14);
        OvlFunc_common0_70(0xf2 << 17, 0, z, 0x14);
    } else if (e == 8) {
        __ClearFlag(0x12f);
        __MapActor_SetAnim(0xa, 6);
    }
    return 0;
}
