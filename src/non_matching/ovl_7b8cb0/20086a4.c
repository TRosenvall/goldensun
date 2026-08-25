/* OvlFunc_931_20086a4  [ovl_7b8cb0]  --  0x020086a4
 *
 * Source asm: goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c.s
 *
 * Two of twenty-eight differ, and this is the first function PARKED BY
 * PREDICTION rather than by discovery -- the mechanism was settled earlier the
 * same round and it said in advance that this one cannot match.
 *
 *     rom    mov r2, #0x10 / mov r1, #0x2 / neg r2, r2 / mov r0, #0
 *     ours   mov r2, #0x10 / neg r2, r2 / mov r1, #0x2 / mov r0, #0
 *
 * The ROM splits a two-instruction constant (-0x10, built `mov`/`neg`) around
 * another argument. That is the arg-interleave shape, and the basic-block lever
 * retires it -- but only when the value can be assigned in a block that
 * DOMINATES the call and is different from it. THIS FUNCTION HAS NO BRANCH.
 *
 * Tried anyway, to confirm the prediction rather than assume it:
 *
 *     literal at the call site            2 of 28   (this file)
 *     `n = -0x10;` at the top of the      6 of 28   (worse -- gcc keeps it
 *       function, which is the lever                 live, exactly as
 *                                                    update_equiv_regs says)
 *
 * See docs/elevation.md, "WHY THE BASIC-BLOCK LEVER WORKS": rebuilding needs
 * REG_BASIC_BLOCK < 0, which requires the pseudo to span more than one basic
 * block, and a straight-line function has one. This is unreachable in plain C.
 */
extern unsigned int iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Func_8091e9c(int a);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_931_20086a4(void)
{
    unsigned char *base;
    unsigned char *q;

    base = (unsigned char *)iwram_3001ebc;
    __CutsceneStart();
    q = (unsigned char *)__MapActor_GetActor(0) + 0x55;
    *q = 0;
    __PlaySound(0x7b);
    __Func_8092208(0, 2, -0x10);
    base += 0xb6 << 1;
    __Func_8091e9c(*(short *)base);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
