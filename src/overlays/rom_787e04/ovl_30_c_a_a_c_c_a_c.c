// fakematch
/* OvlFunc_887_20081e0  --  0x020081e0
 *
 * From goldensun/asm/overlays/rom_787e04/ovl_30_c_a_a_c_c_a_c.s, which held
 * this function alone, so no split was needed.
 *
 * PARKED AT 2 OF 61 as "arg-interleave at an UNGUARDED site":
 *
 *     rom   mov r1,#0x81 / mov r2,#0x0 / mov r0,#0x10 / lsl r1,#0x1
 *     ours  mov r1,#0x81 / mov r2,#0x0 / lsl r1,#0x1  / mov r0,#0x10
 *
 * Pinning the three argument registers and assigning them in the ROM's order
 * matches:
 *
 *     q1 = 0x81;  q2 = 0;  q0 = 0x10;  q1 <<= 1;
 *
 * The park's reasoning was that the function's only conditional branch is
 * fourteen instructions AFTER this call, so no block dominates the site and the
 * dominating-block lever has nothing to work with. That is correct about that
 * lever and is the same shape of conclusion corrected in batches 193 and 196 --
 * a pin does not need a dominating block, because it names the hard register
 * and the placement is decided at the assignment.
 *
 * This is the reachable sub-case of the interleave: the displaced `mov r0` has
 * to land INSIDE another register's build, between `mov r1` and its shift, and
 * the shift is an operation whose order the source can set. Compare
 * src/non_matching/ovl_7ebdfc/2008120.c, where the interleaved movs have no
 * such operation anywhere and no pin reaches them.
 *
 * The park kept its candidate body and it screens at exactly the 2 of 61
 * recorded, so the starting point was verified before anything was changed.
 */

extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __Func_809259c(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_Emote(int a, int b, int c);
extern int __MapActor_DoAnim(int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_887_20081e0(void)
{
    unsigned char *p;

    __CutsceneStart();
    __Func_809259c(0x10, 2);
    __CutsceneWait(0x1e);
    __MessageID(0xf5b);
    __Func_8092848(0, 0x10, 0xa);
    __Func_8093040(0x10, 0, 6);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81;
        q2 = 0;
        q0 = 0x10;
        q1 <<= 1;
        __MapActor_Emote(q0, q1, q2);
    }
    __Func_809259c(0x10, 1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x10, 4);
    __CutsceneWait(0x14);
    __Func_8092c40(0x10, 0);
    if (__Func_8091c7c(0, 0) == 1) {
        p = iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
    }
    __Func_809259c(0x10, 1);
    __CutsceneWait(0x14);
    __Func_8093040(0x10, 0, 4);
    __CutsceneEnd();
}
