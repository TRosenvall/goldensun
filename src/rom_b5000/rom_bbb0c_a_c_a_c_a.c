/* Func_80be02c -- 0x080be02c, from asm/rom_b5000/rom_bbb0c_a_c_a_c_a.s.
 *
 * MATCHING -- 68 bytes, 29 encodings, 5 relocations identical, --func AND
 * --whole.  The park read 16 of 29.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_bbb0c_a_c_a_c_a.c \
 *     asm/rom_b5000/rom_bbb0c_a_c_a_c_a.s --whole
 *
 * SPLIT SHAPE.  NONE NEEDED -- rom_bbb0c_a_c_a_c_a.s holds exactly one
 * .thumb_func_start, and tools/datacheck.py on it is silent (no data section).
 * Pins: 0.  No exports.  No per-file flag group.
 *
 * THE PARK REPORTED ONE FIGURE FOR TWO INDEPENDENT CAUSES.  Decomposing its 16
 * by index -- the streams are the SAME LENGTH, 27 instructions plus 2 pool
 * words each, so 16 was a true distance -- gives:
 *
 *   indices 2-7, 9-11, 13-15, 18-19  (14)  a three-way r1/r2/r3 permutation
 *   indices 25-26                     (2)  `pop {r1} / bx r1` vs `pop {r0} / bx r0`
 *
 * The park saw only the first and called the whole 16 "register allocation ...
 * r1/r2/r3 are permuted throughout and each permuted line counts".
 *
 * CAUSE 2, THE EPILOGUE, IS A RETURN TYPE.  arm.c's thumb_exit picks the
 * register to pop the return address into from the FUNCTION'S RESULT MODE:
 * "In a void function we can use any argument register", so a `void` function
 * pops into r0 and a value-returning one cannot and pops into r1.  The ROM pops
 * r1, so Func_80be02c returns a value -- and the landed
 * src/rom_b5000/rom_bb588_c_c_a_a.c already declares `extern int
 * Func_80bdfec(void)` and ends `return Func_80bdfec();`, the same tail.  Worth
 * exactly 2 encodings, 16 -> 14, and it is INVISIBLE in the permutation count.
 *
 * CAUSE 1 WAS A STATEMENT ORDER INSIDE THE `if`, AND ONLY CROSSING FOUND IT.
 * `*q = 1; v = 1;` for `v = 1; *q = v;` closes all 14 at once.  One-at-a-time
 * it is not reachable: on the `void` body it is worth NOTHING, and the park had
 * already measured the statement-order lever at the TOP of the function in both
 * directions and recorded it inert ("the ordering gcc chooses here is not
 * coming from the source").  It only appears crossed with the return type.
 * tools/crossfire.py found it at depth 2.
 *
 * WHY IT MOVES THE WHOLE PERMUTATION, from .18.greg and the compiler source.
 * The park's body makes the store read `v`, which ties the stored pseudo to the
 * loaded one; writing the literal leaves them separate and drops one allocno.
 * With the shorter allocno list the global allocator runs on `36 34 35 32`,
 * ARM's REG_ALLOC_ORDER is `{3, 2, 1, 0, ...}` (arm.h:989) and the undistorted
 * assignment is v->r3, q->r2, p->r1 -- the ROM's.
 *
 * The park's body loses that to a preference nobody had named.  global.c's
 * set_preference does `if (GET_RTX_FORMAT (GET_CODE (src))[0] == 'e') src =
 * XEXP (src, 0)` -- it STRIPS ONE LEVEL -- so `(set (reg P) (mem (reg A)))`
 * donates A's hard register to P as a preference.  The pool-address pseudo for
 * `iwram_3001e74` is local-allocated to r3, so `p` acquired `preferences: 3`;
 * prune_preferences then publishes that as `regs_someone_prefers` and find_reg
 * ORs it into `used` (global.c:1016), so `v` -- allocated FIRST -- was pushed
 * off r3 onto r2 and the whole permutation followed.
 *
 * MEASURED AND WORSE: inlining the global at all three uses (24, RELOC); `q`
 * without the `k` temporary (24, RELOC COUNT); `r = q` instead of recomputing
 * the offset (24, RELOC COUNT -- the ROM really does recompute `0x80 << 4`);
 * polling `*r` without naming `v` (12).  EXACTLY INERT: declaration order,
 * `int k` for `unsigned int k`.
 */
extern unsigned char *iwram_3001e74;
extern void Func_80bd898(void);
extern void StopTask(void (*fn)(void));
extern int Func_80bdfec(void);
extern void WaitFrames(int n);

int Func_80be02c(void)
{
    unsigned char *p;
    unsigned int k;
    int *q;
    int *r;
    int v;

    p = iwram_3001e74;
    k = 0x80 << 4;
    q = (int *)(p + k);
    v = *q;
    if (v == 0) {
        *q = 1;
        v = 1;
    }
    if (v == 4)
        goto done;
    k = 0x80 << 4;
    r = (int *)(p + k);
    do {
        WaitFrames(1);
        v = *r;
    } while (v != 4);
done:
    StopTask(Func_80bd898);
    return Func_80bdfec();
}
