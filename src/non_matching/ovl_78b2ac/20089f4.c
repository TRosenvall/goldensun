/* OvlFunc_890_20089f4 -- 0x020089f4
 * [asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_a.s lines 548-743, beside
 *  OvlFunc_890_2008488 -- needs tools/split_s.py, and tryc's size check is
 *  skipped on the two-function reference, so objcmp is the verdict here]
 *
 * FIVE REAL INSTRUCTIONS. objcmp: sizes 524/524, 201/201 encodings, all 49
 * R_ARM_THM_CALL relocations identical at identical offsets, literal-pool
 * order identical. The five are a rotation of `mov r0` against two `lsl`s at
 * THE FIRST TWO CALL SITES, and nothing else in the function differs. One
 * further index is a PHANTOM: the pool word carrying `_MSG_ffc`, which
 * resolves once `_MSG_ffc = 0xffc;` joins message.sym (which already carries
 * _MSG_1000 for the sibling). Landing it would also want a CSE_CFLAGS rule.
 *
 * BLOCKER: UNGUARDED ARG-INTERLEAVE. The interleave lever needs a split build
 * AND a preceding branch; these two sites are the function's FIRST TWO CALLS,
 * before its only early branch, so there is nothing to make a dominating block
 * out of. The ROM wants
 *     mov r1,#0x80 / mov r2,#0x80 / mov r0,#0 / lsl r1,#9 / lsl r2,#8
 *     mov r1,#0xf4 / mov r0,#0    / lsl r1,#1 / mov r2,#0xb0
 * -- the slot's `mov r0` BETWEEN the movs and the shifts. Everything after the
 * `if (p != 0)` is reachable and exact.
 *
 * THE GENERALISATION THAT GOT IT TO FIVE, and it is worth having: AN EARLIER
 * USE OF THE SAME LITERAL SERVES AS THE DEFINITION. The documented lever says
 * the dominating block needs a NAMED LOCAL. It does not. Site 3 --
 * __MapActor_SetSpeed(0x10, 0x80<<9, 0x80<<8) with BARE LITERALS -- comes out
 * in the ROM's order anyway, because site 1 earlier in the function already
 * defined that value: CSE rewrites site 3's literals as a reference to site
 * 1's pseudo, the pseudo is never allocated a hard register, and the
 * rematerialisation at the use interleaves. That is why the guarded half of
 * this function fell so cheaply, and it is a cheaper thing to try than naming.
 *
 * WHY IT CANNOT REACH SITES 1 AND 2: they are the FIRST occurrence, so nothing
 * can define the value ahead of them. probe1.c rules out the obvious
 * alternative -- the slot argument's value (0, 0x10, 3) makes no difference;
 * standalone, all three come out in gcc's order.
 *
 * TWO OTHER LEVERS, BOTH LOAD-BEARING, both already documented:
 *  - -fno-rerun-cse-after-loop, 143 -> 114. The ROM rebuilds 0x80<<9, 0x80<<8,
 *    0xc0<<6 and -1 at every site; the second CSE pass commons each into a
 *    callee-saved register. -fno-gcse (89), -fno-cse-follow-jumps and
 *    -fno-expensive-optimizations (60, no change) do NOT reach it.
 *  - `m = (int)&_MSG_ffc`, 114 -> 60 and the length goes 193 -> 194, the ROM's.
 *    With `m = 0xffc` gcc folds m+1 and m+2 at compile time and emits three
 *    independent pool loads, never spending a register -- rematerialising a
 *    pool constant is cheaper than a push/pop pair. A SYMBOL address is not
 *    foldable, so gcc must keep the base and emit both `add`s.
 *  - `n1 = -1` written as the negative literal, not `x = 1; x = -x;`. The
 *    two-step form is a COMPUTED value and gcc holds it.
 *
 * WHAT MUST NOT BE NAMED: the pooled constants (0x4010, 0x26666, 0x23f0000)
 * and the two split builds used at the unguarded prefix. Naming every cheap
 * constant is 199 of 194 at 202 lines -- far worse than naming none.
 *
 * MEASURED (differ counts out of the ROM's 194 lines, --no-rerun-cse unless
 * noted): plain C at -O2 143; plain + flag 114; + _MSG_ffc symbol 60; at -O1
 * 75; -fno-gcse 89; --no-sched2 added 75; every cheap constant named 199.
 * Naming one at a time: 0xc0<<6 37, -1 47, 0x80<<9/0x80<<8 191, 0x105 58,
 * {0xf3<<1,0x90<<2,0xc0<<8} 54, 0xec<<1 58, 0xa0<<8 58. Combining the useful
 * ones 12; + 0xb5<<16 10; + 0xa8<<16 8; + BOTH 6 (5 real). Adding 0xf6<<17 no
 * change; 0x80<<11 back to 10. No-prototype lever on __MapActor_SetSpeed 15,
 * on __Func_80921c4 22, both 25. K&R declaration 6. Dropping a prototype 9/16.
 * Naming assigned after __MessageID 190. Constants written pre-folded 6.
 * On the best version, ALL of -fno-schedule-insns, -fno-peephole,
 * -fno-defer-pop, -fno-caller-saves, -fno-function-cse, -fno-strength-reduce,
 * -fno-strict-aliasing, -fno-thread-jumps, +/-fforce-mem, -fno-inline,
 * -fno-optimize-sibling-calls, -fomit-frame-pointer, -fno-delayed-branch,
 * -fno-reorder-blocks and -frerun-loop-opt measure 6 -- no change.
 * -fno-cse-skip-blocks 9, --no-sched2 65, -O1 157.
 *
 * DO NOT SPEND MORE ROUNDS ON CONSTANT SPELLINGS HERE. The space is mapped.
 * Either the two leading sites are accepted as a five-instruction park, or the
 * class is attacked at the compiler level -- this is the same systematic
 * ordering difference the REG_ALLOC_ORDER entry at the tail of HANDOFF.md
 * points at, and a compiler that emitted the slot's `mov` before the shifts
 * unconditionally would drop all eleven sites in this function at once with no
 * naming at all.
 *
 * The named neighbour in the templated.py ranking (rom_7ac2d8) was NOT the
 * useful one; the same-overlay sibling src/overlays/rom_78b2ac/
 * ovl_30_c_c_a_c_b_a_b.c is, and it carries the _MSG_1000 idiom.
 */
