// fakematch
/* OvlFunc_930_2008924  --  0x02008924
 *
 * Was the whole of goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_c.s,
 * so no split was needed.
 *
 * 154 instructions with THREE CROSSED SITES, exact on the first screen. All
 * three barriers were written into the first draft from the listing, because
 * `tools/crossed.py` reported `crossed-sites=3` during selection and the ROM
 * says exactly where each one goes.
 *
 * That is the whole entry, and it is a process result rather than a new lever:
 * a shape that cost two functions two rounds when batch 195 first met it, and
 * that batch 206 had to correct a rule to reach at all, is now something the
 * pre-filter locates and the first compile clears. Two of the three are the
 * two-register `mov r1 / mov r2 / lsl r2 / ... / lsl r1` form and take a
 * barrier after the first mov; the third is the four-argument
 * `__Func_80933f8` with its negation and shifts running against its movs, and
 * takes one after `q0`.
 *
 * Everything else came from siblings: r0 pins on the repeated flag id, the
 * four-statement accumulate for the byte flag whose constant the ROM leaves in
 * the destination, `(int)gScript_...` for the two behaviour scripts so one
 * declaration of the callee serves both it and the integer form, and a
 * callee-saved zero named as `register int z __asm__("r5")` because the ROM
 * spends a pushed register on it rather than materialising it at the store.
 */
extern unsigned char gScript_930__0200962c[];
extern unsigned char gScript_930__020096b8[];
extern void OvlFunc_930_2008054(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __MapActor_WaitScript(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093530(void);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_930_2008924(void)
{
    register int z __asm__("r5");

    if (__GetFlag(0x89a) != 0) {
        __CutsceneStart();
        { PIN3; q1 = 0x86; q2 = 0xd8; q1 <<= 18; q2 <<= 16; q0 = 0xa;
          __MapActor_SetPos(q0, q1, q2); }
        __MessageID(0x18b5);
        { PIN3; q2 = 0x14; q0 = 0xa; q1 = 0; __Func_8093040(q0, q1, q2); }
        { PIN2; q1 = 2; q0 = 0; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        *(void **)(__MapActor_GetActor(0) + 0x6c) = OvlFunc_930_2008054;
        if ((*(int *)(__MapActor_GetActor(0) + 0x10) >> 20) == 0xd) {
            PIN3; q1 = 0xdc; q0 = 0; q1 <<= 1; q2 = 0xc8;
            __Func_80921c4(q0, q1, q2);
        }
        {
            PIN3;
            q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
            q2 = 0x80; q2 <<= 9; q0 = 0xa; q1 <<= 10;
            __MapActor_SetSpeed(q0, q1, q2);
        }
        __Func_8092b08(0xa, 2);
        { PIN3; q1 = 0xcc; q1 <<= 1; q2 = 0xd8; q0 = 0xa;
          __Func_80921c4(q0, q1, q2); }
        {
            unsigned char *q = __MapActor_GetActor(0xa);
            int u, k;
            u = q[0x23]; k = 1; k |= u;
            q[0x23] = k;
        }
        __CutsceneWait(0xa);
        { PIN3; q1 = 0x80; q0 = 0xa; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q2 = 0x14; q0 = 0xa; q1 = 0; __Func_8093040(q0, q1, q2); }
        __Func_809259c(0xa, 2);
        { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0xa; __MapActor_Surprise(q0, q1); }
        __CutsceneWait(0x3c);
        { PIN3; q2 = 0x14; q0 = 0xa; q1 = 0; __Func_8093040(q0, q1, q2); }
        { PIN2; q1 = (int)gScript_930__0200962c; q0 = 0xa;
          __MapActor_SetBehavior(q0, q1); }
        {
            PIN4;
            q0 = 0x94; __asm__ volatile ("" : : "r" (q0));
            q1 = 1; q2 = 0xac; q3 = 1; q1 = -q1; q2 <<= 17; q0 <<= 17;
            __Func_80933f8(q0, q1, q2, q3);
        }
        __SetFlag(0x8b << 4);
        __MapActor_WaitScript(0xa);
        __Func_8093530();
        {
            PIN3;
            q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
            q2 = 0x80; q2 <<= 8; q0 = 0; q1 <<= 9;
            __MapActor_SetSpeed(q0, q1, q2);
        }
        { PIN2; q1 = (int)gScript_930__020096b8; q0 = 0;
          __MapActor_SetBehavior(q0, q1); }
        __MapActor_WaitScript(0);
        __CutsceneWait(0xa);
        z = 0;
        *(int *)(__MapActor_GetActor(0) + 0x6c) = z;
        __CutsceneWait(0x1e);
        { PIN2; q1 = 2; q0 = 0xa; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0xa0; q0 = 0xa; q1 <<= 7; q2 = 0x78;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0xa; q1 = 0x105; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q2 = 0x3c; q0 = 0; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
        { PIN2; q1 = 4; q0 = 0xa; __MapActor_DoAnim(q0, q1); }
        __CutsceneWait(0x14);
        __Func_8093040(0xa, 0, 0x14);
        __CutsceneEnd();
    }
}
