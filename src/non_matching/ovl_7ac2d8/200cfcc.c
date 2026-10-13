/* OvlFunc_924_200cfcc -- NON-MATCHING, PARKED AT **5 DIFFERING ENCODINGS OF
 * 179** (stream terms: 179 encodings = 159 16-bit instructions + 20 32-bit
 * entries).  RE-DERIVED AT THE HEAD OF BATCH 337 AND IT HOLDS, for the fourth
 * time: size exact, INSTRUCTION COUNT EXACT (159 = 159), relocations identical,
 * first differing index 54 (ref 4640, ours 189b).  So this IS a distance, and
 * the length-gap criterion has no purchase on it.  Pin-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ac2d8/200cfcc.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s --func OvlFunc_924_200cfcc
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
 *   residue.  ANCHOR AXIS: 41 placements swept, but 26 WERE BIT-IDENTICAL to
 *   the base, so only 15 DISTINCT PROGRAMS were actually tested.
 *   Those 15 are inert; the other 26 measured nothing.  No variant was capped and none failed to
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
/* OvlFunc_924_200cfcc  --  overlay 924, 168 instructions.  NON-MATCHING,
 * PARKED AT 5 ENCODINGS OF 179, and this IS a true distance: SIZE EXACT,
 * INSTRUCTION COUNT EXACT (179 = 179), all relocations identical.
 * Production flags, no per-file Makefile adjustment (checked).
 * tools/shimcount.py: ZERO register pins in this file (include/dma.h's
 * DMA3_SET contributes four of its own).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ac2d8/200cfcc.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s --func OvlFunc_924_200cfcc
 *   XX ENCODINGS differ in 5 place(s) (ref 179, ours 179)
 *      first at index 54
 *
 * SPLIT SHAPE.  TWO functions in the reference (this one and
 * OvlFunc_924_200d158), target FIRST, so a TEXT split with no _a part.
 * tools/datacheck.py prints nothing (no data section).  EXPORT LIST: EMPTY.
 *
 * ***** BATCH 316: THE DUPLICATE TWIN IS SOLVED BY PORTING THIS BODY *****
 *
 * tools/dupfuncs.py pairs this with OvlFunc_923_2009a3c
 * (asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s).  The twin park
 * src/non_matching/ovl_7aa430/2009a3c.c carries a DIFFERENT and much worse
 * body -- its own header says "181 of 177 -- SATURATED", figure withdrawn.
 * MEASURED THIS BATCH: this body, with only three renames
 *   OvlFunc_924_200cfcc    -> OvlFunc_923_2009a3c
 *   gScript_924__0200de20  -> gScript_923__0200a7d0
 *   gScript_924__0200de38  -> gScript_923__0200a7e8
 * reads **5 of 179 against the twin's own reference**, same first index 54,
 * identical pool and relocations -- i.e. 181 -> 5 for free.  The candidate is
 * docs/repro-200cfcc-twin/p2twin_port.c.  The twin does NOT need its own recipe
 * continued; it needs this body installed.
 *
 * ================== BATCH 316 CORRECTION TO THE BLOCKER ==================
 *
 * The park named "local-alloc.c:1131 ties the output to the FIRST input it can
 * combine with".  THAT PASS IS NOT INVOLVED.  Read from the dumps:
 *
 *   `.17.lreg` insn 118 is
 *       (set (reg/v:SI 37) (plus:SI (reg:SI 66) (reg:SI 67)))
 *       REG_DEAD 66, REG_DEAD 67
 *   where 66 = idx<<2, 67 = the gBuffer pool load (REG_EQUIV symbol_ref
 *   "gBuffer"), and 37 = `cell`.  `.18.greg`'s dispositions put **37 in 10**
 *   -- `cell` is a GLOBAL allocno in sl.  local-alloc never saw it, so
 *   local-alloc.c:1131's tying cannot be the mechanism.
 *
 * WHAT ACTUALLY DECIDES IT is reload.c's **find_dummy_reload**.  sl is not in
 * class `l`, so operand 0 of *thumb_addsi3 needs a reload; the matched
 * alternative ties operand 1 to operand 0 (`%0`), so reload calls
 * find_dummy_reload(in = operand 1, out = reg 10 sl).  It first tries OUT,
 * which fails the class test, then takes IN because
 * `find_reg_note (this_insn, REG_DEAD, real_in)` holds -- and IN is
 * **XEXP (plus, 0)**.  So rd follows the PLUS's FIRST operand, which is the
 * brief's lever 6, and `.19.flow2` shows the consequence directly:
 *     insn 118: (set (reg:SI 3 r3) (plus (reg 3) (reg 2)))
 *     insn 444: (set (reg/v:SI 10 sl) (reg:SI 3 r3))     <- reload-created
 *     insn 447: (set (reg:SI 2 r2) (reg/v:SI 8 r8))      <- reload-created
 * The ROM has the two reload-created copies the other way round, because its
 * PLUS reads (plus gBuffer idx) and ours reads (plus idx gBuffer).
 *
 * THE cse1 HALF OF THE PARK'S ANALYSIS STANDS: cse.c:3652's commutative
 * canonicalisation in fold_rtx moves the operand whose value is a known
 * constant -- a SYMBOL_REF counts, via reg 67's REG_EQUIV -- to XEXP (x, 1),
 * so gBuffer arrives at reload SECOND whichever way the source writes it.
 * That is why both source orders measured inert.  The CORRECTION is only to
 * the second half: the order is converted into a register by find_dummy_reload
 * in reload.c, not by local-alloc.
 *
 * CONSEQUENCE FOR THE NEXT READER.  Two routes remain, and they are different
 * from everything in the park's inert list:
 *   (a) break reg 67's constant equivalence so fold_rtx has no `const_arg` to
 *       swap, WITHOUT moving the pool load (every attempt so far moved it:
 *       named base local 21, "+r" barrier 33, r2 pin 33, `cell = gBuffer;
 *       cell += ...` 8);
 *   (b) make the INDEX not die at the add, so find_dummy_reload's IN clause
 *       (`REG_DEAD real_in`) fails and the choice falls through to
 *       allocate_reload_reg.  No spelling for (b) was found that does not add
 *       an instruction, and it is untested.
 *
 * MEASURED INERT / WORSE, BATCH 316: flag sweep of 29 flags -- nothing helps
 * (-fno-gcse 5, -fno-strict-aliasing 5, -fno-rerun-cse-after-loop 15,
 * -fno-regmove 11, -fno-force-mem 11, -fno-schedule-insns2 34,
 * -fno-expensive-optimizations 171).  Both `extern void` callees
 * (__Actor_SetScript, __Sprite_SetAnim) -> `extern int`: INERT at 5
 *
 *
 * ===================== BATCH 329 (brief A) =====================
 *
 * FIGURE RE-DERIVED A THIRD TIME AND IT HOLDS: 5 DIFFERING ENCODINGS OF 179,
 * size exact, instruction count exact, first differing index 54 (ref 4640,
 * ours 189b).  The whole-TU run agrees exactly (5 of 179, same index).
 *
 * THE TWO TOOLS RECONCILE, AND THE DIFFERENCE IS THE TAIL PAD -- WORTH KNOWING
 * BEFORE ANYONE "FINDS" A SIXTH DIFFERENCE.  aligncmp reports 6 differing in 3
 * hunks; objcmp reports 5.  The accounting is exact:
 *     ref[54:58]  4 encodings   the add / the r8 copy / the sl park
 *     ref[60:61]  1 encoding    ldr r3, [r0, #20]  vs  ldr r3, [r2, #20]
 *     ref[178]    1 encoding    ref .short 0x0000  vs  ours nop (mov r8, r8)
 * The third is the FUNCTION-TAIL ALIGNMENT FILL, and objcmp deliberately does
 * not count it.  So the real distance is 5 code encodings in TWO hunks, and
 * every one of them is in one place.  Do not chase index 178.
 *
 * ROUTE (c) REPRODUCED INDEPENDENTLY, AND IT SHRINKS THE RESIDUE TO ONE FACT.
 *     { int n4 = -((((t[0x10] term) * 0x80 + (t[8] term)) << 2));
 *       cell = gBuffer - n4; }
 * reads 5 of 179 with first index 54 == 18d2, i.e. the ROM's own
 * `adds r2, r2, r3`.  Behind it the 5 is NOT five things, it is ONE:
 *     rom   mov r0, r8 / ... / ldr r3, [r0, #0] / ... / ldr r3, [r0, #20]
 *     ours  mov r2, r8 / ... / ldr r3, [r2, #0] / ... / ldr r3, [r2, #20]
 * The copy of `t` out of r8 into a low base for the Thumb loads goes to r0 in
 * the ROM and r2 here, and it also sits two slots later.  That is the entire
 * remaining residue of this function.  Route (c) stays an INSTRUMENT, not a
 * shipped body -- a negated index subtracted from a base is a matching artifact
 * and belongs to pass 4, not to pass 2 -- but it isolates the blocker cleanly
 * and it refutes "the add order is the blocker": the add order is reachable.
 *
 * THE BLOCKER IS NOW NAMED, AND IT IS SHARED WITH THIS BANK'S OTHER TWO PARKS.
 * WHICH LOW REGISTER A THUMB BASE COPY / RELOAD PICKS.  Read from reload1.c:
 * allocate_reload_reg (:4962) walks `i = last_spill_reg; i++` round robin over
 * spill_regs (:5003-5013), and last_spill_reg is set only on success in
 * set_reload_reg (:4937) and reset ONCE PER FUNCTION (:821).  So the register
 * is a FUNCTION-SCOPED PHASE COUNTER, not a local decision.
 *   NOTE THE SHARP PROBLEM THIS RAISES HERE, because it bounds the next round:
 *   encodings 0-53 are BYTE-IDENTICAL on both sides, so if the phase were the
 *   whole story the phase entering index 54 would be identical too.  Either the
 *   number of RELOADS differs while the emitted code does not (an inherited
 *   reload emits nothing and still advances the counter), or this copy is not
 *   chosen by the round robin at all but by find_dummy_reload, which the batch
 *   316 correction already showed owns the other half of this hunk.
 *   ANSWERING THAT QUESTION IS THE NEXT ROUND.  Read the reload dumps, do not
 *   guess, and do not re-run the expand-time levers: they are closed.
 *
 * THE DUPLICATE PORT IS NOW PROVEN TWO WAYS, NOT ONE.
 *   (1) STRUCTURAL.  A normalised diff of
 *       asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s (lines 1-197) against
 *       asm/overlays/rom_7aa430/ovl_1a3c_a_a_a_a.s     (lines 1-197)
 *       -- strip the banner comments and blanks, canonicalise every .L<hex>
 *       label in order of first appearance, canonicalise every bank-qualified
 *       OvlFunc/gScript name -- is EMPTY, 185 significant lines each.
 *   (2) MEASURED.  This body with the three renames below compiles against the
 *       TWIN'S OWN reference to 5 differing encodings of 179, first index 54,
 *       relocations identical.  Same figure, same index, nothing to re-derive.
 *       OvlFunc_924_200cfcc   -> OvlFunc_923_2009a3c
 *       gScript_924__0200de20 -> gScript_923__0200a7d0
 *       gScript_924__0200de38 -> gScript_923__0200a7e8
 * The twin park src/non_matching/ovl_7aa430/2009a3c.c still carries its own
 * withdrawn "181 of 177 SATURATED" body.  IT IS OWED THIS ONE, at 5.
 *
 * Verify with: python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200cfcc.c asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s --func OvlFunc_924_200cfcc
 */
