/* OvlFunc_881_20082f0 -- asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s,
 * NON-MATCHING, 5 of 17 encodings (re-measured batch 317 as installed).
 * 0x020082f0, 15 instructions.
 * Park: src/non_matching/ovl_77a7c8/20082f0.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/20082f0.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s --func OvlFunc_881_20082f0
 *
 * FIGURE IN  : 5 of 17 encodings (15 instructions + pool), SIZE EXACT,
 *              RELOCATIONS EXACT. Measured; no recipe named this function and
 *              the park carried no figure at all, only
 *              "TODO(residual): reg-alloc/scheduling divergence (register swap /
 *              op-order); logic correct. Permuter seed."
 * FIGURE OUT : 2 of 17 encodings, pin-free route not found; 2 of 17 WITH ONE
 *              REGISTER PIN (see below). Does not land.
 *
 * SPLIT SHAPE: see the p3_candidate.c header -- these two are the only
 * functions in the .s, so if BOTH land there is NO split and no linker script
 * change; if only one lands, one split_s.py run is needed and the two dry-runs
 * name their outputs differently.
 *
 * PINS: 1 in the 2-of-17 variant (`register int one __asm__("r3")`).
 *       tools/shimcount.py reports: register pins : 1, and
 *       "*** has a fakematch-class shim and NO fakematch.txt row".
 *       SO IF THAT VARIANT WERE EVER SHIPPED IT WOULD NEED A fakematch.txt ROW.
 *       It should not be shipped: it does not land.
 *
 * THE PARK'S DIAGNOSIS IS HALF RIGHT, AND THE HALF IT NAMES IS NOW SOLVED
 * ======================================================================
 * "register swap / op-order" is the right shape. The 5-residue is TWO adjacent
 * swaps of independent instructions plus ONE register swap, and the register
 * swap CAUSES one of the two reorderings:
 *
 *   rom   ldr r3,=iwram | ldr r4,[r0,#0x50] | add r0,#0x59 | ldrb r2,[r0] | ldr r1,[r3] | mov r3,#1  | orr r3,r2 | mov r2,#0x8d | lsl r2,#1    | strb r3,[r0] | ...
 *   ours  ldr r3,=iwram | ldr r4,[r0,#0x50] | add r0,#0x59 | ldr r1,[r3]  | ldrb r3,[r0]| mov r2,#1  | orr r3,r2 | mov r2,#0x8d | strb r3,[r0] | lsl r2,#1    | ...
 *
 * RESIDUE A (indices 3-5) -- THE REGISTER SWAP AND ITS ANTI-DEPENDENCE.
 * The ROM puts the flag byte in r2 and the constant 1 in r3; we do the
 * reverse. Because our ldrb WRITES r3, and r3 is the register holding the
 * iwram symbol address that `ldr r1,[r3]` still has to READ, the ldrb acquires
 * an ANTI-DEPENDENCE on the deref and is forced after it. Read off
 * -fsched-verbose=6 on the best order (r0a_BOFS): insn 23 (the ldrb) has
 * dep 3, and insn 14 (`r1=[r3]`) lists 23 among its forward dependents -- an
 * edge that exists ONLY because of the register reuse. The ROM's ldrb writes
 * r2, has no such edge, and issues first. So residue A is one register
 * assignment, and the reordering is downstream of it, not independent.
 *
 * Mechanically: the ROM's `orr r3, r2` takes its DESTINATION from the register
 * holding the CONSTANT (local-alloc's destination/dying-source combine landing
 * on the constant's quantity); ours takes it from the loaded byte's. Both
 * sources die at the orr, and `*thumb_iorsi3`'s destination follows operand 1,
 * so this is the IOR analogue of the brief's PLUS-operand-order lever -- except
 * that fold canonicalises a constant to the SECOND operand of a commutative
 * operator unconditionally, so `1 | f`, `f | 1` and `*flagp |= 1` are all the
 * same tree. Measured: all three spellings are EXACTLY INERT at 5 of 17.
 *
 * TWO THINGS FIX RESIDUE A, both reaching the SAME 2 of 17 with indices 0-7
 * then exact -- which is what proves the register swap was the mechanism:
 *   1. `register int one __asm__("r3")` for the constant. 1 pin.
 *   2. -fno-expensive-optimizations. No pin.
 * CROSSED, they add nothing (identical stream), so they are ONE lever reached
 * two ways, not two levers.
 *
 * RESIDUE B (indices 8-9) -- AND IT IS THE SAME WALL AS TARGET 2 OF THIS BRIEF.
 * What is left after residue A is one adjacent swap of two INDEPENDENT
 * instructions: `lsl r2,#1` (finishing 0x11a) and `strb r3,[r0]` (the flag
 * store). From the dependence table:
 *       insn 61  lsl r2 = r2<<1    prio 37   dependents: 65 52 37          = 3
 *       insn 30  strb [r0] = r3    prio 37   dependents: 65 52 43 40 39 37 = 6
 * Priority ties at 37; rank_for_schedule falls to DEPENDENT COUNT, MORE WINS,
 * and the store takes the slot. The ROM wants the shift first.
 *
 * The store's count is 6 because it is a STORE: it collects a memory
 * dependence against every later memory reference (insns 39, 40 -- the ldrh and
 * the strh). Those edges are false, and they cannot be removed from C, because
 * the store must stay a `strb` and EVERY ONE-BYTE C TYPE IS A CHARACTER TYPE,
 * which is alias set 0 and conflicts with everything. DIFFERENT_ALIAS_SETS_P
 * needs BOTH sets non-zero. And the shift cannot be given a fourth dependent:
 * it is not a memory insn, so an edge out of it needs another reader or writer
 * of r2, and the ROM's own fifteen instructions contain exactly one of each.
 *
 * MEASURED INERT / WORSE (figures are objcmp encodings out of 17 unless noted)
 * ---------------------------------------------------------------------------
 *   216 statement-order x OR-spelling x off-local variants swept
 *   (scratch_elev/b317/A/x4b/, gen4b.py, RESULTS.txt). tryc histogram:
 *   9 at 5, 12 at 11, 6 at 12, 12 at 13, 6 at 14, 30 at 15, 90 at 16, 42 at 17,
 *   3 at 18, 6 at 20. The nine best all share B-before-S and are otherwise
 *   indistinguishable, so the OR spelling and the statement order are both
 *   inert at the floor.
 *   A NAMED `off` LOCAL for 0x11a: always a regression, 11 or worse. The 0x11a
 *   must stay inline -- this is the one spelling choice that matters and it
 *   goes the opposite way from target 2, where naming everything was the lever.
 *   A NAMED `f` LOCAL for the flag byte: inert at 5 (x4c/s1).
 *   A `one` LOCAL, unpinned, for the constant: inert at 5 (x4c/s2, s3) --
 *   cprop propagates the 1 back into the IOR, which is the brief's
 *   "a source-level copy is NOT a region split" rule showing up on a constant.
 *   `unsigned char f` instead of `int f`: inert at 5 (x4c/s4).
 *   A FIRST ATTEMPT THAT WAS MUCH WORSE, recorded because it looked right:
 *   retyping the sprite write as `(char *)out + 0x1e` and naming both `f` and
 *   `off` (240 variants, x4/) bottoms out at ELEVEN. The park's own `out[0xf]`
 *   spelling is load-bearing and better than the "more explicit" one.
 *   FLAGS: -fno-strict-aliasing, -fno-gcse, -fno-rerun-cse-after-loop,
 *   -fno-peephole, -fno-regmove all EXACTLY INERT at 5.
 *   -fno-schedule-insns2 is WORSE (10 positional).
 *
 * THE FILE-MATE FLAG CHECK THE COORDINATOR ASKED FOR: NO CONFLICT. NOT THE
 * BATCH-310 SHAPE.
 * ====================================================================
 * Every flag was measured on BOTH functions in this .s at once. 20082cc is
 * EXACTLY INERT under all of them (10 positional throughout), including under
 * -fno-expensive-optimizations, which is the one flag that helps 20082f0
 * (5 -> 2). So the two file-mates do NOT want opposite flags: there is no
 * requirement in opposite directions here and therefore NO SPLIT IS FORCED BY
 * A FLAG. The only flag either of them reacts to at all helps one and leaves
 * the other untouched. -fno-schedule-insns2 is the one flag that hurts, and it
 * hurts 20082f0 while leaving 20082cc unchanged -- again no conflict.
 *
 *   flag                            20082cc   20082f0
 *   production                        10         5
 *   -fno-schedule-insns2              10        10
 *   -fno-strict-aliasing              10         5
 *   -fno-gcse                         10         5
 *   -fno-rerun-cse-after-loop         10         5
 *   -fno-peephole                     10         5
 *   -fno-regmove                      10         5
 *   -fno-expensive-optimizations      10         2
 *
 * I am NOT proposing a -fno-expensive-optimizations Makefile row: it does not
 * land the function, and a per-file flag that leaves a residue buys nothing and
 * costs a rule that someone will later have to justify.
 *
 * DECLINING TO CLOSE. The park's "register swap / op-order" is refuted as a
 * single blocker and resolved into two: a register assignment (solved, two
 * ways) and a sched2 dependent-count tie-break whose winner's count is
 * inflated by unremovable alias-set-0 memory edges. 5 -> 2, open park, better
 * map.
 *
 * The body below is the PIN-FREE 5-of-17 best. It is the one to install if this
 * park is rewritten, because it is honest production-flag output; the 2-of-17
 * is reachable from it by adding one pin or one non-production flag, and
 * neither lands.
 */
extern unsigned int iwram_3001e70;

/* Sets bit 0 of the flag byte at +0x59 and copies a halfword out of the block
 * at iwram_3001e70 into the sprite field at +0x1e. */
unsigned int OvlFunc_881_20082f0(unsigned char *p)
{
    unsigned int base;
    unsigned char *flagp;
    unsigned short *out;

    base = iwram_3001e70;
    out = *(unsigned short **)(p + 0x50);
    flagp = p + 0x59;
    *flagp = 1 | *flagp;
    out[0xf] = *(unsigned short *)(base + 0x11a);
    return 1;
}
