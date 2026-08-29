/* THE MESSAGE BASE HELD IN A CALLEE-SAVED REGISTER -- a two-member class, and
 * a NEW blocker rather than an instance of an old one.
 *
 *   OvlFunc_962_200806c  asm/overlays/rom_7ec19c/ovl_30_c_a_c_a_a.s   57 vs 58
 *   OvlFunc_950_2008500  asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a.s
 *
 * Both are sanctum attendants whose long branch speaks three CONSECUTIVE
 * message ids. The ROM loads the first from the pool into a callee-saved
 * register and reaches the other two with an immediate add:
 *
 *      ldr r5, =0x261c
 *      mov r0, r5      / bl __MessageID          <- base
 *      ...two calls...
 *      add r0, r5, #1  / bl __MessageID          <- base + 1
 *      add r0, r5, #2  / bl __MessageID          <- base + 2
 *
 * We get three independent pool loads (`ldr r0, =0x261c`, `=0x261d`, `=0x261e`)
 * and one fewer instruction, because gcc never allocates the second callee-
 * saved register: the ROM pushes {r5, r6, lr}, we push {r5, lr}.
 *
 * WHAT WAS TRIED
 *   - `int base = 0x261c;` then `base`, `base + 1`, `base + 2`
 *         gcc constant-folds the adds at the tree level, long before any pass
 *         could notice that r5 already holds 0x261c.
 *   - the three ids written as three separate literals
 *         byte-identical to the above -- so the folding is not what decides it.
 *         gcc reaches the same three-pool-load form from either spelling.
 *   - assigning `base` at the top of the function instead of inside the arm
 *   - -fno-rerun-cse-after-loop, -fno-expensive-optimizations, -O1
 *
 * WHY THIS IS ITS OWN CLASS AND NOT src/non_matching/overlays/constant_reuse.c.
 * That file collects cases where gcc reuses a value the ROM recomputes. This is
 * the exact reverse and it is NOT the reverse-direction counterexample recorded
 * there either (Func_80167ac, where the ROM derives and gcc loads fresh, but
 * all inside ONE basic block with no calls between). Here the reuse has to
 * survive THREE intervening calls, which means it is not a CSE question at all
 * but a register-allocation one: something must decide that a constant is worth
 * a callee-saved register plus a push/pop pair, and this build decides it is
 * not.
 *
 * `-fcall-used-r4` IS RULED OUT, batch 92. It was the obvious suspect -- it is
 * in GCC296_CFLAGS and it takes one register out of the callee-saved set -- so
 * OvlFunc_962_200806c was compiled with the flag REMOVED to see whether the
 * extra callee-saved register would change the decision. It does not. gcc uses
 * r4 for the slot instead of r5 and still emits four independent pool loads;
 * the base is not promoted to a register either way. So the difference is not
 * about how many callee-saved registers are available, and the next hypothesis
 * has to come from somewhere else.
 *
 * WHAT IS ALREADY RIGHT in both, and should not be re-derived: the quadrant
 * facing test (`(unsigned short)((f6 + 0x2000) & ~0x3fff) == 0xc000`, settled
 * in src/overlays/rom_7d5838/ovl_30_c_c_c_c_a.c), the two-flag message ladder,
 * and the `== 0` sense of the inner test -- the ROM falls through to the
 * __CutsceneWait arm, so that arm is the `if` and base+2 is the `else`.
 *
 * Everything up to `.L52a` / `.L9c` and everything after the third __MessageID
 * screens clean; the 36 differing positions are all downstream of the missing
 * r6 allocation in the prologue.
 */
struct A { unsigned char pad00[6]; unsigned short f6; };

extern struct A *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __CutsceneWait(int n);
extern void __Func_80b0278(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_962_200806c(int slot)
{
    struct A *a;
    unsigned short d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x1f, slot);
    } else if (__GetFlag(0x96f)) {
        base = 0x261c;
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x25cf);
        __ActorMessage(slot, 0);
    }
}

void OvlFunc_950_2008500(int slot)
{
    struct A *a;
    unsigned short d;
    int base;

    a = __MapActor_GetActor(0);
    d = (a->f6 + 0x2000) & ~0x3fff;
    if (d == 0x8000) {
        __Func_80b0278(0x1c, slot);
    } else if (__GetFlag(0x95 << 4)) {
        __MessageID(0x238d);
        __ActorMessage(slot, 0);
    } else if (__GetFlag(0x962)) {
        __MessageID(0x221b);
        __ActorMessage(slot, 0);
    } else {
        base = 0x1fd5;
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
    }
}
