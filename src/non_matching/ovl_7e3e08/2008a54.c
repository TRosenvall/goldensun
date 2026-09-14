/* OvlFunc_957_2008a54  --  0x02008a54  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_c.s
 * Best: 3 of 48 encodings (3 of 50 lines), measured 3x.
 *
 *     XX ENCODINGS differ in 3 place(s) (ref 48, ours 48)
 *        first at index 5: ref 8019  ours 0612
 *
 * The previous park file was COMMENT-ONLY -- it had no body, so the 3-differing
 * source had been lost. It is re-derived here and shipped, so the next attempt
 * starts from the residue rather than from scratch.
 *
 * BLOCKER CLASS: A PRIORITY-FLOOR WALL. This is NOT a flippable scheduling tie,
 * and that distinction is the point -- the two devices that close an adjacent
 * pair both need somewhere to attach, and here there is nowhere.
 *
 *     rom    ldr r3,=0x4000050 / strh r1,[r3] / lsl r2,#24 / asr r2,#24
 *     ours   ldr r3,=0x4000050 / lsl r2,#24   / asr r2,#24 / strh r1,[r3]
 *
 * Read out of .23.sched2 with -fsched-verbose=5 (ARM7TDMI is ldsched=no, so every
 * load/store holds core for 2 cycles and ALU for 1). With E the store, F/G the
 * lsl/asr, B the ldrb and C/D the value and address loads:
 *
 *     p(H)=1, p(G)=2, p(F)=3, p(E)=1, p(C)=p(D)=2+p(E), p(B)=max(2+p(F), p(D))
 *
 *   * t=2, ready {C, B} -- the ROM needs B. B is data-dependent on the just
 *     -scheduled A at cost 2, so it is class 1 against C's class 3 and C wins
 *     every tie-break. B can only win on PRIORITY, which requires p(F) > p(E).
 *   * t=8, ready {E, F} -- the ROM needs E. E is data-dependent on the just
 *     -scheduled D at cost 2, so it is class 1 against F's class 3, and E has one
 *     dependent to F's two -- it loses the class step AND the dependent-count
 *     step and never reaches the LUID step it would win. E can only win on
 *     PRIORITY, which requires p(E) > p(F).
 *
 * Contradiction, for any source emitting these eight insns in these registers.
 * The ROM's own output satisfies both, so its source is not emitting this shape.
 *
 *   A store whose operands die in it and which has no in-block dependent has
 *   INSN_PRIORITY 1 -- the floor -- because add_branch_dependences links it to
 *   the block-ending branch with REG_DEP_ANTI and arm_adjust_cost returns 0 for
 *   anti-dependences. Raising it is exactly what makes the load above it lose
 *   its own earlier contest.
 *
 * WHY EACH RECORDED DEVICE IS UNREACHABLE HERE, rather than merely unmeasured:
 *   * THE ALIAS DEVICE is inert BY CONSTRUCTION. The ewram read PRECEDES the
 *     store, so any memory conflict is an ANTI-dependence at cost 0 and changes
 *     neither p(E) nor p(B). The device needs the store to be the PRODUCER of the
 *     new edge; here it can only ever be the consumer. Measured all 3 anyway: raw
 *     cast, union{int; signed char}, union{u16; signed char}, volatile read.
 *     Putting the store first so the load COULD depend on it is 14, and
 *     contradicts the ROM, which loads first.
 *   * PROMOTE AN ANTI-EDGE: there is no anti-edge to promote. The store reads
 *     r1/r3, every insn after it writes only r2, and none touches memory.
 *   * ADD A DEPENDENT works mechanically -- appending (void)REG_BLDCNT; after the
 *     store moves it up past the lsl immediately -- but block 0 ends at the first
 *     compare, so any dependent is a REAL extra instruction. Sinking a statement
 *     into both arms cannot reach a block that PRECEDES the branch, and
 *     cross-jumping only merges block tails.
 *   * -fno-schedule-insns2 regresses 3 -> 38, and here it is more than
 *     diagnostic: it makes block 0 exact but collapses all six arms, because
 *     their pre-sched order is mov/lsl/addr and cross-jumping then merges the
 *     address load and store out of all six. The ROM's arms are mov/ADDR/lsl, and
 *     mov+lsl are two adjacent insns from ONE define_split at .19.flow2 -- nothing
 *     but sched2 can put the address load between them. THE ARMS PROVE SCHED2 IS
 *     ON, which closes the only flag escape.
 *
 * WHAT GOT IT FROM 45 TO 3, and this is the reusable half:
 *
 *   The stored values must arrive from INT LOCALS, on both the BLDCNT and the
 *   BLDALPHA side. Thumb's movhi expander does force_reg (HImode, operands[1])
 *   for a store to memory, so a literal REG_BLDCNT = 0x3f42 materialises in
 *   HImode and comes out as a 4-byte ldrh from the pool. Only an SImode
 *   (set (reg) (const_int)) reaches *thumb_movsi_insn's K alternative -- the
 *   define_split that emits the ROM's mov/lsl pairs -- and because I/K sit before
 *   the pool alternative, SImode also turns 0x3f42 into the ROM's 2-byte
 *   ldr r1, =0x3f42. Raw literal stores 45; int locals 3; int on the BLDALPHA
 *   side only 7.
 *
 * Also settled: the fall-through arm's add r3, #2 is reload_cse_move2add, not
 * CSE -- 0x4000052 is still a CONST_INT at .09.cse2 and only becomes a
 * *thumb_addsi3 at .18.greg.
 *
 * Inert at 3: -fno-strict-aliasing, -fno-gcse, -fno-cse-follow-jumps,
 * -fno-rerun-cse-after-loop, -fno-peephole, -fno-strength-reduce,
 * -fno-delayed-branch, -fno-expensive-optimizations, -fno-caller-saves,
 * -fno-function-cse, -fno-thread-jumps, -fno-force-mem, -fno-regmove.
 * -O1 is 44. Shapes: switch 46, plain char 47, explicit <<24>>24 on an int 48,
 * pointer into the IO block 45, const int 7, shared value variable 16,
 * statement-order permutations 3 or 12.
 *
 * WHAT WOULD CHANGE THE ANSWER: a source shape that does NOT emit these eight
 * insns in these registers. Everything above is conditional on the shape, and the
 * shape is 45 of 48 correct -- so the next attempt should be looking for a
 * different decomposition of the whole block, not another lever on this one.
 */
