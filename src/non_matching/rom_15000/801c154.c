/* Func_801c154  @  0x0801c154  [rom_15000]   *** DOES NOT LAND -- 8 of 17 ***
 * Declining to close, with a corpus-wide structural result that narrows the
 * blocker from "OPEN" to one named precondition this function cannot satisfy.
 *
 * Source asm: goldensun/asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s
 *   BOTH parks cite asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a.s, which no longer
 *   exists -- the file has been split once more since they were written.
 *
 * FIGURE (measured here; NEITHER park quoted an objcmp figure):
 *   objcmp --func : 8 of 17 encodings differ (ours 15), 40 bytes against 36
 *   objcmp --whole: same 8 of 17; RELOCATIONS IDENTICAL, so the 8 IS a distance.
 *   Both parks claim "13 lines against 15". tryc reads rom 15 / ours 13 lines,
 *   which is where that came from -- but one of the ROM's 15 "lines" is a LABEL,
 *   not an instruction. The honest count is 14 ROM instructions against our 13.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/rom_1c154.c \
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s --func Func_801c154
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   python3 tools/datacheck.py asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s -> clean
 *   The .s holds exactly ONE thumb_func_start. rom_1c154.c's header still says
 *   "NOT SPLIT. The .s still holds both of its functions and the linker script
 *   is untouched" -- that is STALE; the split has already happened.
 *   INSTALL PATH if it ever lands: src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.c
 *
 * PIN COUNT: 0.
 *
 * TWO PARKS, ONE SUBJECT. src/non_matching/rom_15000/rom_1c154.c and
 * src/non_matching/rom_15000/801c154.c BOTH define `void Func_801c154(...)`.
 * They are duplicate parks, not a mis-naming; rom_1c154.c is the later and
 * better one and its body is reproduced below. 801c154.c's citation of
 * asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s is a cross-reference to Func_80b09fc,
 * as the brief said. THE TWO PARKS CONTRADICT EACH OTHER on the blocker, and
 * the measurement below settles it against 801c154.c.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE RESIDUE IS: exactly two things, and the second is the real one.
 *
 *   1. A TWO-REGISTER RENAME in the first four instructions. The ROM keeps the
 *      mask in r3 and the loaded halfword in r4; we use r4 and r3.
 *          rom  ldr r3, =0x1ff / ldrh r4, [r0,#6] / and r1, r3 / ldr r3, =0xfffffe00
 *          ours ldr r4, =0x1ff / ldrh r3, [r0,#6] / and r1, r4 / ldr r4, =0xfffffe00
 *      The remaining nine body instructions, `and r3, r4` through `bl`, are
 *      exact. Both parks report this and both call it secondary; that is right.
 *
 *   2. THE LITERAL POOL SITS BEFORE THE EPILOGUE, BEHIND A BRANCH:
 *          rom   bl Func_8003dec / b .L1c178 / <pool> / .L1c178: pop {r0} / bx r0
 *          ours  bl Func_8003dec / pop {r0} / bx r0 / <pool>
 *      That `b` is a real instruction and the pool's 4-byte alignment adds a
 *      padding halfword, which is the whole 4-byte size deficit and 2 of the 8
 *      differing encodings; the rest is misalignment downstream of it.
 *      THE POOL CONTENTS ARE ALREADY EXACT -- gcc emits `.word 511 / .word -512`
 *      in the ROM's order. Only the PLACEMENT differs.
 *
 * 801c154.c's CORRECTION IS ITSELF WRONG. It says, in a block marked
 * "*** CORRECTED, SAME BATCH ***": "POOL PLACEMENT IS NOT A RESIDUE AT ALL ...
 * The pool branch is a consequence of CODE LENGTH, not of a source construct.
 * So on this function the missing `b` is not the blocker ... `.pool_aligned` in
 * a reference is neither a ceiling nor a signal. Screen the class normally."
 * On this function the placement IS the residue -- it is 2 of the 8 encodings
 * and all 4 bytes -- and it is NOT only a function of code length. There is a
 * second, measurable discriminator, and this function fails it.
 *
 * ---------------------------------------------------------------------------
 * THE CORPUS RESULT (new; this is the part worth more than the function)
 *
 * I scanned all 4,469 tracked .s files in asm/ that carry gcc's own banner and
 * partitioned every function that has a literal pool by (a) whether the pool is
 * dumped BEFORE the function's final `bx` and (b) whether any of its
 * PC-relative loads is a NARROW-MODE load (gcc writes `ldrh`/`ldrb rX, .LN`;
 * Thumb-1 has no PC-relative halfword load, so these assemble to the SAME
 * ENCODING as `ldr` and are INVISIBLE in any ROM disassembly -- the brief's own
 * trap, here load-bearing):
 *
 *     3,088 functions have a pool.  299 dump it before the epilogue, 2,789 after.
 *
 *                              pool BEFORE   pool AFTER
 *       has a narrow pool load      189           20
 *       only `ldr` pool loads       110        2,769
 *
 * A narrow pool load gives pool-before 90% of the time; without one it happens
 * 3.8% of the time. And the size cut is sharp:
 *
 *   *** OF THE 110 POOL-BEFORE FUNCTIONS WITH ONLY `ldr` POOL LOADS, THE
 *       SMALLEST IS 21 INSTRUCTIONS. THERE IS NOT ONE EXAMPLE BELOW 21 IN THE
 *       WHOLE TREE. ***
 *
 *   With a narrow pool load they go down to 11 instructions:
 *       11 insns, 2 words  OvlFunc_924_2008dfc  asm/overlays/rom_7ac2d8/ovl_d58_b.s
 *       11 insns, 1 word   StartLuckyDice       asm/rom_f4000/rom_f4008_a_a_b.s
 *       14 insns, 1 word   Func_80b09fc         asm/rom_b0000/rom_b0070_a_a_c_c_a_a_b.s
 *       14 insns, 2 words  Func_8006358         asm/rom_c0/rom_5cf8_a_a_b.s
 *
 * Func_801c154 is 14 ROM instructions. So the corpus says its pool-before shape
 * needs a narrow-mode pool load, and the two landed 14-instruction twins show
 * exactly where one comes from -- a NARROW STORE OF A CONSTANT, where gcc-2.96
 * has no movhi/movqi alternative taking a CONST_INT and calls force_const_mem:
 *     Func_8006358  `iwram_3001cb0 = 0;` (volatile u16 global)
 *                   -> ldrh r3, .L3 / strh r3, [r2]      + b over the pool
 *     Func_80b09fc  `a->fc = 0;` (u8 struct member)
 *                   -> ldrh r6, .L3 / strb r6, [r0,#12]  + b over the pool
 * Both are SINGLE-FUNCTION translation units, which independently confirms
 * rom_1c154.c's refutation of the TU-context theory: pool-before happens with
 * nothing before or after the function.
 *
 * I ALSO OBSERVED THE MECHANISM FIRE LIVE, in this same brief, on an unrelated
 * target. While sweeping Func_80270ac, the spelling `unsigned short *h = &s.a;
 * *h = 0xff;` turned `mov r3, #0xff` into a POOLED HImode constant and gcc
 * immediately emitted `ldr r3, =0xff ... b .L0 / <pool> / .L0: pop` -- the
 * branch-over-pool shape appearing in a 17-instruction function the moment a
 * narrow constant store existed, and vanishing when it did not. That is a
 * controlled before/after on the discriminator, not just a correlation.
 *
 * WHY THIS FUNCTION CANNOT SATISFY IT. Func_801c154's two pool words are its two
 * masks. For gcc to load them narrowly they would have to be HImode constants
 * moved into registers, and they cannot be: gcc-2.96 Thumb has no HImode AND, so
 * every narrow spelling of the mask is promoted to SImode before it reaches the
 * mask load. Measured, all at the same 4-byte deficit (no `b` in any of them):
 *     masks u16, t u32                 16, RELOCDIFF
 *     masks s16, t u32                  8  (exactly inert)
 *     masks u32, t u16                  8  (exactly inert)
 *     masks u16, t u16                 16, RELOCDIFF
 *     two u16 masks                    16, RELOCDIFF
 *     two u32 masks                    10
 *     v narrowed to u16                16, RELOCDIFF
 *     masks s16, t u16                  8  (exactly inert)
 * The function's only narrow stores are `strh` and `strb` of COMPUTED values
 * (`p->f6 = t | v`, `p->f4 = b`), never of a constant, so force_const_mem is
 * never reached. The 0xfc argument is an SImode int that fits mov #imm8.
 *
 * CROSSED PAIRS TRIED on the register rename (the AND's operand order; in
 * two-operand Thumb `and rd, rs` the destination follows operand 1, and the ROM
 * makes the MASK the destination -- `and r3, r4` with the mask in r3 -- while we
 * make `t` the destination):
 *     flip, one reassigned mask                      11  (worse)
 *     flip written as `m = m & t` not `m &= t`       11  (worse)
 *     flip only the second AND                       11  (worse)
 *     flip + two separate masks                      13, dsize 0, first=0
 *     flip + t declared first                        11  (worse)
 *     flip + t read before the mask is assigned      11  (worse)
 *     flip + t u16                                   11  (worse)
 *     flip + mask u16                                16, RELOCDIFF
 * The flip is a regression in every pairing, so the ROM's `and r3, r4` is NOT
 * reached by reordering the AND's operands -- it is downstream of the same
 * allocation question as the rename itself. The "flip + two masks" row is the
 * one oddity worth a second look: it is the ONLY c154 variant in this brief with
 * dsize 0, i.e. the ROM's 40 bytes -- but it reaches them by pushing r5 in the
 * prologue (first=0) rather than by emitting the `b`, so it is a coincidence of
 * size, not progress. It is the kind of row that would be misread as a near-miss
 * by a size-only screen.
 *
 * STATUS: OPEN, with a named precondition. This is not a reachability proof --
 * it is a 4,469-file structural argument plus one controlled observation, and
 * the honest statement is that no source shape measured here produces a
 * narrow-mode pool load in this dataflow. What would retire it: any
 * demonstration of pool-before-epilogue in a <=20-instruction gcc-2.96 Thumb
 * function with only `ldr` pool loads. The scan found none in this tree.
 * WHAT NOT TO DO: do not "screen the class normally" per 801c154.c, and do not
 * spend more passes on the register rename -- it is 2 encodings of the 8 and the
 * placement is the other 6.
 *
 * The body below is rom_1c154.c's, unchanged, reproduced so the figure above is
 * reproducible from this file alone. Its own TRIED list (plain literals 18;
 * named pointer to +6 costs r5, 18; two named u32 masks 14; one reassigned u32
 * mask 13; declaration order byte-identical either way) all still holds.
 */
#include "gba/types.h"

struct S { u8 pad_00[4]; u8 f4; u8 pad_05; u16 f6; };

extern void Func_8003dec(struct S *p, s32 n);

void Func_801c154(struct S *p, u32 v, u32 b)
{
    u32 m;
    u32 t;

    m = 0x1ff;
    t = p->f6;
    v &= m;
    m = 0xfffffe00;
    t &= m;
    p->f6 = t | v;
    p->f4 = b;
    Func_8003dec(p, 0xfc);
}
