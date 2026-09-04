// fakematch
/* OvlFunc_888_2008360  --  0x02008360
 *
 * Was the whole of goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_c_a.s,
 * so no split was needed.
 *
 * 155 instructions, exact on the first screen, and the reason it went first
 * time is that the CONTROL FLOW was written as a single exit from the start.
 * The ROM branches to `bl __CutsceneEnd` from four places -- three early bail
 * -outs on a repeated dialogue check, plus the fall-through -- and batch 209
 * measured what happens when that is spelled with a teardown call per arm: a
 * length overshoot and a shifted label structure worth 86 instructions. Nesting
 * the three checks and putting one __CutsceneEnd at the bottom reproduces it
 * with no lever at all.
 *
 *     { PIN2; ... __Func_8092c40(q0, q1); }
 *     if (__Func_8091c7c(0, 0) == 0) { ...short arm... }
 *     else { ... if (...) { ... if (...) { ...body... } } }
 *     __CutsceneEnd();
 *
 * THE RANGE TEST IS UNSIGNED and has to be spelled that way. The ROM uses `bcc`
 * and `bhi` on a value loaded with `ldrh`; an `unsigned short` promotes to
 * `int`, so the natural comparison against `0xa0 << 8` is SIGNED and gcc emits
 * the signed branches. Casting the bound to `(unsigned)` gives the ROM's pair.
 * The actor is fetched separately for each half of the `&&`, as the ROM does.
 *
 * The two byte flags take the two forms already on file and they differ from
 * each other in this one function: `&= 0xfe` written directly, because the ROM
 * leaves the constant in the destination, and `|= 1` written as four statements
 * naming the accumulation's destination, because it does not.
 */
extern void OvlFunc_888_200987c(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091e9c(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80f95a0(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_888_2008360(void)
{
    __CutsceneStart();
    __MessageID(0x1164);
    { PIN2; q1 = 0; q0 = 8; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_DoAnim(8, 3);
        __CutsceneWait(0x14);
    } else {
        __CutsceneWait(0x14);
        { PIN2; q1 = 0; q0 = 8; __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x14);
            { PIN2; q1 = 0; q0 = 8; __Func_8092c40(q0, q1); }
            if (__Func_8091c7c(0, 0) == 0) {
                __CutsceneWait(0x14);
                if (*(unsigned short *)(__MapActor_GetActor(8) + 6)
                        >= (unsigned)(0xa0 << 8)
                    && *(unsigned short *)(__MapActor_GetActor(8) + 6)
                        <= (unsigned)(0xe0 << 8)) {
                    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 8; q2 <<= 7;
                      __MapActor_SetSpeed(q0, q1, q2); }
                    { PIN3; q1 = 0; q2 = 0; q0 = 8; __Func_8092adc(q0, q1, q2); }
                    __CutsceneWait(0xa);
                    __MapActor_GetActor(8)[0x5a] &= 0xfe;
                    { PIN3; q1 = 0x98; q2 = 0x78; q0 = 8;
                      __Func_80921c4(q0, q1, q2); }
                    __CutsceneWait(1);
                    {
                        unsigned char *q = __MapActor_GetActor(8);
                        int u, k;
                        u = q[0x5a]; k = 1; k |= u;
                        q[0x5a] = k;
                    }
                    __CutsceneWait(0x14);
                    { PIN2; q1 = 3; q0 = 8; __MapActor_DoAnim(q0, q1); }
                    __CutsceneWait(0x14);
                    __Func_80921c4(0, 0xa8, 0x78);
                    { PIN3; q1 = 0xc0; q2 = 0xa8; q0 = 0;
                      __Func_809218c(q0, q1, q2); }
                    __CutsceneWait(0x14);
                    __Func_80921c4(8, 0xa8, 0x78);
                    { PIN3; q1 = 0xc0; q0 = 8; q1 <<= 6; q2 = 0;
                      __Func_8092adc(q0, q1, q2); }
                    __MapActor_WaitMovement(0);
                } else {
                    { PIN3; q1 = 0xc0; q2 = 0xa8; q0 = 0;
                      __Func_809218c(q0, q1, q2); }
                    __CutsceneWait(0x14);
                    { PIN3; q1 = 0xc0; q0 = 8; q1 <<= 6; q2 = 0;
                      __Func_8092adc(q0, q1, q2); }
                    __MapActor_WaitMovement(0);
                }
                OvlFunc_888_200987c();
                { PIN2; q1 = 0; q0 = 0; __Func_8091200(q0, q1); }
                __Func_8091254(0x78);
                __CutsceneWait(0x78);
                __PlaySound(0x56);
                __Func_80f95a0();
                __SetFlag(0x9f << 4);
                __Func_8091e9c(0x1e);
            }
        }
    }
    __CutsceneEnd();
}
