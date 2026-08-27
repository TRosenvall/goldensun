/* OvlFunc_959_200a308
 *
 * Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_a_b.s.
 *
 * Opens the sealed door when both conditions hold. Needs CSE_CFLAGS; the stack
 * argument pair is named adjacent to the call and SHARED across two calls,
 * because the ROM shares it.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_801776c(int a, int b);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_959_200a26c(void);

void OvlFunc_959_200a308(void)
{
    char *p;
    int e;
    int f;

    p = iwram_3001ebc;
    p += 0xcb8;
    if (*(short *)(p + (unsigned int)0) != 0 && __GetFlag(0x947) == 0) {
        __Func_801776c(0x1528, 1);
        __PlaySound(0xbc);
        __CutsceneWait(1);
        e = 0x11;
        f = 0x52;
        __Func_80105d4(6, 0x4d, 1, 2, e, f);
        __CutsceneWait(5);
        __Func_80105d4(7, 0x4d, 1, 2, e, f);
        __CutsceneWait(1);
        OvlFunc_959_200a26c();
        __SetFlag(0x947);
    }
}
