/* Func_8015e8c  @  0x08015e8c  [rom_15000]
 * *** LANDS BYTE-IDENTICAL, BUT WITH ONE REGISTER PIN ***
 * A pin-free body at 2 of 23 is given at the bottom; read the choice note.
 *
 * Source asm: goldensun/asm/rom_15000/rom_15e8c_a_a.s   (path still current)
 *
 * FIGURE: 0.
 *   OK Func_8015e8c -- 52 bytes, 23 encodings and 1 relocations identical   (--func)
 *   OK whole file  -- 52 bytes, 23 encodings and 1 relocations identical   (--whole)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_15000/rom_15e8c_a_a.c \
 *     asm/rom_15000/rom_15e8c_a_a.s --whole
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   python3 tools/datacheck.py asm/rom_15000/rom_15e8c_a_a.s   -> clean, code only
 *   The .s holds exactly ONE thumb_func_start (Func_8015e8c), so it is already
 *   its own translation unit. INSTALL PATH: src/rom_15000/rom_15e8c_a_a.c
 *
 * PIN COUNT: 1.  tools/shimcount.py:
 *     register pins : 1
 *     *** has a fakematch-class shim and NO fakematch.txt row
 *   A fakematch.txt row IS REQUIRED for this body:
 *     Func_8015e8c       src/rom_15000/rom_15e8c_a_a.c
 *
 * ---------------------------------------------------------------------------
 * THE PARK'S DIAGNOSIS: HALF RIGHT, AND ITS MECHANISM IS WRONG.
 *
 * src/non_matching/rom_15000/rom_15e8c.c measured "REGISTER ALLOCATION ONLY.
 * Every instruction is right; r1 and r2 are swapped throughout". That much
 * SURVIVES -- the measured figure is 7 of 23 encodings at IDENTICAL size and
 * identical relocations, and all seven are the r1/r2 swap. (The park quoted no
 * objcmp figure at all; 7 of 23 is this brief's measurement.)
 *
 * Its explanation is refuted. The park wrote: "gcc-2.96 allocates in birth order
 * against REG_ALLOC_ORDER {3, 2, 1, 0, ...}, so whichever pseudo is created
 * first takes r3, then r2, then r1. In the ROM the 0xd98 constant is born before
 * the head pointer; in every formulation tried here the head pointer is born
 * first, and that one difference flips the whole function."
 *
 * THERE IS NO PSEUDO FOR THE 0xd98 CONSTANT. It is a RELOAD. The .18.greg dump
 * for the park's own body says so:
 *     ;; 4 regs to allocate: 32 36 37 34      <- head, node, next, base
 *     ;; 32 conflicts: 32 34 36 37 42 3 13
 *     Using reg 1 for reload 0
 *     Using reg 4 for reload 0
 * and the .17.lreg list has seven pseudos (32 head, 33 global address, 34 base,
 * 36 node, 37 next, 42 the 0xd9c constant, 43 the zero) -- no 0xd98. The address
 * is a MEM with an out-of-range constant offset; reload fixes it by loading the
 * offset into a reload register. So "birth order of the 0xd98 pseudo" cannot be
 * the lever, because that pseudo does not exist.
 *
 * WHAT ACTUALLY DECIDES IT. global_alloc runs before reload and allocates in
 * allocno_compare order. From .17.lreg (R used / L insns -> floor_log2(R)*R/L):
 *     head  reg 32   4 / 8   -> 1.00
 *     node  reg 36   5 / 10  -> 1.00   (prefers r0, it is the return value)
 *     next  reg 37   3 / 5   -> 0.60
 *     base  reg 34   3 / 7   -> 0.43   (prefers r3)
 * head is allocated first (ties break to the lower allocno, and 32 < 36), its
 * conflict set holds hard reg 3, so it takes the next register in
 * REG_ALLOC_ORDER: r2. Reload then cannot use r2 and takes r1. The ROM has it
 * the other way round, so THE WHOLE RESIDUE IS "head must be excluded from r2",
 * and reload's choice follows for free. The pinned version's dump confirms the
 * causal direction exactly: with head pinned to r1 the same two lines read
 * `Using reg 2 for reload 0`.
 *
 * SO THE LEVER IS ALLOCNO PRIORITY, NOT BIRTH ORDER -- and that is reachable
 * from C. See the pin-free body below: duplicating `*head = next` into both arms
 * of the inner `if` raises `next`'s REG_N_REFS from 3 to 4 BEFORE flow1 measures
 * it, so next outranks head (2*4/L > 1.0), takes r2 first, and head falls to r1.
 * Cross-jumping runs after reload and merges the duplicate back out, so the
 * duplication costs nothing in the emitted stream. That took 7 -> 2.
 *
 * CROSSED AND MEASURED (sweep_variants, figures are objcmp encodings of 23):
 *   SOURCE SHAPE -- ALL EXACTLY INERT AT 7, first=2:
 *     park body (u8* base + 0xd98 literals)                      7
 *     struct members, q->head / &q->head                         7
 *     struct members + named `head` pointer                      7
 *     struct members on a u8* base, cast                         7
 *     all four locals declared up front                          7
 *     union-wrapped struct (alias-set lever)                     7
 *     tail recomputed as &q->pad_000[0xd98] (defeat cse attempt)  7
 *     union Slot head/tail with an unsigned-int store             7
 *     head retyped `void **`                                      7
 *     offset forced through an r2-pinned local                    7
 *     integer-space base + 0xd98                                  7
 *     `next` declared before `node`                               7
 *   WORSE:
 *     early return instead of the nested if                      22 (+4 bytes)
 *     node->next = 0 hoisted above the inner if                  15
 *     stores reordered in the arms                                8
 *   MOVED (the crossing that mattered):
 *     next given a 4th reference inside the arms                  7 but first=7
 *         -- same count, but the first SEVEN encodings became exact
 *     *head = next duplicated into both arms (pin-free body)      2  first=14
 *     head pinned to r1                                           0
 *   The "7 but first=7" row is the whole finding. One-at-a-time it looks like
 *   another inert 7; read at the INSTRUCTION it says the early window closed,
 *   and that is what pointed at the duplication.
 *
 * REMAINING RESIDUE OF THE PIN-FREE BODY (2 of 23) -- a pure sched2 swap:
 *     rom    mov r3, #0  /  str r2, [r1]  /  str r3, [r0]
 *     ours   str r2, [r1]  /  mov r3, #0  /  str r3, [r0]
 *   Two adjacent independent insns. At the top of that block
 *   last_scheduled_insn is 0, so the CLASS rung is skipped (haifa-sched.c:5963)
 *   and the order is priority -> dependent count -> INSN_LUID. The zero's `mov`
 *   feeds one store (path 2); `str r2,[r1]` feeds nothing (path 1) -- so the mov
 *   SHOULD win on priority, and does in the ROM. It loses here, which means the
 *   two stores carry an output dependence that lengthens the str's path to 2 and
 *   the tie falls to LUID, which favours the earlier source statement.
 *   THE ALIAS LEVER DOES NOT BREAK IT -- measured, all exactly inert at 2:
 *     head store through `unsigned int *`                         2
 *     node->next store through `unsigned int *`                   2
 *     union-typed head slot with an unsigned-int store            2
 *   and the LUID attempts are all regressions:
 *     zero materialised into a named local before the if         23 (+4 bytes)
 *     node->next = 0 duplicated into the arms instead             7 first=7
 *     both stores duplicated, zero first                          7 first=7
 *     both stores duplicated, next first                         17 (+4 bytes)
 *   NOT CROSSED AND WORTH DOING NEXT: the three inert alias spellings against
 *   the two "7 first=7" duplications -- the alias edit is a candidate half-fix
 *   sitting in the inert list, which is the exact shape this pass keeps finding.
 *
 * WHICH BODY TO SHIP -- THE OWNER'S CALL, NOT MINE.
 * The pinned body is byte-identical but needs a fakematch.txt row, so it is not
 * a true match. The pin-free body is 2 away and names a fully-diagnosed
 * single-mechanism residue. If the tree's convention is that a fakematch row is
 * worse than an honest park, install neither and update the park with the
 * pin-free body plus the diagnosis above.
 */
struct Node { struct Node *next; };

struct Pool {
    unsigned char pad_000[0xd98];
    struct Node *head;
    struct Node **tail;
};

extern struct Pool *iwram_3001e8c;

/* Pops the head off the free list of display nodes, or returns NULL when it is
 * exhausted. Popping the last node moves the tail cache at +0xD9C back to the
 * head slot at +0xD98. The popped node's link word is cleared.
 */
struct Node *Func_8015e8c(void)
{
    struct Pool *q = iwram_3001e8c;
    register struct Node **head __asm__("r1") = &q->head;
    struct Node *node = *head;

    if (node != 0) {
        struct Node *next = node->next;

        if (next == 0)
            q->tail = head;
        *head = next;
        node->next = 0;
    }
    return node;
}
