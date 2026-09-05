// fakematch
/* OvlFunc_932_2009398  --  0x02009398
 * [asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c.s, third of three]
 *
 * 283 instructions of cutscene. Byte-exact: 736 bytes, 291 encodings and 66
 * relocations identical.
 *
 * A PIN FUNCTION DESPITE A WIDE PUSH, decided by listing what the registers
 * hold rather than counting them. `push {r5, r6, lr}` keeps 0x1953 across
 * thirteen calls (the tell being `add r0, r5, #1` feeding __MessageID), then
 * recycles r5 for the 0xfe mask, with r6 carrying the constant 1. Both
 * callee-saved registers are spoken for by VALUES, so none is available to
 * hold a duplicated argument constant and every repeat is rebuilt.
 *
 * THE UNIFORM FILL DID ALMOST ALL OF IT IN ONE STEP -- 280 differing to 28.
 * One statement per argument, ascending, reproduced the ROM's non-ascending
 * emission orders for free: each large constant splits into mov+shift after
 * expand, so seeds sit at depth 2 and shifts at depth 1, and sched takes the
 * depth-2 class in argument order. No shift transcription and no barrier was
 * needed at any of the thirty fills.
 *
 * `int` RATHER THAN `unsigned char` FOR THE TWO BIT LOCALS was the largest
 * remaining step, 24 differing to 2. With `unsigned char` gcc allocated the
 * mask and the 1 to the ROM's own registers AND chose the wrong tied
 * destination at three of four and/orr sites; widening both fixed the
 * allocation and three destinations at once. The fourth needed
 * `register int bit __asm__("r6")`, the recorded commutative-orr idiom -- and
 * its companion two-statement spelling became INERT once the pin was present
 * and was dropped.
 *
 * Operand order on the commutative ops is fully inert here: gcc canonicalises,
 * and the destination is decided by liveness plus the pin, not by source order.
 *
 * ON LANDING THIS AS THE THIRD OF THREE: the screening note suggested all three
 * functions had to be elevated before the TU could convert, because the
 * function calls its same-file sibling OvlFunc_932_2008ec0. That is not so --
 * `.thumb_func_start` emits `.global` for every symbol it defines, so the
 * sibling is already exported and tools/split_s.py peels this function out on
 * its own. `make compare` after the split and before the .c confirmed it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __Func_801776c(int a, int b);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_932_2008ec0(int a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_932_2009398(void)
{
    unsigned char *p;
    register int bit __asm__("r6");
    register int m __asm__("r5");

    __CutsceneStart();
    do { } while (0);
    m = 0x1953;
    __Func_801776c(m, 1);
    if (__GetFlag(0x908) == 0 && __GetFlag(0xf14) == 0) {
        __SetFlag(0x205);
        { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x316; q2 = 0x8c; __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xc3 << 2; q2 = 0x8c; __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc8 << 2; q2 = 0x8c; __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __MessageID(m + 1);
        __MapActor_SetAnim(1, 4);
        __CutsceneWait(0x14);
        __Func_8093040(1, 0, 0xa);
        __MapActor_Jump(1, 6, 0);
        __MapActor_SetSpeed(1, 0x19999, 0xcccc);
        p = __MapActor_GetActor(1) + 0x5a;
        *p = 0xfe & *p;
        { PIN3; q0 = 1; q1 = 0xc6 << 2; q2 = 0x6e; __Func_80921c4(q0, q1, q2); }
        __CutsceneWait(1);
        p = __MapActor_GetActor(1) + 0x5a;
        bit = 1;
        *p = bit | *p;
        __PlaySound(0xa1);
        { PIN3; q0 = 0x80 << 10; q1 = 0x80 << 9; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
        p = __MapActor_GetActor(1) + 0x5a;
        *p = 0xfe & *p;
        __Func_80921c4(1, 0xc6 << 2, 0x78);
        __CutsceneWait(1);
        p = __MapActor_GetActor(1) + 0x5a;
        *p = bit | *p;
        { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
        __CutsceneWait(0x50);
        __PlaySound(0x8d);
        { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
        __CutsceneWait(0x28);
        { PIN3; q0 = 0; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(1, 0, 0x14);
        __Func_8092adc(0, 0, 0);
        { PIN3; q0 = 1; q1 = 0x80 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(1, 0, 0x28);
        { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __Func_809259c(1, 2);
        __Func_8093040(1, 0, 0xa);
        { PIN3; q0 = 1; q1 = 0xa0 << 10; q2 = 0xa0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(1, 5);
        { PIN3; q0 = 1; q1 = 0xc7 << 2; q2 = 0x8a; __Func_8092158(q0, q1, q2); }
        __Func_8092adc(0, 0, 0);
        { PIN3; q0 = 1; q1 = 0xc9 << 2; q2 = 0x8c; __Func_8092158(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xc9 << 2; q2 = 0xa6; __Func_8092158(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xbf << 2; q2 = 0xa6; __Func_8092158(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xbf << 2; q2 = 0xc6; __Func_8092158(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x312; q2 = 0xc6; __Func_8092158(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x312; q2 = 0xf6; __Func_8092158(q0, q1, q2); }
        __MapActor_SetAnim(1, 1);
        __MapActor_SetPos(1, 0, 0);
        __CutsceneWait(0x28);
        OvlFunc_932_2008ec0(0xa);
    }
    __CutsceneEnd();
}
