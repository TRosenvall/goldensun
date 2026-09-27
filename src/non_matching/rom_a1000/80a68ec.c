/* Func_80a68ec -- FilterItemList -- NON-MATCHING, 47 encodings of 133 differ
 * (objcmp: ref 133 / ours 133, same size).
 *
 * Verify with:
 *   python3 tools/objcmp.py /tmp/claude-0/-home-user-goldensun/ad08b1ee-c1a0-56c8-b3c2-c0ff6481844c/scratchpad/b287/V/Func_80a68ec.park.c \
 *     asm/rom_a1000/rom_a5534_c_c_c_a_a_c.s --func Func_80a68ec
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
    for (k = 31; k >= 0; k--)
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
