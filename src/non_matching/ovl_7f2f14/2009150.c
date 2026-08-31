/* OvlFunc_968_2009150 -- 0x02009150  (asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c.s)
 *
 * BLOCKER: two straight-line store-interleaves plus three register-role swaps.
 * 14 of 81, EXACT length. Both classes are documented walls.
 *
 * 59 differing to 14 on ONE edit, and it is the lever from the round before:
 * CONSUME THE CALL RESULT, do not route it through a named local.
 *
 *     ours  p = __MapActor_GetActor(0); p[0x5a] &= 0xfe;
 *           -> mov r3, r0 / add r3, #0x5a / ...        (a copy the ROM lacks)
 *     rom   add r0, #0x5a / ldrb r2, [r0, #0] / ...
 *
 * Writing `p = __MapActor_GetActor(0) + 0x5a; *p &= 0xfe;` puts the arithmetic
 * on the returned register in place. Three sites in this function, and it also
 * fixed the length. 0x5a is past the thumb ldrb/strb immediate range, so the
 * ROM's separate `add` is forced rather than chosen -- the same reading that
 * applied to the 0x23 store in OvlFunc_913_2008a68.
 *
 * WHAT REMAINS, and a MISDIAGNOSIS worth recording.
 *
 * (1) Two stores that belong INSIDE a two-instruction constant build:
 *
 *       rom   mov r2,#0x80 / str r1,[r5,#0x28] / mov r0,#0 / lsl r2,#0xa
 *       ours  str r1,[r5,#0x28] / mov r2,#0x80 / lsl r2,#0xa / mov r0,#0
 *
 *     and the same shape around `str r3,[r5,#0x6c]` inside the 0x81 << 1 build
 *     for __MapActor_Emote. The statement-split lever is exactly this shape --
 *     `s = 0x80; <store>; s <<= 10;` -- and it is INERT here, byte-identical to
 *     the literal. Both sites are straight-line and both constants are argument
 *     temporaries that die at the call, which is the documented boundary: gcc
 *     rematerialises argument temporaries during argument fill and discards the
 *     source's statement structure. A store is "cheap work in the gap" and
 *     satisfies that half of the precondition; the lifetime half is what fails.
 *
 * (2) MISDIAGNOSIS -- this is NOT the and-destination lever. It looks like it:
 *
 *       rom   ldrb r2, [r0, #0] / mov r3, #0xfe / and r3, r2
 *       ours  ldrb r3, [r0, #0] / mov r2, #0xfe / and r2, r3
 *
 *     but read the destinations: the constant is in the destination register in
 *     BOTH. r3 holds 0xfe in the ROM and r2 holds it in ours, and each is the
 *     `and`'s first operand. So the lever has nothing to do -- what differs is
 *     only WHICH register each value got, which is the register-role swap.
 *
 *     Screened anyway before that was noticed, and the results are consistent
 *     with it being the wrong lever: `int m = 0xfe` for the ands is INERT (14),
 *     `unsigned char n = 1` for the orr is INERT (14), and applying BOTH at once
 *     is much worse (32, prologue changes). Loading through an explicit local
 *     first -- `t = *p; *p = t & 0xfe;` -- is worse again at 58 and two lines
 *     long, in both operand orders.
 *
 *     THE READING RULE: before reaching for the constant-as-destination lever,
 *     check whether the constant already IS the destination in our output. If
 *     it is, the difference is register assignment and the lever cannot help.
 *     The two shapes look identical at a glance because both show a `mov` of a
 *     constant next to a load, and only the operand order of the `and` tells
 *     them apart.
 *
 * Everything else reproduces: the two-arm branch that differs only in a constant
 * (0xd2 against 0xee) with the `<< 2` after the join, the sign-extended halfword
 * read, the behaviour-script pointer, and both writes to the 0x6c callback slot.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int n);
extern void __Func_8092950(int slot, int n);
extern void __Func_80921c4(int slot, int a, int b);
extern unsigned char gScript_968__0200d21c[];
extern void OvlFunc_968_20085e4(void);

void OvlFunc_968_2009150(void)
{
    unsigned char *a;
    unsigned char *p;
    int k;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    __MapActor_SetBehavior(0, gScript_968__0200d21c);
    __MapActor_WaitScript(0);
    __Func_8092950(0, 6);
    *(int *)(a + 0x28) = 0x80 << 11;
    __MapActor_SetSpeed(0, 0x80 << 11, 0x80 << 10);
    if (*(int *)(a + 0x10) >> 20 <= 0x36) {
        p = __MapActor_GetActor(0) + 0x5a;
        *p &= 0xfe;
        k = 0xd2;
    } else {
        p = __MapActor_GetActor(0) + 0x5a;
        *p &= 0xfe;
        k = 0xee;
    }
    __Func_80921c4(0, *(short *)(a + 0xa), k << 2);
    __CutsceneWait(1);
    p = __MapActor_GetActor(0) + 0x5a;
    *p |= 1;
    __CutsceneWait(0x14);
    *(void **)(a + 0x6c) = (void *)OvlFunc_968_20085e4;
    __MapActor_Emote(0, 0x81 << 1, 0x3c);
    __MapActor_DoAnim(0, 4);
    __Func_8092950(0, 0);
    __MapActor_DoAnim(0, 4);
    *(void **)(a + 0x6c) = 0;
    __CutsceneEnd();
}