extern void OvlFunc_890_200a5fc(int a, int b);

extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __SetCameraTarget(int slot, int a);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

extern int _MSG_ffc;

void OvlFunc_890_20089f4(void)
{
    unsigned char *p;
    int m;
    int c6, n1, v1d8, a08, e5, f3, n90, c8, b5, a8;

    c6 = 0xc0 << 6;
    n1 = -1;
    v1d8 = 0xec << 1;
    a08 = 0xa0 << 8;
    e5 = 0x105;
    f3 = 0xf3 << 1;
    n90 = 0x90 << 2;
    c8 = 0xc0 << 8;
    b5 = 0xb5 << 16;
    a8 = 0xa8 << 16;

    __MessageID(0xff6);
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(0, 0xf4 << 1, 0xb0);
    __MapActor_SetAnim(0, 0);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0x10, *(int *)(p + 8), *(int *)(p + 0x10));
    __Func_8092adc(0, 0, 1);
    __MapActor_SetSpeed(0x10, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(0x10, v1d8, 0xa8);
    __Func_8092adc(0x10, 0, 0x3c);
    __MapActor_Jump(0x10, 4, 0x28);
    OvlFunc_890_200a5fc(0x10, 6);
    __Func_80933d4(0x26666, 0x4ccc);
    __Func_80933f8(0x23f0000, n1, b5, 1);
    __Func_8093530();
    __CutsceneWait(0x78);
    OvlFunc_890_200a5fc(0x1010, 0x50);
    __Func_80933f8(0xf6 << 17, n1, a8, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_8092adc(0x10, c6, 0x14);
    OvlFunc_890_200a5fc(0x4010, 6);
    __Func_8092adc(0x10, 0, 0x3c);
    __Func_80925cc(0x10, 2);
    __Func_8092adc(0x10, c6, 0xa);
    __Func_8092c40(0x4010, 0);
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID(0xffa);
    else
        __MessageID(0xffb);
    __Func_8092adc(0, a08, 0xa);
    OvlFunc_890_200a5fc(0x4010, 0xa);
    m = (int)&_MSG_ffc;
    __MessageID(m);
    __Func_8092adc(0x10, 0, 0x28);
    __MapActor_Emote(0x10, e5, 0x28);
    __MapActor_DoAnim(0x10, 4);
    __Func_8092adc(0x10, c6, 0xa);
    __MapActor_SetAnim(0x10, 4);
    __Func_8092c40(0x4010, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(m + 1);
        __SetFlag(0x896);
    } else {
        __MessageID(m + 2);
    }
    OvlFunc_890_200a5fc(0x4010, 4);
    __SetCameraTarget(0x10, 1);
    __Func_80921c4(0x10, f3, 0x83);
    __Func_80921c4(0x10, n90, 0x78);
    __Func_8092adc(0x10, c8, 2);
    __Func_80933d4(0x80 << 11, 0x80 << 8);
    __SetFlag(0x80a);
}
