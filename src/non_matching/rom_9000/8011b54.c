/* Func_8011b54 (AddPaletteCycleChannel) -- asm/rom_9000/rom_11568_c_c_c_c_a_a_c.s
 *
 * NON-MATCHING, 14 of 56 encodings  (MEASURED, batch 335 brief D).
 *   92.9 percent ALIGNED, seven instructions in five hunks, first difference at
 *   index twenty-five of fifty-six.  REAL INSTRUCTION COUNT NOW AGREES:
 *   fifty-three on both sides.  SIZE 116 bytes both.  Relocations clean.
 *   NO INSTRUCTION IS MISSING OR EXTRA -- every remaining difference is a
 *   sched2 tie-break inside the one block.  The previous body claimed
 *   forty-eight of the same fifty-six.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/8011b54.c \
 *     asm/rom_9000/rom_11568_c_c_c_c_a_a_c.s --whole
 *
 * Builds a palette-cycle channel: guard the channel count against 3, fill five
 * fields of a 0x2c-byte slot, kick a DMA3 transfer from palette RAM into the
 * slot's buffer, and bump the count.
 *
 * ========== THE OLD DIAGNOSIS IS REFUTED, AND SO IS ITS NEXT STEP =========
 *
 * The previous header said the entry sign-extension ORDER was the whole problem
 * and that it "most likely follows from which argument is USED first".  IT IS
 * NOT THE PROBLEM AND IT NEVER WAS.  With the count reloaded (below) the entry
 * block is BYTE-IDENTICAL to the reference through the guard branch, and the
 * order the old header called unreachable falls straight out of the plain
 * `short` parameter declarations it already had.  Nothing about first use was
 * involved.
 *
 * ========== THE DIRECTION WAS HIDDEN BY AN ALIGNMENT PAD ==================
 *
 * The streams were ALREADY equal at fifty-six encodings both, so nothing
 * flagged a length gap.  What was one long is the REAL instruction count --
 * fifty-four against fifty-three -- and the streams tie only because the
 * reference's odd instruction count leaves a halfword the assembler pads before
 * the pool while an even one does not.  An equal encoding count can therefore
 * HIDE a genuine one-instruction excess.  Count both.
 *
 * ========== WHAT CLOSED IT: THREE CROSSED LEVERS =========================
 *
 *   1. RE-READ the channel count at the end instead of reusing the value:
 *        *cnt = *cnt + 1;      not      *cnt = n + 1;
 *      The reference reloads it -- a halfword load, an add of one, a store --
 *      because the intervening word store and the DMA3 stmia may alias, so gcc
 *      cannot common the two reads.  That ENDS the count's live range at the
 *      multiply, which frees a low callee-saved register, and the frame-delay
 *      argument then reaches r7 instead of ip.  That kills both the copy into
 *      ip at entry and the copy back out before its store.  ONE source change,
 *      THREE instructions, and it is worth eighteen differences.  The increment
 *      written as a post-increment of the lvalue is byte-identical to it.
 *   2. the count local declared UNSIGNED.  The reference's guard is an unsigned
 *      branch; a signed local gives the signed one.  Worth one.
 *   3. a NAMED LOCAL for the stored zero:
 *        int z;  ...  z = 0;  *(u16 *)(slot + 4) = z;  *(u16 *)(slot + 6) = z;
 *      Without it gcc stores the register it has just built for the palette-RAM
 *      base into the two halfword fields, because that constant's low sixteen
 *      bits are zero -- so the reference's own load of zero never appears.
 *      Naming the zero gives it its own pseudo and the load comes back.  Worth
 *      sixteen.  The TYPE of the name does not matter (int, short and unsigned
 *      short are byte-identical); its PLACEMENT does -- the assignment must sit
 *      after the early return, not before it.
 *
 * ========== WHAT IS LEFT: SCHED2 TIE-BREAKS ONLY =========================
 *
 *   ours puts the slot address add two slots early;
 *   ours puts the frame-delay halfword store nine slots early;
 *   ours emits the control-word shift, then the zero, then the delay mask,
 *     where the reference has the delay mask, then the zero, then the shift.
 *
 * NEXT: this is a scheduling question and nothing else -- there is no missing or
 * extra instruction left to find.  The rungs are rank_for_schedule's, and the
 * last one is INSN_LUID, so the remaining lever is what reaches the dependence
 * priorities: dma.h's own DMA3_CLEAR_OFS note records the one shape that has
 * moved a rung here before (taking an offset as its own argument moves an add
 * INSIDE the inlined body and lowers its luid), and the delay store is the
 * instruction that needs to move LATER.
 *
 * ========== MEASURED, AND BIT-IDENTICALLY INERT OR WORSE =================
 *
 * On the schedule, all with levers 1-3 applied:
 *   all six permutations of the five store statements          inert, or +2
 *                                                              when the word
 *                                                              store moves last
 *   both operand orders of the DMA3 control word               inert
 *   three types for the named zero                             inert
 *   three spellings of the palette-RAM base -- a shifted
 *     product, a plain literal, an explicit shift              inert
 *   the source address computed BEFORE the slot address        +4
 *   the two zero stores written in the other order             +18
 *   the named zero assigned before the early return            +38, and five
 *                                                              instructions long
 * Superseded negatives from the previous header, both re-measured and both
 * explained by the levers above rather than by casts:
 *   `int` parameters with explicit short casts                 five short
 *   plain `short` parameters                                   the right answer,
 *                                                              and already here
 *
 * SOLVED and worth keeping: the stride arithmetic, the guard against three, the
 * five field stores and their offsets, the DMA3 kick, and the reference's
 * epilogue popping into r1, which is the int return this declares.
 */
#include "dma.h"

extern int iwram_3001ec0;

int Func_8011b54(short a, short b, short c, short d)
{
    char *base;
    unsigned short *cnt;
    unsigned int n;
    char *slot;
    int src;
    int z;

    base = (char *)iwram_3001ec0;
    cnt = (unsigned short *)(base + 0xb0);
    n = *cnt;
    if (n > 3)
        return -1;
    slot = base + n * 0x2c;
    src = (((unsigned short)a << 4) + (unsigned short)b) * 2 + 0xa0 * 0x80000;
    z = 0;
    *(unsigned short *)(slot + 4) = z;
    *(unsigned short *)(slot + 6) = z;
    *(unsigned short *)(slot + 0xa) = d;
    *(int *)slot = src;
    *(unsigned short *)(slot + 8) = c;
    DMA3_SET((void *)src, slot + 0xc, (unsigned short)d | 0x80000000);
    *cnt = *cnt + 1;
    return 0;
}
