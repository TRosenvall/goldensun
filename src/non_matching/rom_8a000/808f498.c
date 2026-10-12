/* Func_808f498 -- 0x0808f498 -- asm/rom_8a000/rom_8d9a4_c_c_a_a_c_a.s
 *
 * NON-MATCHING, **6 of 64** stream encodings  (MEASURED batch 342 brief C on
 *   the body below -- the park it replaces read 25 of 64).
 *   16-BIT INSTRUCTION COUNTS ref 53 / ours 53, IN PHASE on both bodies, so
 *   these figures ARE distances.
 *   SIZE ref 148 bytes, ours 148.  POOL WORDS ref 10, ours 10.
 *   RELOCATIONS IDENTICAL -- and that is NEW: the parked body read RELOCDIFF.
 *   PINS 0 (tools/shimcount.py reports no rows).  DEVICES 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/808f498.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_a_a_c_a.s --func Func_808f498
 *
 * *** TWO FIGURES IN THE OLD HEADER WERE WRONG AND BOTH MISLED. ***
 *  1. "RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *     this project treats as a CONSEQUENCE of the length difference."  THERE
 *     WAS NO LENGTH DIFFERENCE.  On the parked body objcmp reported ref 64 /
 *     ours 64 encodings, 53 / 53 instructions and EQUAL SIZE, with
 *     `iwram_3001ecc` at 0x64 against 0x68.  An equal-length stream whose
 *     relocation moved four bytes is a POOL-CONTENT difference, not a
 *     consequence of anything -- and that is exactly what it was: the parked
 *     body pooled a 0xa0 halfword the ROM does not pool.  The 322
 *     re-classification was applied here without the precondition it needs.
 *  2. "BLOCKER: gcc CSEs the IO register addresses ... 12 of 54, LENGTH
 *     EXACT."  `12 of 54` is a **tryc.py line count**, not an objcmp stream
 *     figure, sitting in the same header as `25 of 64`.  The two are not
 *     comparable and the file read as if it had regressed.
 *
 * PIECE: `asm/rom_8a000/rom_8d9a4_c_c_a_a_c_a.s` holds TWO functions
 * (tools/piece.py); the other is Task_ScreenWindowTransition, also parked
 * (src/non_matching/rom_8a000/Task_ScreenWindowTransition.c, no claim).  So a
 * landing here ships on a cut -- but it is NOT a landing yet.
 *
 * ===========================================================================
 * WHAT MOVED IT 25 -> 6, AND THE PASS IS **reload_cse_move2add**, NOT cse
 * ===========================================================================
 *
 * THE OLD DIAGNOSIS NAMED THE WRONG PASS AND THEREFORE THE WRONG LEVER.  It
 * said "gcc CSEs the IO register addresses; the ROM loads each fresh ... the
 * repeated-expensive-constant class docs/elevation.md already records as not
 * yielding to spelling."  The rewrite is NOT done by any cse pass.  It is
 * `reload_cse_move2add`, which runs POST-RELOAD out of `reload_cse_regs`
 * (reload1.c:7986-7994) and whose transform is spelled out in its own comment
 * at **reload1.c:8885-8893**:
 *
 *     Try to transform (set (REGX) (CONST_INT A))
 *                      ...
 *                      (set (REGX) (CONST_INT B))
 *     to               (set (REGX) (CONST_INT A))
 *                      ...
 *                      (set (REGX) (plus (REGX) (CONST_INT B-A)))
 *
 * gated at reload1.c:8905-8907 on
 *     `rtx_cost (new_src, PLUS) < rtx_cost (src, SET) && have_add2_insn (...)`
 * -- in Thumb a small `add`/`sub` immediate always beats a pool load, so the
 * gate is effectively always true.  *** THE DECISIVE FACT IS THE INDEX: it is
 * `reg_offset[regno]`, keyed on the HARD REGISTER, and the pass only fires when
 * the two constants are assigned THE SAME HARD REGISTER. ***  (The second
 * precondition, reload1.c:8873 `reg_set_luid[regno] > last_label_luid`, means a
 * LABEL between the two sets also kills it -- but no C-level label exists
 * anywhere in this function, and the ROM's `b .L8f51c` is a MINIPOOL branch
 * inserted at pass 26, long after this pass, so that lever is unreachable here.)
 *
 * So this was never a constant-spelling problem.  It is an ALLOCATION problem:
 *   parked body   r1 carries 0x4000048 -> 0x400004a -> `sub r1,#10` = 0x4000040
 *                 r4 carries 0x4000000 -> `add r4,#66` = 0x4000042
 *                 and 0x4000044 is pooled instead
 *   ROM           0x4000048 and 0x400004a in r2; 0x4000040 POOLED into r1;
 *                 0x4000042 POOLED into r2; 0x44 and 0x46 derived from it
 * Three of our four derivations are move2add firing on a register the ROM kept
 * a different value in.
 *
 * TWO EDITS DID IT, AND THE SECOND IS THE BIG ONE:
 *
 *   1. `a0 = 0xa0;` IN A LOCAL, ASSIGNED IMMEDIATELY BEFORE THE TWO STORES
 *      THAT USE IT.                                            25 -> 22
 *      `REG_WIN0V = 0xa0` stores an HImode constant, which Thumb cannot
 *      materialise, so it becomes a POOLED HALFWORD (`ldrh r4, .L21+4`) and
 *      that pool word displaces every later one -- which is the whole of the
 *      relocation difference.  Routing it through an `int` makes it SImode and
 *      gcc emits the ROM's `mov r4, #0xa0`.
 *      *** PLACEMENT IS THE WHOLE LEVER, and the park's own note that this is
 *      "BYTE-IDENTICAL at 12" was measuring the wrong placement. ***  Measured:
 *          `int a0;` assigned at the TOP of the function   67, +4 bytes
 *          `u32 a0;` at the top                            67, +4 bytes
 *          `unsigned short a0;` at the top                 67, +4 bytes
 *          two locals a0/a1 at the top                     67, +4 bytes
 *          `int a0;` assigned JUST BEFORE THE STORES       *** 22 ***
 *      That is batch 339 brief D's rule exactly -- the only source handle on
 *      a value's live_length is WHERE ITS DEFINITION SITS -- reproduced on a
 *      HImode/SImode operand-mode fix rather than on an allocation race.
 *
 *   2. THREE DISTINCT NAMED ADDRESS POINTERS, ONE PER WORD THE ROM POOLS.
 *                                                            22 -> *** 6 ***
 *      `w1 = &REG_WININ` (0x48, then `w1++` for 0x4a),
 *      `w2 = &REG_WIN0H` (0x40), `w3 = &REG_WIN1H` (0x42, then two `w3++`).
 *      Each named pointer is its own pseudo with its own live range, so the
 *      allocator stops giving two of them one hard register and move2add has
 *      nothing to chain.  The emitted pool becomes the ROM's TEN WORDS IN THE
 *      ROM'S ORDER -- 0x6000, iwram_3001ecc, 0x539, 0x40000b0, 0xc5ff, 0x7fff,
 *      0x4000048, 0x4000040, 0x4000042, 0xa6600001 -- relocations go clean, and
 *      the last three store groups are INSTRUCTION-FOR-INSTRUCTION EXACT.
 *      This is the batch-340 three-distinct-locals result (Func_80a33d4, a
 *      brief-C piece-mate) transferred from repeated CONSTANTS to repeated
 *      ADDRESSES: one name is commoned, three are not.
 *      TWO pointers is enough -- w1 and w3, leaving REG_WIN0H as the macro,
 *      also measures 6 -- so the minimal set is TWO, and the body below ships
 *      three because `w2` is also what the DMA's dest argument wants.
 *
 * ===========================================================================
 * WHAT IS LEFT: SIX ENCODINGS, ONE REGISTER PAIR, PURELY ALLOCATION
 * ===========================================================================
 *     ref   ldrh r1,[r0] / ldr r2,=0x4000048 / strh r1,[r2]
 *           add r0,#2 / ldrh r1,[r0] / add r2,#2 / strh r1,[r2]
 *     ours  ldr r1,=0x4000048 / ldrh r2,[r0] / strh r2,[r1]
 *           add r0,#2 / ldrh r2,[r0] / add r1,#2 / strh r2,[r1]
 * The ROM keeps the FIRST group's address in r2 and the loaded value in r1; we
 * do the opposite.  Groups three and four already match, and there the ROM uses
 * r1 for the address and r2 for the value -- so the ROM ALTERNATES and we do
 * not.  r1 is forced for 0x4000040 by DMA0_SET's `register void *_dst
 * __asm__("r1")`; w1's range ends before that begins, so nothing makes them
 * conflict and the allocator hands w1 r1 as well.
 *
 * *** THE INSTRUMENT CONFIRMS THE CLASS AND LEAVES ONLY ORDER. ***
 *     `register vu16 *w1 __asm__("r2")`              *** 2 of 64 ***, size and
 *                                                    pool exact, relocs clean
 *     the same plus a pin on w3 to r2                2 of 64 (no better)
 *     a pin on w2 to r1 instead                      18, +4 bytes
 * With the pin the two streams differ ONLY in the order of the group's first
 * two instructions -- `ldr r2,pool` before `ldrh r1,[r0]` where the ROM has it
 * the other way.  Both are `load` class (arm.md:260, latency 2), both are
 * leaves feeding the same `strh`, so their priorities tie and
 * rank_for_schedule's last rung is INSN_LUID, lowest first.  `w1 = &REG_WININ;`
 * is the lower LUID because it is written first, and THE ONLY SOURCE HANDLE ON
 * THAT IS WHERE THE DEFINITION SITS -- so the remaining question is a spelling
 * in which the pointer's definition has a HIGHER luid than the value load while
 * still being a named local.  Naming the loaded value is the obvious try and it
 * is measured WORSE (below), because the extra pseudo breaks lever 2.
 *
 * NOT A LANDING: the pin would make it 2, not 0, so there is no pinned landing
 * to decline either.  Pass-3 does not inherit anything here.
 *
 * ===========================================================================
 * MEASURED THIS BATCH -- 41 variants in six sweeps.  All ref 64 / ours 64 with
 * relocations CLEAN unless noted.
 * ===========================================================================
 * AT 6 (the floor, eleven device-free spellings -- so 6 is robust, not lucky):
 *   three pointers, `w1++`                                            6
 *   three pointers, `w1 += 1`                                         6
 *   TWO pointers (w1 and w3), REG_WIN0H left as the macro             6
 *   DMA dest spelled `(void *)w2` instead of `(void *)&REG_WIN0H`     6
 *   w1 declared LAST of the four locals                               6
 *   the names rotated (0x42 trio on w1, 0x48 pair on w3)              6
 *   `int w1` holding the address with a cast at each store            6
 *   `*w1++ = ...` post-increment for the pair                         6
 *   w1 declared inside a NESTED BLOCK (to reach local-alloc)          6
 *   the second store's address as a second local `wb = w1 + 1`        6
 *   `e = e;` ahead of the pointer definition                          6
 * WORSE:
 *   `a0` at the top of the function, four spellings            67, +4 bytes
 *   `a0 = 0xa0` left as the bare literal in both stores        31, +4 bytes
 *   the loaded values named (`v`, or `v0..v3`)             43-49, +4/+12 bytes
 *   only the SECOND loaded value named                         44, +4 bytes
 *   `w1[0]` / `w1[1]` instead of `*w1` and `w1++`              29, -4 bytes
 *   `*(w1 + 1)` inline for the second store                    29, -4 bytes
 *   `unsigned short *w1` (non-volatile) with a cast            29, -4 bytes
 *   w3's two 0xa0 stores through three fresh `&REG_WIN0V` /
 *     `&REG_WIN1V` assignments instead of `w3++`               23, -4 bytes
 *   w2 assigned BEFORE the WININ pair (to force a conflict)    19, -4 bytes
 *   w2 assigned between the two WININ stores                   19, -4 bytes
 *   four pointers, one per address                             18, -4 bytes
 *   the WININ pair left as macros, w2 and w3 named             18, -4 bytes
 *   `void *dst` named for the DMA argument, a0 at the top      23/67
 *   `w1 = &REG_WIN0H + 4` (derived from the other address)      6 (inert)
 *
 * AUTOSWEEP (tools/autosweep.py, run FIRST; raw in scratch_elev/b342/C):
 *   PARKED BODY, BOTH AXES CLOSED, nothing improved.
 *   FLAG AXIS: -fno-gcse 25, -ffixed-r7 25, -fno-strict-aliasing 25,
 *   -fno-strength-reduce 25, -fno-rerun-cse-after-loop 25, -fno-rerun-loop-opt
 *   25 (all inert -- note -ffixed-r7 is INERT here, not worse, unlike the other
 *   three targets in this brief); -fno-schedule-insns2 34, -O1 53 (worse).
 *   ANCHOR AXIS: 17 of 17 boundaries, NOT capped, all 25 or worse.
 *   *** RE-CROSSED ON THE IMPROVED BODY (c_flagcross.py and autosweep
 *   --anchor), because a flag inert on a park need not stay inert: all eight
 *   flag groups still inert at 6 except -fno-schedule-insns2 40 and -O1 53, and
 *   the ANCHOR AXIS re-run at 24 of 24 boundaries is also all 6. ***  The
 *   -fno-sched2 row is the positive control for the paragraph above: the order
 *   we have is sched2's, and turning sched2 off costs 34 encodings.
 *
 * KEPT FROM THE PARK, ALL STILL LOAD-BEARING:
 *   - WALK THE POINTER, do not index it (`e += 2;` between the stores).  Still
 *     worth three instructions; `e[2]`-style indexing is still worse.
 *   - the record arithmetic `i * 644` as written -- gcc builds the ROM's
 *     lsl/add/lsl/add/lsl chain unaided.
 *   - `UnknownDMAPrefix();` with its result DISCARDED.  dma.h:318-325 returns
 *     `dma[5]`, and the discarded volatile read is the ROM's third
 *     `ldrh r2,[r3,#0xa]`, whose value is overwritten two instructions later.
 *   - `DMA0_SET(e, (void *)&REG_WIN0H, 0xa6600001)` from dma.h -- the `stmia`
 *     reuses the r1 that already holds 0x4000040, which is why the DMA dest
 *     must be that same address and not a copy.
 *   - `REG_DISPCNT |= 0x6000`.  Our `ldrh r1, .L21` against the ROM's
 *     `ldr r1, .L8f4f8` is NOT a difference: gas lowers a Thumb PC-relative
 *     `ldrh` to `ldr` (there is no PC-relative `ldrh` in Thumb-1) and the two
 *     assemble to the same `4900`.  Verified by object comparison this batch.
 */
#include "dma.h"

extern int iwram_3001ecc;

void Func_808f498(void)
{
    char *e;
    int i;
    int a0;
    vu16 *w1;
    vu16 *w2;
    vu16 *w3;

    e = (char *)iwram_3001ecc;
    i = *(unsigned char *)(e + 0x539);
    e = e + i * 644;
    UnknownDMAPrefix();
    REG_DISPCNT |= 0x6000;
    w1 = &REG_WININ;
    *w1 = *(unsigned short *)e;
    e += 2;
    w1++;
    *w1 = *(unsigned short *)e;
    e += 2;
    w2 = &REG_WIN0H;
    *w2 = *(unsigned short *)e;
    e += 2;
    w3 = &REG_WIN1H;
    *w3 = *(unsigned short *)e;
    a0 = 0xa0;
    w3++;
    *w3 = a0;
    w3++;
    *w3 = a0;
    e += 2;
    DMA0_SET(e, (void *)&REG_WIN0H, 0xa6600001);
}
