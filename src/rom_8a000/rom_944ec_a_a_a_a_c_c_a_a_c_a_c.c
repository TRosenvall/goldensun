/*
 * StartRain -- PARK at 4 differing STREAM ENCODINGS of 104; 84 INSTRUCTIONS
 * against 84 (lengths agree, so this is a DISTANCE, not a phase).  PIN-FREE.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/StartRain.c asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_c.s --func StartRain
 *
 * SPLIT SHAPE: unchanged -- the reference holds one function, no split needed.
 *
 * BATCH 337, brief E -- CORRECTION TO THE LINE BELOW: it says "103 instructions
 * against 103".  103 is wrong.  objcmp reports 84/84; 104 is the STREAM ENCODING
 * count (instructions + pool words + pad).  The old header copied the stream
 * figure into the instruction line.  Everything else in it reproduces exactly.
 *
 * p3 -- StartRain -- PARK at 4 differing encodings of 104.  PIN-FREE.
 *
 * Figure I measured (not inherited): 4 differing encodings of 104 (ref 104, ours
 * 104), first differing index 80 -- ref 3302 (add r3,#2) against ours 21c8
 * (mov r1,#0xc8).  No SIZE line, no INSTRUCTION COUNT line, no RELOCATIONS line:
 * 244 bytes against 244, 103 instructions against 103.  PINS 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/StartRain.c asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_c.s --func StartRain
 *
 * SPLIT SHAPE: unchanged from the park.
 *
 * THE RESIDUE is one instruction displaced by four slots, the second argument's
 * high half hoisted ahead of the pointer-walk add and the two stores.  One run,
 * one cause.
 *
 * rank_for_schedule READ IN FULL (haifa-sched.c:4029-4112) -- the park's rung
 * list is confirmed complete.  It is a qsort comparator and schedule_block issues
 * ready[--n_ready], so the array sorts ASCENDING and the LAST element wins:
 * highest priority, then highest class.  One detail worth carrying forward: the
 * class test is `if (link == 0 || insn_cost (...) == 1) class = 3`, so a DATA
 * dependent of last_scheduled_insn whose cost is exactly 1 ALSO gets class 3 --
 * only a cost that is not 1 demotes an insn.  That is precisely why
 * arm_adjust_cost returning 0 for anti/output costs the pointer-walk add the tie.
 *
 * THE ESCAPE THE PARK NEVER CONSIDERED, AND IT IS CLOSED BY INVARIANCE.
 * The park bounds priority(281) = cost(281,282) + priority(282) = 1 + 66 and
 * rules out the two ways the 1 can become 0.  It never asks whether the 66 can
 * become 65.  From its own region table:
 *     priority(bl)              = 65
 *     priority(lsl)             = 1 + 65 = 66    ONE link from the call
 *     priority(mov high half)   = 1 + 66 = 67    TWO links from the call
 *     priority(ldr =Task_Rain)  = 1 + 65 = 66    ONE link
 *     priority(strh)            = 1 + 65 = 66    ONE link
 * The mov sits TWO dependence links from the call and every insn it must lose to
 * sits ONE, so any change to the call's priority shifts both sides equally and
 * the one-unit gap is INVARIANT under it.  Reaching 66 therefore requires
 * shortening the mov's distance to the call to one link, and the K-split puts the
 * shift between them unconditionally -- arm.md:3844 alternative 3 emits a split
 * marker for any thumb K constant and split_all_insns runs at toplev.c:3376,
 * before sched2 at :3481.  So the park's "no spelling of the second argument
 * reaches the 4" GENERALISES: the blocker is a path LENGTH in the dependence
 * graph, not a cost, and no spelling of anything reaches it.
 *
 * MEASURED THIS ROUND -- a dimension the park never varied.  The park swept nine
 * TAIL spellings; the blend-register store block is a different dimension and it
 * is the one that supplies the anti-dependence deciding the class rung.  All
 * worse, all at 104 encodings unless noted:
 *   both constants assigned before any store ............... 6
 *   the zero store between the other two ................... 6
 *   the zero store first ................................... 9
 *   the alpha block before the control block ............... 12
 *   the call before the zero store ... 84 of 105, 248 bytes, +2 insns, RELOCDIFF
 * So the park's store order is confirmed load-bearing and this dimension is a
 * measured dead end.
 *
 * STILL REFUTED, not re-proposed: a no-sched2 flag group (35 of 95).
 
 *

 * ===== BATCH 337, BRIEF E: THE PARK IS CONFIRMED BY GCC'S OWN PRINTED TABLE,
 * ===== AND THE CORPUS NAMES THE LEVER THAT WOULD WORK AND CANNOT BE SUPPLIED.
 *
 * RE-MEASURED: 4 differing encodings of 104 (ref 104, ours 104), 84 instructions
 * against 84, first differing index 80 -- ref 3302 against ours 21c8.  No SIZE,
 * no INSTRUCTION COUNT, no RELOCATIONS line.
 *
 * THE PARK DERIVED ITS PRIORITY TABLE BY HAND.  I GOT GCC TO PRINT IT.
 * -fsched-verbose=6 writes the .23.sched2 DUMP FILE (not stderr); its "Region
 * Dependences" table carries a `prio' column.  Recipe: scratch_elev/b337/E/e_sched.sh.
 *
 *   ;;      insn  code    bb   dep  prio  cost   units
 *   ;;      212     5     0     2    68     1    core  : 298 218 215        add r3,#2 (1st)
 *   ;;      215   180     0     3    67     2    core  : 299 298 231 221 218  strh r2,[r3] BLDALPHA
 *   ;;      218     5     0     2    67     1    core  : 299 298 231 221    add r3,#2 (2nd)
 *   ;;      221   180     0     3    66     2    core  : 299 298 231        strh r4,[r3] BLDY
 *   ;;      224   173     0     0    66     2    core  : 299 298 231        ldr r0,=Task_Rain
 *   ;;      281   173     0     0    67     1    core  : 298 282           mov r1,#0xc8
 *   ;;      282   112     0     1    66     1    core  : 299 298 231       lsl r1,#4
 *   ;;      231   239     0     7    65    32    core  : 299 298 297       bl StartTask
 *
 *   ;;	Ready list (t = 11):    224  218  281
 *   ;;		--> scheduling insn <<<281>>> on unit core
 *
 * ready sorts ASCENDING and schedule_block issues the LAST element, so at t=11
 * the contest is 218 (prio 67) against 281 (prio 67) -- an EXACT TIE -- with
 * last_scheduled_insn = 215.  rank_for_schedule then reaches the class rung
 * (haifa-sched.c:4074-4094):
 *   - 218 has an anti link from 215 (215 reads r3, 218 writes r3).
 *     arm_adjust_cost (config/arm/arm.c:2416-2426) returns 0 for REG_DEP_ANTI /
 *     REG_DEP_OUTPUT as its FIRST statement, so insn_cost != 1 and
 *     REG_NOTE_KIND != 0 -> CLASS 2.
 *   - 281 has no link from 215 at all -> link == 0 -> CLASS 3.
 * 3 > 2, so the mov wins.  Had the classes tied, the dependent-count rung
 * (:4096-4108) gives 218 four dependents against 281's two, and the LUID rung
 * after it is ALREADY ROM ORDER -- the Region-Dependences table prints in LUID
 * order and reads 215 218 221 224 281 282 231, which IS the ROM stream.  sched2
 * is the only thing that moves this function's tail.
 *
 * WHY 4 IS A FLOOR, each rung with its PRECONDITION:
 *  1. prio(281) = insn_cost(281,link,282) + prio(282) is FORCED at 67.
 *     insn_cost (haifa-sched.c:3060-3122) zeroes a link only when
 *     recog_memoized (used) < 0; `used' is 282, INSN_CODE 112 (*thumb_ashlsi3),
 *     recognised.  ADJUST_COST then calls arm_adjust_cost (282, link, 281, 1):
 *     kind 0, 282 is not a CALL_INSN, SET_SRC is an ASHIFT not a MEM, so it
 *     falls through to `return cost' = 1.
 *  2. prio(218) = 1 + prio(221) is FORCED at 67, because arm_adjust_cost's
 *     SECOND clause -- `if (REG_NOTE_KIND (link) == 0 && GET_CODE (insn) ==
 *     CALL_INSN) return 1;' -- OVERRIDES store1's latency 2 for a store feeding
 *     a call, and crushes ldr r0,=Task_Rain (type load, latency 2) to 1 as well.
 *     Every chain into the call is one-per-link and only 281, two links out,
 *     reaches 67.
 *  3. EVERYTHING IS INVARIANT UNDER prio(231).  From the dump:
 *     prio(224) = prio(221) = prio(282) = 1 + prio(231) and
 *     prio(281) = prio(218) = 2 + prio(231).  The park said this; the printed
 *     numbers confirm it.
 *  4. The class rung is UNREACHABLE, not merely lost.  Demoting 281 needs a link
 *     from 215 = (set (mem/v:HI (reg r3)) (reg:HI r2)): it reads r3 and r2 and
 *     writes memory, while 281 writes r1 -- no anti, no output, no data
 *     dependence is constructible.  Promoting 218 needs
 *     insn_cost (215, anti-link, 218) == 1, and the anti return is unconditional.
 *
 * SPLIT CLAIM CONFIRMED WITH LINES: *thumb_movsi_insn alternative 3 (constraint
 * K) emits `#' and is split by the define_split at arm.md:3858-3890;
 * split_all_insns (0) runs at toplev.c:3375-3376 UNCONDITIONALLY for
 * optimize > 0, and sched2 at toplev.c:3473-3481.  So sched2 always sees two
 * insns and prio = 2 + prio(call) cannot be avoided.  (The park cited
 * arm.md:3844; the split body is at 3858-3890 in this tree.)
 *
 * A WOULD-BE ESCAPE OF MY OWN, REFUTED, recorded so nobody spends a round on it:
 * priority() SKIPS links marked RTX_INTEGRATED_P (haifa-sched.c:3148), which
 * would drop prio(281) to 1 and produce ROM order exactly.  NOTHING in this
 * compiler ever SETS that bit on a dependence link -- the only other occurrence
 * in the file is :4533, which merely PROPAGATES it.  The rung is dead code.
 *
 * THE LEVER THE CORPUS NAMES, AND WHY THIS FUNCTION CANNOT SUPPLY IT.
 * scratch_elev/b337/E/e_corpus_movlsl.py partitions asm/ (4,527 generated .s,
 * 704 hand-written) and finds every `mov rX,#imm / lsl rX,#n / bl' window -- the
 * ROM's shape here.  561 in the GENERATED half, so the un-hoisted shape is amply
 * reachable; 26 of those have a STORE immediately in front of the mov.  In EVERY
 * one of them the store READS THE REGISTER THE MOV WRITES, e.g.
 *     asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c_b.s   strb r3,[r0] / mov r0,#128
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_b.s  strh r3,[r0,#6] / mov r0,#190
 * i.e. an ANTI-DEPENDENCE on the mov's DESTINATION drops it to class 2 and hands
 * the store the class rung.  StartRain cannot supply one: the ROM's whole
 * REG_BLDCNT/REG_BLDALPHA/REG_BLDY tail uses r0, r2, r3 and r4 and never touches
 * r1, and r1's last use is inside the loop -- a different basic block, so no
 * intra-block dependence exists.  The mov's register is fixed by the ABI (second
 * argument) and the tail's registers are fixed by the reference stream.
 *
 * SO THE BLOCKER IS A REGISTER IDENTITY, not a cost, a spelling or an ordering.
 * DO NOT RE-SWEEP THE SECOND ARGUMENT OR THE STORE BLOCK.
 * -- scratch_elev/b337/E
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
    do { } while (0);
    StartTask(Task_Rain, 0xc8 << 4);
}
