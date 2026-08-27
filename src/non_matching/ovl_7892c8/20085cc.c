/* OvlFunc_888_20085cc -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_a.s
 * Best screen: 96 instructions against the ROM's 95, 25 differing.
 *
 * BLOCKER CLASS: `mov r0, #0xa` transposed with two `lsl`s, plus one extra
 * instruction in the shared tail.
 *
 *     rom    mov r0, #0xa / lsl r1, #0x10 / lsl r2, #0xf / bl __MapActor_SetPos
 *     ours   lsl r1, #0x10 / lsl r2, #0xf / mov r0, #0xa / bl __MapActor_SetPos
 *
 * That is the r0-against-a-shift rotation the return-type lever does not reach
 * (batch 100), now seen on a third function.
 *
 * The extra instruction is a label the ROM does not have: case 0x14's `bne`
 * goes straight to the shared `__ClearFlag(0x12f)` block, where ours falls
 * through a label of its own into it. Writing case 0x14 as a genuine C
 * fallthrough into the 0x1d/0x20/0x23 arm -- which is what the ROM's layout
 * suggests -- does NOT change the output, so the merge gcc is doing here is not
 * the one the source expresses.
 *
 * WHAT IS RIGHT: the 26-slot table's groups (0xa/0xb/0xc, 0x14, 0x15,
 * 0x1d/0x20/0x23) and their block order; the stored 0x209 being derived by gcc
 * from the 0x1c0 offset already in a register; and the two `if (GetFlag(0x109)
 * == 0)` guards.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void OvlFunc_888_200b270(void);
extern void OvlFunc_888_200888c(void);

int OvlFunc_888_20085cc(void)
{
    char *p;
    unsigned char *g;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(1);
    __CutsceneWait(1);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 0xa:
    case 0xb:
    case 0xc:
        if (__GetFlag(0x855))
            __MapActor_SetPos(0xa, 0xc8 << 16, 0xa0 << 15);
        __ClearFlag(0x12f);
        break;
    case 0x14:
        OvlFunc_888_200b270();
        if (__GetFlag(0x109) == 0)
            OvlFunc_888_200888c();
        __ClearFlag(0x12f);
        break;
    case 0x1d:
    case 0x20:
    case 0x23:
        __ClearFlag(0x12f);
        break;
    case 0x15:
        OvlFunc_888_200b270();
        __SetFlag(0x201);
        if (__GetFlag(0x109) == 0)
            OvlFunc_888_200888c();
        __ClearFlag(0x12f);
        break;
    }
    return 0;
}
