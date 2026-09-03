/* OvlFunc_964_20090c4  --  0x020090c4
 * asm/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_c_a.s, line 12 (one function, no data).
 *
 * PARKED at 17 aligned of 116, length exact (116 / 116). FOUR OF THE 17 ARE
 * PHANTOM: the two `bl __umodsi3` vs `bl _umodsi3_RAM` lines, and
 * overlays/rom_7ed0a0/overlay.ld:110 already carries `__umodsi3 = _umodsi3_RAM;`.
 * The real residual is 13, all register choice and scheduling. Instructions
 * 0-35 and the whole loop body up to the argument fill are byte-for-byte.
 *
 * BLOCKER CLASS: 2 (register birth order) with a 5 (scheduling) tail -- the
 * decomp-permuter class.
 *
 * THE WALL, NAMED PRECISELY. The ROM has
 *
 *      add r3, sp, #0x10       <- address into a low scratch
 *      mov r8, r3              <- committed to a callee-saved high register
 *      ldr r3, =OvlFunc_964_2009068
 *      mov r2, r10
 *      mov r0, r8              <- a SECOND reload of the same pointer
 *      ...
 *      str r3, [r0, #0x24]
 *
 * gcc emits that second reload only when the pointer's live range crosses a
 * CALL between its definition and the store. With the definition adjacent to
 * the store, reload INHERITS the scratch and stores through it directly, which
 * is one instruction shorter and drops the `mov r0, r8`. Both placements are
 * reachable and neither is the ROM's:
 *
 *      `c = &s;` after  the WaitFrames  -> address rematerialised at the store,
 *                                          no `mov r0, r8`, 115 lines
 *      `c = &s;` before the WaitFrames  -> the `mov r0, r8` appears, but the
 *                                          `add / mov r8` pair is emitted
 *                                          BEFORE the bl instead of after
 *
 * sched2 will not sink the pair past the call -- the `add` writes a
 * call-clobbered register, so the dependence pins it -- and no statement order
 * within the block produces the extra reload. GENERALISATION WORTH KEEPING:
 * `mov rLow, rHigh` immediately before a store whose base is that high register
 * is a RE-READ of a pointer already committed to a callee-saved register. It is
 * evidence that a CALL sits between the pointer's definition and its use, not
 * evidence about statement order. The remaining 9 instructions are all
 * downstream of this one allocation; every loop, tail and declaration-order
 * spelling left them unchanged.
 *
 * THREE LEVERS DID FIRE, and the first is a new rule.
 *
 * A NEGATIVE CONSTANT MULTIPLIER IS WHAT SELECTS THE SHIFT CHAIN. This function
 * multiplies twice and the ROM spells the two differently -- a two-instruction
 * `ldr` + `mul` for one, a seven-instruction shift chain for the other, where
 * the chain is plainly the more expensive. That is not gcc preferring shifts:
 * `expand_mult` calls `synth_mult` on the ABSOLUTE value and negates afterwards,
 * and the cost budget it passes comes from the negative MULT. So `x * 6553`
 * gives `ldr =0x1999 / mul` and only `x * -6553` gives the chain. READ THE SIGN
 * OFF THE ROM: a `neg` after a shift chain means the source multiplier was
 * negative. Two multipliers in one function is a clean internal control.
 *
 * A STATEMENT BREAK STOPS THE DISTRIBUTION. `(r - 5) * 0x3332` folds to
 * `r * 0x3332 - 65530` and emits `mul` then `ldr =0xffff0006 / add`. Splitting
 * it -- `t = r; t -= 5; vx = t * 0x3332;` -- gives the ROM's `sub #5 / mov /
 * mul`. Worth 4 aligned. This is the mirror of the one-expression-not-two rule,
 * and which way it goes is decided by whether the ROM distributed.
 *
 * DELETING THE BYTE-POINTER LOCAL WAS WORTH 3 (21 to 18), and it is the
 * converse of the standing habit. The ROM holds `&actor->f55` in r10 across the
 * whole function and writes through it three times, which reads like the
 * strongest possible case for a named `unsigned char *p`. It is not: gcc CSEs
 * the field reference into that same pseudo on its own, and the named local
 * perturbs the config pointer's allocation two statements later.
 *
 * Also confirmed: `C - x*k` and `x*-k + C` are indistinguishable in an isolated
 * probe -- both fold to `ldr C / sub` -- and only under this function's real
 * register pressure does the negative-multiplier form keep the ROM's `neg`+`add`
 * pair. Isolated probes are not safe for sign questions.
 *
 * ~100 spellings measured (34-way c-placement sweep, 28-way declaration order,
 * 7 loop forms, 6 tail forms, 4 array forms) and 8 flags; the 17 floor is
 * insensitive to all of them. -fno-gcse is much worse (32); --no-sched2 is
 * worse still (36-38), which confirms sched2 is producing the ROM elsewhere.
 * Screened with tools/tryc.py --align. Not built.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x14];
    int f28;
    unsigned char pad2c[0x10];
    int f3c;
    unsigned char pad40[0x15];
    unsigned char f55;
};

struct Cfg {
    unsigned char pad00[0x24];
    void (*f24)(void);
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Func_8092504(int a);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __WaitFrames(int n);
extern unsigned int __Random(void);
extern void OvlFunc_964_2009068(void);
extern void OvlFunc_964_2008ae8(int x, int y, int z, int a, int b, int c, int d, struct Cfg *s);

void OvlFunc_964_20090c4(void)
{
    struct Cfg s;
    struct Cfg *c;
    struct Actor *a;
    unsigned int i;
    int vx;
    int vz;
    int t;
    unsigned char two;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __Actor_SetAnim(a, 6);
    __Func_8092504(0);
    __Actor_SetAnim(a, 1);
    __Actor_SetSpriteFlags(a, 0);
    two = 2;
    a->f55 = two | a->f55;
    __PlaySound(0x98);
    a->f28 = 0x80 << 11;
    __Actor_TravelTo(a, a->x, a->y, a->z + (0xc0 << 12));
    c = &s;
    __WaitFrames(6);
    a->f55 = 0;
    c->f24 = OvlFunc_964_2009068;
    __PlaySound(0x7f);
    for (i = 0; i < 8; i++) {
        a->y -= 0x20000;
        a->f3c = a->y;
        __WaitFrames(1);
        if (i & 1) {
            t = (int)(__Random() % 10);
            t -= 5;
            vx = t * 0x3332;
            vz = (int)(__Random() % 10) * -6553;
            vz = -32765 + vz;
            OvlFunc_964_2008ae8(a->x, a->y, a->z, vx, 0, vz, 0x1000001, c);
        }
    }
    __Actor_SetSpriteFlags(a, 1);
    a->f55 = 3;
    __CutsceneEnd();
}
