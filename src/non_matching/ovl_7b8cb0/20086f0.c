/* OvlFunc_931_20086f0 -- 0x020086f0  (asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c.s)
 *
 * TWIN: OvlFunc_932_2009770 (asm/overlays/rom_7b9cb4/...) is the same function
 * over a different actor record -- same three halfword state machines, same
 * two __Func_8012330 calls, same 0x5b flag write. Solving one solves both, so
 * this park covers two functions.
 *
 * BLOCKER: which register holds the SIGNED and which the UNSIGNED read of the
 * same halfword. 17 of 96, exact length. Written from scratch this round;
 * 83 differing at the first screen.
 *
 * THE PROGRESSION, each step a documented lever:
 *
 *   83  naive
 *   68  a separate local for the decremented value (the ROM's `sub r3, r2, #1`
 *       is three-operand, so the result is not the same variable as the source)
 *       plus the two -1 arguments named in the entry block
 *   25  three at once, and all three were readable from the diff:
 *         - __Actor_SetAnim's prototype deleted (fill order)
 *         - `*p = 4` routed through an `int` local -- a bare literal into a
 *           HALFWORD store gets POOLED (`ldr r3, =0x4` against `mov r3, #4`)
 *         - two separate locals for the repeated 0x80 << 9 argument pair,
 *           which we were sharing in one register
 *   24  the same int-local treatment for the `*p = 0` store
 *   17  READ THE UNSIGNED VALUE BEFORE THE SIGNED ONE  <- best, scratch/o931f.c
 *
 * That last step is the reusable one. The ROM reads the same halfword twice --
 * `ldrsh r3, [r6, r1]` then `ldrh r2, [r6, #0]` -- and emits them in that
 * order regardless. But writing `v = *p;` BEFORE `s = *(short *)p;` in the
 * SOURCE changes the register assignment even though gcc schedules the loads
 * back into the ROM's order. Source order picks registers for two independent
 * values; it does not pick the emission order here.
 *
 * WHAT REMAINS is that assignment, still one slot off, at all three sites:
 *
 *     rom   ldrsh r3, [r6, r1] / ldrh r2, [r6, #0] / cmp r3, #0
 *     ours  ldrsh r2, [r6, r1] / ldrh r3, [r6, #0] / cmp r2, #0
 *
 * and the two three-operand arithmetic sites (`sub r3, r2, #1`,
 * `add r3, r2, #1`) that follow from it -- with the signed value in the other
 * register, gcc has nowhere to put the result but back into the source.
 *
 * MEASURED AND WORSE: testing the signed read directly in the `if` and moving
 * the unsigned read inside the guarded block -- the lazy form that reads most
 * naturally -- is 99 differing and 101 lines. Both reads must be up front, and
 * that is what the ROM's two loads before the `cmp` are telling you.
 *
 * Also unresolved, three instructions: the ROM interleaves `str r7, [r5, #0x68]`
 * into the two `lsl #9` builds for the __Func_8012330 arguments, and the two
 * `neg` instructions come before the pooled 0xe666 rather than after. Both are
 * the straight-line interleave class -- the site has a guard above it but the
 * entry-block naming is already applied (m1/m2/q1/q2) and does not move them.
 */
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);

void OvlFunc_931_20086f0(unsigned char *a)
{
    unsigned short *p;
    int s;
    int v;
    int t;
    int lim;
    int w;
    int m1;
    int m2;
    int four;
    int q1;
    int q2;
    int u;
    int zero;

    q1 = 0x80 << 9;
    q2 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    p = (unsigned short *)(a + 0x66);
    v = *p;
    s = *(short *)((char *)p + (unsigned int)0);
    if (s != 0) {
        w = v - 1;
        *p = w;
        if ((short)w == 1)
            __Func_8012330(m1, m2, 0xe666);
    }
    t = *(int *)(a + 0x28);
    if (t == 0) {
        __Actor_SetAnim(a, 1);
        v = *(int *)(a + 0xc) + 0xfffe8000;
        lim = *(int *)(a + 0x14);
        *(int *)(a + 0xc) = v;
        if (v < lim) {
            if (*(int *)(a + 0x68) != 0) {
                __PlaySound(0xe5);
                *(int *)(a + 0x68) = t;
                four = 4;
                *p = four;
                __Func_8012330(0, q1, q2);
                lim = *(int *)(a + 0x14);
            }
            *(int *)(a + 0xc) = lim;
        }
        a[0x5b] = 1;
    } else {
        a[0x5b] = 0;
    }
    p = (unsigned short *)(a + 0x64);
    v = *p;
    s = *(short *)((char *)p + (unsigned int)0);
    if (s == 0) {
        __PlaySound(0x98);
        *(int *)(a + 0x68) = 1;
        __Actor_SetAnim(a, 2);
        *(int *)(a + 0x28) = 0xc0 << 10;
        v = *p;
    }
    u = v + 1;
    *p = u;
    if ((short)u == 0x3c) {
        zero = 0;
        *p = zero;
    }
}
