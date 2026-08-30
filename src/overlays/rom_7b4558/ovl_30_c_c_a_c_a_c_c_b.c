/* Cluster OvlFunc_927_2009328..OvlFunc_927_2009328 extracted from
 * goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c.s.
 *
 * The OvlFunc_927 cutscene template again, at actor slot 0xc.  Two values are
 * held in callee-saved registers and both are written where the ROM starts
 * building them, not where they are used:
 *
 *   x = 0xac << 1   -- `mov r6, #0xac` sits between __MapActor_GetActor and
 *      __CutsceneStart and the `lsl r6, #1` lands after OvlFunc_927_2008ea8, so
 *      the assignment goes before __CutsceneStart even though the first use is
 *      two calls later.  It feeds the third argument of all four
 *      OvlFunc_927_2008d90 calls.
 *   y = 0xc0 << 10  -- same split around __CutsceneWait(0x3c), same as
 *      OvlFunc_927_2009de0 and OvlFunc_927_2009150.
 *
 * Like its sibling _c_b, this file's split-derived name falls inside the
 * ovl_30_c_c_a_c_a% O1 wildcard that belongs to an unrelated stem; the explicit
 * Makefile rule pins it to the default flags.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_927_2009328(void)
{
    unsigned char *e;
    int x;
    int y;

    e = __MapActor_GetActor(0xc);
    x = 0xac << 1;
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xc, 1);
    OvlFunc_927_2008d90(0xc, 0x86 << 2, x, 0xe0 << 11);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0x80 << 13), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xc, 1);
    __Func_8092848(0xc, 0, 0);
    __CutsceneWait(0x14);
    __Func_809259c(0xc, 2);
    __MapActor_Surprise(0xc, 0x81 << 1);
    y = 0xc0 << 10;
    __CutsceneWait(0x3c);
    OvlFunc_927_2008d90(0xc, 0x92 << 2, x, y);
    __Func_809280c(0, 0xc, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xc, 0x9e << 2, x, y);
    __Func_809280c(0, 0xc, 0);
    __CutsceneWait(6);
    OvlFunc_927_2008d90(0xc, 0xaa << 2, x, y);
    __Func_809280c(0, 0xc, 0);
    __CutsceneWait(6);
    __SetFlag(0x302);
    __MapActor_SetPos(0xf, 0, 0);
    __CutsceneEnd();
}
