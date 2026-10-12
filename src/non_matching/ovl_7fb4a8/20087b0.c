/* OvlFunc_971_20087b0 -- asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s
 *
 * NON-MATCHING, 4 differing STREAM encodings of 71  (ref 71, ours 71;
 *   16-bit INSTRUCTIONS 53 on BOTH sides, so this is a TRUE DISTANCE).
 *   SIZE 176 bytes both.  RELOCATIONS CLEAN.  First difference at index 52.
 *   MEASURED batch 338 brief D, down from 14.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7fb4a8/20087b0.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s --func OvlFunc_971_20087b0
 *
 * PINS 0 (tools/shimcount.py prints no rows).  DEVICES 0 -- the three
 * `do { } while (0);` anchors are device-free and shimcount does not count
 * them (reports/batch-337.md, "do { } while (0) beats the barrier").
 *
 * SHIP SHAPE.  The reference holds THREE functions and all three are parked
 * (`OvlFunc_971_2008580` at 171 of 256, this one, `OvlFunc_971_2008860` at
 * 120 of 318), so this one ships only by a split.  tools/datacheck.py is
 * SILENT on the reference (no data).  tools/split_s.py --dry-run:
 *     ovl_30_a_c_c_c_a_c_c_a_a_a.s  1 function, 286 lines
 *     ovl_30_a_c_c_c_a_c_c_a_a_b.s  1 function,  77 lines   <- this function
 *     ovl_30_a_c_c_c_a_c_c_a_a_c.s  1 function, 338 lines
 *   ONE linker script: overlays/rom_7fb4a8/overlay.ld.  Not ovl_common, so the
 *   split is local to that overlay.
 *   The previous header's "nothing installs here until all three are matching"
 *   IS FALSE -- split_s.py cuts before/target/after and any member cuts out
 *   alone (docs/brief-template.md).
 *
 * ====================================================================
 * BATCH 338 BRIEF D -- THE 14 WAS NOT A DISTANCE, AND CLUSTER 1 IS NOT A
 * PINNING PROBLEM.  THREE ANCHORS AND ONE STORE MOVE: 14 -> 4.
 * ====================================================================
 *
 * TWO CORRECTIONS TO THE HEADER BELOW, both from reading the objects rather
 * than the park:
 *
 * 1. THE 14 HAD DIRTY RELOCATIONS, so it bounded nothing.  `objcmp --func`
 *    reported `XX RELOCATIONS differ`: the ROM's pool is
 *      0x90 0x302 / 0x94 ewram_20023a0 / 0x98 ewram_2002220 / 0x9c 0x04000208
 *    and ours had 0x98 and 0x9c TRANSPOSED, with `bl __GetFlag` at offset 4
 *    instead of 6.  Both are fixed below and `rel=ok` now.
 *
 * 2. CLUSTER 1 IS NOT THE REG_IME PINNING CLASS AND NEEDS NO FAKEMATCH.
 *    The header below says "the base lands in r4 instead of r1, savedIme in r5
 *    instead of r4, and gcc then needs a second zero".  NONE OF THAT IS TRUE OF
 *    THE BATCH-333 BODY.  Disassembled side by side, BOTH SIDES PUT
 *    `p` IN r1, THE IME ADDRESS IN r0 AND savedIme IN r4, and there is no
 *    second zero anywhere.  The only difference was WHICH OF THE TWO POOL
 *    LOADS ISSUES FIRST -- and its pool consequence.  That is a scheduling
 *    question, not an allocation one, and an anchor closes it outright.
 *    >> The park's "reachable only as a FAKEMATCH" verdict is RETIRED.
 *
 * WHAT CLOSED IT, as a ladder.  Every row rel=ok unless said:
 *   14  the batch-333 body                                      RELOCDIFF
 *   10  + anchor after `res = 0;`      (closes cluster 2)        RELOCDIFF
 *    6  + anchor after `p = ewram_2002220;`  (closes cluster 1, and the pool
 *       order with it -- relocations become clean here)
 *    4  + anchor after `zero = 0;` AND the two byte stores moved to sit after
 *       the FIRST TWO wide stores instead of after the first
 *   54  the same store move with NO anchors, +4 bytes             RELOCDIFF
 * So all three anchors are load-bearing and the store move is worth nothing
 * without them: dropping the `p` anchor costs 4 and re-dirties the pool.
 *
 * WHY THE ANCHOR REACHES THIS.  `haifa-sched.c:3744-3757` makes the anchor
 * depend on every register's last uses, last sets and last clobbers and then
 * calls `flush_pending_lists`, so it orders memory too.  Cluster 2 was the
 * `movs r6, #0` sinking below `bl __GetFlag`; cluster 1 was the two pool loads
 * issuing in the wrong order.  Both are pure sched2 placements of insns with
 * no dependence to hold them, which is exactly what an anchor supplies.
 *
 * WHAT REMAINS, and it is ONE lever away.  4 of 71, all in one window:
 *     rom   ldr r3,=20023ac / str r2,[r3] / ldr r3,=2002238 / strb r2,[r1,#3]
 *           / strb r2,[r1,#2] / strh r2,[r3]
 *     ours  ldr r3,=20023ac / strb r2,[r1,#3] / strb r2,[r1,#2] / str r2,[r3]
 *           / ldr r3,=2002238 / strh r2,[r3]
 * The two byte stores sit in the gap after the WRONG address load -- exactly
 * one (ldr, store) pair early.  It is decided at INSN_LUID
 * (`haifa-sched.c` rank_for_schedule's last rung): both candidates are
 * `store1` so the class rung at :4068-4095 ties, and the lower luid wins.
 *
 * AND THE SOURCE ORDER THAT FIXES THE SCHEDULE BREAKS THE ALLOCATION.
 * With the byte stores one statement later (after the THIRD wide store) the
 * SCHEDULE IS BYTE-FOR-BYTE THE REFERENCE'S -- and then `p` and the IME
 * address SWAP r0/r1, for 8.  `.18.greg` says `32 in 0  42 in 1` for that
 * body and `32 in 1  42 in 0` for this one (32 is `p`, 42 the IME address).
 * `allocno_compare` (`global.c:598-620`) reproduces both, twelve for twelve:
 *     pri = floor_log2(n_refs) * n_refs / live_length * 10000 * size
 *   bytes after W2 (here):  p 5/28 -> 2*5/28 = 0.357 ; ime 6/36 -> 0.333  p FIRST
 *   bytes after W3:         p 5/32 -> 2*5/32 = 0.313 ; ime 6/36 -> 0.333  IME FIRST
 * `.17.lreg` prints `Register 32 ... across 28 insns` here and `across 32
 * insns` there, which is the whole of it.  Whoever takes this needs p's
 * live_length <= 29 WITH the byte stores after the third wide store, or
 * p's n_refs at 6 (floor_log2 stays 2, so 12/32 = 0.375 wins).
 *
 * MEASURED INERT, counts equal, on top of the three anchors:
 *   `savedIme` taken before `p` is materialised                    4
 *   savedIme declared before p / p declared last / u16 / int / uint 4  (x5)
 *   a second pointer `q` for the byte stores                       4
 *   the two byte stores written in the other order                 4
 *   -fno-gcse / -ffixed-r7 / -fno-strict-aliasing /
 *     -fno-strength-reduce / -fno-rerun-loop-opt                   4
 * MEASURED WORSE on top of the three anchors:
 *   byte stores after the third wide store                         8
 *   byte stores after the third, the two swapped                   8
 *   byte stores last                                               9
 *   `p[1] = 0x80` moved down among the wide stores                 9-30
 *   direct `ewram_2002220[n]` indexing, no `p`                     7, RELOCDIFF
 *   -fno-schedule-insns2                                           5
 *   -fno-rerun-cse-after-loop                                     14
 *   -O1                                                           15
 *   an anchor anywhere inside the WIDE-store run         22-54, +4 or +8 bytes
 * NOTE the last row: an anchor is NOT free among the stores -- it lengthens
 * every live range crossing it by one and re-creates the pooled HImode zero
 * the batch-333 header describes.
 *
 * AUTOSWEEP (batch 338): flag axis -- `-fno-schedule-insns2` took the UNCHANGED
 * body 14 -> 7, every other flag inert or worse.  Anchor axis -- 12 depth-1
 * boundaries, best 10 (RELOCDIFF).  BOTH ARE SUPERSEDED by the body below, and
 * note that autosweep NEVER REACHED THE RESIDUE: `boundaries()` only walks
 * brace-depth-1 statements and this whole window is inside `if (v < 0)`.
 *
 * ===== CARRIED FORWARD: the batch-319/333 header, verbatim apart from the two
 * ===== markers below, which stop parkcheck reading a SUPERSEDED
 * ===== figure as this park's claim.
 *
 *
 * [SUPERSEDED FIGURE] 14 differing / 71 encodings  (RE-MEASURED, batch 333 brief B).
 *   COUNTS NOW AGREE: ref 71, ours 71, instructions fifty-three against
 *   fifty-three, SIZE one hundred and seventy-six bytes both sides.  The
 *   earlier twenty-six was measured on a stream one instruction long.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7fb4a8/20087b0.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s --func OvlFunc_971_20087b0
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER: the REG_IME PINNING class -- reachable only as a FAKEMATCH.
 * 20 of 67, length 68 vs 67.
 *
 * ALL CONTROL FLOW IS EXACT. Every call, every branch, both arms, all five
 * labels, the shared error path and the interwork epilogue line up
 * instruction for instruction. The residue is two localised clusters.
 *
 * CLUSTER 1 -- the cleanup block's register birth order:
 *
 *     rom   ldr r1, =0x2002220 / ldr r0, =0x4000208 / ldrh r4, [r0]
 *     ours  ldr r0, =0x4000208 / ldr r4, =0x2002220 / ldrh r5, [r0]
 *
 * The ROM materialises the ewram base BEFORE reading REG_IME; gcc reads
 * REG_IME first and the whole block's allocation follows from that (the base
 * lands in r4 instead of r1, savedIme in r5 instead of r4, and gcc then needs
 * a second zero, which it pool-loads as `ldr r1, =0x0`).
 *
 * THIS IS A KNOWN CLASS, ALREADY DIAGNOSED IN THIS TREE.
 * src/non_matching/ovl_7eaf28/2008f50.c records the same behaviour and names
 * the only known fix: register pinning (`register void *p __asm__("r1")`)
 * plus an empty `__asm__ volatile("" : : "r"(p))` barrier to force the base
 * out before REG_IME is read. That is what src/rom_c0/rom_3650_c_b.c does,
 * and that file marks itself `// fakematch`.
 *
 * NOT APPLIED HERE, deliberately. A fakematch is a different kind of result
 * from a match and is worth choosing on purpose and consistently, which is the
 * same judgement 2008f50.c made. If the project decides to fakematch the
 * REG_IME family, this function is a candidate and the template is one file
 * away -- the rest of it is already exact.
 *
 * CLUSTER 2 -- `mov r6, #0` placement, and it is independent of cluster 1:
 *
 *     rom   mov r6, #0 / ldr r0, =0x302 / bl __GetFlag / ldr r3,=... / strb r6
 *     ours  ldr r0, =0x302 / bl __GetFlag / ldr r3,=... / mov r6, #0 / strb r6
 *
 * The ROM keeps the zero live ACROSS the call in a callee-saved register; gcc
 * sinks the mov below the call and rebuilds it for the store. Both versions
 * choose r6 and push the same registers, so this is placement only.
 *
 * MEASURED on this cluster:
 *   storing the variable (`ewram_20023a0 = res`) rather than the literal 0
 *                                                          -- 23, no change
 * gcc folds the initialiser to one rtx before these passes run, so `res` and
 * `0` are the same input. This is the behaviour docs/elevation.md records
 * under "a zero survives in a callee-saved register only inside a LOOP":
 * there is no loop here and the value has one dynamic use, so nothing buys
 * the placement.
 *
 * FULL MEASURED LIST -- six spellings:
 *   baseline, literal zero stores                                    23
 *   `ewram_20023a0 = res` instead of the literal                     23
 *   savedIme declared before p                                       23
 *   savedIme as u16 rather than u32                       72 lines,  28
 *   ONE NAMED `zero` local used for every zero store                 20  <- best
 *   array indexed directly, no `p` pointer                           24
 *   `p` materialised at the top of the function          first diff 0, 56
 *
 * The named zero is a genuine gain and is kept below: it removes one of the
 * two spurious constant materialisations. It does NOT remove the pool-loaded
 * `ldr r1, =0x0`, which belongs to cluster 1's allocation.
 *
 * The two worse results are useful negatives: widening savedIme to u16 costs
 * five lines, and hoisting the base pointer to the top of the function makes
 * it live across three calls and wrecks the whole body.
 * 
 * ========================================================================
 * BATCH 333 BRIEF B -- TWENTY-SIX DOWN TO FOURTEEN.  THE EXTRA LINE WAS A
 * POOLED HImode ZERO, AND STORE ORDER IS WHAT DECIDES WHETHER ONE EXISTS.
 * ========================================================================
 *
 * DIRECTION, STATED BACK: this park was OURS LONGER by one instruction, so the
 * search was for a value we were keeping, not a statement we were missing.
 *
 * The park above is right that the named zero is a genuine gain and right about
 * both clusters.  What it did not see is that ONE named zero is not enough: gcc
 * still built a SECOND zero in HImode and PUT IT IN THE POOL --
 *
 *     ours   ... mov r2, #0 ... LDRH r1, <pool>  ... strb r1, [p, #3]
 *     rom    ... mov r2, #0 ...                  ... strb r2, [p, #3]
 *
 * -- one extra instruction and one extra pool word, which is also the whole of
 * the size difference.  The park's own measured row "ONE NAMED zero local used
 * for every zero store" removed one of the two spurious materialisations; this
 * removes the other.
 *
 * WHY AN HImode CONSTANT ALWAYS REACHES THE POOL IN THUMB.  `*thumb_movhi_insn`
 * (arm.md:4318-4320) constrains operand 1 as "l,mn,l,*h,*r,I".  Alternative 1
 * is `mn` and `n` matches ANY const_int; the 8-bit immediate alternative `I` is
 * alternative 5, and recog takes the FIRST match.  So an HImode const_int --
 * zero included -- is recognised as the memory alternative and goes through the
 * minipool, where the same value in SImode is a one-instruction `mov`.
 * Func_80ae9f0 in rom_a1000 had the identical extra instruction from the
 * identical cause and was re-priced by the same reasoning in this batch.
 *
 * THE LEVER IS THE ORDER OF THE STORES, not their spelling.  With the two byte
 * stores placed after the FIRST wide store, no HImode constant is ever built:
 *
 *     p[1] = 0x80;  <wide 0>;  p[3] = zero;  p[2] = zero;  <wide 1..3>;
 *
 * reads fourteen with the counts equal.  Moving them to the very top of the
 * block reads sixteen; leaving them where the park had them reads twenty-six,
 * one instruction long.  All seven stores are to distinct, non-volatile
 * addresses, so this is an ordering choice and not a change of program.
 *
 * MEASURED EXACTLY INERT at fourteen: the interrupt-enable save taken before
 * the base pointer is materialised; the result's zero initialiser moved below
 * the first call; the byte value named as well as the zero.
 * MEASURED, with every figure on a stream of equal length unless said:
 *   byte stores after the first wide store                14   <- kept below
 *   byte stores at the top of the block                   16
 *   byte stores at the top, before the 0x80 byte store    16
 *   byte stores after the first two wide stores           17
 *   the zero's assignment hoisted above the save          16
 *   byte stores last                        one long,     27
 *   a second named zero just for the byte stores
 *                                           one long,     26
 *   an unsigned-char or unsigned-short named zero
 *                                           one long,     26
 *   a cast at each byte store               one long,     26
 *   literal zero at every store             one long,     28
 *   one wide store moved before another     three long,   56
 * The last row is the useful negative: only the BYTE stores may move.  Moving
 * a wide store past another costs three instructions.
 *
 * WHAT REMAINS IS THE PARK'S TWO CLUSTERS AND NOTHING ELSE, and cluster 2 is
 * now a pure placement of one instruction: the reference materialises its zero
 * BEFORE the first flag call and ours after it.  Cluster 1 is unchanged and is
 * still the REG_IME birth order the park says is reachable only as a fakematch.
 *
 * PIECE-MATES, AND THEY MATTER HERE.  This reference piece holds THREE
 * functions and all three are parked: the two subroutines this one calls are
 * `OvlFunc_971_2008580`, which docs/directory/01-parked.md prices at ONE
 * differing encoding, and `OvlFunc_971_2008860` at twelve.  One `.c` is one
 * TU, so nothing installs here until all three are matching -- and the piece
 * is collectively very close.  Anyone picking this up should take the three
 * together.
 *  
 */