#include "dma.h"

extern unsigned char gState[];
extern unsigned char gBuffer[];
extern unsigned char gScript_924__0200de20[];
extern unsigned char gScript_924__0200de38[];

extern void *__galloc_ewram(int tag, int size);
extern int __GetFlag(int id);
extern unsigned char *__GetFieldActor(int id);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);

void OvlFunc_924_200cfcc(int a, unsigned char *c)
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
            __Actor_SetScript(n, gScript_924__0200de38);
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
        __Actor_SetScript(n, gScript_924__0200de20);
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

/* ===================== BATCH 327 =====================
 *
 * FIGURE RE-DERIVED AND THE PARK IS CORRECT IN EVERY PARTICULAR:
 *   5 differing encodings of 179, SIZE EXACT, INSTRUCTION COUNT EXACT
 *   (179 = 179), first differing index 54 (ref 4640, ours 189b).
 *   --whole differs ONLY by the missing second function (the split shape the
 *   header describes), and the 13 relocations belonging to THIS function are
 *   byte-for-byte identical to the reference's.
 * This is one of the very few parks this session whose figure AND diagnosis both
 * survived re-measurement.  The batch-316 correction (find_dummy_reload in
 * reload.c, not local-alloc.c:1131) is confirmed by reading the same code.
 *
 * NEW NEGATIVE, AND IT CLOSES A LEVER THAT POSTDATES THIS PARK.  Batch 326
 * landed Func_801219c on expr.c:7340 -- "put a constant term last and put a
 * multiplication first" -- reached only when the modifier is EXPAND_SUM and
 * `mode == ptr_mode` (expr.c:7290-7292), which an ARRAY_REF address satisfies,
 * so expr.c:7324 puts the MULT first unconditionally inside a subscript while a
 * plain `p + E` falls to `goto binop` and keeps the written order.  That is
 * precisely a lever on the PLUS operand order this park needs, and it was not
 * available when this park was last worked.
 *
 *   TRIED, THREE SUBSCRIPT SPELLINGS OF `cell`, ALL EXACTLY INERT AT 5 of 179
 *   WITH THE SAME FIRST INDEX 54:
 *     cell = &gBuffer[(A * 0x80 + B) << 2];
 *     cell = &gBuffer[(A * 0x80 + B) * 4];
 *     cell = &gBuffer[A * 0x200 + B * 4];
 *
 * WHY, AND THIS STRENGTHENS THE PARK RATHER THAN WEAKENING IT: the park's cse1
 * half already explains it.  cse.c:3652's commutative canonicalisation in
 * fold_rtx moves the operand with a known constant value -- a SYMBOL_REF
 * qualifies, through reg 67's REG_EQUIV -- to XEXP (x, 1).  cse runs AFTER
 * expand, so WHATEVER ORDER expand CHOOSES IS UNDONE.  An expand-time lever can
 * therefore never reach this blocker, and the park's route (a) ("break reg 67's
 * constant equivalence") is not just one option among several: IT IS THE ONLY
 * ROUTE THAT CAN WORK AT ALL ON THE OPERAND ORDER.  Route (b) (stop the index
 * dying at the add, so find_dummy_reload's `REG_DEAD real_in` clause fails)
 * remains untested and is now the only other door.
 *
 * THE WHOLE PIECE IS THE RIGHT UNIT -- THIS IS THE CHEAPEST STRUCTURE IN THE BANK.
 * The other function in asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s is
 * OvlFunc_924_200d158, which is ALSO parked (7 of 40,
 * src/non_matching/ovl_7ac2d8/200d158.c).  tools/dupfuncs.py pairs THIS function
 * with OvlFunc_923_2009a3c and that one with OvlFunc_923_2009bc8, and both
 * duplicates live in asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_a.s -- 244 lines, two
 * functions, same order, same lengths as our piece.  So:
 *   - solving BOTH functions in ONE .c is a whole-piece match with NO SPLIT;
 *   - the same .c with three symbol renames covers the foreign piece too;
 *   - FOUR functions from one translation unit.
 * `ovl_1a3c` copies more of `ovl_35b8` than these two: 200d244 = 2009cb4 and
 * 200d5c0 = 200a030 pair the same way and are both parked here as well.  When
 * this bank is next worked, work it by PIECE.
 *
 * ON THE BANK ITSELF, because a brief was told the opposite: ovl_7ac2d8 has
 * **72 landed .c files / 80 landed OvlFunc_924_* definitions** against 12 parks.
 * tools/upstream_module.py puts this function in upstream `overlays/ovl_35b8.s`,
 * which alone has **18 landed .c** siblings.  It reports `our parks: 0` for the
 * module only because parks are filed as `src/non_matching/<bank>/<addr>.c`
 * while landings are split-named `src/overlays/<bank>/ovl_XXXX_<path>.c`, so a
 * FILENAME scan of either directory sees none of the other.  A name check is not
 * a definition check.
 */

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
