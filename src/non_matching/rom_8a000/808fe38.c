/* Func_808fe38 -- 0x0808fe38 -- asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s
 *
 * NON-MATCHING, 11 of 50 encodings  (MEASURED, batch 323 brief E).
 *   COUNT EXACT  (ref 50, ours 50).  SIZE EXACT (120 bytes).
 *   RELOCATIONS EXACT -- all five, at the ROM's offsets.
 *   So THE FIGURE IS A TRUE DISTANCE, and it is the first time this park has
 *   had one: the installed body read 18 at 52 instructions and 124 bytes, i.e.
 *   its figure was measuring MISALIGNMENT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/808fe38.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s --func Func_808fe38
 *
 * PINS: 0.  DEVICES: 0.  Default flags.  No split: this is a park, one
 * function out of a multi-function reference, so --whole does not apply.
 *
 * Allocates a 0x540-byte block, DMA-clears it, writes four fields, and starts
 * two tasks.  Found via tools/shapesib.py against Func_8090824
 * (src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_b.c, matching, same directory), which
 * supplied the allocation, the DMA3_CLEAR and the `(0xa5 << 3)` field write.
 *
 * ========================================================================
 * WHAT BATCH 323 CHANGED, and the two things it closed
 * ========================================================================
 *
 * The 18 was THREE causes, not the one the park named.
 *
 * CAUSE A -- THE 0x3f3f STORE WAS THE SAME DEFECT THE `one` LEVER FIXES.
 *
 * The park correctly found that `1` stored through a halfword pointer pools
 * (`ldr r3, =0x1` against the ROM's `mov r3, #0x1`) and that an `int` carrier
 * fixes it.  It did not notice that 0x3f3f, two lines up, has the SAME
 * problem -- and the reason it is invisible is that 0x3f3f pools EITHER WAY,
 * so the instruction looks right.  What is wrong is the pool-fix MODE:
 *
 *   `*thumb_movhi_insn` alternative 1 takes `mn`, and `n` matches ANY
 *   const_int, so recog reaches the POOL before alternative 5's `I`.  Every
 *   HImode literal pools, `1` included.  A HImode fix's pool_range is 64
 *   (arm.md:4353) against movsi's 1020, and add_minipool_forward_ref
 *   (arm.c:4817) keeps the pool sorted by `fix->address + fix->forwards` --
 *   so a HImode fix SORTS AHEAD OF EVERY SImode FIX IN THE FUNCTION.
 *
 * That one fix did all of this:
 *   - moved the 0x3f3f pool word to the FRONT of the pool, changing two
 *     `ldr rN,[pc,#imm]` offsets (indices 25, 27);
 *   - forced the pool to be DUMPED BEFORE THE EPILOGUE (64 bytes of reach is
 *     not enough to put it after), so gcc emitted `b .L0` around it plus a
 *     2-byte alignment `nop`: +2 encodings, +4 bytes, and the last two
 *     relocations shifted by 4 (indices 40-51).
 *
 * `hi = 0x3f3f;` immediately before the store takes it to 15 of 50 with the
 * count, the size and all five relocations EXACT.  Its placement obeys the
 * same precondition the park recorded for `one` -- the carrier's live range
 * has to stay SHORT:
 *
 *     hi = 0x3f3f; immediately before the store      15   <- kept
 *     hi = 0x3f3f; as the first statement            19   (49 insns)
 *     one reused for 0x3f3f and then for 1           19
 *     both as declaration initialisers               58   (59 insns)
 *     only hi an int (1 left a literal)              27   (53 insns)
 *     only one an int (the installed park body)      18   (52 insns)
 *
 * `unsigned short hi` is identical; declaration order of hi/one is inert.
 *
 * CAUSES B and C -- THE ARGUMENT FILL ORDER AT BOTH StartTask CALLS. CLOSED.
 *
 * The park declared this unreachable and said so explicitly: "gcc
 * rematerialises argument temporaries during fill and discards any statement
 * structure the source imposes.  Four spellings were measured against that
 * boundary on ovl_780898/2008fec and came back byte-identical; it is not
 * re-tested here."
 *
 * It is reachable.  NAME THE PRIORITY ARGUMENT IN AN int LOCAL, assigned as
 * its own statement before each call.  15 -> 11, and both call sites go
 * exact:
 *
 *     rom/ours now   mov r1, #0xc8 / strh / lsl r1, #0x4 / ldr r0, =Task_...
 *
 * The rule is the same one the carrier placement obeys: PUT THE STATEMENT
 * WHERE THE ROM MATERIALISES THE VALUE.  The ROM builds the priority BEFORE
 * the pooled function address, so the priority is the operand that has to be
 * a statement.  Measured: naming the FUNCTION POINTER instead is inert (15);
 * writing the priorities as plain literals 0xc80/0x480 is inert (15).  One
 * local reused for both calls is enough.
 *
 * (A bound wrongly recorded closed costs every future agent the class.  This
 * one was propagated from a different function in a different bank and was
 * never tested here.)
 *
 * ========================================================================
 * WHAT REMAINS: 11 encodings, ONE cause, indices 16-26, NOT source-reachable
 * ========================================================================
 *
 *     rom    movs r2, #0xa5 / lsls r2, #3 / adds r3, r4, r2 / adds r2, #2 ...
 *     ours   movs r1, #0xa5 / lsls r1, #3 / ldr r2, =0x52a  / adds r3, r4, r1 ...
 *
 * The ROM puts the `0xa5 << 3` constant (0x528) in r2; we put it in r1.  That
 * single register choice decides which constant reload_cse_move2add
 * (reload1.c:8840) can derive, because it needs both values in ONE hard
 * register:
 *
 *   ROM:  0x528 in r2  ->  `adds r2, #2` derives 0x52a, and 0x534 is POOLED
 *   ours: 0x528 in r1  ->  `adds r1, #12` derives 0x534, and 0x52a is POOLED
 *
 * so the same two words are in the pool in the opposite order (index 45), and
 * ten instructions rotate behind it.  REG_ALLOC_ORDER is {3,2,1,0,...}
 * (recorded on Field_Whirlwind); the `hi` carrier is a local-alloc quantity
 * that takes r2 and pushes reload's scratch for 0x528 down to r1.  In the ROM
 * BOTH carriers live in r3, with disjoint ranges.
 *
 * MEASURED INERT, all exactly 11 -- nine spellings, which is the "the pairing
 * is the finding" plateau:
 *     a named pointer for the 0x534 store only                11
 *     a named pointer for the 0x536 store only                11
 *     a named pointer for the arg0 store only                 11
 *     a named pointer for the 0x52a store only                11
 *     an advancing pointer across the arg0/0x52a pair         11
 *     an int carrier for the halfword zero as well            11
 *     `unsigned char *p` base instead of (unsigned int)p      11
 *     swapping which carrier holds which value                11
 *     register int hi __asm__("r2")                           11
 * MEASURED WORSE: `hi = one;` chained from the other carrier (15); a third
 * store through `one` to raise REG_N_REFS (37 at 53 insns -- it changes the
 * program).
 *
 * DEVICE FIGURE, kept as a figure ABOUT THIS BLOCKER and not shipped:
 * `register int hi __asm__("r3")` reads 8 of 50.  It confirms the mechanism
 * is the carrier's hard register and it is DIRECTIONAL -- r3 helps, r2 is
 * inert, pinning `one` instead is inert.  It is a pin and it does not finish
 * the job, so the body below is pin-free at 11.
 *
 * POOL CHECK: `<base>.c.26.mach` for this body lists seven fixes, all SImode,
 * all range 1020, in exactly the ROM's order.  Cause A is closed at the pool
 * level, which is what makes the 11 a distance.
 */
#include "dma.h"

extern void *galloc_ewram(int index, unsigned int size);
extern void StartTask(void (*task)(void), unsigned int priority);
extern void Task_ScreenWindowTransition(void);
extern void Func_808f498(void);

void Func_808fe38(unsigned int arg0)
{
    void *p;
    int hi;
    int pr;
    int one;

    p = galloc_ewram(0x1f, 0xa8 << 3);
    DMA3_CLEAR(p, 0xa8 << 3);
    *(unsigned short *)((unsigned int)p + (0xa5 << 3)) = arg0;
    *(unsigned short *)((unsigned int)p + 0x52a) = 0;
    hi = 0x3f3f;
    *(unsigned short *)((unsigned int)p + 0x534) = hi;
    one = 1;
    *(unsigned short *)((unsigned int)p + 0x536) = one;
    pr = 0xc8 << 4;
    StartTask(Task_ScreenWindowTransition, pr);
    pr = 0x90 << 3;
    StartTask(Func_808f498, pr);
}
