/* OvlFunc_955_2008970 -- 0x02008970
 *
 * Countdown loop: waits ten frames, then polls two scratch words once per
 * frame until they reach their targets or the attempt limit runs out.
 *
 * // fakematch: one EMPTY volatile asm.  It emits no bytes and is a
 * SCHEDULING BARRIER, which is all it is here for.  Without it the post-reload
 * scheduler hoists `mov r5, #0` into the load-use slot between `ldr r3, =L4834`
 * and `ldr r3, [r3]`; that leaves the entry block and the loop-back block
 * ending in the SAME instruction, and jump.c's cross-jumping then sinks that
 * one `ldr r3, [r3]` into a shared successor -- one instruction short of the
 * ROM, which dereferences on both paths.  The barrier pins the counter's
 * initialisation below the load, the two block tails stop matching, and there
 * is nothing left to cross-jump.  25 differing of 28 without it, 0 with it.
 */
extern unsigned char L4834[] __asm__(".L4834");
extern unsigned char L4838[] __asm__(".L4838");
extern void __WaitFrames(int n);

void OvlFunc_955_2008970(void)
{
    int v;
    int i;
    int lim;

    __WaitFrames(0xa);
    v = *(int *)L4834;
    __asm__ __volatile__("");
    i = 0;
    goto test;
loop:
    __WaitFrames(1);
    i++;
    lim = 0x96 << 2;
    if (i >= lim)
        goto done;
    v = *(int *)L4834;
test:
    if (v != 0)
        goto loop;
    if (*(int *)L4838 != 0x4b)
        goto loop;
done:
    ;
}
