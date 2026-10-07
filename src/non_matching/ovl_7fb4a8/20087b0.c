/* OvlFunc_971_20087b0 -- asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c.s
 *
 * NON-MATCHING, 14 of 71 encodings  (RE-MEASURED, batch 333 brief B).
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
        savedIme = REG_IME;
        SET_IO(REG_IME, REG_ADDR_IME);
        zero = 0;
        p[1] = 0x80;
        ewram_2002080 = zero;
        p[3] = zero;
        p[2] = zero;
        ewram_2002008 = zero;
        ewram_20023ac = zero;
        ewram_2002238 = zero;
        SET_IO(REG_IME, savedIme);
    }
    return res;
}
