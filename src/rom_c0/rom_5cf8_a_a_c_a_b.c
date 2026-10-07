/* Func_8006384 -- MATCHING, 0 of 25 encodings, 56 bytes, PIN-FREE.
 *
 * PINS: 0.  FLAG GROUP: none.  Production flags.
 *
 * SPLIT: asm/rom_c0/rom_5cf8_a_a_c_a.s holds THREE functions and this is the
 * FIRST.  tools/split_s.py --dry-run:
 *     would write asm/rom_c0/rom_5cf8_a_a_c_a_b.s  (1 function,  30 lines)
 *     would write asm/rom_c0/rom_5cf8_a_a_c_a_c.s  (2 functions, 72 lines)
 *     would REMOVE asm/rom_c0/rom_5cf8_a_a_c_a.s
 *     would rewrite stage1.ld
 * tools/datacheck.py on the source .s is SILENT (exit 0) and stage1.ld names
 * the object ONCE (line 81, `.text`).  exports: [].
 *
 * REPOINT: src/non_matching/rom_c0/80063bc.c and .../8006408.c are parks for
 * the two functions that stay in the _c half and their recipes name the
 * pre-split path.  Run tools/repoint_parks.py after the split.
 *
 * Verify with (INSTALLED path, after the split):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c0/rom_5cf8_a_a_c_a_b.c \
 *     asm/rom_c0/rom_5cf8_a_a_c_a_b.s --func Func_8006384
 *   -> OK Func_8006384 -- 56 bytes, 25 encodings and 2 relocations identical
 *   (verified in batch 334 against a scratch copy of the post-split .s with
 *    --whole: "OK whole file")
 *
 * WHAT IT DOES.  A spin-wait: poll an IWRAM flag halfword until every bit of
 * the mask argument is set, calling WaitFrames(1) between reads, then return
 * bits 4-5 of REG_SIOCNT.
 *
 * ===================================================================
 * BATCH 334: THE PARK READ 24 of 25 ON UNEQUAL LENGTHS (ref 22
 * instructions against ours 19) AND ITS BLOCKER CLASS WAS WRONG.
 * FOUR LEVERS, AND NOT ONE OF THEM WORKS ALONE.
 * ===================================================================
 *
 * The park called the blocker "operand canonicalisation on AND,
 * register-to-register form" and recorded, correctly, that spelling the copy
 * (`t = m; t &= v;`) is EXACTLY INERT.  That observation is right and the
 * conclusion drawn from it was wrong: the AND spelling is inert because the
 * copy is deleted by a different pass in each of the two places it occurs, and
 * because which register the surviving copy lands in is decided by whether the
 * temp is a LOCAL or a GLOBAL allocno -- neither of which the spelling touches.
 *
 * (1) THE REG_SIOCNT RELOCATION, which `make compare` could never have passed.
 *     The park reached the register through `extern volatile unsigned int
 *     REG_SIOCNT;`, so our object carried `R_ARM_ABS32 REG_SIOCNT` where the
 *     reference has a bare pool word: include/gba.inc:49 is
 *     `.set REG_SIOCNT, 0x04000128`, an absolute, so the hand `.s` emits no
 *     relocation at all.  `*(vu32 *)&REG_SIOCNT` is the fix and it is also the
 *     correct WIDTH: include/gba/io.h:482 types the register `vu16`, and the
 *     reference reads it `ldr r0, [r3]`, 32 bits wide.  This is the same
 *     correction the two siblings in this piece already carry, and the
 *     established spelling for it in this tree is src/rom_c0/rom_56cc_a_c.c's
 *     `(vu32 *)&REG_DMA3SAD`.
 *
 * (2) NO POINTER VARIABLE.  Read the global directly at both sites.  With a
 *     `p = &iwram_3001f64` local, gcse's copy propagation (pass 07) merges the
 *     guard's address pseudo and the loop's into ONE pseudo, which then crosses
 *     the call and so cannot take r0-r3 or r4 (find_reg, global.c:992-995) --
 *     it gets r6 for both uses and the reference's `mov r6, r1` never exists.
 *     Naming a second pointer for the loop (`q = p`) does NOT reintroduce it,
 *     exactly as the park says, because cprop eats that copy too:
 *     `COPY-PROP: Replacing reg 34 in insn 43 with reg 33`.  With no pointer
 *     local the two reads materialise the address independently, the guard's
 *     lands in r1 and the loop's in r6, and the copy appears in the loop
 *     PREHEADER where the reference has it.  19 instructions -> 20.
 *
 * (3) THE GLOBAL IS `volatile`, AND THAT IS WHAT KEEPS THE TWO `mov r3, r5`
 *     MASK COPIES.  20 -> 22, and the figure 24 -> 8.
 *     Mechanism: `*thumb_andsi3_insn` (arm.md:1710-1717) constrains operand 1
 *     `"%0"`, so one AND operand must become the destination.  regmove's
 *     forward pass (regmove.c:1180-1215) ties the destination to whichever
 *     operand DIES -- its own dump says `Fixed operand 2 of insn 24 matching
 *     operand 0` -- and for a non-volatile read the loaded value dies at the
 *     AND, so the AND is retargeted onto it and no copy is needed.  The gate
 *     that refuses this is in fixup_match_1 (regmove.c:1630-1637): with no
 *     REG_DEAD note on the operand it falls to
 *     `stable_and_no_regs_but_for_p`, which cannot pass an AND whose other
 *     operand is a third register, and returns 0 for BOTH operands -- so
 *     reload must satisfy the matching constraint with a copy, and it copies
 *     operand 1, the mask.  That is the reference, instruction for instruction.
 *     This matches the three places in the byte-matching corpus that carry the
 *     same window -- src/rom_9000/rom_f9cc_a_c_c_b.c's `m = v; m &= mask;`,
 *     and the same shape in asm/rom_8a000/rom_8ace0_a_a_a_a.s and
 *     asm/rom_9000/rom_11568_c_c_a_a.s -- in every one of which BOTH AND
 *     operands are live past the AND.
 *     `volatile` here is a LEVER and not a device under the batch-321 ruling
 *     (docs/owner-decisions.md, decision 4): it is applied to the OBJECT, so
 *     every read of it is volatile -- there is no second, plain read of the
 *     same lvalue -- and re-reading is a genuine property of the data, since a
 *     spin-wait on a flag another agent sets is only correct if the read is
 *     re-done.  MEASURED WORSE, and worth recording as the trap: volatile on a
 *     POINTER instead of the object reads SEVENTEEN instructions, three fewer
 *     than the plain body, not more.
 *
 * (4) A SEPARATE TEMP FOR THE LOOP'S AND RESULT.  8 -> 0.
 *     With one temp the pseudo is referenced in both the guard block and the
 *     loop block, so it is a GLOBAL allocno and global-alloc hands it r2 after
 *     local-alloc has already given r3 to the block-local loaded value.  With
 *     two temps both are block-local, local-alloc allocates them, and Thumb's
 *     REG_ALLOC_ORDER (arm.h:989-995) is `{3, 2, 1, 0, ...}` -- so each AND
 *     destination takes r3 and each loaded value takes r2, which is the
 *     reference's assignment.  Confirmed in the dumps: with one temp
 *     `.17.lreg` has `Register 38 in 3` (the value) and `.18.greg` then
 *     assigns `35 in 2` (the temp); the reference is the other way round.
 *
 * MEASURED EXACTLY INERT, at every stage: the statement order at either AND
 * site (all four crossings of "load first" against "copy first" read 8), a
 * declared `unsigned short` for the loaded value, and a separate loaded-value
 * variable for the loop as well as a separate temp (`u`/`w` both reach 0, so
 * the second variable is not a prerequisite -- only the second TEMP is).
 */
#include "gba/types.h"
#include "gba/io.h"

extern volatile u16 iwram_3001f64;
extern void WaitFrames(int n);

int Func_8006384(int mask)
{
    int m;
    int v;
    int t;
    int u;
    u32 x;

    m = mask;
    v = iwram_3001f64;
    t = m;
    t &= v;
    if (t != m) {
        do {
            WaitFrames(1);
            v = iwram_3001f64;
            u = m;
            u &= v;
        } while (u != m);
    }
    x = *(vu32 *)&REG_SIOCNT;
    return (x << 26) >> 30;
}
