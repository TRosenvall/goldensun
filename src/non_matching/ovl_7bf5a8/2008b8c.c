/* OvlFunc_935_2008b8c  [ovl_7bf5a8]  --  0x02008b8c
 *
 * Source asm: goldensun/asm/overlays/rom_7bf5a8/ovl_b8c_a.s
 *
 * Spawns up to four actors and configures each. TWO instructions out of
 * fifty-one, and the two are one displacement:
 *
 *     rom    mov r0, r5 / ldr r1, =gScript_935__02009884
 *     ours   ldr r1, =gScript_935__02009884 / mov r0, r5
 *
 * That is the pool-load-first shape, which batch 37's BASIC-BLOCK LEVER
 * retires -- and this file is the boundary condition on that lever, which is
 * why it is worth keeping rather than just leaving as assembly.
 *
 * THE LEVER DOES NOT REACH INSIDE A LOOP BODY. It needs the value assigned in a
 * block that dominates the call, and in a loop every such block is also
 * reachable across the BACK EDGE -- so the value is live around the loop and
 * gcc keeps it in a callee-saved register instead of rematerialising. Both
 * placements were tried:
 *
 *     assigned before the loop                    7 of 51  (worse)
 *     assigned in the block that jumps INTO the   9 of 51  (worse)
 *       loop, i.e. the `if (a != 0)` arm
 *     left as a literal at the call site          2 of 51  (this file)
 *
 * Also tried, all 2 of 51: __Actor_SetScript undeclared, its second parameter
 * as `unsigned char *`, and an `int` return on the preceding callee.
 *
 * So the rule from reports/arg-interleave.md needs a clause: the assignment
 * must be in a dominating block that is NOT part of the loop the call sits in.
 * For a call in the only block of a self-contained loop, there is no such
 * block, and the lever has nothing to work with -- the same position the
 * straight-line functions are in, for a different reason.
 *
 * EVERYTHING ELSE IN THIS FILE MATCHES, including three levers that were needed
 * to get here and should be kept in any further attempt:
 *
 *   * the loop is written UN-ROTATED with `goto check;` first, because the ROM
 *     tests before the first iteration (`b .Lbda`);
 *   * the exit falls THROUGH rather than returning -- an early `return` inside
 *     the loop costs an extra `b` and a label;
 *   * `*(short *)q = 8` goes through a named `int`, or gcc pools the 8 as a
 *     halfword constant. That is narrow_constant inverted, fourth sighting.
 */
extern unsigned char gScript_935__02009884[];
extern unsigned int __Random(void);
extern void *__CreateActor(int a, int b, int c, int d);
extern void OvlFunc_935_2008b54(void *a, unsigned int b, unsigned int c);
extern void __Actor_SetScript(void *a, void *s);

void OvlFunc_935_2008b8c(unsigned char *arg)
{
    unsigned char *a;
    unsigned char *q;
    int i;
    unsigned int spd;
    int v;

    spd = 0x80 << 14;
    i = 0;
    goto check;
loop:
    v = 0x8ccc;
    *(int *)(a + 0x1c) = v;
    *(int *)(a + 0x18) = v;
    q = a + 0x55;
    *q = 2;
    *(int *)(a + 0x28) = 0xffff0000;
    *(int *)(a + 0x30) = __Random() + 0xcccc;
    q = a + 0x59;
    *q = 1;
    OvlFunc_935_2008b54(a, spd, __Random());
    q = a + 0x5e;
    v = 8;
    *(short *)q = v;
    __Actor_SetScript(a, gScript_935__02009884);
    i++;
check:
    if (i <= 3) {
        a = (unsigned char *)__CreateActor(0xf0, *(int *)(arg + 8),
                                           *(int *)(arg + 0xc),
                                           *(int *)(arg + 0x10));
        if (a != 0)
            goto loop;
    }
}
