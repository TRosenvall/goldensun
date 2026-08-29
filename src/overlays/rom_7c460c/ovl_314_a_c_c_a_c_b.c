/* Cluster OvlFunc_939_2008c10..OvlFunc_939_2008c10 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c.s.
 *
 * Slotted between ovl_314_a_c_c_a_c_a.o and the rest of the overlay.
 *
 * `n = -8;` is assigned before the two guards, which is the basic-block lever:
 * the ROM splits the mov/neg pair around the other two arguments, and the
 * assignment has to be in a block that dominates the call. Written at the call
 * site it is two positions out.
 *
 * The zero stored at +0x55 is the RESULT of __GetFlag(0x201), which is known to
 * be zero on that path -- the ROM reuses r5 rather than building a fresh zero.
 * It reads oddly and is what the ROM does.
 */
extern int __GetFlag(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092208(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int a);

void OvlFunc_939_2008c10(void)
{
    int r;
    unsigned char *q;
    int n;

    n = -8;
    if (!__GetFlag(0x202))
        return;
    r = __GetFlag(0x201);
    if (r)
        return;
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    q = (unsigned char *)__MapActor_GetActor(0) + 0x55;
    *q = r;
    __MapActor_SetAnim(0, 2);
    __Func_8092208(0, 2, n);
    __CutsceneWait(0xd);
    __Func_8091e9c(0xc);
}