/* OvlFunc_957_2008a54 -- asm/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_c.s
 *
 * BLOCKER: SCHED2 PRIORITY, AND IT IS PROVABLY UNREACHABLE, NOT A TIE.
 *
 * 3 of 50 lines, 3 of 48 encodings.  Score re-verified from scratch; the old
 * park carried only a comment, no body, so this .c IS the 3-differing source.
 *
 * The .s holds this one function and nothing else, so the file converts WHOLE
 * and overlays/rom_7e3e08/overlay.ld:31 keeps its asm/ prefix verbatim.
 *
 * THE RESIDUE.  Everything but block 0's store placement is exact -- including
 * the six mov+lsl constant builds, the fall-through arm's `add r3, #2`
 * (reload_cse_move2add, not CSE: the constant 0x4000052 is still a CONST_INT at
 * .09.cse2 and only becomes a *thumb_addsi3 at .18.greg), and the cross-jumped
 * single `strh r2, [r3]` tail.
 *
 *     rom    ldr r3,=0x4000050 / strh r1,[r3] / lsl r2,#24 / asr r2,#24
 *     ours   ldr r3,=0x4000050 / lsl r2,#24   / asr r2,#24 / strh r1,[r3]
 *
 * TWO SPELLING FACTS THAT GOT THE SCORE FROM 45 TO 3, both required:
 *
 *   1. The stored values must arrive from `int` LOCALS.  Thumb's movhi expander
 *      does `force_reg (HImode, operands[1])` for a store to memory, so a
 *      literal `REG_BLDCNT = 0x3f42` materialises in HImode and comes out as a
 *      4-byte `ldrh rX, .Lpool`.  Only an SImode `(set (reg) (const_int))`
 *      reaches *thumb_movsi_insn's `K` alternative, which is the define_split
 *      that emits the ROM's `mov r2,#0x80 / lsl r2,#5` pairs -- and the `I`/`K`
 *      alternatives sit BEFORE the `mi` pool-load alternative, so SImode also
 *      turns 0x3f42 into the ROM's 2-byte `ldr r1, =0x3f42`.  Raw literal
 *      stores score 45; int locals score 3.
 *   2. Both the BLDCNT value and the BLDALPHA value need it.  Doing only the
 *      BLDALPHA side is 7.
 *
 * WHY THE LAST THREE LINES CANNOT BE REACHED.  Read out of `.23.sched2` with
 * -fsched-verbose=5; the ARM7TDMI model is ldsched=no, so every load and store1
 * holds the `core` unit for 2 cycles and every ALU insn for 1.  Block 0, with
 * the ROM's own register assignment:
 *
 *     A  r3 = pool(ewram_2001004)   load  prio 7
 *     B  r2 = zxn([r3])             load  prio 5
 *     C  r1 = 0x3f42                load  prio 3
 *     D  r3 = 0x4000050             load  prio 3
 *     E  [r3] = r1                  store prio 1     <-- must move up
 *     F  r2 = r2 << 24              alu   prio 3
 *     G  r2 = r2 >> 24              alu   prio 2
 *     H  cmp r2,#0 / bne            prio 1
 *
 *   p(H)=1, p(G)=2, p(F)=3, p(E)=1, p(C)=p(D)=2+p(E), p(B)=max(2+p(F), p(D)).
 *   D is not ready until B issues (B reads r3, D writes it: anti-dependence).
 *
 *   t=2, ready {C, B}: the ROM needs B.  B is data-dependent on the
 *   just-scheduled A at cost 2, so rank_for_schedule gives it class 1 against
 *   C's class 3 and C wins every tie.  So B must win on PRIORITY:
 *       max(2+p(F), p(D)) > p(D) = 2+p(E)   =>   p(F) > p(E).
 *
 *   t=8, ready {E, F}: the ROM needs E.  E is data-dependent on the
 *   just-scheduled D at cost 2 -> class 1, against F's class 3; and E has one
 *   dependent to F's two, so E loses the class step and the dependent-count
 *   step and never reaches the INSN_LUID step it would win.  So E must win on
 *   PRIORITY:
 *       p(E) > p(F).
 *
 *   p(F) > p(E) and p(E) > p(F).  CONTRADICTION -- for every source that emits
 *   these eight insns in these registers, which the ROM's own output fixes.
 *
 * > A store whose operands die in it and that has no in-block dependent has
 * > INSN_PRIORITY 1, the floor, because add_branch_dependences links it to the
 * > block-ending branch with REG_DEP_ANTI and arm_adjust_cost returns 0 for
 * > anti-dependences.  It therefore loses every ready-list contest to any insn
 * > with a downstream chain -- and raising it is what forces the load above it
 * > to lose its own earlier contest.  This is a two-sided wall, not a tie to be
 * > flipped.
 *
 * WHY THE ALIAS DEVICE IS INERT HERE, and it is inert by construction, not by
 * accident: the ewram read PRECEDES the store, so any memory conflict between
 * them is an ANTI-dependence, cost 0, which changes neither p(E) nor p(B).
 * The device needs the store to be the PRODUCER of the new edge; here it can
 * only ever be the consumer.  Measured: raw cast 3, union{int,signed char} 3,
 * union{u16,signed char} 3, volatile read 3.  Putting the store FIRST so the
 * load could depend on it is 14 and contradicts the ROM, which loads first.
 *
 * ADDING A DEPENDENT DOES WORK, AND THERE IS NOWHERE TO PUT ONE.  Proof that
 * priority is the whole lever: append `(void)REG_BLDCNT;` after the store and
 * the store immediately moves up past the lsl.  But block 0 ends at the first
 * compare, so any dependent is a real extra instruction; the batch-262 trick of
 * sinking a statement into both arms cannot reach a block that precedes the
 * branch, and cross-jumping (.25.jump2) only merges block TAILS.
 *
 * -fno-schedule-insns2 IS NOT AVAILABLE.  It makes block 0 exact and scores 38:
 * the six arms collapse because their pre-sched order is mov/lsl/addr and
 * cross-jumping then merges `ldr r3,=0x4000052 / strh` out of all six.  The
 * ROM's arms are mov/addr/lsl, and the mov+lsl pair is produced by ONE
 * define_split at .19.flow2 -- two adjacent insns -- so nothing but sched2 can
 * put the address load between them.  The arms therefore PROVE sched2 is on,
 * which closes the only flag escape.
 *
 * ALSO MEASURED INERT (all 3): -fno-strict-aliasing, -fno-gcse,
 * -fno-cse-follow-jumps, -fno-rerun-cse-after-loop, -fno-peephole,
 * -fno-strength-reduce, -fno-delayed-branch, -fno-expensive-optimizations,
 * -fno-caller-saves, -fno-function-cse, -fno-thread-jumps, -fno-force-mem,
 * -fno-regmove.  -fno-schedule-insns is inert, as everywhere in this tree (no
 * .21.sched dump; the pass order is 20.ce2 -> 23.sched2).  -O1 is 44.
 * Shapes measured: switch 46, plain `char` 47, int + explicit <<24>>24 48,
 * pointer-into-the-IO-block 45, `const int` 7, value-only int 7, shared
 * value variable 16, statement-order permutations 3 or 12.
 */
#include "gba/io.h"

extern unsigned char ewram_2001004[];

void OvlFunc_957_2008a54(void) {
    signed char mode = ewram_2001004[0];
    int cnt = 0x3f42;
    int v;

    REG_BLDCNT = cnt;
    if (mode == 0) {
        v = 0x1000;
        REG_BLDALPHA = v;
    } else if (mode == 1) {
        v = 0x0e00;
        REG_BLDALPHA = v;
    } else if (mode == 2) {
        v = 0x0c00;
        REG_BLDALPHA = v;
    } else if (mode == 3) {
        v = 0x0a00;
        REG_BLDALPHA = v;
    } else if (mode == 4) {
        v = 0x0800;
        REG_BLDALPHA = v;
    } else {
        v = 0x0600;
        REG_BLDALPHA = v;
    }
}
