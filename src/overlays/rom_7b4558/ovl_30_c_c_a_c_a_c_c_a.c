/* Cluster OvlFunc_927_2009244..OvlFunc_927_2009244 extracted from
 * goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c.s.
 *
 * The OvlFunc_927 cutscene template at actor slot 0xb, with two things the
 * other members of the family do not have:
 *
 *   TWO __MapActor_GetActor(0) CALLS, one per coordinate.  The ROM fetches the
 *   entity, reads the signed halfword at +0xa, fetches it AGAIN and reads +0x12.
 *   Only the first result needs a name -- it has to survive the second call --
 *   and the second read is written inline in the argument list, which is what
 *   puts the second `bl` between the two `ldrsh`.  Naming both, or naming
 *   neither, moves the fetches.
 *
 *   A BYTE STORE TO gState THROUGH A NAMED BASE.  `g = gState; g[0x22b] = 3;`
 *   gives `ldr r3, =gState / ldr r2, =0x22b / add r3, r2 / strb`, the ROM's
 *   spelling; the offset is too large for the mov/lsl pair the smaller gState
 *   offsets use, so it comes from the pool as its own entry rather than being
 *   folded into `=gState+555`.
 *
 * Split-derived name falls inside the ovl_30_c_c_a_c_a% O1 wildcard belonging to
 * an unrelated stem; the explicit Makefile rule pins it to the default flags.
 */
extern unsigned char gState[];
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
extern void __MapActor_Emote(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __Func_8091eb0(int a, int b);
extern void __SetFlag(int id);

void OvlFunc_927_2009244(void)
{
    unsigned char *e;
    unsigned char *g;
    int hx;

    e = __MapActor_GetActor(0xb);
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xb, 0);
    OvlFunc_927_2008d90(0xb, 0xcc << 1, 0xe4 << 1, 0xc0 << 11);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + (0xc0 << 13), 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xb, 1);
    __Func_8092848(0xb, 0, 0);
    __CutsceneWait(0x1e);
    __Func_809259c(0xb, 2);
    __MapActor_Emote(0xb, 0x103, 0);
    __PlaySound(0x93);
    __CutsceneWait(0x3c);
    hx = *(short *)(__MapActor_GetActor(0) + 0xa);
    OvlFunc_927_2008d90(0xb, hx, *(short *)(__MapActor_GetActor(0) + 0x12),
                        0x80 << 11);
    __CutsceneWait(0xa);
    __SetFlag(0x301);
    __MapActor_SetPos(0xe, 0, 0);
    g = gState;
    g[0x22b] = 3;
    __Func_8091eb0(0x35, 0);
    __CutsceneEnd();
}
