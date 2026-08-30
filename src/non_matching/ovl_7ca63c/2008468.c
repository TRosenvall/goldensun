/*
 * OvlFunc_944_2008468 -- asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: argument emission interleave. 23 lines against 23, TWO differing:
 *      rom   ldr r2,=0x1410000 / mov r0,#0x0 / lsl r1,#0x10
 *      ours  ldr r2,=0x1410000 / lsl r1,#0x10 / mov r0,#0x0
 * The ROM emits the slot argument BETWEEN the pooled load and the shift.
 *
 * TRIED AND REJECTED, both byte-identical to the version below: naming the
 * slot in a local; naming the shifted argument in a local.
 *
 * Same class as OvlFunc_932_20082cc and OvlFunc_946_2009494. Note the key found
 * for OvlFunc_964_2008cd0 -- retyping the memory operand so sched2 sees a
 * different alias set -- cannot apply here: every operand is a constant and
 * there is no memory reference to retype.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);
extern void OvlFunc_944_20084b0(void);

void OvlFunc_944_2008468(void)
{
    __CutsceneStart();
    __MapActor_SetPos(0, 0xa4 << 16, 0x1410000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_944_20084b0();
    __CutsceneEnd();
}
