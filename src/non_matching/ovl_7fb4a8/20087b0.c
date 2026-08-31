/* OvlFunc_971_20087b0 -- asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c.s
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
        ewram_2002008 = zero;
        ewram_20023ac = zero;
        p[3] = zero;
        p[2] = zero;
        ewram_2002238 = zero;
        SET_IO(REG_IME, savedIme);
    }
    return res;
}
