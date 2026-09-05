// fakematch
/* OvlFunc_943_200bc88  --  0x0200bc88
 * [asm/overlays/rom_7c7b9c/ovl_30_c_c.s, second of four functions]
 *
 * 261 instructions of cutscene with a re-ask loop. Byte-exact: 680 bytes, 264
 * encodings and 68 relocations identical.
 *
 * THE PROLOGUE HOLDS TWO BITMASKS, NOT THE SCRIPT CONSTANTS, and reading it
 * that way is what makes the function tractable. `push {r5, r6, lr}` keeps
 * 0xfe in r5 and 1 in r6, each used at two sprite-flag pokes thirty
 * instructions apart. EVERYTHING ELSE is rebuilt at every use. So the two
 * masks must stay BARE LITERALS -- the "leave the ones CSE should hoist"
 * direction -- while the script constants all need pins. Plain C is 270 lines
 * against 261 with high-register traffic, the textbook length tell.
 *
 * THE LOOP IS NOT A `while`. The ROM has the test at the top and an
 * unconditional branch back: `L: test / bne END / body / b L`. gcc's `while`
 * -- and `for (;;) { if (!c) break; }`, which lowers identically -- rotates
 * that into `b Ltest / Lbody: ... / Ltest: test / beq Lbody`, one instruction
 * longer, shifting the whole tail: 262 lines and 111 differing.
 * `do { if (c != 1) break; ... } while (1);` suppresses expand_end_loop's
 * rotation and is exact. A goto-back spelling is byte-identical; the do/while
 * form is shipped because the tree reserves goto for real joins.
 *
 * `register int one __asm__("r6")` -- the gcse/cprop cure in a new shape, and
 * the subtlest thing here. The two `orr` sites want OPPOSITE tied operands: at
 * the first, r6 is still live and the ROM emits `orr r3, r6`, so the value is
 * the destination and plain `|= one` is right; at the second, r6 dies and the
 * ROM emits `orr r6, r3 / strb r6`, so the CONSTANT is the destination and the
 * variable has to be the accumulator, `one |= r[0x5a]; r[0x5a] = one;`.
 * Because `orr` is commutative with two register operands, gcc ties the
 * destination to whichever is written first, so ONE shared variable needs two
 * different source orders. As a plain `int` that shared variable does not
 * survive: cprop substitutes the 1 back into the second block and
 * rematerialises `mov r6, #1` there. The hard-register binding blocks the
 * substitution.
 *
 * EIGHTEEN PINS, and the set rule bit again. Thirty-two candidate sites;
 * fifteen individually inert and ALL FIFTEEN TOGETHER FAIL. Greedy stripping
 * to fixpoint from both ends converges on fourteen removals either way but
 * keeps DIFFERENT SETS -- forward drops one member of a same-value pair and
 * backward the other, exactly one of which must survive. A second round over
 * the eighteen survivors found every one load-bearing. The fourteen that fall
 * are all the LAST use of their value, consistent with the first-use rule.
 *
 * Every relocation in this function is an R_ARM_THM_CALL -- no R_ARM_ABS32 at
 * all -- so all eight pooled values are bare literals, not symbols.
 *
 * PRE-EXISTING INCONSISTENCY, flagged rather than introduced silently:
 * OvlFunc_943_200b9ec is declared `(void)` in src/overlays/rom_7c7b9c/
 * ovl_30_c_a_b.c and `(int)` here and in two other files. Different
 * translation units, so it compiles and links; it is not this function's doing.
 */
extern void OvlFunc_943_200ba00(int a, int b);
extern void OvlFunc_943_200b9ec(int a);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __DeleteFieldActor(int slot);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_943_200bc88(void)
{
    unsigned char *p;
    register int one __asm__("r6");

    __CutsceneStart();
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xb4; q2 = 0x28e;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0x80 << 8, 0);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(2, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(3, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 2; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xa0; q0 = 1; q1 = 0xc2; q2 <<= 2;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc6; q2 = 0x28e;
      __Func_809218c(q0, q1, q2); }
    __Func_80921c4(3, 0xc2, 0xa8 << 2);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0; q0 = 2; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0x80; q1 <<= 8; q0 = 3;
      OvlFunc_943_200ba00(q0, q1); }
    OvlFunc_943_200ba00(0x16, 0);
    __MessageID(0x1f55);
    OvlFunc_943_200b9ec(0x16);
    OvlFunc_943_200ba00(0x15, 0xd0 << 8);
    __Func_8093040(0x15, 0, 0x28);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0x16; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x16, 1);
    { PIN2; q1 = 0; q0 = 0x16;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 1) {
        __MapActor_DoAnim(2, 4);
        OvlFunc_943_200b9ec(2);
        OvlFunc_943_200ba00(3, 0xa0 << 8);
        __MapActor_SetAnim(3, 3);
        OvlFunc_943_200b9ec(3);
        OvlFunc_943_200ba00(1, 0xc0 << 7);
        __Func_80925cc(1, 1);
        __Func_8092c40(1, 0);
        do {
            if (__Func_8091c7c(0, 0) != 1)
                break;
            __Func_80925cc(2, 1);
            __MessageID(0x1f53);
            __Func_8092c40(2, 0);
        } while (1);
    }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x16, 3);
    __MessageID(0x1f5b);
    OvlFunc_943_200b9ec(0x16);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x16; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetSpeed(0x15, 0x80 << 9, 0x80 << 8);
    { PIN3;
      __MapActor_GetActor(0x16)[0x5a] &= 0xfe;
      q1 = 0xa2; q2 = 0x27a; q0 = 0x16;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    { unsigned char *r = __MapActor_GetActor(0x16);
      one = 1; r[0x5a] |= one; }
    { PIN3; unsigned char *r = __MapActor_GetActor(0x15);
      q2 = 0xa9; r[0x5a] &= 0xfe; q1 = 0xa2; q2 <<= 2; q0 = 0x15;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    { unsigned char *r = __MapActor_GetActor(0x15);
      one |= r[0x5a]; r[0x5a] = one; }
    __Func_8092adc(0x16, 0xc0 << 6, 0);
    OvlFunc_943_200ba00(0x15, 0xd0 << 8);
    OvlFunc_943_200b9ec(0x16);
    { PIN3; q0 = 1; q1 = 0xb4; q2 = 0x28e;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xb4; q2 = 0x28e;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xb4; q2 = 0x28e; q0 = 3;
      __Func_80921c4(q0, q1, q2); }
    __DeleteFieldActor(1);
    __DeleteFieldActor(2);
    __DeleteFieldActor(3);
    __SetFlag(0x903);
    __CutsceneEnd();
}
