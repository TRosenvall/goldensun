/* Func_80a68ec -- FilterItemList -- NON-MATCHING, 23 encodings of 133 differ
 * (objcmp: ref 133 / ours 133, same size AND SAME INSTRUCTION COUNT, 126 each).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a68ec.c \
 *     asm/rom_a1000/rom_a5534_c_c_c_a_a_c.s --func Func_80a68ec
 *
 * ===== BATCH 335, BRIEF H: THE LENGTH GAP IS CLOSED.  READ THIS FIRST. =====
 *
 * The park's figure of forty-seven was measured against a body ONE INSTRUCTION
 * LONG (one hundred and twenty-seven against the reference's one hundred and
 * twenty-six), so it was counting phase, not distance.  The extra instruction
 * was the ZERO LOOP'S SURVIVING COUNTER, and what removes it is the DIRECTION
 * THE LOOP IS WRITTEN IN, not its form:
 *
 *     WRITE IT UP-COUNTING.  `for (k = 0; k < 32; k++) dst[k] = 0;`
 *
 * loop.c's check_dbra_loop REVERSES an up-counting loop whose index is not used
 * after it, and the reversal is what performs the biv elimination: the counter
 * disappears, the walking pointer is compared against a SAVED COPY OF dst in a
 * scratch high register, and that copy -- the one the park records as missing
 * from its pointer recipe, and attributes to dst being a parameter pseudo -- is
 * MANUFACTURED BY THE REVERSAL ITSELF.  Written down-counting in source there is
 * nothing for check_dbra_loop to reverse, biv elimination is refused on the
 * "not worth while" benefit test the park documents, and the counter survives.
 *
 * So BLOCKER 1 IS CLOSED and its diagnosis was inverted: the obstacle was never
 * the benefit test, it was that loop.c had already been handed the loop in the
 * direction it wanted to produce.
 *
 * GENERAL RULE WORTH MORE THAN THIS FUNCTION.  When a reference loop counts DOWN
 * and the candidate is ONE LONG as a source down-counter and ONE SHORT as a
 * source pointer walk, write it UP-COUNTING and let loop.c reverse it.  Three
 * pointer spellings were measured here -- the park's own recipe, a named lower
 * bound, an int-cast bound, a bound derived non-trivially, and a bound written
 * as the address of the zeroth element -- and ALL FIVE are byte-identical at one
 * hundred and twenty-five instructions and one hundred and thirteen differing.
 * They are one point, not five.
 *
 * AND THE ZERO THIS LOOP STORES MUST STAY A BARE LITERAL.  The reference loads
 * it from the pool, which is the HImode pooling rule read in the OTHER
 * direction: a bare literal through a halfword lvalue is an HImode constant and
 * always pools, and here that is what the original did.  Do NOT give it an int
 * carrier.  The matched OvlFunc_886_2008368 in this same batch is the same
 * mechanism used the opposite way round, and the two together are the clearest
 * statement of it in the tree.
 *
 * WHAT IS LEFT, all of it allocation, and it is ONE ROTATION rather than N ties:
 *  - one HIGH-REGISTER SWAP -- the record pointer and the hoisted mask constant
 *    hold each other's registers;
 *  - one CYCLIC LOW-REGISTER ROTATION across the pass counter, the walking entry
 *    pointer and the destination cursor;
 *  - one operand order in the cursor's add, and one position for the add of the
 *    list offset, which the reference places after the inner counter's init.
 * MEASURED INERT THIS BATCH, DO NOT RE-RUN: the declaration order of the locals
 * in three orders, all byte-identical; naming the mask constant in a
 * pass-loop-scoped int, which is one encoding WORSE.
 *
 * FRESH TARGET (batch 287).  The .s holds only this function (no split needed).
 *
 * WHAT IS RIGHT: both filter loops' control flow and every compare.  Levers measured:
 *  - The pass test is SEQUENTIAL ifs, not a switch: `if (pass == 0) { if (kind ||
 *    flags & 0x40) goto add; }  if (pass == 1) continue;  if (pass == 2) continue;
 *    if (pass == 3 && ...) goto add;`  -- the ROM re-tests pass==1..3 after the pass-0
 *    test fails.  A switch gives a balanced compare tree (100 -> 47 with the next item).
 *  - The grouping loop is an UP-counting `for (i = 0; i < 32; i++)` over
 *    rec->list[i]: loop.c reverses it into the ROM's `mov r1,#31 ... sub/cmp #0/bge`
 *    counter plus a walking pointer, with the pointer init inside the pass loop.
 *  - The zero loop needs its OWN counter (k): sharing `i` keeps the biv alive past the
 *    loop (REGNO_LAST_UID) and it can never be eliminated.
 *
 * BLOCKER 1 -- THE ZERO LOOP.  ROM: `add r3,#0x3e / mov r12,r10 / strh / sub r3,#2 /
 * cmp r3,r12 / bge` -- the counter eliminated, compared as a SIGNED pointer against a
 * copy of dst in r12.  Ours keeps the counter: in the -da .08.loop dump the k*2 giv
 * (insn 37, "mult 2 add 0") is "not worth while, 0 vs 6" (benefit 2 - add_cost 2), so
 * all_reduced is false and biv elimination is refused.  -freduce-all-givs DOES eliminate
 * it (with extra copies), which confirms the mechanism.  The batch-286 recipe from
 * Func_80a7f44 -- `z = &dst[31]; do { *z = 0; z--; } while ((int)z >= (int)dst);` --
 * gives the ROM's loop body exactly but WITHOUT the `mov r12, r10` copy (dst is a
 * parameter pseudo here, not a stack address), so it is one instruction short
 * (113 differing).  Tried inert/worse: k-=2 byte offsets, (short *), `*(dst+k)`,
 * `k > -1`, do/while counter, short/char k, dst[31-k] (reverses direction), `*d-- = 0`.
 *
 * BLOCKER 2 -- register roles in the grouping loop: ROM pass r7 / p r6 / d r5 /
 * rec r9 / 0x40 r11; ours pass r6 / p r5 / d r7 / rec r9 / 0x40 r10.  Likely moves
 * with blocker 1 (the zero loop's pseudo count), so not chased separately.
 */
