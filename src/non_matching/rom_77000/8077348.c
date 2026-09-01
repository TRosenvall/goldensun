/* Func_8077348 (0x08077348) -- NON-MATCHING.
 * Blocker class: REGISTER-ROLE SWAP across the loop's call.
 *
 * 36 lines against the ROM's 36, TWELVE differing, and every one is the same
 * exchange: which of the two long-lived values gets the callee-saved register
 * and which is spilled to the stack.
 *
 *     rom    r7 = n (the party size), the walking pointer spilled to [sp]
 *     ours   r7 = the walking pointer, n spilled to [sp]
 *
 * Both spill exactly one value per iteration, so the cost is identical and the
 * instruction count is identical; only the roles differ. The ROM's spill/reload
 * pair sits around `bl GetUnit` in both streams.
 *
 * One consequence shows separately and is worth naming: the ROM increments the
 * pointer BEFORE spilling it (`ldrb r0, [r2] / add r2, #1 / str r2, [sp]`), so
 * the saved value is the ALREADY-ADVANCED pointer -- a plain `*p++`. Ours
 * spills n instead, so the increment has nowhere to sit and lands after the
 * call. That difference is downstream of the role swap, not independent of it.
 *
 * MEASURED, none better than the 12 below:
 *   `for (i = 0; i < n; i++)` over `gState[K + i]`      36 lines, 12  <- best
 *   an explicit `p` with `GetUnit(*p++)`                33 lines, 28
 *   an explicit `p` with `GetUnit(p[i])`                34 lines, 30
 *   `i = 0; do { ... } while (i != n);`                 33 lines, 29
 *
 * The three alternatives all LOSE instructions, which says they change the
 * induction structure rather than the allocation: the ROM keeps two induction
 * variables (a countdown in r5 and a walking pointer) plus n, and only the
 * index form reproduces that.
 *
 * WHAT IS RIGHT: the two separate guards. The ROM tests `n == 0` and then
 * tests `0 < n` again with the zero it already has in r6 (`cmp r6, r7 / bge`),
 * which is the for-loop entry test, not a duplicate -- an early
 * `if (n == 0) return 0;` followed by `for (i = 0; i < n; i++)` gives both, in
 * order, with gcc reusing the accumulator's zero for the second compare. The
 * trailing `mov r6, r0` after `__divsi3` is likewise correct and comes from
 * assigning the quotient back to the accumulator before returning it.
 *
 * NEXT: nothing source-level outstanding. This wants whatever cracks the
 * register-role-swap class.
 */
extern int GetPartySize(void);
extern unsigned char *GetUnit(int id);
extern unsigned char gState[];

int Func_8077348(void)
{
    int n;
    int sum;
    int i;

    n = GetPartySize();
    sum = 0;
    if (n == 0) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        sum = sum + GetUnit(gState[(0xfc << 1) + i])[0xf];
    }
    sum = sum / n;
    return sum;
}
