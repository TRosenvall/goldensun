/* Func_80b84c0 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s
 * Best screen: 50 instructions against the ROM's 44.
 *
 * BLOCKER CLASS: one extra callee-saved register. The indirect-call shape is
 * RIGHT -- this is the second use of the `_call_via_rN` lever found in
 * src/rom_c9000/rom_e0524.c, and it does produce the indirect call. It just
 * lands in the wrong register.
 *
 *     rom    ldr r5, =Func_8000888 ... bl _call_via_r5   (pushes {r5,r6,r7,r8})
 *     ours   ldr r3, =Func_8000888 / mov r10, r3 ... bl _call_via_sl
 *                                                     (pushes r10 as well)
 *
 * The ROM REUSES r5 -- which held the actor pointer and is dead after PhysMove
 * -- for the function pointer, and reuses r6 (which held the move record) for
 * the result. We keep both alive and reach for r10, which costs the extra push,
 * the extra pop and the moves around them: six instructions.
 *
 * WHAT WAS TRIED, all 48-49 differing of 50:
 *   - chaining `f(PhysMove(a, p), m->f18)` so the PhysMove result is never a
 *     named local
 *   - declaring the function pointer after the result rather than before
 *
 * Nothing at the statement level makes gcc run out of registers at the point
 * the ROM does. The two-way choice of 0x18/0x30 is already the right shape --
 * a call inside each arm, batch 95's lever -- and the rest of the body screens
 * clean.
 *
 * WORTH KNOWING: the lever itself transfers. `Fn f; f = Func_8000888; f(...)`
 * gives the indirect call where a direct call would be one instruction shorter,
 * exactly as rom_e0524.c records. gcc-2.96 does not constant-propagate the
 * pointer back at -O2. So the `_call_via_rN` class is reachable in principle
 * and this function is not blocked ON it.
 */
struct M { unsigned char pad00[0x18]; int f18; };
struct P { unsigned char pad00[4]; int f4; };
struct C { void *f0; };

typedef int (*Fn)(int a, int b);

extern int Func_8000888(int a, int b);
extern struct C *GetBattleActor(void);
extern struct M *Func_80b7f70(void *a, int n);
extern void Func_80b7ed8(void);
extern int PhysMove(void *p, struct P *q);
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int n);

int Func_80b84c0(int unit, struct P *p)
{
    char *a;
    struct M *m;
    Fn f;
    int v;

    a = (char *)GetBattleActor()->f0;
    m = Func_80b7f70(a, 0);
    a += 8;
    Func_80b7ed8();
    v = PhysMove(a, p);
    f = Func_8000888;
    v = f(v, m->f18);
    if (Func_80c23c0(*(_GetUnit(unit) + (0x94 << 1))))
        v = f(v, 0x18);
    else
        v = f(v, 0x30);
    p->f4 -= v;
    return 0;
}
