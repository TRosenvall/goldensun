/* Func_80175c0 -- 0x080175c0  (asm/rom_15000/rom_15e8c_c_a_c_c_c.s)
 *
 * BLOCKER: register coalescing of two variables that both hold 0, plus an
 * outgoing-argument frame 8 bytes larger than the call needs.  18 of 43 differ.
 *
 * The middle of the function -- the whole BufferString call, the table index,
 * and the entire six-argument setup for Func_80165d8 including both stack
 * stores -- is instruction-for-instruction exact.  What is left is at the two
 * ends.
 *
 * PROGRESS, each an isolated edit:
 *
 *   38  naive
 *   26  named zero + named store addresses + single exit
 *   18  offset-before-base, complete-offset naming for the table  <-- best
 *
 * The 26 -> 18 step used two levers already in docs/elevation.md and both paid:
 *
 *   offset-before-base: `ldr r2, =0x12f4` sits BETWEEN the symbol address and
 *   its dereference in the ROM.  Assigning `off` before `base` reproduces it.
 *
 *   complete-offset naming: `base + ((idx << 1) + 0xeb0)` written inline made
 *   gcc fold incrementally into the base register and load with `[r5, #0]`.
 *   Hoisting the whole index into `int t` gave the ROM's `add r3, r2` +
 *   `ldrh r3, [r5, r3]` register-offset form exactly.  This is the same lever
 *   that failed on Func_801d94c, and the difference is instructive: there the
 *   named value was a compile-time constant and folded away; here it is a
 *   RUNTIME expression, so the name survives to influence code generation.
 *   Naming only helps when the value is not foldable.
 *
 * WHAT REMAINS
 *
 * 1. Two variables holding 0 that must NOT share a register.  The ROM keeps a
 *    zero in r6 across the BufferString call (used by both `strh` stores and by
 *    `str r6, [sp]`), and keeps the return value in r0:
 *
 *      rom   mov r6, r0 / mov r0, #0 / cmp r6, #0 / beq L0 / mov r0, r6
 *      ours  cmp r0, #0 / beq L0 / mov r6, r0 / L0: mov r0, r6
 *
 *    gcc coalesced my `zero` and my `ret` because both are 0 and it can prove
 *    the merge safe, which frees r0 and removes the ROM's `mov r6, r0` -- that
 *    single missing instruction is why we come out at 42 lines, not 43.  Four
 *    initialization positions were tried and none separates them:
 *
 *      ret = 0 as the first statement of the function        23 differ
 *      ret = 0 after the two guard tests                     27 differ
 *      ret declared before zero                              18 differ (ties)
 *      early returns instead of a ret variable               26 differ
 *
 *    There is no source-level way to say "these two zeros are different".
 *
 * 2. `sub sp, #0x10` where we emit `sub sp, #0x8`.  The call passes six
 *    arguments, so two stack words -- 8 bytes -- is what the ABI needs, and 8
 *    is what we reserve.  The ROM reserves 16 while storing only [sp] and
 *    [sp+4].  A seventh or eighth argument would have to be stored somewhere
 *    and the ROM stores nothing above [sp+4], so the extra 8 bytes is not an
 *    argument.  Unresolved: it is either a spill slot the original needed and
 *    we do not, or an alignment rounding this build does not reproduce.  Worth
 *    revisiting if another function shows the same 8-byte surplus -- two
 *    instances would separate "quirk" from "our register pressure is lower".
 */
extern int iwram_3001e8c;
extern int BufferString(int s, int n);
extern int Func_80165d8(int a, int b, int c, int d, int e, int f);

int Func_80175c0(int a, int b)
{
    char *base;
    unsigned short *p;
    int off;
    int zero;
    int idx;
    int t;
    int r;
    int ret;

    off = 0x12f4;
    base = (char *)iwram_3001e8c;
    zero = 0;
    p = (unsigned short *)(base + off);
    *p = zero;
    off += 2;
    p = (unsigned short *)(base + off);
    *p = zero;
    idx = BufferString(b, 1);
    t = (idx << 1) + (0xeb << 4);
    ret = 0;
    if (*(unsigned short *)(base + t) != 0 && a != 0) {
        r = Func_80165d8(a, idx, 0, 0, zero, 1);
        if (r != 0)
            ret = r;
    }
    return ret;
}
