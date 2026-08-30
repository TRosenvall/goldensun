/* OvlFunc_887_20081e0  [overlays/rom_787e04]
 *
 * Source asm: goldensun/asm/overlays/rom_787e04/ovl_30_c_a_a_c_c_a_c.s
 *
 * BLOCKER CLASS: arg-interleave at an UNGUARDED site. 2 of 61.
 *
 *     rom   mov r1,#0x81 / mov r2,#0x0 / mov r0,#0x10 / lsl r1,#0x1
 *     ours  mov r1,#0x81 / mov r2,#0x0 / lsl r1,#0x1  / mov r0,#0x10
 *
 * The function's only conditional branch is fourteen instructions AFTER this
 * call, so no block dominates the site and the dominating-block lever has
 * nothing to attach to. Seventh confirmed instance of that boundary. No
 * spellings were attempted, which is the correct response now that the shape
 * is established -- the six earlier instances measured between three and nine
 * spellings each and none moved a line.
 *
 * Everything else is exact, including the three two-argument calls that want
 * r1 filled first (fixed by declaring the callees `int`-returning) and the
 * iwram_3001ebc counter increment.
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
    __MapActor_Emote(0x10, 0x81 << 1, 0);
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
