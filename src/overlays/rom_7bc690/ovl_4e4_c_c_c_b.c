/* OvlFunc_933_2009874  [ovl_7bc690]   --  LANDS, byte-identical
 *
 *   OK OvlFunc_933_2009874 -- 48 bytes, 20 encodings and 4 relocations identical
 * against --func on the original asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s AND
 * against --whole on the single-function .s the split produces.
 *
 * INSTALL AT: src/overlays/rom_7bc690/ovl_4e4_c_c_c_b.c  (AFTER the split below)
 * NEEDS A fakematch.txt ROW:  OvlFunc_933_2009874  src/overlays/rom_7bc690/ovl_4e4_c_c_c_b.c
 *   tools/shimcount.py: "register pins : 2  via PIN2".
 *
 * SPLIT SHAPE -- REQUIRED, and it is a TEXT/DATA split as well as a function one.
 *   python3 tools/datacheck.py asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s
 *     data sections : .data
 *     functions     : OvlFunc_933_2009638, OvlFunc_933_2009874
 *     EXPORTS       : Events_TolbiSpring, .L1f48, .L1f70  (already global)
 *     -> converting a function here needs a TEXT/DATA SPLIT
 *     OvlFunc_933_2009874  reads no data label -> split needs NO new export
 *   python3 tools/split_s.py asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s \
 *        OvlFunc_933_2009874 --dry-run
 *     would write ovl_4e4_c_c_c_a.s  (1 function, 257 lines)   <- 2009638
 *     would write ovl_4e4_c_c_c_b.s  (1 function,  27 lines)   <- THIS function
 *     would write ovl_4e4_c_c_c_c.s  (1 function,  14 lines)   <- the .data block
 *     would REMOVE ovl_4e4_c_c_c.s ; would rewrite overlays/rom_7bc690/overlay.ld
 *   So: run the split, confirm `make compare` GREEN, THEN install this .c as
 *   src/overlays/rom_4e4.../ovl_4e4_c_c_c_b.c and delete
 *   asm/overlays/rom_7bc690/ovl_4e4_c_c_c_b.s.
 *
 *   AND -- batch 316's rule -- the split INVALIDATES the `Verify with:` recipe of
 *   the sibling park src/non_matching/ovl_7bc690/2009638.c, which must be
 *   repointed at asm/overlays/rom_7bc690/ovl_4e4_c_c_c_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7bc690/ovl_4e4_c_c_c_b.c \
 *     asm/overlays/rom_7bc690/ovl_4e4_c_c_c_b.s --whole
 *   (before the split, the equivalent is --func OvlFunc_933_2009874 against
 *    asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s)
 *
 * No flag group: objcmp prints no "(built with: ...)" line for this path, so the
 * TU falls to the tree default `asm/%.o: src/%.c` at -O2.
 *
 * ------------------------------------------------------------------
 * THE FIGURE, AND WHAT THE PARK GOT WRONG
 *
 * Measured baseline from the park body: 2 of 20 encodings (19 instructions plus
 * the .short alignment tail), relocations IDENTICAL.  The park says "Nineteen
 * against nineteen, diverging at instruction 4".  The divergence is a single
 * ADJACENT TRANSPOSITION and nothing else:
 *
 *     rom    mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / mov r0,#8  / lsl r1,#8
 *     ours   mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / lsl r1,#8  / mov r0,#8
 *
 * The park is not wrong about WHAT differs.  What it gets wrong is the verdict:
 * it writes the case up as a gap in the arg-interleave FILTER -- "the filter
 * looks two lines back from an lsl for the mov that starts it ... Left as a
 * known gap rather than a bad heuristic".  That is a note about a detector, and
 * it was allowed to stand in place of a diagnosis.  The function is reachable.
 *
 * ------------------------------------------------------------------
 * THE MECHANISM: sched2's LUID tie-break, read off -fsched-verbose=6
 *
 * `-dS -fsched-verbose=6` on the park body gives the whole answer.  In the
 * dependence table all five setup insns are priority 69/68 and the ready list
 * resolves like this (insn 42 = mov r1,#0x80, 43 = lsl r1,#8, 44 = mov r2,#0x80,
 * 45 = lsl r2,#7, 13 = mov r0,#8; the scheduler takes the RIGHTMOST entry):
 *
 *     Ready list (t = 0):  13  44  42   --> 42     prio 69 beats 13's 68
 *     Ready list (t = 1):  13  43  44   --> 44     prio 69
 *     Ready list (t = 2):  13  43  45   --> 45     INSN_DEPEND count 4 vs 3, 3
 *     Ready list (t = 3):  13  43       --> 43     counts tie at 3; LOWER LUID wins
 *     Ready list (t = 4):  13           --> 13
 *
 * So the residue is decided at t=3 by the LUID rung, and the dependent counts
 * that get there are structural:
 *     r2 (insn 45): dependents 18 (call 1 uses r2), 23 (call 2 CLOBBERS r2
 *                   without using it), 29 (call 3 sets r2), 35          = 4
 *     r1 (insn 43): dependents 18 (use), 22 (call 2 sets r1), 35        = 3
 *     r0 (insn 13): dependents 18 (use), 20 (call 2 sets r0), 35        = 3
 * There is no way to give r0 a fourth dependent -- that would need a call
 * between insn 13 and insn 20 that clobbers r0 without using it, and call 1
 * uses it.  So the ONLY route is the LUID: `mov r0,#8` has to be born earlier
 * in the insn chain than `lsl r1,#8`.
 *
 * AND THAT IS WHY EVERY NON-PIN SPELLING IS INERT.  From .00.rtl:
 *     insn  9: (set (reg 32) (const_int 32768))   <- precompute_register_parameters
 *     insn 11: (set (reg 33) (const_int 16384))   <-   hoists the two EXPENSIVE
 *     insn 13: (set (reg r0) (const_int 8))       <-   args ahead of the cheap one
 *     insn 15: r1 = reg32 ; insn 17: r2 = reg33 ; call 18
 * calls.c precomputes an argument whose `rtx_cost (value, SET) > COSTS_N_INSNS
 * (1)`, in ARGUMENT order, BEFORE load_register_parameters emits any of the hard
 * register loads.  `8` is cheap and is never precomputed, so `mov r0,#8` is
 * ALWAYS behind both shift chains in the chain order, and reload's
 * rematerialisation of the two pseudos lands at the pseudos' own positions.
 * Naming the big values as locals does not help either: a VAR_DECL is already a
 * REG so precompute skips it, but its INITIALISER insns sit at the declaration,
 * which is still ahead of the argument load.
 *
 * A HARD-REGISTER PIN IS EXACTLY THE INSTRUMENT FOR THIS: `q0 = 8` is a source
 * statement, so it is emitted where it is written -- chain position 0 -- and it
 * is already in r0, so nothing moves it.  With q0 and q1 pinned the chain is
 * `r0=8 / mov r1,#0x80 / lsl r1,#8 / mov r2.. / lsl r2..`, the t=3 tie is
 * between LUID 0 and LUID 2, and `mov r0,#8` wins.  Exactly the ROM.
 *
 * REACHABILITY WAS ALREADY PROVEN IN THE TREE, by this same idiom and this same
 * callee.  src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_b.c (landed,
 * fakematch.txt:319) uses `#define PIN3 ... register int q2 __asm__("r2")` and
 * its SECOND __MapActor_SetSpeed emits, in the generated
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_b.s:
 *     mov r1,#192 / mov r2,#192 / lsl r2,#8 / mov r0,#1 / lsl r1,#9 / bl
 * -- our target order, instruction for instruction.  A corpus sweep over the
 * generated (gcc-banner) .s files finds the exact six-insn window
 * `mov r1,#k / mov r2,#k / lsl r2 / mov r0,#k / lsl r1 / bl` in 79 places, so
 * the shape is ordinary gcc output, not a shim artefact.
 *
 * ------------------------------------------------------------------
 * THE CROSS.  Each pin alone fails; two of them together land.  (ndiff of 20,
 * and WHERE it differs, because two variants read 2 at different places.)
 *
 *     no pin                               2   at [4,5]    the park
 *     q0 pinned to r0 only                 2   at [1,2]    moved, not fixed
 *     q1 pinned to r1 only                 2   at [4,5]    EXACTLY INERT
 *     q2 pinned to r2 only                 4   at [1,2,4,5]  worse
 *     q0 + q2                              2   at [1,2]
 *     q1 + q2                              2   at [4,5]    EXACTLY INERT
 *     q0 + q1            (THIS FILE)       0               LANDS
 *     q0 + q1 + q2                         0               lands, one pin spare
 *
 * The {q0} row is a same-count DIFFERENT-PLACE result and the {q1} row is
 * exactly inert; singly neither is a lead, and a one-at-a-time sweep stops at
 * "2, nothing helps".  q2's pin is measurably surplus, so the fixpoint is TWO
 * pins and PIN2 is what ships.
 *
 * MEASURED INERT OR WORSE, pin-free (all 20 encodings, none below 2):
 *     bare literals (the park)                            2 at [4,5]
 *     `int a=0x80<<8,b=0x80<<7;` named, in decl order     2 at [4,5]
 *     the same two declared in the OTHER order            4 at [1,2,4,5]
 *     only b named / only a named                     4 at [1,2,4,5] / 2 at [4,5]
 *     `int s=8` reused as the slot of all four calls      2 at [4,5]
 *     `int s=8` plus named a,b                            2 at [4,5]
 *     0x8000 / 0x4000 written as plain hex                2 at [4,5]
 *     `0x80u << 8` (unsigned literals)                    2 at [4,5]
 *     prototype args widened to `unsigned`                2 at [4,5]
 *     a,b in a nested block around the call               2 at [4,5]
 *     a,b in two NESTED blocks                            2 at [4,5]
 *     `int a,b; a=..; b=..;` as separate statements       2 at [4,5]
 *     declared-return-type lever, SetSpeed -> int    2 at [4,5] (inert, x3 crosses)
 *     declared-return-type lever, Func_80921c4 -> int     5  worse
 *     declared-return-type lever, SetAnim -> int          6  worse
 *     this function declared `int` instead of `void`      4  worse (pop {r1}/bx r1)
 * The `void` return is CORRECT and is itself a reading: the ROM's epilogue is
 * `pop {r0} / bx r0`, so r0 is free as the epilogue scratch -- a value-returning
 * function keeps r0 reserved and takes r1, which is the sibling
 * OvlFunc_933_20084e4's shape, not this one's.
 */
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80921c4(int slot, int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

void OvlFunc_933_2009874(void)
{
    { PIN2; q0 = 8; q1 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, 0x80 << 7); }
    __MapActor_SetAnim(8, 1);
    __Func_80921c4(8, 0xa8, 0x60);
    __MapActor_SetAnim(8, 2);
}
