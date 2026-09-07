/* OvlFunc_955_2008714 -- 0x02008714
 *
 * NO fakematch: no pins, no barriers, no scaffolding.
 *
 * The only lever is the DOMINATING-BLOCK NAMING of the three z coordinates the
 * eight __Actor_TravelTo calls share.  As literals they are commoned by cse --
 * `mov r6,#0xb8 / lsl r6,#16` once and `mov r3, r6` at three of the four sites,
 * which costs two callee-saved registers and 86 differing.  Assigned to locals
 * at the top of the `L4838[0] == 0` block, ahead of the `L4834[0]` branch that
 * dominates every use, they are REMATERIALISED at each call instead: the ROM's
 * own `mov r3,#0xb8 / lsl r3,#16` per site, and nothing is left to common.
 * Rematerialising also moves the build from the assignment to the ARGUMENT
 * LOAD position, which is what puts `mov r2, #0` between the two `lsl`s the
 * way the ROM has it -- the same INSN_LUID effect the pins buy elsewhere,
 * bought here for free because there is a branch to name across.
 *
 * Naming the four x coordinates as well is exactly inert (also 0 of 207); they
 * are all distinct, so there was never anything for cse to common.  Left out.
 *
 * Two other things the listing decides and guessing gets wrong:
 *   - the two `abs(delta) > 0x9ffff` guards are `if (d >= 0) ... else ...`.
 *     Spelled `if (d < 0)` gcc inverts the branch and the arms swap: 104 of 207.
 *   - the fifth and sixth arguments of the first two __Func_8010704 calls are a
 *     PAIR OF LOCALS, one pair per site (`m`, `n`), which is what emits both
 *     `mov`s before both `str`s.  As literals gcc interleaves mov/str/mov/str:
 *     86 of 207.  The THIRD and FOURTH calls are different -- there the ROM
 *     builds 0x3a once into r5 and shares it across both calls while rebuilding
 *     0xb twice, so only the 0x3a is a local there.  Only the listing says
 *     which; see batch 205 on a repeated value built BOTH ways.
 */
extern unsigned char gState[];
extern unsigned int L4834[] __asm__(".L4834");
extern unsigned int L4838[] __asm__(".L4838");
extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int flag);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_2008714(void)
{
    unsigned char *base;
    unsigned char *hero;
    unsigned char *p;
    int i;
    int a, b, d;
    unsigned int v;

    base = gState;
    hero = __MapActor_GetActor(*(int *)(base + (0xfa << 1)));
    for (i = 0x16; i <= 0x19; i++) {
        p = __MapActor_GetActor(i);
        *(p + 0x5b) = 0;
        a = *(int *)(p + 8);
        b = *(int *)(hero + 8);
        d = a - b;
        if (d >= 0) {
            if (d > 0x9ffff)
                continue;
        } else if (b - a > 0x9ffff) {
            continue;
        }
        a = *(int *)(p + 0x10);
        b = *(int *)(hero + 0x10);
        d = a - b;
        if (d >= 0) {
            if (d > 0x9ffff)
                continue;
        } else if (b - a > 0x9ffff) {
            continue;
        }
        if (__GetFlag(0x82 << 1))
            *(int *)(hero + 0x10) = *(int *)(p + 0x10);
        else
            *(int *)(hero + 0x10) = *(int *)(hero + 0x10) + *(int *)(p + 0x2c);
    }
    if (L4838[0] != 0 && *(int *)(p + 0x38) == (0x80 << 24)) {
        if (L4834[0] == 0)
        { int m = 0x3a, n = 0xd; __Func_8010704(0x3a, 0x1c, 7, 1, m, n); }
        else
        { int m = 0x3a, n = 0xb; __Func_8010704(0x3a, 0xa, 1, 1, m, n); }
    } else {
        int s = 0x3a;
        __Func_8010704(0x39, 0xb, 1, 1, s, 0xb);
        __Func_8010704(0x3a, 0xe, 7, 1, s, 0xd);
    }
    if (L4838[0] == 0) {
        int zb = 0xb8 << 16, zf = 0xf8 << 16, zd = 0xd8 << 16;
        L4834[0] ^= 1;
        if (L4834[0] != 0) {
            __Actor_TravelTo(__MapActor_GetActor(0x16), 0xea << 18, 0, zb);
            __Actor_TravelTo(__MapActor_GetActor(0x17), 0xf2 << 18, 0, zf);
            __Actor_TravelTo(__MapActor_GetActor(0x18), 0xfa << 18, 0, zb);
            __Actor_TravelTo(__MapActor_GetActor(0x19), 0x81 << 19, 0, zf);
            __MapActor_SetAnim(0x1f, 0xb);
        } else {
            __Actor_TravelTo(__MapActor_GetActor(0x16), 0xea << 18, 0, zd);
            __Actor_TravelTo(__MapActor_GetActor(0x17), 0xf2 << 18, 0, zd);
            __Actor_TravelTo(__MapActor_GetActor(0x18), 0xfa << 18, 0, zd);
            __Actor_TravelTo(__MapActor_GetActor(0x19), 0x81 << 19, 0, zd);
            __MapActor_SetAnim(0x1f, 0xa);
        }
    }
    v = L4838[0] + 1;
    L4838[0] = v;
    if (v > 0x77) {
        if (__GetFlag(0x82 << 1) == 0)
            L4838[0] = 0;
    }
}