struct MoveInfo {
    unsigned char pad_00;
    unsigned char flags;
    unsigned char pad_02[0x0a];
    unsigned char kind;
};

struct ListEntry {
    unsigned short id;
    unsigned short pad;
};

struct CharRecord {
    unsigned char pad_00[0x58];
    struct ListEntry list[32];
};

extern struct MoveInfo *_GetMoveInfo(int id);

int Func_80a68ec(struct CharRecord *rec, unsigned short *dst, int filter)
{
    int passes;
    int i;
    int k;
    int n;
    int pass;
    unsigned short *d;
    struct MoveInfo *info;

    passes = (filter != 2) + 3;
    for (k = 0; k < 32; k++)
        dst[k] = 0;
    n = 0;
    if (filter == 1) {
        for (i = 0; i < 32; i++) {
            if (rec->list[i].id != 0) {
                info = _GetMoveInfo(rec->list[i].id & 0x3fff);
                if (info->kind != 0)
                    dst[n++] = rec->list[i].id;
            }
        }
    } else {
        for (pass = 0; pass < passes; pass++) {
            d = &dst[n];
            for (i = 0; i < 32; i++) {
                if (rec->list[i].id == 0)
                    continue;
                info = _GetMoveInfo(rec->list[i].id & 0x3fff);
                if (pass == 0) {
                    if (info->kind != 0 || (info->flags & 0x40))
                        goto add;
                }
                if (pass == 1)
                    continue;
                if (pass == 2)
                    continue;
                if (pass == 3 && info->kind == 0 && !(info->flags & 0x40))
                    goto add;
                continue;
            add:
                *d++ = rec->list[i].id;
                n++;
            }
        }
    }
    return n;
}
