/* Field_Move  --  0x0809802c, asm/rom_8a000/rom_97b54_a_c_a_a.s
 *
 * BLOCKER CLASS: scheduling -- where the prologue's `sub sp` lands.
 * Status: 25 lines against the ROM's 25, TWO transposed, everything else exact.
 *
 * WHAT IT DOES
 * The field "Move" psynergy entry point. Reads the caster out of the block at
 * [iwram_3001f30]+0x10, runs the setup call, hands the caster to Func_8098070,
 * and if that yields an actor plays animation 4 on it and waits thirty frames
 * before the two teardown calls.
 *
 * THE WHOLE DIFFERENCE
 *      rom   ldr r3, [r3] / sub sp, #0xc / ldr r5, [r3, #0x10] / bl
 *      ours  ldr r3, [r3] / ldr r5, [r3, #0x10] / sub sp, #0xc / bl
 *
 * gcc emits the prologue as `push` then `sub sp`, and the post-reload scheduler
 * then hoists body instructions above the `sub sp` -- it has no dependency on
 * anything. The ROM hoisted two loads over it; we hoist three. Nothing in the
 * source orders an instruction against the frame adjustment.
 *
 * THE TWELVE BYTES ARE REAL AND ARE NEVER USED. Nothing is stored to the frame
 * and nothing takes its address; the function allocates 0xc bytes and returns.
 * An unused `char buf[12]` reproduces it exactly -- gcc-2.96 does not remove a
 * declared local array at -O2 -- and without it the function is two
 * instructions short. Same shape as the dead stack buffer in Func_80b606c,
 * except that one is at least written to.
 *
 * WHAT WAS TRIED
 *   - the array declared before and after the other locals: identical output
 *   - `volatile char buf[12]`: identical output (the frame is already there;
 *     volatile changes nothing because nothing accesses it)
 *   - the global deref split into a named intermediate pointer: identical
 *   - `--no-sched2`: WORSE, 3 of 25 diverging at instruction 1
 *   - `-fno-strict-aliasing`: identical
 *   - `--O1`: worse, 3 of 25
 *
 * The `--no-sched2` result is the informative one: turning the scheduler off
 * does not put the `sub sp` back where the ROM has it, it just breaks something
 * else. So this is not simply "the scheduler moved one instruction too many" --
 * the ROM's stream is not gcc's unscheduled stream either.
 */

extern char *iwram_3001f30;
extern void Func_8097384(void);
extern void *Func_8098070(void *a);
extern void Func_8098184(void);
extern void _Actor_SetAnim(void *a, int n);
extern void WaitFrames(int n);
extern void Func_809748c(void);
extern void Func_80981b0(void *a);

void Field_Move(void)
{
    char buf[12];
    void *a;
    void *r;

    a = *(void **)(iwram_3001f30 + 0x10);
    Func_8097384();
    r = Func_8098070(a);
    Func_8098184();
    if (r != 0) {
        _Actor_SetAnim(r, 4);
        WaitFrames(0x1e);
    }
    Func_809748c();
    Func_80981b0(r);
}

/* ==================== CLOSED IN BATCH 271: THE SOURCE CANNOT REACH THIS ====================
 *
 * Still 3 lines / 2 encodings, and the residue is exactly the adjacent
 * transposition described above. What is new is that it is now decided from the
 * scheduler rather than guessed at, and the answer rules the source out.
 *
 * sched2 ready lists for BB0:
 *
 *     t=0  ready {77 (sub sp), 10}  -> 10   ldr r3, =.LC0      (occupies t0-1)
 *     t=2                           -> 12   ldr r3, [r3]       (t2-3)
 *     t=4  ready {77, 14}           -> 14   ldr r5, [r3+0x10]  <- ROM picks 77 here
 *     t=6                           -> 77   sub sp, #0xc
 *     t=7                           -> 16   bl Func_8097384
 *
 * At t=4 both are ready and the scheduler ranks by priority, the longest path to
 * the end of the block. On arm7tdmi the ldsched model gives a load a 2-cycle
 * result cost, while arm_adjust_cost's "call insns do not incur a stall" rule
 * makes any true dependence INTO a CALL_INSN cost 1. So
 *
 *     priority(14) = 2 + priority(mov r0, r5)
 *     priority(77) = 1 + priority(bl) = 1 + priority(mov r0, r5)
 *
 * -- the bl -> mov r0,r5 link being an output/anti dep of cost 0. `sub sp`
 * therefore loses by EXACTLY ONE UNIT, structurally, for any source that loads
 * the caster from memory. Nothing in C changes either term: the frame size is
 * fixed at 0xc, `sub sp` has no successor but the first call, and the load's only
 * consumer is the argument `mov`.
 *
 * A TIE DOES NOT HELP EITHER. Replacing the load with `add r5, #0x10` (cost 1, so
 * the priorities tie) breaks the tie toward the LARGER LUID -- still not `sub sp`.
 * To win, `sub sp` would need priority strictly greater, i.e. +2, which is
 * unreachable.
 *
 * MEASURED THIS ROUND, all byte-identical to the baseline at 2 encodings:
 * `int buf[3]` instead of `char buf[12]`; a `struct F { int a, b, c; }` instead of
 * the array; the array declared after the locals with a dead initialiser;
 * volatile on the global (`*(char * volatile *)&iwram_3001f30`); volatile on the
 * slot; and `#define G (*(char **)0x3001f30)`, an absolute address instead of an
 * extern symbol.
 *
 * WORSE: a VLA (`int n = 12; char buf[n];`) at 28 lines and 26 differing, with
 * `push {r5, r6, r7, lr}` and two `mov rX, sp`. That was the only construct found
 * that puts a stack adjust mid-body, and it costs a frame pointer.
 *
 * DO NOT SWEEP THIS AGAIN. If it is ever to move it needs a compiler-side change,
 * not a spelling.
 */
