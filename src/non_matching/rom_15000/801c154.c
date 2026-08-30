/* Func_801c154  @ 0x0801c154  [rom_15000]
 *
 * Source asm: goldensun/asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a.s
 *
 * BLOCKER CLASS: pool PLACEMENT (not pool contents), plus a register rename.
 * 13 lines against 15.
 *
 * THIS IS A DATA POINT ON THE 516-FUNCTION BRANCH-OVER-POOL CLASS, and it
 * refines the correction rather than contradicting it. The class was written
 * off on old_agbcc's behaviour, which is irrelevant here; the ceiling claim is
 * withdrawn and one function has been elevated straight out of the class. But
 * this function shows what the residue actually is.
 *
 * THE POOL CONTENTS REPRODUCE PERFECTLY FROM PLAIN C. The ROM pools two
 * constants, 0x1ff and 0xfffffe00, and both appear correctly once the mask is
 * forced into INT width:
 *
 *     int t = *(unsigned short *)(p + 6);
 *     *(unsigned short *)(p + 6) = (t & ~0x1ff) | a;
 *
 * Written directly on the halfword, `& ~0x1ff` narrows to 0xfe00 and gcc
 * builds it with `mov #0xfe / lsl #8` instead of pooling it -- 14 differing.
 * With the int temporary both constants pool and the whole tail from
 * `mov r1, #0xfc` onward matches instruction for instruction.
 *
 * WHAT DOES NOT REPRODUCE IS PLACEMENT. The ROM puts its pool BEFORE the
 * epilogue and jumps over it:
 *
 *     bl Func_8003dec / b .L1c178 / .pool_aligned / .L1c178: / pop {r0} / bx r0
 *
 * gcc puts the pool after the function and needs no branch, so we are exactly
 * the `b` and its label short. That `b` is a real instruction and it is
 * counted.
 *
 * *** CORRECTED, SAME BATCH: THE READING BELOW IS WRONG. ***
 *
 * An independent screen of four other pool-class functions answered this
 * directly, and the answer is that POOL PLACEMENT IS NOT A RESIDUE AT ALL.
 * On StartSnow and AnimEnd, gcc-2.96 emitted the branch-over-pool
 * instructions at the ROM's own positions with NO help -- and on AnimEnd the
 * two sites were ABSENT while the diff stood at 64, then APPEARED
 * SPONTANEOUSLY and matched once the instruction count converged.
 *
 * The pool branch is a consequence of CODE LENGTH, not of a source construct.
 * So on this function the missing `b` is not the blocker; it is downstream of
 * being two instructions short for some other reason, and the real residue is
 * the register rename below. `.pool_aligned` in a reference is neither a
 * ceiling nor a signal. Screen the class normally.
 *
 * The superseded reasoning is kept below because the size hypothesis was a
 * reasonable inference from one function and someone will form it again:
 *
 * SO THE REFINED READING, worth testing on the rest of the class: pool
 * CONTENTS are reachable from ordinary C, and pool PLACEMENT is the open
 * question. gcc dumps a pool early when it must -- under branch-range pressure
 * in a long function -- and this function is fifteen instructions, so it never
 * has to. That would explain both facts at once: why the one elevated example
 * came out of this class with no special handling, and why this one cannot.
 * If that holds, the class splits by SIZE rather than being uniformly open or
 * uniformly shut, and the large members are the reachable ones.
 *
 * Also remaining: a three-register rename in the first six instructions (the
 * ROM keeps the mask constants in r3 and the loaded halfword in r4; we use r4
 * and r1). Not worth chasing while the placement question is open.
 *
 * Its neighbour Func_80b09fc in asm/rom_b0000/rom_b0070_a_a_c_c_a_a.s is the
 * same story at 14 lines against 16: the body matches, the missing two lines
 * are the same `b` and label, and its pooled ZERO (`ldr r6, =0x0`) does not
 * reproduce from a named int local either -- gcc emits `mov r6, #0`.
 */
extern void Func_8003dec(unsigned char *p, int n);

void Func_801c154(unsigned char *p, int a, int b)
{
    int t;

    a &= 0x1ff;
    t = *(unsigned short *)(p + 6);
    *(unsigned short *)(p + 6) = (t & ~0x1ff) | a;
    p[4] = b;
    Func_8003dec(p, 0xfc);
}
