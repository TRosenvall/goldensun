/* Func_80c0130 (0x080c0130) -- NON-MATCHING.
 *
 * NON-MATCHING, 6 of 37 encodings  (MEASURED, batch 323; unchanged figure,
 * corrected diagnosis).  The BODY IS UNCHANGED from the installed park -- it
 * is still the best of the 32 spellings now measured.  What this revision
 * carries is the mechanism, which the park had attributed to the wrong pass,
 * and a BOUND with its evidence attached.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c0130.c \
 *     asm/rom_b5000/rom_bffb8_a_a_a_c.s --func Func_80c0130
 *
 * PINS: 0.  `tools/shimcount.py` -- the authority for this field -- reports no
 * pins in this candidate.  The four `register ... __asm__` declarations this
 * body depends on live in include/dma.h, which is that header's house pattern
 * for all 32 of its helpers (`stmia r3!, {r0, r1, r2}` needs those exact
 * registers) and is shared with the 82 landed DMA users; they are not counted
 * against a candidate.  Noted here only so a future pass does not mistake them
 * for an unbooked fakematch.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE SIX PLACES ARE: four instructions and TWO POOL WORDS
 *
 *   idx  ROM                             ours
 *    16  strh r3, [r1, #0]               adds r0, #0x22
 *    17  ldr  r3, =0x040000b0            strh r3, [r1, #0]
 *    18  ldr  r2, =0xa2600001            ldr  r2, =0xa2600001
 *    19  adds r0, #0x22                  ldr  r3, =0x040000b0
 *    ...
 *    34  .word 0x040000b0                .word 0xa2600001
 *    35  .word 0xa2600001                .word 0x040000b0
 *
 * THE TWO POOL WORDS ARE NOT A SECOND DEFECT.  `<base>.c.26.mach` prints every
 * pool fix (`push_minipool_fix`, arm.c:5380):
 *
 *   ;; SImode fixup for i42; addr 34, range (0,1020): 0x400000c
 *   ;; SImode fixup for i59; addr 40, range (0,1020): 0xa2600001
 *   ;; SImode fixup for i56; addr 42, range (0,1020): 0x40000b0
 *
 * Both constants are SImode, so both take *thumb_movsi_insn's `mi` load
 * alternative at pool_range 1020, and the fixes are emitted in ascending
 * `addr + range` order (arm.c:4820) -- i.e. in the order of the referencing
 * `ldr`s.  Fix the insn order and the pool follows.  THERE IS NO SEPARATE POOL
 * LEVER HERE.  (This is also why the park reported 3 and objcmp reports 6: the
 * park screened with tryc.py, whose `=value` normalisation hides the rotation.)
 *
 * So the whole residue is one basic block's insn order:
 * ROM `strh / ldr / ldr / add` against ours `add / strh / ldr / ldr`.
 *
 * ---------------------------------------------------------------------------
 * THE MECHANISM: sched2's CLASS rung, not LUID and not an earlier pass
 *
 * From `.23.sched2` (`-da -fsched-verbose=6`), all four insns at issue carry
 * `prio 3`, so rank_for_schedule (haifa-sched.c:4029) falls past the priority
 * rung.  The reg-weight rung is `!reload_completed`-gated and sched2 runs after
 * reload; the interblock rungs need INSN_BB to differ and it does not.  That
 * leaves THE CLASS RUNG: an insn data-dependent on `last_scheduled_insn` with
 * cost != 1 is class 1, an independent one is class 3, higher class wins.
 *
 * At the deciding cycle `last_scheduled_insn` is insn 42, `ldr r1, =0x400000c`,
 * and IT HAS TO BE: it carries prio 5 against the others' 3, so the priority
 * rung puts it there for every source spelling.  The ROM's next insn is the
 * `strh`, which is data-dependent on insn 42 FOR ITS ADDRESS REGISTER at cost
 * 2 -- class 1 -- while the `add` and the `_cnt` load are independent of it,
 * class 3.  arm_adjust_cost (arm.c:2416) does not rescue it: it zeroes
 * anti/output deps and discounts a LOAD after a STORE, and this is a STORE
 * after a LOAD, so `single_set`'s SET_SRC is not a MEM and the cost passes
 * through unchanged.
 *
 * The class rung is evaluated BEFORE depend_count and BEFORE INSN_LUID.  So no
 * amount of statement or declaration reordering can get the `strh` scheduled
 * there, and the measured sweep below is the evidence, not the inference.
 *
 * ---------------------------------------------------------------------------
 * MEASURED (ref 37 encodings, tools/sweep_variants.py -> tools/objcmp.py)
 *
 *   the installed body (baseline)                          6, first 16
 *   named dest pointer `vu16 *d`, store through it          6, first 16
 *   `*d = *s++` -- the idiom BOTH landed DMA0_SET siblings
 *     use (src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_b.c,
 *     src/rom_8a000/rom_97384_c_a_c_b.c)                    6, first 16
 *   `q = p + 0x22;` named BEFORE the store                  6, first 16
 *   `q = p + 0x22;` named AFTER the store                   6, first 16
 *   explicit `(void *)` casts on both DMA sources           6, first 16
 *   ALL 24 PERMUTATIONS of the four pinned operand
 *     declarations in a file-local DMA0_SET                 6, first 16  (24/24)
 *   -fno-schedule-insns2  (flag figure, not shippable)      6, first 14
 *
 * THE 24-ROW SWEEP IS NOT A FLAT ROW WITH BIT-IDENTICAL INPUTS -- the edit
 * demonstrably reached the scheduler.  Base numbers the three operand insns 56
 * (`_base`), 57 (`add` = `_src`), 59 (`_cnt`); the `_src`-last order numbers
 * them 56, 58 (`_cnt`), 59 (`add`), and that variant's SCHEDULE CHANGES -- it
 * emits `ldr r1 / ldr cnt / strh / add / ldr base`.  Still 6, composed
 * differently.  The LUID rung fires and does not reach the ROM.
 *
 * ---------------------------------------------------------------------------
 * TWO CORRECTIONS TO THE PARK
 *
 *  1. The park reasoned that "-fno-schedule-insns2 doubling the count is the
 *     recorded 'destroying the evidence' signature, which per batch 173 RULES
 *     OUT THE SCHEDULER PASS rather than merely that flag -- so this is insn
 *     placement decided earlier."  Measured at object level the flag doubles
 *     nothing: it reads 6 of 37, first at index 14 -- the SAME figure, one
 *     index earlier.  sched2 IS the pass.  The park reached the right
 *     conclusion ("no spelling reaches it") from the wrong premise, and that
 *     premise would have sent the next reader to cse/combine/loop.
 *  2. The park's figure line said 3 differing (a tryc.py figure).  It is 6.
 *
 * WHAT IS RIGHT, and still worth reading before touching this again -- three
 * things gcc reproduces unprompted that look like they would need levers:
 *
 *   1. TWO ADJACENT GLOBALS FROM ONE POOL ENTRY, AT A NEGATIVE OFFSET.  The
 *      ROM reaches iwram_3001e78 as `mov r3, r2 / sub r3, #0x88` off
 *      iwram_3001f00's pool address.  `extern unsigned char iwram_3001f00[];`
 *      with `*(unsigned char **)(iwram_3001f00 - 0x88)` gives exactly that --
 *      the batch-174 `ldmia` rule (adjacent globals the ROM reaches from one
 *      pool entry are one array), and it works backwards too.
 *   2. THE DMA3 BASE DERIVED FROM THE DMA0 BASE.  The ROM's second transfer
 *      uses `add r3, #0x24` off &REG_DMA0SAD rather than a fresh &REG_DMA3SAD
 *      pool load.  DMA0_SET followed by DMA3_SET produces it: gcc's constant
 *      CSE finds 0x40000d4 = 0x40000b0 + 0x24 by itself.
 *   3. THE SECOND DMA'S DESTINATION LIKEWISE.  `add r1, #0x14` off &REG_BG2CNT
 *      comes from plainly writing `(void *)&REG_BG2PA`.
 *
 * NEXT.  Nothing source-level, now with a mechanism behind the claim.  The only
 * routes left are to raise the `strh`'s priority above 3 -- its only dependents
 * are the `stmia` asm and the anti-dep `_base` load, both at priority 3 with
 * cost 0, so the longest path through it is fixed -- or to put a real insn into
 * the stalled cycle between insn 42 and the store, and the instruction count is
 * exact at 37, so there is nothing to put there.
 *

 * ===== BATCH 327 BRIEF H: THE CLASS-RUNG DIAGNOSIS CONFIRMED FROM THE DUMP,
 * ===== AND THE ESCAPE SET ENUMERATED (the park named two of three)
 *
 * Figure re-derived: **6 differing encodings of 37**, ref 37 / ours 37, first at
 * index 16, SIZE / INSTRUCTION COUNT / RELOCATIONS all silent -- no pad is
 * absorbing a length difference.  The reference is 32 Thumb instructions plus 5
 * pool words (=iwram_3001f00, =0x400000c, =0x40000b0, =0xa2600001,
 * =0x84000004).  BODY UNCHANGED.
 *
 * SPLIT NOTE THE PARK LACKS: `tools/upstream_module.py` puts this in upstream
 * module rom_b5000/rom_bffb8.s with **17 landed siblings and 0 parks**, and the
 * reference `asm/rom_b5000/rom_bffb8_a_a_a_c.s` carries **Func_80c00d8 as
 * well** -- so a landing needs a SPLIT, not a whole-file conversion.
 *
 * THE TRACE, verbatim from `.23.sched2` (block 1, `-da -fsched-verbose=6`).
 * Table rows: 42 prio 5 cost 2; 44 prio 5 cost 2; **45 (the `strh`) prio 3
 * cost 2, dependents `60 56`**; 56 prio 3 cost 2, dependents `60`; 57 (`add
 * r0,#0x22`) prio 3 cost 1, dependents `60`; 59 prio 3 cost 2, dependents `60`.
 *     t = 12  ready `57 59 42` -> schedules 42, "insn 45 into queue with cost=2"
 *     t = 13  ready `59 57`    -> BOTH re-queued: a FORCED STALL, core unit busy
 *     t = 14  ready `45 59 57` -> schedules **57**        <-- the decision
 *     t = 15  -> 45 (the strh);  t = 17 -> 59;  t = 18 -> 56
 * Note what this adds: **45 has TWO in-block dependents against 56/57/59's one,
 * so 45 WINS the dependent-count rung** (haifa-sched.c:4097-4108).  It never
 * reaches it because the class rung sits above.
 *
 * THE CLASS RUNG, read verbatim (**haifa-sched.c:4068-4094**): an insn gets
 * class 3 when `link == 0 || insn_cost (last_scheduled_insn, link, tmp) == 1`,
 * class 1 for a data dependence, class 2 for anti/output, higher wins.  At
 * t = 14 `last_scheduled_insn` is 42, and 45 is data-dependent on it at cost 2.
 *
 * **THREE escapes exist; the park named the first two:**
 *   1. `link == 0`.  Impossible -- the reference is itself
 *      `ldr r1, =REG_BG2CNT / strh r3, [r1]`, and r1 is also the DMA0
 *      destination operand, which is exactly why insn 42 carries prio 5.
 *   2. `insn_cost == 1`.  Needs insn 42 not to be a `load`: the `core` unit
 *      gives a load ready-delay 2 (**arm.md:253-264**) and `arm_adjust_cost`
 *      (**arm.c:2415-2451**, read in full) returns 0 only for anti/output, 1 for
 *      a CALL or a **load-after-store** from a cached address, and otherwise
 *      passes `cost` through -- a STORE after a LOAD gets no discount and no
 *      branch of it ever raises a cost.
 *   3. **NEW: make insn 42 not be `last_scheduled_insn` at that moment.**  Any
 *      insn issued in the t = 13 stall would leave 45 INDEPENDENT of it, class
 *      3, tied with 57/59, and 45 would then win on dependent count 2 against 1.
 *      Measured dead end: t = 13 is a FORCED stall because the only non-`core`
 *      insns in the block are the `stmia` asm (insn 60, units "none") and insn
 *      78, both of which must follow.  Unreachable from C -- but it is the only
 *      rung-level route left and belongs in the record rather than omitted.
 *
 * NEXT: unchanged -- nothing source-level.  The bound is now the escape set
 * above; disagree with those three lines, not with another spelling.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char iwram_3001f00[];

void Func_80c0130(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int n;

    a = *(unsigned char **)iwram_3001f00;
    if (*(int *)(a + 8) != 2)
        return;
    b = *(unsigned char **)(iwram_3001f00 - 0x88);
    n = *(int *)b;
    p = b + ((n * 5) << 6);
    REG_BG2CNT = *(unsigned short *)(p + 0x20);
    DMA0_SET(p + 0x22, (void *)&REG_BG2CNT, 0xa2600001);
    DMA3_SET(b + 0x10, (void *)&REG_BG2PA, 0x84000004);
}
