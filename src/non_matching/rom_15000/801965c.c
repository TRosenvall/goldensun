/* Func_801965c (0x0801965c) -- NON-MATCHING.
 * Blocker class: a redundant copy of the loop bound into a HIGH register
 * (r12), the wall recorded three times already for r7/r8 copies.
 *
 * 51 lines against the ROM's 49, 36 differing. The prologue, the guard and the
 * loop's shape all match; the residue centres on two ROM instructions we do
 * not emit and two of ours the ROM does not:
 *
 *   rom    mov r12, r5          <- the loop bound copied to a high register
 *          add r2, r6, r2       <- the walking pointer formed AFTER the
 *                                  peeled first load, which is
 *                                  `ldrh r3, [r6, r2]` -- an INDEXED load off
 *                                  the block base, not a pointer dereference
 *   ours   mov r1, #0x0         <- the empty-path byte index hoisted
 *          b L2 / L2:           <- a branch to the next label
 *
 * and we compare against r5 directly where the ROM compares against r12.
 *
 * MEASURED (rom 49 lines):
 *   `src` as a named pointer before the loop                51, 43
 *   the source expression INLINED into the loop body        51, 36  <- best
 *   a named index `k` with `blk + k + i * 2`                50, 37
 *   -fno-strength-reduce                                    48, 32
 *   -fno-gcse                                               44, 35
 *   -fno-strict-aliasing                                    51, 36
 *   -fno-schedule-insns2                                    51, 41
 *
 * Neither flag that changes the length improves the match; both cut
 * instructions the ROM has. Inlining the source expression rather than naming
 * a `src` pointer is a real 43 -> 36 and is kept: it lets gcc build the
 * address after the guard, as the ROM does, instead of hoisting it above.
 *
 * WHAT IS RIGHT, and is the reusable part:
 *
 *   THE ASSIGNMENT'S VALUE IS WHAT IS TESTED. The ROM's
 *   `strh r3, [r7] / lsl r3, #0x10 / cmp r3, #0x0` is a 16-bit zero test on a
 *   value that came from a halfword LOAD -- so gcc already knows the high bits
 *   are clear and would not narrow it. Writing the copy and the test as one
 *   expression, `if ((out[i] = src[i]) == 0)`, makes the tested value the
 *   ASSIGNMENT's value, whose type is the `unsigned short` lvalue's, and that
 *   is what produces the `lsl #16`. Two separate statements do not.
 *
 *   TWO ZEROS THROUGH HALFWORD STORES, ONE POOLED AND ONE NOT. The ROM has
 *   `mov r3, #0x0` for the store at +0x12b2 and `ldr r3, =0x0` for the
 *   terminator. gcc CSEs ours into a single pooled zero used at both. There is
 *   no `_CONST_0` in const.sym and none is wanted -- this is the same
 *   duplicate-constant CSE that parked OvlFunc_970_20092ac, in its cheapest
 *   possible form (one line, not five), and it confirms from a third angle
 *   that a POOLED ZERO through a halfword store is ordinary gcc output.
 *
 * NEXT: nothing source-level. The `mov r12, r5` is the wall.
 */
extern unsigned char *iwram_3001e8c;
extern void BufferString(int a, int b);

int Func_801965c(int a, unsigned short *out, unsigned int n)
{
    unsigned char *blk;
    unsigned int i;

    blk = iwram_3001e8c;
    *(unsigned short *)(blk + 0x12b2) = 0;
    BufferString(a, 1);
    n--;
    for (i = 0; i < n; i++) {
        if ((out[i] = ((unsigned short *)(blk + (0xeb << 4)))[i]) == 0)
            break;
    }
    out[i] = 0;
    return i;
}
