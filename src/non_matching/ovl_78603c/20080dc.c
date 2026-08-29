/* OvlFunc_885_20080dc -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_78603c/ovl_30_c_c_a_c_a_a.s
 * Best screen: 56 instructions against the ROM's 56, 9 differing.
 *
 * BLOCKER CLASS: argument precompute -- the ROM issues a CHEAP `mov` for a
 * later argument BEFORE r0, and gcc as we invoke it never does that.
 *
 *     rom    mov r2, #0xa / mov r0, #0xc / mov r1, #0    (__Func_809280c)
 *     ours   mov r0, #0xc / mov r1, #0    / mov r2, #0xa
 *
 *     rom    mov r1, #2 / mov r0, #0xc                   (__Func_80925cc)
 *     ours   mov r0, #0xc / mov r1, #2
 *
 * Three calls in one arm do this; a fourth splits a `mov`/`lsl` pair the same
 * way. Two other calls in the SAME arm -- __Func_809259c and __ActorMessage,
 * both `(0xc, k)` -- have r0 first in the ROM and match. So it is per-callee.
 *
 * A COMPILER PROBE SETTLES WHAT THE C CAN AND CANNOT REACH. Compiling four
 * one-line functions with gcc-2.96 under this tree's exact flags gives the
 * complete table of argument orders (scratch probe, batch 106):
 *
 *     callee   third argument            emitted order
 *     void     cheap constant            r0, r1, r2
 *     int      cheap constant            r1, r2, r0
 *     void     pool constant / mov+lsl   r2, r0, r1     <-- the ROM's shape
 *     int      pool constant / mov+lsl   r2, r1, r0
 *     void     a global read             ldr base, r0, ldr r2, r1
 *
 * The ROM's order IS the "void callee, expensive third argument" row -- but
 * with a third argument that assembles to one cheap `mov`. gcc's
 * `precompute_register_parameters` (calls.c:805) decides by `rtx_cost` on the
 * EXPANDED argument, and `#0xa` never clears the threshold. There is no
 * spelling of a value that both costs more than 2 at expand and assembles to
 * `mov r2, #0xa`.
 *
 * MEASURED, all 9 of 56:
 *   the three callees declared `int` (the return-type lever, batch 99)
 *   all callees declared `int`
 *   the three callees unprototyped, `extern __Func_809280c();`
 *   the trailing arguments as named locals in a dominating block
 *     (the basic-block lever -- gcc constant-propagates them away)
 *   short and unsigned char parameter types on the third argument
 *   -fno-schedule-insns, -fno-schedule-insns2, -fno-gcse,
 *   -fno-strict-aliasing, -fno-expensive-optimizations, -fno-force-mem, -O1
 *   -fno-rerun-cse-after-loop
 *
 * THE RETURN-TYPE LEVER IS NOT BROKEN, it just does not produce this order.
 * The probe confirms it still works: `int` moves r0 LAST (r1, r2, r0). The ROM
 * wants r0 in the MIDDLE. That is a third order the lever cannot express, and
 * it is worth recording because the lever's write-up in docs/elevation.md
 * describes only the two it can.
 *
 * WHAT IS RIGHT, and it is most of the function: the message base held in a
 * register (`ldr r5, =0xf76` then `add r0, r5, #1` / `add r0, r5, #2`) comes
 * from a named `int m = 0xf76` and the two follow-up ids written as `m + 1` and
 * `m + 2` -- this is the message-base-in-a-register family, and this is the
 * first function where that reading reproduces exactly.
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_885_20080dc(void)
{
    int m;

    __CutsceneStart();
    if (__GetFlag(0x815)) {
        __MessageID(0x11c4);
        __ActorMessage(0xc, 0);
    } else {
        m = 0xf76;
        __MessageID(m);
        __Func_809280c(0xc, 0, 0xa);
        __Func_80925cc(0xc, 2);
        __CutsceneWait(6);
        __Func_8092c40(0xc, 0);
        if (__Func_8091c7c(0, 0) == 0)
            __MessageID(m + 1);
        else
            __MessageID(m + 2);
        __Func_809259c(0xc, 3);
        __ActorMessage(0xc, 0);
        __Func_8092adc(0xc, 0xc0 << 8, 0xa);
    }
    __CutsceneEnd();
}
