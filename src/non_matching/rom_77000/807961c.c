/* AddPartyMember (0x0807961c) -- NON-MATCHING.
 * Blocker class: loop-invariant placement relative to the loop guard, plus the
 * rematerialised pool load and two join branches that follow from it.
 *
 * 34 lines against the ROM's 37, 23 differing. THE LOOP BODY IS EXACT --
 * ldrb / add / cmp / beq / add / cmp / blt, all seven instructions, same
 * registers. The whole residue is where the loop's invariant setup lives:
 *
 *   rom    mov r2, #0x0 / cmp r2, r5 / bge L0        <- guard FIRST
 *          ldr r0, =gState / mov r3, #0xfc / lsl r3, #0x1 / add r1, r0, r3
 *   ours   ldr r0, =gState / mov r3, #0xfc / lsl r3, #0x1
 *          mov r2, #0x0 / add r1, r0, r3 / cmp r2, r5 / bge L0
 *
 * and the three lines that costs: the ROM rematerialises `ldr r0, =gState` on
 * the guard's not-taken path, which makes the store block a join reached from
 * two places, which costs `b L3` after the loop and `b L4` after the found
 * block. We compute the base once before the guard, so our store block IS the
 * not-taken path and needs no branches.
 *
 * MEASURED (rom 37 lines):
 *   `g = gState; p = g + 0x1f8;` + `return n` inside the loop   34, 27
 *   the same with `goto found;` and the return placed last      34, 23  <- best
 *   `p = gState + 0x1f8;` as one expression                     30, 30 (folds
 *                                to a pooled gState+504 -- the named base is
 *                                REQUIRED and this is what it costs to drop)
 *   a second named `gState` read for the store                  34, 23 (CSEd)
 *   `p = gState; p += 0xfc << 1;` as two statements             34, 23
 *   an EXPLICIT `i = 0; if (i < n) { ... do {} while }` guard   38, 31 (worse;
 *                                and note gcc does NOT fold `i = 0; if (i < n)`
 *                                to `if (n > 0)` -- it compares the variable,
 *                                exactly as the `for` does, so the hand-written
 *                                guard buys nothing and costs four lines)
 *   -fno-strict-aliasing / -fno-gcse / -fno-strength-reduce /
 *     -fno-rerun-cse-after-loop / -fno-schedule-insns2          34, 23 (ALL five
 *                                inert, run early per the batch-173 rule)
 *
 * Five source spellings and five flag groups all land on exactly 34 and 23.
 * That is the "if a source construct controlled it, one of these would have
 * moved" argument, and -fno-gcse being inert says the hoist is the loop
 * optimiser's, not global CSE's. The ROM's version is not cheaper -- it is
 * three instructions LONGER -- so this is gcc declining to rematerialise a pool
 * load that the original build did rematerialise, which is the same allocator
 * difference the scratch-register and callee-saved-copy parks record.
 *
 * WHAT IS RIGHT: `goto found` for the early exit (27 -> 23, and it is what puts
 * the `return n` block out of line as the ROM does); the walking pointer
 * `*p++`; the NAMED index `k = i + 0x1f8` for the register-plus-register store;
 * and the named `gState` base without which the offset folds into the pool.
 *
 * NEXT: nothing source-level found in seven probes.
 */
extern unsigned char gState[];
extern int GetPartySize(void);
extern void SetFlag(int id);

int AddPartyMember(int id)
{
    unsigned char *g;
    unsigned char *p;
    int n;
    int i;
    int k;

    n = GetPartySize();
    SetFlag(id);
    g = gState;
    p = g + 0x1f8;
    for (i = 0; i < n; i++) {
        if (*p++ == id)
            goto found;
    }
    k = i + 0x1f8;
    g[k] = id;
    return n + 1;
found:
    return n;
}
