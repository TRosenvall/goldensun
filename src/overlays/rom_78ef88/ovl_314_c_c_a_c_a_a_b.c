// fakematch
/* OvlFunc_896_2008d5c  --  0x02008d5c
 * [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a.s, second of eight functions]
 *
 * 201 instructions of straight-line cutscene script with a single guard.
 * Byte-exact: 560 bytes, 216 encodings and 48 relocations identical.
 *
 * THE WHOLE FUNCTION IS ONE INSTANCE OF "A CONSTANT USED TWICE ACROSS CALLS".
 * The ROM's prologue is `push {lr}` and NOTHING ELSE -- not one callee-saved
 * register is spent, so every constant is rebuilt at every use. Plain C is 207
 * lines against 201, `push {r5, r6, r7, lr}` plus an r8 spill, 195 of 201
 * differing: cse_main commons 0x80<<8 into r7, 0x80<<7 into r5, 0x101 into r6
 * and 0xe0<<8 into r8, and because every use straddles a bl the allocator has
 * to give each one a callee-saved home.
 *
 * NO FLAG REACHES IT, confirmed here on a third function: -fno-gcse,
 * -fno-rerun-cse-after-loop and -fno-expensive-optimizations each produce
 * output byte-identical to the default (207 lines, 195 differing).
 *
 * AND THE NAMED-LOCAL CURE IS THE WRONG ONE HERE -- it is measurably WORSE.
 * `int AA = 0x80 << 8; ...` for the three values with three uses gives 208
 * lines and 199 differing, against plain C's 207/195. The named local buys the
 * value a pseudo that local-alloc is happy to keep live; what is wanted is the
 * opposite. A pin on a call-clobbered argument register makes the value DEAD
 * across the next bl, so gcc has no register it could carry it in and must
 * rematerialise -- which is exactly what a `push {lr}` function does.
 *
 * TWENTY-EIGHT PINS, MINIMAL BY MEASUREMENT. Thirty pinned sites were stripped
 * one at a time: only two were inert (__Func_8092adc(0, 0xe0<<8, 0xa) and
 * __Func_80933f8), both were removed together and the result still screens
 * byte-exact, and a second round over the surviving 28 found every one
 * load-bearing. The cheapest of them still costs 2 differing lines; the dearest
 * three cost the whole function (removing the __MapActor_Emote(0, 0x101, 0x3c)
 * pin costs 180 differing and a wider push, because 0x101 is shared with three
 * later sites).
 *
 * THE PIN DOES TWO JOBS AT ONCE and that is why so many are needed. Beyond
 * killing the hoist, the assignment order inside each block IS the argument
 * setup order, and this function's setup order is not uniform: __Func_8092adc
 * fills r0 second at five sites and third at two, __MapActor_SetSpeed fills r0
 * first at two sites and third at three. No declaration lever can express that
 * -- the return type is a per-callee property and these are per-site
 * differences -- so the sites that need only ordering are pinned too. The
 * fourth __MapActor_SetPos is the sharpest case: it is the only one of the four
 * that shifts r2 BEFORE setting r0 (`mov r2,#0xae / lsl r2,#17 / mov r0,#0xa`)
 * where its three siblings shift last, and the block is written that way.
 *
 * Deleting all prototypes (the implicit-int lever) is inert on the plain form:
 * 207 lines, 194 differing against 195. The blocker was never fill order.
 *
 * No -O1 wildcard captures rom_78ef88 -- the tree default -O2 applies, and
 * tryc from a scratch path and objcmp from the asm/ path agree.
 */
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_896_2008d5c(void)
{
    unsigned char *p;

    __PlaySound(0x11);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 8; q2 <<= 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xf5; q0 = 0; q1 = 0xe7; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0x1e; q0 = 0; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0xb4);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x50);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 8; q2 <<= 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xf6; q2 = 0x1df;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0, 0xe0 << 8, 0xa);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x1eb;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q2 = 0x28; q0 = 1; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0, 2);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x50; q0 = 1; q1 = 0x101;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 4);
    { PIN3; q0 = 0; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x109; q2 = 0x1c5;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x8d; q2 = 0x1d5; q0 = 1; q1 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    { PIN3; q1 = 0xe0; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0, 6, 0);
    __MapActor_Jump(1, 6, 0x3c);
    { PIN3; q2 = 0xa6; q0 = 5; q1 = 0x1db0000; q2 <<= 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0xa6; q0 = 9; q1 = 0x1eb0000; q2 <<= 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0xae; q0 = 0xb; q1 = 0x1cb0000; q2 <<= 17;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q2 = 0xae; q2 <<= 17; q0 = 0xa; q1 = 0x1fb0000;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80933d4(0x73333, 0xe666);
    __Func_80933f8(0x1e50000, -1, 0x1590000, 1);
    { PIN3; q1 = 0xc0; q0 = 5; q1 <<= 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 9; q1 <<= 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0xb; q1 <<= 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0xa; q1 <<= 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093530();
    __CutsceneWait(0x28);
}
