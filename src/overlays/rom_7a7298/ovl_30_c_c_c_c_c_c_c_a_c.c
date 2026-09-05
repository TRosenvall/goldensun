// fakematch
/* OvlFunc_921_20099e8  --  0x020099e8
 *   [asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_a_c.s, the only function]
 *
 * 506 instructions of straight-line cutscene: one guarded actor fetch, four
 * bare actor fetches whose byte is masked in place, two writes through
 * iwram_3001ebc and 117 constant-argument calls.  Built at the tree default
 * -O2: nothing in the Makefile matches this path with a wildcard (the only two
 * rom_7a7298 rules name other objects literally), and objcmp against the real
 * asm/ ref and against a scratch copy of the same .s give the identical
 * verdict with no `(built with: ...)` line.
 *
 * READ WHAT THE PROLOGUE KEEPS.  `push {r5, r6, r7, lr}` looks wide, but the
 * three saved registers hold only r7 = &iwram_3001ebc, r5 = 0xfe and r6 = 1 --
 * a symbol address and the two bit masks of `p[0x5a] &= 0xfe` / `|= 1`, each
 * used at two sites hundreds of instructions apart.  EVERYTHING ELSE THE ROM
 * REBUILDS.  Plain C does the opposite: cse_main commons 0x9999, 0xa0<<8,
 * 0xc0<<8 and friends into pseudos that straddle `bl`, the allocator runs out
 * of low registers and the function opens by shuffling r8-r11 into r5-r7 to
 * spill them.  1364 bytes against 1340, 466 encodings differing.
 *
 * SO THE CURE IS PINS, NOT NAMED LOCALS -- the wide push is a red herring here
 * because what it holds is two literals and a symbol, not loop state.  57
 * argument fills are pinned to r0..r3.  A hard register cannot be the target
 * of cse's substitution, so the constant is rebuilt at that site and the
 * pseudo never forms.
 *
 * THE UNIFORM FILL SPELLING WAS ENOUGH.  Every pinned site is written the same
 * way -- one statement per argument, ascending q0..q3, the shift left inside
 * the expression -- even though the ROM emits the four shapes in at least six
 * different orders (`mov r1 / mov r0 / lsl r1 / mov r2`, `mov r2 / ldr r0 /
 * mov r1`, ...).  sched2 reproduces all of them from the uniform source.  No
 * site needed a hand-ordered fill: two sites were tried with one
 * (__Func_809218c(0, 0x2b2, 0xc8) as q2,q1,q0 and __Func_80933d4 as q1,q0) and
 * both turned out not to need a pin at all.
 *
 * A POINTER-RETURNING CALL WHOSE RESULT DIES IMMEDIATELY MUST NOT GET A NAMED
 * LOCAL.  `__MapActor_GetActor(0x14)[0x5a] &= 0xfe;` compiles to the ROM's
 * `add r0, #0x5a / ldrb / and / strb` -- r0 updated in place.  Spelled
 * `p = __MapActor_GetActor(0x14); p[0x5a] &= 0xfe;` with `p` the same local the
 * guarded fetch uses, the four sites each gain a `mov rN, r0` copy because the
 * shared pseudo is allocated away from r0: 1348 bytes, 262 encodings differing.
 * The same holds for `__Func_8093554()[0x55] = 0` on its own -- 1344 bytes, 498
 * encodings differing -- which is a size difference of one instruction that
 * shifts the whole stream.  Only the guarded fetch, whose value is genuinely
 * live across the `cmp`/`beq`, keeps a named local.
 *
 * THE iwram OFFSET ARITHMETIC NEEDS NOTHING.  The ROM builds 0xe0<<1 = 0x1c0,
 * stores `sub r3, #0xc0` (= 0x100) through it, then `add r3, #0xc8` to reach
 * 0x1c8, and at the end reuses 0x1c0 with `add r2, #0x49` to make 0x209.  That
 * chain of related constants is what cse's get_related_value emits from the
 * plain two assignments; it is not a tell for a variable.
 *
 * 0x165b IS A LITERAL, AND THE RELOCATIONS SAY SO.  The candidate carries 128
 * relocations and the reference carries the same 128 -- 127 R_ARM_THM_PC22 for
 * the calls plus one R_ARM_ABS32 for iwram_3001ebc.  A `(int)&_MSG_165b`
 * spelling would add one; none is there, and 0x9999, 0x4ccc, 0x2b2, 0xcccc,
 * 0x1999, 0x2b20000, 0x2a1, 0x4014, 0x2003, 0x4003, 0x105, 0x101, 0x82e and
 * 0x82d are bare literals too.
 *
 * FIFTY-SEVEN PINS, MINIMAL BY MEASUREMENT.  All 117 constant-argument calls
 * were pinned first (that also matches, and is the shape the sweep started
 * from).  Sixty were stripped one at a time under objcmp, then removed greedily
 * with a re-test after each drop, to a fixpoint.  A second one-at-a-time round
 * over the surviving 57 found every one of them load-bearing.  The survivors
 * are not "the repeated constants": __Func_8093040(0x13, 0, 0xa) appears six
 * times and needs no pin at any of them, __CutsceneWait(0x14) nine times and
 * needs none, while __Func_8093040(0x4014, 0, 0xa) needs one at two of its
 * three sites and __Func_8093040(0x2003, 0, 0xa) at one of its two.  Those two
 * are the only signatures in the function that split.  N pins is a size, not a
 * set.
 *
 * LANDING NEEDS NO SPLIT.  The .s holds this function alone and
 * overlays/rom_7a7298/overlay.ld:56 already names the single .o.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_921_20099e8(void)
{
    unsigned char *p;


    __CutsceneStart();
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(0, 0x2b2, 0xc8);
    __Func_8093554()[0x55] = 0;
    __Func_80933d4(0xcccc, 0x1999);
    { PIN4; q0 = 0x2b20000; q1 = 0; q2 = 0xa4 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + 0x1c8) = 0x30;
    __MapTransitionIn();
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 1);
    __MapActor_SetSpeed(3, 0x9999, 0x4ccc);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(3, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q0 = 3; q1 = 0x2a1; q2 = 0xb7;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0x13, 2);
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x28);
    __MessageID(0x165b);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(3, 3);
    { PIN2; q0 = 0x14; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x4014; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(3, 4);
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x13; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x14);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q0 = 0; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xf0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x14, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x4014; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 3; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(0x13, 1);
    __CutsceneWait(0x14);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 7; q2 = 0x50;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0xf0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xe0 << 7; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetSpeed(0x14, 0x80 << 9, 0x80 << 8);
    __MapActor_GetActor(0x14)[0x5a] &= 0xfe;
    __Func_80921c4(0x14, 0xa4 << 2, 0xa6);
    __CutsceneWait(1);
    __MapActor_GetActor(0x14)[0x5a] |= 1;
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x4014; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(3, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x4003; q1 = 0; q2 = 0xa;
      __Func_8093040(q0, q1, q2); }
    { PIN2; q0 = 0x13; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 0x14; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(3, 4);
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(0x13, 1);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(0x14, 1);
    { PIN3; q0 = 0x4014; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    __Func_80925cc(3, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(3, 3);
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0x50;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0x105; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 4);
    __Func_8093040(0x13, 0, 0xa);
    __MapActor_SetAnim(0x14, 4);
    { PIN3; q0 = 0x4014; q1 = 0; q2 = 0x14;
      __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x2003; q1 = 0; q2 = 0x28;
      __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 3);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x14, 3);
    __Func_8093040(0x4014, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(3, 3);
    __Func_8093040(0x2003, 0, 0xa);
    __MapActor_SetAnim(0x13, 3);
    __MapActor_DoAnim(0x14, 3);
    __CutsceneWait(0x28);
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x4003, 0, 0x14);
    { PIN3; q0 = 0; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 3; q1 = 0xac << 2; q2 = 0xc8;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(3, 0, 0);
    __MapActor_GetActor(0x14)[0x5a] &= 0xfe;
    __Func_80921c4(0x14, 0xa1 << 2, 0xa6);
    __CutsceneWait(1);
    __MapActor_GetActor(0x14)[0x5a] |= 1;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __SetFlag(0x82e);
    __ClearFlag(0x82d);
    __CutsceneEnd();
}
