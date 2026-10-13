/* Func_80c00d8 @ 0x080c00d8
 *
 * NON-MATCHING, 7 of 37 stream encodings  (RE-MEASURED, batch 345 brief A,
 *   on a NEW BODY -- the park's 33 is retired).
 *   ref 37 encoding rows / 88 bytes; ours 37 / 88.  dsize 0.
 *   ALIGNED residue 33 equal / 4 differing, against the parked body's
 *   17 equal / 20 differing.  THE LENGTH GAP IS CLOSED: the prologue is now
 *   `push {r5, r6, lr}` against the parked body's `push {r5, r6, r7, lr}`,
 *   and the ROM's second build of 0x100 is emitted.
 *   Of the 4 remaining aligned rows ONE IS THE REFERENCE-SIDE ewram_2010200
 *   ARTEFACT the batch-333 note already documents (replacing the literal with
 *   `(u32)&ewram_2010200` reads 6 of 37 with rel=ok and aligned 34/3, which is
 *   the measurement that CONFIRMS that row is the hand-written .s symbolising
 *   a numeric literal).  The real code residue is THREE aligned instructions
 *   in two argument fills.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/rom_c00d8.c \
 *     asm/rom_b5000/rom_bffb8_a_a_a_c_a.s --func Func_80c00d8
 *
 * ===== BATCH 345 BRIEF A: THE cse1 BOUND IS REFUTED, DEVICE-FREE, AND THE
 * ===== LEVER IS TO MAKE THE ADVANCE NOT BE 256 UNTIL AFTER cse1 RUNS.
 *
 * Batches 334 and 335 closed nineteen cells in five dimensions and concluded
 * "disagree with cse.c:509-514 or with arm.c:2077-2080, or find a construct
 * that reaches cse1 with the two values in different MODES.  Do NOT re-spell
 * the constant or re-type the pointer."  BOTH HALVES OF THAT ARITHMETIC ARE
 * CORRECT AND THE CONCLUSION IS WRONG, because there is a third option neither
 * round considered: do not let EITHER VALUE BE 256 WHEN cse1 RUNS.
 *
 *     p += 64;                 33 of 37, aligned 17/20   (the parked body)
 *     p += 32; p += 32;         7 of 37, aligned 33/4    <-- this body
 *     p = (u32 *)((char *)p + 0x80); twice
 *                               7 of 37, aligned 33/4    (identical figure)
 *
 * Two increments of 128 bytes are each a DIFFERENT constant from the size
 * argument's 256, so cse1's equivalence class never contains both; the fold
 * back to one `adds r6, r6, r3` happens later, after cse1, and the 256 for the
 * advance is built fresh exactly as the ROM builds it.  The park's own
 * "defect 1" and "defect 2" BOTH go: the prologue loses the extra push, the
 * `movs r3, #128 / lsls r3, #1 / adds r6, r6, r3` appears, and the
 * `_call_via_r5` relocations line up.
 *
 * CONTROL, and it is the measurement that names the pass: a cse-flushing
 * instrument -- `__asm__ ("" :: "i"(0));` per cse.c:5742-5745 -- placed between
 * the call and the advance gives the SAME 7 of 37 and the same aligned 33/4.
 * So the device-free spelling reaches exactly as far as the device does, and
 * the whole of the old 20-row residue was the cse1 unification and nothing
 * else.  The same barrier placed BEFORE the call instead reads 21 of 37.
 *
 * WHAT REMAINS, three aligned instructions, both of them ARGUMENT-FILL ORDER:
 *     call 2   rom   adds r0, r6, #0 / movs r1, #128 / ldr r2, =0x3ff03ff
 *              ours  ldr r2, =0x3ff03ff / adds r0, r6, #0 / movs r1, #128
 *     call 4   rom   movs r1, #160 / ldr r3, =fp / adds r0, r6, #0
 *                    / lsls r1, #2 / ldr r2, =0x3ff03ff
 *              ours  movs r1, #160 / ldr r3, =fp / lsls r1, #2
 *                    / ldr r2, =0x3ff03ff / adds r0, r6, #0
 * In both the ROM fills r0 EARLIER than we do and puts the pool load LAST.
 * Same shape twice, so it is one question, not two.
 *
 * MEASURED INERT OR WORSE on this body, all at ni 37 / dsize 0:
 *   - the anchor axis, all 7 single boundaries and all 21 pairs: best 6 of 37
 *     but aligned 32/5, i.e. WORSE than the base's 33/4.  None reaches 3.
 *   - all eight per-file flag groups: -fno-gcse, -ffixed-r7,
 *     -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-cse-after-loop and
 *     -fno-rerun-loop-opt are EXACTLY INERT at 7 of 37; -O1 and
 *     -fno-schedule-insns2 both read 18 of 37 with aligned 27/10, i.e. much
 *     worse.  (NOTE: the parked body's autosweep row "-O1 18 of 37, instruction
 *     count moved to +0" is this same 18 -- it closes the gap and raises the
 *     aligned residue, so it was never progress.)
 *   - `fp((void *)p, ...)` and the first argument in a block-scope temp: both
 *     EXACTLY INERT at 7 of 37, aligned 33/4.
 *   - `fp(p, 0x100, -1)` instead of `fp(p, 0x80 << 1, -1)`: 33 of 37.  Still
 *     the parked figure, confirming batch 335's spelling dimension.
 *   - a bare `__asm__ volatile ("")` before call 2: 6 of 37, aligned 33/4 --
 *     the aligned residue does NOT move, so the ndiff fall is phase only.
 *
 * NEXT: the argument fill order at a call whose last argument is a pool load.
 * This is now the ONLY thing between this function and a match, it is the same
 * shape at both sites, and it is a sched2 question -- the pre-sched order is
 * what differs, so read .19.flow2 against .23.sched2 and compare INSN_LUIDs
 * rather than sweeping spellings.
 *
 * NOTE ON `p += 32; p += 32;` AS SOURCE.  It is device-free, pin-free and
 * semantically identical to `p += 64`, and shimcount counts nothing -- but it
 * is a matching ARTEFACT in the pass-3/4 sense and belongs in
 * reports/pass3-depin.md's taxonomy if this ever lands with it.  Keep it only
 * while it is load-bearing.
 *
 * --------------------- the park's header, carried forward ---------------------
 *
 * NON-MATCHING, 33 of 37 stream encodings  (RE-MEASURED, batch 333).
 *   The STREAM lengths are both 37 because a pad absorbs the difference; the
 *   16-bit INSTRUCTION counts differ, ref 30 against ours 29, so this
 *   positional figure measures MISALIGNMENT, not distance.
 *   *** RELOCATIONS ALSO DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS.
 *   Two of those rows are real and one is not; see the batch-333 note below
 *   before trusting the encoding count. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/rom_c00d8.c \
 *     asm/rom_b5000/rom_bffb8_a_a_a_c_a.s --func Func_80c00d8
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bffb8_a_a_a_c_a.s
 *
 * NOT SPLIT. The .s still holds its remaining functions and the linker script
 * is untouched.
 *
 * Sets up a screen's worth of tile data: clear 0x100 bytes, fill 0x80 bytes
 * with 0x03ff03ff, write a 240-word ramp of packed tile indices, then fill
 * another 0x280 bytes. Its neighbour Func_80c0098 in the same original .s IS
 * elevated (src/rom_b5000/rom_bffb8_a_a_a_b.c) and the ramp loop here uses the
 * same literal-in-the-body lever that one needed.
 *
 * Blocker: CONSTANT-CSE, twice over, 33 instructions against 34.
 *
 * 1. THE VALUE 0x100 IS BUILT TWICE IN THE ROM and once by gcc. It is the size
 *    argument of the first call and also the amount the pointer advances:
 *
 *        rom    mov r1, #0x80 / lsl r1, #1   ... mov r3, #0x80 / lsl r3, #1 / add r6, r3
 *        ours   mov r5, #0x80 / lsl r5, #1   ... mov r1, r5 / add r7, r5
 *
 *    gcc computes it once into a callee-saved register and uses it for both,
 *    which costs it an extra live value and pushes r7.
 *
 * 2. THE FUNCTION POINTER IS RE-MATERIALISED for the last call. The ROM holds
 *    Func_80008d8 in r5 across the first two calls, then loads it AGAIN into r3
 *    for the fourth:
 *
 *        rom    ldr r5, =Func_80008d8 ... bl _call_via_r5 ... bl _call_via_r5
 *               ... ldr r3, =Func_80008d8 / bl _call_via_r3
 *
 *    r5 is callee-saved and nothing in the loop clobbers it, so gcc reuses it
 *    and never emits the second load.
 *
 * Both are the same defect in the same direction, and it is the direction worth
 * restating because a note in this tree once had it backwards: THE ROM'S
 * REDUNDANT FORM IS THE TARGET. gcc's common-subexpression elimination is what
 * has to be defeated. The ROM is not doing anything clever; it is rebuilding a
 * cheap value rather than keeping a register tied up, and gcc is too eager to
 * reproduce it.
 *
 * TRIED, all giving byte-identical output to each other (33 lines, same
 * registers, first diff at instruction 0):
 *   1. the pointer advance as `p += 64` on a u32* (a different source constant
 *      from the size argument's `0x80 << 1`, but the same machine value)
 *   2. the advance as `p += 0x80 >> 1`
 *   3. two separate function-pointer variables, fp and fp2
 *   4. fp2 declared in an inner scope to shorten its live range so it would
 *      land in a scratch register rather than a callee-saved one
 *
 * (3) and (4) are the trick that DID work for the two script pointers in
 * OvlFunc_945_200d068 -- separate variables there moved the first divergence by
 * thirty instructions. It does nothing here, which is the useful negative:
 * separate variables defeat gcc's reuse of a value it COMPUTED, not of a
 * constant it can rematerialise.
 *
 * This is the same wall as src/non_matching/ovl_7f2f14/20087d8.c, where the ROM
 * materialises -1 three times. Nothing in the tree defeats it yet.
 *
 * ---------------------------------------------------------------------------
 * BATCH 333: THE MISSING INSTRUCTION, THE REASON FOR IT, AND ONE RELOCATION
 * ROW THAT IS A REFERENCE-SIDE ARTEFACT.
 *
 * THE GAP IS DEFECT 1 ABOVE AND NOTHING ELSE.  Counting only the instructions
 * that build or consume the first size value: the reference spends five (two to
 * build it for the call argument, two to build it AGAIN, one to add it to the
 * pointer) and we spend four (two to build it once, one to copy it into the
 * argument register, one to add it).  That one instruction is the whole length
 * gap.  Defect 2, the re-materialised function pointer, costs no instruction --
 * it moves which `_call_via_rN` veneer is referenced, which is why it shows up
 * as a relocation row rather than a count.
 *
 * WHY WE KEEP THE VALUE AND THE REFERENCE DOES NOT, stated more precisely than
 * "gcc's common-subexpression elimination is too eager".  In the reference the
 * value is built straight into the argument register and dies at the first
 * call, so the second use has to rebuild it.  In our output it is a pseudo that
 * is live across the argument setup, so it is given a CALLEE-SAVED register --
 * which is also why our prologue pushes one register more than the
 * reference's, and why our whole first block is register-renamed.  The brief's
 * standing lever is the relevant one: a pseudo that crosses a call cannot take
 * r0-r3 or r4 (global.c:967, :992-995), so the register the reference uses is
 * available only to a value that does NOT cross the call.  The question to ask
 * is therefore "what keeps the size value out of a pseudo that outlives the
 * argument setup", not "what spelling of the constant differs".
 *
 * MEASURED AND EXACTLY INERT, all twenty-nine instructions and thirty-three
 * differing with the first difference at the prologue: the advance written
 * through a byte-pointer cast with the size as a plain literal; the size pulled
 * into its own local used once; the whole function on a byte pointer.  Add
 * those to the four spellings already listed above; all seven are the same
 * cell of the search.
 * MEASURED WORSE: a second pointer name for the later calls (thirty-four
 * instructions); naming the callee directly at every call site instead of
 * through a pointer (twenty-seven instructions, and it loses a veneer call
 * altogether).
 *
 * THE `ewram_2010200` RELOCATION ROW IS NOT OUR DEFECT.  The reference `.s`
 * carries `ldr r2, =ewram_2010200` for the ramp's starting value, and
 * `wram.sym` defines `ewram_2010200 = 0x02010200`, so the symbol resolves to
 * exactly the literal this body writes.  The value is not an address: the loop
 * stores it and adds 0x20002 each iteration, which is a pair of packed
 * halfwords each stepping by one -- tile data, not a pointer.  So the row is
 * the hand-written disassembly having symbolised a numeric literal that happens
 * to fall in the EWRAM range, and it disappears from the build the moment this
 * function is elevated, because the generated `.s` replaces the hand-written
 * one.  (The symbol IS a real object elsewhere -- another park declares it as
 * an array -- so this is a misattribution at THIS use, not a bad symbol.)
 * The `_call_via_r5` against `_call_via_r6` rows ARE real: they are defect 2's
 * register allocation showing through, and they go when the allocation does.
 *
 * PIECE: `asm/rom_b5000/rom_bffb8_a_a_a_c_a.s` also holds Func_80c0130, and THAT
 * FUNCTION IS ALSO PARKED (src/non_matching/rom_b5000/80c0130.c).  One .c is
 * one TU, so neither converts alone -- a landing here must carry both.
 * ---------------------------------------------------------------------------
 
 * ===== BATCH 334 BRIEF B: THE PASS IS cse1, AND THE SUBSTITUTION IS FORCED
 * ===== ARITHMETIC RATHER THAN A HEURISTIC.
 *
 * Figure re-derived unchanged: thirty-three of thirty-seven stream encodings,
 * instructions ref thirty against ours twenty-nine.  Body unchanged.
 *
 * THE PARK IS WRONG ABOUT EXPAND, AND THAT MATTERS.  It says "gcc computes it
 * once".  It does not: `.00.rtl` holds TWO pseudos for the value, insn 15
 * `(set (reg 36) (const_int 256))` feeding the argument register and insn 27
 * `(set (reg 38) (const_int 256))` feeding the pointer advance.  Insn 27 is
 * ALREADY GONE in `.03.cse`, where the advance reads reg 36.  So the merge
 * happens in cse1 -- not in gcse, not in combine, not in the allocator -- and
 * the batch-333 note's "what keeps the size value out of a pseudo that
 * outlives the argument setup" is the right question asked of the wrong pass.
 *
 * AND NO SPELLING OF A 256 CAN LOSE THAT SUBSTITUTION.  cse picks the cheapest
 * member of an equivalence class, and its cost function (**cse.c:509-514**)
 * charges a pseudo 1 while `notreg_cost` defers to `rtx_cost (const_int 256,
 * SET)`, which `thumb_shiftable_const` sends to `COSTS_N_INSNS (2)` = 6
 * (**arm.c:2077-2080**).  One is less than six for every value that needs more
 * than an eight-bit move, so the register always wins.  cse's table is flushed
 * only at a basic-block path boundary, and `invalidate_for_call` clears HARD
 * regs, never pseudos, so the call between the two uses does not help.  The
 * only route is a control-flow boundary between the argument and the advance,
 * which costs more instructions than the one it buys.
 *
 * THE OTHER HALF IS CLOSED TOO: a 256 argument ALWAYS takes a pseudo.
 * `calls.c:841-857` forces one when `rtx_cost (value, SET) > 2`, and the same
 * `arm.c:2077-2080` returns 0 below 256 and `COSTS_N_INSNS (2)` at or above
 * it.  So "build it straight into the argument register and let the call
 * invalidate it" is not reachable from C either.
 *
 * MEASURED INERT THIS ROUND, both still twenty-nine instructions and seventeen
 * aligned-equal of thirty-seven:
 *   - the callee's size parameter declared as a halfword type, so the two
 *     constants would hash in different modes (cse's `lookup` compares `mode`
 *     before `exp_equiv_p`).  The constant is converted back to word mode
 *     before the table ever sees it, so there is no mode to split.
 *   - the advance written through a byte-pointer cast with the size left as a
 *     plain literal.
 *
 * NEXT: disagree with `cse.c:509-514` or with `arm.c:2077-2080`, not with
 * another spelling of the constant.  The piece is still blocked on
 * Func_80c0130 regardless -- see that park's own three-escape bound.
 *
 * ===== BATCH 335 BRIEF B: THE PIECE COMPILES AS ONE TU AND THIS FUNCTION IS
 * ===== THE GATE.  TWELVE NEW CELLS IN THREE UNVARIED DIMENSIONS, ALL FLAT.
 *
 * Figure re-derived unchanged: thirty-three of thirty-seven stream encodings,
 * instructions ref thirty against ours twenty-nine, stream lengths equal at
 * thirty-seven because a pad absorbs it.  BODY UNCHANGED.  DIRECTION: the
 * REFERENCE IS LONGER, by one, so we are MISSING one -- the batch-333 reading.
 *
 * THE PIECE ARITHMETIC IS NOW MEASURED RATHER THAN ARGUED.  Both members of
 * asm/rom_b5000/rom_bffb8_a_a_a_c_a.s were put in ONE translation unit, this
 * function first in the reference's order, and checked whole: this function
 * reads thirty-three of thirty-seven and Func_80c0130 reads six of thirty-seven,
 * EXACTLY their separate figures with the same first indices.  So the piece
 * genuinely converts whole, no split is needed, the joint distance is thirty-
 * nine of seventy-four -- and the batch-327 note's "a landing here needs a
 * split" is retired for good.  datacheck reports no data; `grep -c func_start`
 * is two.  THIS FUNCTION IS THE GATE: Func_80c0130's residue is six scheduling
 * encodings bounded by three escapes in the machine description, while this one
 * is a MISSING INSTRUCTION, which is the larger obstacle of the two.
 *
 * WHAT IT NEEDS, STATED PLAINLY.  One instruction: the reference rebuilds the
 * first size value for the pointer advance where we copy the register cse1
 * already has.  Per the batch-334 reading the merge is cse1's and is FORCED
 * arithmetic, not a heuristic -- `cse.c:509-514` charges a pseudo one against
 * `rtx_cost (const_int 256, SET)` of six from `arm.c:2077-2080` -- so what it
 * needs is for the two values NOT TO BE IN ONE EQUIVALENCE CLASS at pass 03.
 * cse's `lookup` compares MODE before `exp_equiv_p`, so a mode split would do
 * it; the batch-334 round closed the halfword-parameter route, and this round
 * closes the three remaining ways to reach expand with a different constant.
 *
 * MEASURED THIS ROUND -- TWELVE CELLS, AND THE POINT IS THAT THEY ARE A CROSS
 * AND NOT A DIAGONAL.  Pointer type and advance spelling (a word pointer
 * advanced by sixty-four; a byte pointer advanced by the byte count written
 * out; a sixty-four-word STRUCT pointer advanced by `++`, so the byte offset is
 * the struct's size rather than a written constant) crossed with the size
 * argument's carrier (a plain literal; a named word local; a named HALFWORD
 * local; and a negated double-negative spelling).  ALL TWELVE ARE EXACTLY
 * TWENTY-NINE INSTRUCTIONS AND THIRTY-THREE OF THIRTY-SEVEN.  Not one of them
 * changes the constant expand sees: the front end folds every spelling of the
 * byte count to the same word-mode constant, and the struct pointer's `++`
 * produces the same one from the type's size.
 *
 * So the park's and batch 334's bound both hold, now with the pointer-type
 * dimension closed as well as the spelling one.  Add these twelve to the seven
 * already listed above; nineteen cells in five dimensions.
 *
 * The `pop {r0}` reading was checked as a free test and AGREES: the reference's
 * epilogue pops r0, which is the void declaration this body already carries.
 *
 * NEXT: unchanged, and sharper.  Disagree with `cse.c:509-514` or with
 * `arm.c:2077-2080`, or find a construct that reaches cse1 with the two values
 * in different MODES.  Do NOT re-spell the constant or re-type the pointer.
 */
#include "gba/types.h"

extern void Func_80008d8(void *dst, u32 size, u32 value);

void Func_80c00d8(u32 *p)
{
    void (*fp)(void *, u32, u32);
    u32 v, i;

    fp = Func_80008d8;
    fp(p, 0x80 << 1, -1);
    p += 32;
    p += 32;
    fp(p, 0x80, 0x3ff03ff);
    p += 32;
    v = 0x2010200;
    i = 0;
    do { i++; *p++ = v; v += 0x20002; } while (i <= 0xef);
    {
        void (*fp2)(void *, u32, u32) = Func_80008d8;
        fp2(p, 0xa0 << 2, 0x3ff03ff);
    }
}
