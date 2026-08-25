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