#include "gba/io.h"

extern unsigned char ewram_20023a0;
extern unsigned char ewram_2002220[];
extern unsigned int ewram_2002080;
extern unsigned short ewram_2002008;
extern unsigned int ewram_20023ac;
extern unsigned short ewram_2002238;

extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __SetFlagByte(int id, int v);
extern int OvlFunc_971_2008580(void);
extern int OvlFunc_971_2008398(void);

int OvlFunc_971_20087b0(void)
{
    unsigned char *p;
    u32 savedIme;
    int res;
    int v;
    int flag;
    int zero;

    res = 0;
    do { } while (0);
    flag = __GetFlag(0x302);
    ewram_20023a0 = res;
    if (flag == 0) {
        __WaitFrames(5);
        res = OvlFunc_971_2008580();
        if (res < 0)
            goto fail;
        __WaitFrames(5);
        res = OvlFunc_971_2008398();
        v = res;
        if (res < 0)
            goto check;
    } else {
        res = OvlFunc_971_2008398();
        v = res;
        if (res < 0)
            goto fail;
        __WaitFrames(0xa);
        res = OvlFunc_971_2008580();
        if (res < 0)
            goto fail;
    }
    __SetFlagByte(0xfc << 2, v);
    res = v;
check:
    if (v < 0) {
fail:
        p = ewram_2002220;
        do { } while (0);
        savedIme = REG_IME;
        SET_IO(REG_IME, REG_ADDR_IME);
        zero = 0;
        do { } while (0);
        p[1] = 0x80;
        ewram_2002080 = zero;
        ewram_2002008 = zero;
        p[3] = zero;
        p[2] = zero;
        ewram_20023ac = zero;
        ewram_2002238 = zero;
        SET_IO(REG_IME, savedIme);
    }
    return res;
}
