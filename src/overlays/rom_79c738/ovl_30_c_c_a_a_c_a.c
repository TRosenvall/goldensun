// fakematch
/* OvlFunc_909_2008150  --  0x02008150
 *
 * From goldensun/asm/overlays/rom_79c738/ovl_30_c_c_a_a_c_a.s, which held this
 * function alone, so no split was needed.
 *
 * Member of shape group 0 at the widened 45-instruction cut.
 *
 * FOUR PINNED CALLS AND TWO PLAIN ONES, and the split is not by callee. The two
 * __Func_8093040 calls take gcc's natural order and are written plainly; the
 * neighbouring __Func_809280c, which has the same three-argument shape and
 * mostly the same values, is filled r1, r2, r0 and needs the pin. Two helpers
 * that look interchangeable, filled differently.
 *
 * The other pins are the ordinary interleave: `mov r0` inside r1's shift build
 * at __MapActor_Emote and __Func_8092adc, and a reversed two-argument fill at
 * __Func_80925cc.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2008150(void)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81;
        q2 = 0;
        q0 = 0xe;
        q1 <<= 1;
        __MapActor_Emote(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 2;
        q0 = 0xe;
        __Func_80925cc(q0, q1);
    }
    __CutsceneWait(0x28);
    __MessageID(0x1764);
    __Func_8093040(0xe, 0, 0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0;
        q2 = 0;
        q0 = 0xe;
        __Func_809280c(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    __Func_8093040(0xe, 0, 0xa);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xb0;
        q0 = 0xe;
        q1 <<= 8;
        q2 = 0xa;
        __Func_8092adc(q0, q1, q2);
    }
    __CutsceneEnd();
}
