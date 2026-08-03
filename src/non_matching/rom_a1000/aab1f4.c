/* Func_80ab1f4  [rom_a1000]  --  0x080ab1f4
 *
 * Source asm: goldensun/asm/rom_a1000/rom_aa538_c_c_a.s
 *
 * ComputePriceInWindow. Adds the window's own column (+0x0c) and row (+0x0e),
 * plus one for the border, to the incoming coordinates and forwards six
 * arguments to _Func_8022768.
 *
 * Blocker: ONE TRANSPOSITION. Nineteen instructions against nineteen, the
 * first nine identical, diverging only at:
 *
 *     rom    add r0, #1 / ldr r3, [sp, #0x10]
 *     ours   ldr r3, [sp, #0x10] / add r0, #1
 *
 * The ROM finishes the x coordinate before fetching the fifth argument off
 * the incoming stack; gcc fetches first. Everything after that realigns.
 *
 * TRIED, all identical 19-vs-19 at instruction 9:
 *   1. the expression written inline in the call
 *   2. both coordinates computed into locals first
 *   3. the +1 applied as a separate statement after both sums
 *   4. x finished before y, with y's +1 left in the call
 *
 * The incoming stack argument's load is not reachable from statement order,
 * which is consistent: it is an ABI fetch, not something the source sequences.
 *
 * NOTE ON THE FRAME. After `push {r5,r6,lr}` and `sub sp, #4`, the incoming
 * stack arguments sit at [sp,#0x10] and [sp,#0x14] -- the fifth and sixth
 * parameters. Getting that wrong would silently shift both, so it is worth
 * checking against the prologue rather than assuming.
 */
struct Win { unsigned char pad_00[0xc]; unsigned short col, row; };

extern void _Func_8022768(int x, int y, int a4, int a5, int a6);

void Func_80ab1f4(struct Win *w, int x, int y, int a4, int a5, int a6)
{
    _Func_8022768(w->col + x + 1, w->row + y + 1, a4, a5, a6);
}
