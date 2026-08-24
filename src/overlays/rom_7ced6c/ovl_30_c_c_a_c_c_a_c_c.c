/* Cluster OvlFunc_946_200958c..OvlFunc_946_200958c extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a.s.
 *
 * The .s held ONLY this function and no data after the splits above it, so no
 * further split was needed.
/*
 * THE ARG-INTERLEAVE LEVER: assign the shifted constant to a local in a
 * DIFFERENT BASIC BLOCK from the call.
 *
 * The ROM splits a shifted constant’\s mov/lsl pair around another argument:
 *
 *     rom    mov r1, #imm / mov r0, #imm / lsl r1, #n
 *     ours   mov r1, #imm / lsl r1, #n   / mov r0, #imm
 *
 * That was blocker class seven for thirty-six batches, with four parked members
 * and every lever in docs/elevation.md tried against it. The trigger turns out
 * to be nothing at the call site at all -- it is WHERE THE VALUE IS ASSIGNED:
 *
 *     literal at the call site                          contiguous
 *     named local, SAME basic block as the call         held in a callee-saved reg
 *     named local, assigned in a DIFFERENT basic block  *** INTERLEAVED ***
 *
 * Crossing a basic-block boundary stops gcc keeping the value in a register, so
 * it rematerialises at the call -- and its rematerialisation of a shifted
 * constant is the split pair, with the other argument scheduled into the gap.
 *
 * Here `x` and `y` are assigned before the `if`, and the call is after the
 * join. See docs/elevation.md and reports/arg-interleave.md.
 */
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8012078(int a, unsigned int b, unsigned int c, int d);
extern void __SetFlag(int id);

void OvlFunc_946_200958c(int slot)
{
    void *a;
    unsigned int x;
    unsigned int y;

    x = 0xc0 << 15;
    y = 0xa8 << 17;
    a = __MapActor_GetActor(0xd);
    if (a != 0)
        *((unsigned char *)a + 0x59) = 0;
    __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
    __Func_8012078(0, x, y, 0xfd);
    __SetFlag(0x242);
}
