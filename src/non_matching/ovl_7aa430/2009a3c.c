/* OvlFunc_923_2009a3c -- NON-MATCHING, PARKED AT **5 DIFFERING ENCODINGS OF
 * 179** (stream terms: 179 encodings = 159 16-bit instructions + 20 32-bit
 * entries).  RE-DERIVED AT THE HEAD OF BATCH 337 AND IT HOLDS, for the fourth
 * time: size exact, INSTRUCTION COUNT EXACT (159 = 159), relocations identical,
 * first differing index 54 (ref 4640, ours 189b).  So this IS a distance, and
 * the length-gap criterion has no purchase on it.  Pin-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7aa430/2009a3c.c \
 *     asm/overlays/rom_7aa430/ovl_1a3c_a_a_a_a.s --func OvlFunc_923_2009a3c
 *
 * ***** READ THE BATCH 337 SECTION AT THE END OF THIS FILE FIRST. *****
 * The park's route (a) is no longer a proposal: it is PROVEN TO WORK, the cse1
 * operand swap is CLOSED, and the residue is now ONE named quantity.  Four of
 * the five differences in the header below are no longer the obstacle.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 5 of 179, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: all 41 statement boundaries at every brace depth
 *   measured, all inert or worse.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
/* OvlFunc_923_2009a3c -- candidate produced in batch 316 by PORTING the body
 * of OvlFunc_924_200cfcc, which tools/dupfuncs.py reports as a BYTE-IDENTICAL
 * DUPLICATE of this function.
 *
 * **5 ENCODINGS OF 179, DOWN FROM 181.**  Instruction count exact
 * (179 = 179), size exact, pool identical, all relocations identical.
 * tools/shimcount.py: ZERO register pins, so no fakematch.txt row is needed.
 * Production flags, no per-file Makefile adjustment (checked).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7aa430/2009a3c.c \
 *     asm/overlays/rom_7aa430/ovl_1a3c_a_a_a_a.s --func OvlFunc_923_2009a3c
 *   XX ENCODINGS differ in 5 place(s) (ref 179, ours 179)
 *      first at index 54: ref 4640  ours 189b
 *
 * HISTORY -- THE SUPERSEDED BODY.  THE FIGURES IN THIS PARAGRAPH ARE DEAD.
 * Batch 316 replaced this park wholesale.  The body it replaced was a
 * different, SATURATED one at 181 of 177, with 172 differing instructions at
 * nins 169/175 and +12 bytes, and its recorded blocker was "one allocno too
 * many, PLUS an unsolved pooled-zero construct".  None of that describes the
 * body in this file, whose figure is the 5 of 179 stated above.
 *
 * This paragraph is dated on purpose.  It previously read as a PROPOSAL --
 * "WHY THIS REPLACES THE EXISTING PARK WHOLESALE", describing "the installed
 * park" in the third person -- which was true when it was written and became
 * self-referential the moment the port was installed, because this file then
 * BECAME the installed park.  A reader scanning for a figure met "181 of 177
 * -- SATURATED" in a sentence that parsed as current state.  parkcheck never
 * flagged it: it takes the FIRST claim match, which is the correct 5.  Batch
 * 329 brief A read the 181 and reported a "free improvement 181 -> 5" that
 * batch 316 had already banked.  When a port is installed, rewrite its
 * proposal prose into the past tense -- see docs/elevation.md, "prose that
 * looks like machinery".
 *
 * The port is THREE RENAMES and nothing else:
 *   OvlFunc_924_200cfcc    -> OvlFunc_923_2009a3c
 *   gScript_924__0200de20  -> gScript_923__0200a7d0
 *   gScript_924__0200de38  -> gScript_923__0200a7e8
 * (offset order fixes the script pairing: de20 < de38 and a7d0 < a7e8).  The
 * reference's `=` operands are otherwise identical to 200cfcc's: 0x109,
 * 0x85000007, 0xffff8000, 0xfffff, REG_DMA3SAD, gBuffer, gState.
 *
 * THE REMAINING 5 ARE 200cfcc's RESIDUE, IDENTICALLY -- same count, same first
 * index 54.  See the corrected header of
 * src/non_matching/ovl_7ac2d8/200cfcc.c for the mechanism: cse.c:3652's
 * commutative canonicalisation puts gBuffer's register SECOND in the PLUS, and
 * reload.c's find_dummy_reload then takes its reload register from
 * XEXP (plus, 0).  Whatever closes 200cfcc closes this, and vice versa
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned char gBuffer[];
extern unsigned char gScript_923__0200a7d0[];
extern unsigned char gScript_923__0200a7e8[];

extern void *__galloc_ewram(int tag, int size);
extern int __GetFlag(int id);
extern unsigned char *__GetFieldActor(int id);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);

void OvlFunc_923_2009a3c(int a, unsigned char *c)
{
    unsigned char *t;
    unsigned char *n;
    unsigned char *s;
    unsigned char *cell;
    unsigned int g;
    int k;
    int v;
    unsigned char *u;
    int w;
    int y;

    *(unsigned char **)__galloc_ewram(0x23, 4) = c;
    if (__GetFlag(0x109) == 0) {
        DMA3_CLEAR(c, 0x1c);
        *(int *)(c + 4) = a;
        return;
    }
    g = (unsigned int)&gState;
    k = 0xfa;
    k <<= 1;
    g += k;
    t = __GetFieldActor(*(int *)g);
    cell = gBuffer + (((*(int *)(t + 0x10) / 0x100000) * 0x80
                       + *(int *)(t + 8) / 0x100000) << 2);
    if (*(int *)c != 0 && *(int *)(c + 0x14) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8),
                          *(int *)(t + 0xc) + (0xc0 << 13),
                          *(int *)(t + 0x10));
        if (n != 0) {
            *(int *)(n + 0x14) = *(int *)(t + 0x14);
            s = *(unsigned char **)(n + 0x50);
            __Actor_SetScript(n, gScript_923__0200a7e8);
            *(unsigned char **)(n + 0x68) = t;
            v = 4;
            n[0x55] = v;
            *(int *)(n + 0xc) += 0xffff8000;
            if (s != 0) {
                __Sprite_SetAnim(s, 6 - *(int *)c);
                u = s + 0x26;
                w = 0;
                *u = w;
                w -= 0xd;
                s[9] = (w & s[9]) | 4;
            }
            *(unsigned char **)(c + 0x14) = n;
        }
    } else {
        *(unsigned char **)(c + 0x14) = 0;
    }
    if (cell[2] == a && *(int *)(c + 0x18) != 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8), *(int *)(t + 0xc),
                          *(int *)(t + 0x10));
        if (n == 0)
            return;
        *(int *)(n + 0x14) = *(int *)(t + 0x14);
        s = *(unsigned char **)(n + 0x50);
        __Actor_SetScript(n, gScript_923__0200a7d0);
        y = 0;
        n[0x55] = y;
        *(short *)(n + 0x64) = y;
        n[0x23] = 2;
        *(int *)(n + 0x30) = 0x80 << 11;
        if (s != 0) {
            __Sprite_SetAnim(s, 6);
            s[0x26] = 0;
        }
        *(unsigned char **)(c + 0x18) = n;
    } else {
        *(unsigned char **)(c + 0x18) = 0;
    }
}

/* ===================== BATCH 337 (brief C) =====================
 *
 * FIGURE: 5 DIFFERING ENCODINGS OF 179 (159 instructions against 159, 20
 * 32-bit entries each, size exact, relocations identical, first index 54).
 * HELD.  Pin-free.  The twins still read the same figure at the same index, so
 * whatever closes one closes the other; nothing about that changed.
 *
 * ***** THE cse1 OPERAND SWAP IS NOW PROVEN FROM THE DUMPS, NOT ARGUED. *****
 *   .00.rtl  insn 118: (set (reg/v:SI 37) (plus (reg:SI 67) (reg:SI 66)))
 *   .03.cse  insn 118: (set (reg/v:SI 37) (plus (reg:SI 66) (reg:SI 67)))
 * where 67 is the gBuffer pool load (REG_EQUAL symbol_ref "gBuffer") and 66 is
 * the index.  **EXPAND EMITS gBuffer FIRST -- THE ROM'S OWN ORDER -- AND cse1
 * SWAPS IT.**  The deciding code, quoted so the next reader can check it in one
 * command, is the canonicalisation at the end of fold_rtx, cse.c:3649-3663:
 *       if (must_swap || (const_arg0
 *                         && (const_arg1 == 0
 *                             || (GET_CODE (const_arg0) == CONST_INT
 *                                 && GET_CODE (const_arg1) != CONST_INT))))
 *           ... swap XEXP (x, 0) and XEXP (x, 1)
 * const_arg0 comes from the REG fast path at cse.c:3534-3547, which takes the
 * qty's const_rtx unless it is a REG or a PLUS.  So gBuffer-first swaps
 * (const_arg0 = the symbol_ref, const_arg1 = 0) and index-first does not swap
 * (const_arg0 = 0) -- BOTH LAND ON INDEX-FIRST, which is why both source orders
 * measured inert and why no expand-time lever can ever reach this.
 *
 * ***** ROUTE (a) WORKS.  BREAKING THE CONSTANT EQUIVALENCE IS ENOUGH. *****
 * A volatile asm whose PATTERN is a bare ASM_OPERANDS flushes the cse table --
 * cse.c:5741-5745 tests GET_CODE (PATTERN (insn)) == ASM_OPERANDS and calls
 * flush_hash_table (cse.c:1726), which `invalidate`s every REG entry, so
 * REGNO_QTY_VALID_P fails and equiv_constant (cse.c:4105) returns 0.  With the
 * barrier BETWEEN the pool load and the add, const_arg0 is 0, NO SWAP HAPPENS,
 * and `.18.greg` reads
 *       (insn 122 (set (reg/v:SI 2 r2) (plus (reg/v:SI 2 r2) (reg/v:SI 3 r3))))
 *       (insn 450 (set (reg/v:SI 10 sl) (reg/v:SI 2 r2)))
 * which is THE ROM'S `add r2, r2, r3` / `mov r10, r2` EXACTLY.  Measured body:
 *       idx = (A) * 0x80 + (B);
 *       base = gBuffer;
 *       __asm__ volatile ("" :: "i"(0));
 *       cell = base + (idx << 2);
 * reads **5 of 179 with the whole gBuffer hunk byte-identical** -- indices 52,
 * 53, 55 and 57 all come right and the figure only stays at 5 because the
 * residue moved.  IT IS KEPT AS AN INSTRUMENT, NOT SHIPPED: tools/shimcount.py
 * calls a bare ASM_OPERANDS a fakematch-class shim, and the pin policy prefers
 * the shim-free body at the same figure.  The one-line change is in
 * reports/batch-337.md if anyone wants it back.
 *
 * ***** WHAT THE RESIDUE ACTUALLY IS, AND IT IS ONE QUANTITY. *****
 * With the swap gone, all five remaining differences are the SAME fact:
 *       ref   mov r0, r8 / add r2, r2, r3 / ldr r3, [r0] / mov sl, r2 ...
 *                                                       ... ldr r3, [r0, #0x14]
 *       ours  add r2, r2, r3 / mov sl, r2 / mov r2, r8 / ldr r3, [r2] ...
 *                                                       ... ldr r3, [r2, #0x14]
 * THE LOW REGISTER THE `c` BASE ADDRESS RELOAD TAKES: r0 in the ROM, r2 here.
 * Its position (one slot later) follows by anti-dependence -- r2 is still
 * holding `cell` until `mov sl, r2`, so sched2 cannot lift the copy above the
 * add; with r0 it could, and the whole window would fall into place.
 *
 * ***** AND THIS ANSWERS THE BATCH-329 "SHARP PROBLEM" IN THIS PARK. *****
 * 329 asked how the round robin can diverge when encodings 0-53 are identical,
 * and offered "either the number of RELOADS differs while the emitted code does
 * not, or this copy is not chosen by the round robin at all".  IT IS THE FIRST,
 * and the signature is visible in the dump.  `.18.greg` prints
 *       Spilling for insn 118.                 <- NO "Using reg N for reload" line
 *       Spilling for insn 121.
 *       Using reg 2 for reload 0
 * A reload with no "Using reg" line never went through allocate_reload_reg
 * (reload1.c:4962, round robin from last_spill_reg at :5003-5013 over
 * spill_regs, which finish_spills builds in ASCENDING REGNO at :3527-3532).
 * `last_spill_reg` is set only on success in set_reload_reg, SO A DUMMY OR
 * INHERITED RELOAD EMITS NOTHING *AND DOES NOT ADVANCE THE COUNTER*.  Insn
 * 118's reload is exactly that -- find_dummy_reload (called at reload.c:1404
 * only when `in != 0 && out != 0`) takes the IN operand, which is the PLUS's
 * FIRST operand, which is why the add's destination tracks the operand order at
 * all.  The ROM's equivalent reload must therefore have ADVANCED the counter
 * once more than ours, which puts insn 121 one step further round and lands it
 * on r0.  **So the next door is: make insn 118's reload an ALLOCATED reload
 * rather than a dummy, without adding an instruction.** find_dummy_reload's IN
 * clause needs `find_reg_note (this_insn, REG_DEAD, real_in)`, so that is the
 * park's old route (b) -- "stop the index dying at the add" -- now with a
 * reason to want it that is about the PHASE rather than about the operand
 * order, and with the operand order already solved.
 *
 * MEASURED THIS BATCH (all against the 200cfcc reference; the twin reads the
 * same).  All at exact length and relocation-clean unless noted:
 *   base (this body)                                                 5  @54
 *   barrier before the whole `cell = gBuffer + ...` statement         5  @54
 *     (INERT BY CONSTRUCTION: the pool load is then emitted after the
 *      flush, so cse re-records the constant and swaps again)
 *   idx named + base named, no barrier                                5  @54
 *   idx named alone                                                   5  @54
 *   idx named + barrier, gBuffer inline in the add                    7  @52
 *   idx + base + bare `__asm__ volatile ("")`   (ASM_INPUT, no flush) 7  @52
 *   idx + base + `::: "memory"`                 (PARALLEL, no flush)  130 @11  RELOCDIFF
 *   idx + base + `:: "i"(0)`                    (ASM_OPERANDS, FLUSH) 7  @52
 *   the same with the shift AFTER the barrier (the shipped-shape test) 5  @54
 *      *** and THIS one has the whole gBuffer hunk right; see above ***
 *   the same + a second barrier after the add                         7  @52
 *   the same + `base += idx; cell = base;`                            7  @52
 *   the same + `idx <<= 2;` in place                                  7  @52
 *   the same + `unsigned int` idx                                     7  @52
 *   the same + `cell = &base[idx];`                                   7  @52
 *   base named BEFORE idx (pool load hoisted)              33/34 @26  RELOCDIFF
 *   declaration order of idx/base at the head / at the tail of the
 *     declaration list                                                7  @52
 *   the two `c` tests through a named local pointer                    7  @52
 *   `base` kept live past the add via `base[idx + 2]` later        180 @1   +8 bytes
 *
 * NOTE FOR THE LEDGER: the batch-329 note in this file cites
 * "allocate_reload_reg (:4962) walks i = last_spill_reg; i++ round robin" and
 * that citation is CORRECT -- it was checked line by line this batch.  Do not
 * confuse it with find_reg (reload1.c:1588), which is the cost walk that picks
 * which hard register to SPILL and tie-breaks on inv_reg_alloc_order; that one
 * is a different function and a different question.
 */
