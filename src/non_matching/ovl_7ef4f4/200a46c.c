/* OvlFunc_965_200a46c -- NOT MATCHING. 2 of 30, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a.s
 *
 * Blocker: PROLOGUE ORDER, and nothing else. The ROM adjusts the stack AFTER
 * loading the first argument:
 *
 *     rom    push {lr} / ldr r0, =0x985 / sub sp, #0x8 / bl __GetFlag
 *     ours   push {lr} / sub sp, #0x8 / ldr r0, =0x985 / bl __GetFlag
 *
 * Everything else matches, including both arms rebuilding the stack-arg pair
 * (the batch-52 rule -- the ROM materialises it twice, so the locals are
 * assigned inside each arm).
 *
 * gcc emits the frame adjustment as part of the prologue and nothing in the C
 * reaches it. The stack slots are not touched until inside the arms, so it is
 * not a liveness question either.
 *
 * SECOND FUNCTION WITH THIS EXACT RESIDUAL: OvlFunc_968_2009644
 * (src/non_matching/ovl_7f2f14/2009644.c) has the same `sub sp` displacement as
 * half of its four. Two instances make it a shape worth naming rather than a
 * one-off, though there is nothing to try for either.
 */
extern int __GetFlag(int id);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);

void OvlFunc_965_200a46c(void)
{
    int m;
    int n;

    if (!__GetFlag(0x985)) {
        m = 0x11;
        n = 0x4e;
        __Func_8010788(0x24, 0x4e, 1, 2, m, n);
    } else {
        m = 0x11;
        n = 0x4e;
        __Func_8010788(0x22, 0x4e, 1, 2, m, n);
    }
}
