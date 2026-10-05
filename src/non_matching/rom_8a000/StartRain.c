/* StartRain -- 4 differing encodings of 104.  PARKED.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/StartRain.c asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_c.s --func StartRain
 *
 * RE-DERIVED batch 326, brief D: 4 differing encodings of 104.  SIZE 244 bytes
 * against 244.  INSTRUCTION COUNT 103 against 103 (the reference carries one
 * trailing pad word, we carry one too).  RELOCATIONS IDENTICAL.  objcmp prints
 * no SIZE and no INSTRUCTION COUNT line, which is how both of those are known.
 * The backfilled figure from batch 324 is CORRECT.
 *
 * (Batch 324's note on why nothing could measure this park for 300 batches --
 * a star-slash pair inside the header prose -- is retired; it is fixed.  Do not
 * quote a comment terminator in a park header.)
 *
 * THE RESIDUE IS ONE INSTRUCTION DISPLACED BY FOUR SLOTS, and nothing else:
 *
 *   rom   strh r2,[r3] | add r3,#2 | strh r4,[r3] | ldr r0,=Task_Rain | mov r1,#0xc8 | lsl r1,#4 | bl
 *   ours  strh r2,[r3] | mov r1,#0xc8 | add r3,#2 | strh r4,[r3] | ldr r0,=Task_Rain | lsl r1,#4 | bl
 *
 * `mov r1,#0xc8` only.  Insn 282 (`lsl r1,#4`) and the whole epilogue are in the
 * ROM's slots.  One run, one cause.
 *
 * ===================== THE PARK'S VERDICT IS REFUTED =====================
 *
 * The old header said: "sched2 hoists the mov because the dependence dump gives
 * 281 priority 67 against 224's and 221's 66 ... rank_for_schedule never reaches
 * the LUID tie-break, so no statement order can decide it."
 *
 * It MISSED INSN 218.  The `.23.sched2` block-2 region table, measured with
 * `-da -fsched-verbose=6` (below the threshold of 10, so it lands in the file):
 *
 *     insn  code  prio  cost   dependents
 *     215   180    67     2    299 298 231 221 218      strh r2,[r3]
 *     218     5    67     1    299 298 231 221          add  r3,#2
 *     221   180    66     2    299 298 231              strh r4,[r3]
 *     224   173    66     2    299 298 231              ldr  r0,=Task_Rain
 *     281   173    67     1    298 282                  mov  r1,#0xc8
 *     282   112    66     1    299 298 231              lsl  r1,#4
 *     231   239    65    32    299 298 297              bl   StartTask
 *
 * So 218 and 281 TIE at 67 and rank_for_schedule does reach a tie-break -- just
 * not the LUID one.  It stops two tests earlier.
 *
 * WHAT DECIDES IT, read in the compiler:
 *   `schedule_block` (haifa-sched.c:6008) re-sorts the ready list EVERY cycle and
 *   issues `ready[--n_ready]`, the highest-ranked element.  215 issues at t=9 and
 *   occupies the core for 2 cycles, so t=10 is a stall and `last_scheduled_insn`
 *   is STILL 215 when t=11 sorts {224(66), 218(67), 281(67)}.
 *   `rank_for_schedule` (haifa-sched.c:4029):
 *     1 priority -- 218 and 281 tie at 67.
 *     2 INSN_REG_WEIGHT -- SKIPPED, gated `!reload_completed` (:4046).
 *     3 interblock -- same bb, skipped.
 *     4 CLASS AGAINST last_scheduled_insn (:4068-4094) -- THE DECIDER.  218 is in
 *       INSN_DEPEND(215) through an ANTI dependence (215 reads r3, 218 writes it)
 *       and `arm_adjust_cost` (config/arm/arm.c:2425-2427) RETURNS 0 FOR
 *       REG_DEP_ANTI/REG_DEP_OUTPUT, so insn_cost != 1 and 218 gets class 2.  281
 *       is in no dependence with 215, so link == 0 -> class 3.  Class 3 wins.
 *     5 depend_count (unreached) -- 218 has 4 dependents, 281 has 2: 218 would win.
 *     6 INSN_LUID (unreached).
 *
 * So the hoist is decided by an ANTI-DEPENDENCE ON THE POINTER-WALK `add`, not by
 * a priority gap, and the test that fires is a LIVENESS-shaped one.
 *
 * ================= WHAT THE ROM'S ORDER REQUIRES, EXACTLY =================
 *
 * In the ROM 281 sits ready and unchosen from t=0 to t=15 while 218(67), 221(66)
 * and 224(66) all issue ahead of it.  Losing to a 66 means PRIORITY(281) MUST BE
 * 66 IN THE ROM.  With 66: t=11 -> 218 (sole 67); t=12 -> 221; t=14 -> {224,281}
 * both 66, both class 3 against last_scheduled 221, depend_count 3 against 2 ->
 * 224; t=16 -> 281.  THAT IS EXACTLY THE ROM'S ORDER, and nothing else in the
 * lattice reproduces it.
 *
 * PROVEN BY PROBE, not argued.  The same body with the second argument changed to
 * `0x80` -- an 8-bit immediate, so `*thumb_movsi_insn` takes alternative 1, no
 * split, ONE insn, priority 66 -- emits
 *     add r3,#2 | strh r4,[r3] | ldr r0,=Task_Rain | mov r1,#0x80 | bl StartTask
 * with THE MOV NO LONGER HOISTED.  (It reads 11 because the value is wrong and
 * the stream is an instruction short: a DEVICE used as an instrument, labelled,
 * and its number is a figure about the blocker.)
 *
 * THE BOUND, with its evidence attached:
 *   priority(281) = insn_cost(281,link,282) + priority(282) = 1 + 66.  The 1 can
 *   only become 0 two ways -- `LINK_COST_FREE`, which haifa-sched.c:3096 sets when
 *   the CONSUMER is unrecognizable, or an ANTI/OUTPUT link (arm.c:2425).  The
 *   consumer is a legal thumb ashlsi3 (code 112 in the table) and it READS r1
 *   (`lsl r1,#4` has Rd == Rm; that encoding is among the MATCHING ones, so Rd ==
 *   Rm in the ROM too), so the link is a true dependence.  Neither route exists.
 *   And the split is not optional: `*thumb_movsi_insn` alternative 3 (arm.md:3844)
 *   emits `#` for any thumb `K` constant and `split_all_insns` runs at
 *   toplev.c:3376, BEFORE sched2 at :3481.  Both the 0xfc<<6 pair and the 0xc8<<4
 *   pair appear first in `.18.greg` and are recog_memoized during sched.
 *
 * ==> NO SPELLING OF StartTask's SECOND ARGUMENT CAN REACH THE 4.  The open
 *     question is one sentence: why does the ROM's K-split pair behave as if it
 *     were a single insn at sched2?
 *
 * REFUTED, so nobody proposes it: "the object was built unscheduled."
 * `tryc --no-sched2` on this body reads 35 of 95, first diff at index 3 (the
 * prologue's `sub sp,#8` moves).  The ROM's StartRain WAS scheduled.  A
 * `-fno-schedule-insns2` flag group is not the answer.
 *
 * MEASURED batch 326, nine tail spellings, all against 244 bytes / 103 insns:
 *   INERT at 4 -- `0xc80` written literally (confirms the park); a
 *     `void (*f)(void)` local for the callee; a walking `volatile unsigned short *`
 *     with `*bp++` for all three BLD registers; `StartTask(void *, unsigned int)`;
 *     `unsigned short c1, c2`.
 *   WORSE -- a block-scoped `int pri` temp, 5; a function-scope `pri` assigned
 *     before the BLD stores, 5.
 *   STRUCTURALLY DIFFERENT, dead -- `REG_BLDY = z` (the named zero), 106 insns at
 *     248 bytes; `REG_BLDALPHA = 0x1008` with no named `c2`, 105 insns at 252.
 * None of them changes the critical path through the K-split, which is why the
 * tail spelling is inert.  A sweep can be exhaustive over the wrong dimension.
 *
 * KEPT FROM THE PARK, re-confirmed as still load-bearing: the two pointer roles
 * are ONE source variable (`g` reused as the second allocation's buffer pointer
 * and the per-entry walk pointer) -- 46 differing if split, 9 or 16 if the wrong
 * one is reused; `c1 = 0xfc << 6; REG_BLDCNT = c1;` as its own statement, because
 * the bare literal pools as `ldr r3, =0x3f00`; the `sub sp, #8` with r4 absent
 * from the push list is a CALLER-SAVE slot under -fcall-used-r4, not a spill.
 */
#include "dma.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x1c - 0x18];
    short f1c;
    unsigned char pad1e[0x20 - 0x1e];
};

extern int **iwram_3001e70;
extern unsigned char Data_9ff58[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void Func_8091ff0(int a);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void *f, int pri);
extern void Task_Rain(void);

void StartRain(void)
{
    unsigned char *p;
    int *g;
    struct Ent *e;
    unsigned int i;
    int t;
    int *w;
    int x;
    int y;
    int c1;
    int z;
    int c2;

    p = galloc_ewram(0x1d, 0x82 << 3);
    z = 0;
    w = *iwram_3001e70;
    Func_8091ff0(0xaa);
    e = (struct Ent *)(p + 8);
    DMA3_FILL(p, z, 0x82 << 3);
    g = (int *)galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_9ff58, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0xc0 << 2, g);
    gfree(0xe);
    i = 0;
    do {
        g = (int *)e;
        *g++ = 0;
        *g++ = 0x40000400;
        *g = 0xd4 << 8;
        x = w[0];
        y = w[2];
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;
    } while (i <= 0x1f);
    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    REG_BLDY = 0;
    StartTask(Task_Rain, 0xc8 << 4);
}
