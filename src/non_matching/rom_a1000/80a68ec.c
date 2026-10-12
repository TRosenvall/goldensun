/* Func_80a68ec -- FilterItemList -- NON-MATCHING, 23 encodings of 133 differ
 * (objcmp --whole: ref 133 / ours 133, SAME SIZE, SAME INSTRUCTION COUNT,
 *  FIRST DIFFERING INDEX 20, RELOCATIONS IDENTICAL.  Re-measured batch 342
 *  brief D; batch 335's figure survived unchanged.)   (PARK -- HOLD)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a68ec.c \
 *     asm/rom_a1000/rom_a5534_c_c_c_a_a_c.s --func Func_80a68ec
 *
 * The .s holds only this function (tools/piece.py), so no split is needed.
 * ZERO pins (tools/shimcount.py).
 *
 * *** DO NOT TRUST `tryc.py --full` ON THIS FUNCTION. *** It reports
 * "rom 138 lines, ours 140, first diff at 20, 104 differ", because OUR stream
 * carries one EXTRA LABEL at the head of the inner loop and tryc's diff is LINE
 * based, so every later line is shifted.  The figure is 23.  Use the
 * index-aligned side-by-side (batch 342 brief D wrote one at
 * scratch_elev/b342/D/d_sxs.py; it imports tools/objcmp.py rather than forking
 * it and aligns on the encoding index objcmp counts).
 *
 * ===== BATCH 335: THE LENGTH GAP IS CLOSED.  READ THIS FIRST. =====
 *
 * The old figure of forty-seven was measured against a body ONE INSTRUCTION LONG
 * (127 against the reference's 126), so it was counting phase, not distance.
 * The extra instruction was the ZERO LOOP'S SURVIVING COUNTER, and what removes
 * it is the DIRECTION THE LOOP IS WRITTEN IN, not its form:
 *
 *     WRITE IT UP-COUNTING.  `for (k = 0; k < 32; k++) dst[k] = 0;`
 *
 * loop.c's check_dbra_loop REVERSES an up-counting loop whose index is not used
 * after it, and the reversal is what performs the biv elimination: the counter
 * disappears, the walking pointer is compared against a SAVED COPY OF dst in a
 * scratch high register, and that copy -- the one the old park recorded as
 * missing from its pointer recipe -- is MANUFACTURED BY THE REVERSAL ITSELF.
 * Written down-counting in source there is nothing to reverse, biv elimination
 * is refused on the "not worth while" benefit test, and the counter survives.
 *
 * GENERAL RULE WORTH MORE THAN THIS FUNCTION.  When a reference loop counts DOWN
 * and the candidate is ONE LONG as a source down-counter and ONE SHORT as a
 * source pointer walk, write it UP-COUNTING and let loop.c reverse it.  Five
 * pointer spellings were measured here and ALL FIVE are byte-identical at 125
 * instructions and 113 differing: they are one point, not five.
 *
 * AND THE ZERO THIS LOOP STORES MUST STAY A BARE LITERAL.  The reference loads
 * it from the pool, which is the HImode pooling rule read in the OTHER
 * direction: a bare literal through a halfword lvalue is an HImode constant and
 * always pools.  Do NOT give it an int carrier.  OvlFunc_886_2008368 is the same
 * mechanism used the opposite way round.
 *
 * ================= THE REGISTER TABLE, CORRECTED (batch 342) =================
 *
 * The old BLOCKER 2 read "ROM pass r7 / p r6 / d r5 / rec r9 / 0x40 r11; ours
 * pass r6 / p r5 / d r7 / rec r9 / 0x40 r10".  THAT IS STALE -- it predates
 * batch 335's zero loop.  Measured now:
 *
 *     role                             ROM        ours
 *     pass                             r7         r6
 *     p   (walking entry pointer)      r6         r5
 *     d   (destination cursor)         r5         r7
 *     rec                              r9         r11 (fp)
 *     the 0x40 / 0x3fff constant       r11 (fp)   r9
 *
 * So `rec` is NOT r9 on our side any more, and the high-register residue is a
 * clean r9 <-> r11 SWAP between `rec` and the hoisted constant.
 *
 * THE 23, ATTRIBUTED INDEX BY INDEX:
 *   * r9 <-> r11 swap, SIX encodings: 20 (mov rec,r0), 38 and 53
 *     (mov r3,rec / mov r2,rec in the filter==1 loop), 67 (mov const,r3),
 *     91 and 107 (mov r3,const).
 *   * the 3-cycle on the low callee-saved registers, FIFTEEN encodings.  ROM
 *     (pass,p,d) = (r7,r6,r5); ours = (r6,r5,r7) -- the cycle (r7 r6 r5).
 *     pass: 63 64 85 97 99 101 119 120.  p: 70 74 111 116.  d: 71 113 114.
 *   * ONE operand order, index 71: ROM `add r5, r3, r2` against ours
 *     `add r7, r2, r3` -- the cursor's `d = &dst[n]`.
 *   * ONE transposition, indices 72/73: ROM `mov r1,#31` then `#0x58`; ours
 *     `#0x58` then `mov r1,#31`.  The ROM initialises the reversed inner
 *     counter BEFORE the list offset.
 * So "ONE ROTATION rather than N ties" is right; it is TWO rotations (one high,
 * one low) plus two single-instruction items.
 *
 * ================ THE PASS, WITH THE NUMBERS THAT BOUND IT =================
 *
 * `global.c`'s allocno_compare (lines 597-619 of this tree's copy):
 *     pri = (floor_log2 (allocno[v].n_refs) * allocno[v].n_refs
 *            / allocno[v].live_length) * 10000 * allocno[v].size
 *
 * From `.18.greg` on this body the global allocation order is
 *   ;; 19 regs to allocate: 78 41 132 49 123 39 40 135 129 128 36 38 45 35 33 34 97 32 56
 * and that line prints the PSEUDO of each allocno in allocno_order, skipping any
 * already renumbered by local-alloc (global.c:1875-1885).  Identified from
 * `.17.lreg`:
 *   * PSEUDO 40 = `d`, a reg/v, the address of the pass loop's
 *     `(set (mem:HI (reg/v:SI 40)) ...)` at insn 308 -- FOUR references,
 *     allocated SEVENTH.
 *   * PSEUDO 123 = `p`, compiler-generated (the reversed inner loop's biv) --
 *     SIX references, allocated FIFTH.
 * So floor_log2(4)*4 = 8 for `d` against floor_log2(6)*6 = 12 for `p`, and the
 * two live ranges both span the inner loop.  **FOR `d` TO BE ALLOCATED BEFORE
 * `p` -- WHICH IS WHAT THE ROM'S r5 REQUIRES -- `d` NEEDS EITHER MORE
 * REFERENCES OR A LIVE_LENGTH UNDER TWO THIRDS OF `p`'s.**  That is the bound,
 * with its arithmetic.
 *
 * AND THE OBVIOUS WAY TO GET MORE REFERENCES DOES NOT WORK.  Seven spellings of
 * the cursor store crossed against three spellings of `d`'s definition (21
 * variants, one container), all chosen to raise `d`'s n_refs from 4 to 5 while
 * emitting the identical `strh r3,[d,#0] / add d,#2`:
 *     *d++ = x; n++;        *d = x; d++; n++;       *d = x; d = d+1; n = n+1;
 *     d[0] = x; d++; n++;   *d = x; n++; d++;       n++; *d = x; d++;
 *     *d = x; d += 1; n += 1;
 *   x  `d = &dst[n]` / `d = dst + n` / `d` declared inside the pass loop
 * **ALL 21 ARE 23, BYTE FOR BYTE.**  And the reason is citable: dumping the
 * split form and diffing `.18.greg` shows the allocation order is the SAME
 * SEQUENCE with pseudos renumbered by one (132->131, 123->122, 135->134,
 * 129->128, 128->127), so n_refs and live_length are unchanged and
 * allocno_compare produces the identical order.  **THE SPELLING OF A
 * POST-INCREMENT STORE IS NOT A HANDLE ON n_refs** -- the tree optimiser
 * normalises all seven to one RTL shape.
 *
 * ========================= AXES CLOSED, DO NOT RE-SCREEN ====================
 *
 * FLAG AXIS (tools/autosweep.py, batch 342): -fno-gcse 23,
 * -fno-strict-aliasing 23 (inert); -fno-schedule-insns2 47,
 * -fno-rerun-loop-opt 83, -fno-rerun-cse-after-loop 103, -ffixed-r7 121,
 * -O1 125, -fno-strength-reduce 125.
 *
 * ANCHOR AXIS: FULLY SWEPT BY HAND, batch 342.  All 45 statement boundaries
 * enumerated; the 7 that would make the anchor the body of a braceless
 * `for`/`if` were dropped as semantics changes; the remaining 38 measured:
 *     after the last declaration             35 (first 8)
 *     after `passes = (filter != 2) + 3;`    27
 *     inside the filter==1 guard             80 (RELOCDIFF)
 *     after the filter==1 _GetMoveInfo      103 (dsize -4, RELOCDIFF)
 *     after the filter==1 loop               30
 *     after the second `goto add;`          105 (RELOCDIFF)
 *     after `*d++ = rec->list[i].id;`        24
 *     every other placement (24 of them)     23, byte-identical
 * autosweep's warning "8 of 25 variant(s) produced NO ROW ... UNTESTED" is a
 * FALSE ALARM on this park: those 8 place a statement before the end of the
 * declaration block, which is invalid C89.  They are UNTESTABLE, not untested.
 *
 * MEASURED INERT, EARLIER BATCHES, DO NOT RE-RUN: the declaration order of the
 * locals in three orders (byte-identical); naming the mask constant in a
 * pass-loop-scoped int (24, one WORSE).
 *
 * WHAT IS RIGHT, and worth reusing:
 *  - The pass test is SEQUENTIAL ifs, not a switch: the ROM re-tests pass==1..3
 *    after the pass-0 test fails.  A switch gives a balanced compare tree
 *    (100 -> 47 with the next item).
 *  - The grouping loop is an UP-counting `for (i = 0; i < 32; i++)` over
 *    rec->list[i]: loop.c reverses it into the ROM's `mov r1,#31 ... sub/cmp
 *    #0/bge` counter plus a walking pointer, with the pointer init inside the
 *    pass loop.
 *  - The zero loop needs its OWN counter (k): sharing `i` keeps the biv alive
 *    past the loop (REGNO_LAST_UID) and it can never be eliminated.
 *  - Blocker-1 history (now closed): in the -da .08.loop dump the k*2 giv
 *    (insn 37, "mult 2 add 0") was "not worth while, 0 vs 6", all_reduced was
 *    false and biv elimination was refused; -freduce-all-givs eliminated it with
 *    extra copies, which confirmed the mechanism before the up-counting form
 *    made it unnecessary.  Tried inert/worse then: k-=2 byte offsets,
 *    (short *), `*(dst+k)`, `k > -1`, do/while counter, short/char k,
 *    dst[31-k] (reverses direction), `*d-- = 0`.
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
