// fakematch
/* OvlFunc_969_200cb28  --  0x0200cb28
 *
 * Cut out of goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c.s.
 *
 * Sixty-seven instructions, exact on the first screen. Two crossed sites, both
 * flagged by `tools/crossed.py` during selection and both anchored from the
 * listing before the first compile.
 *
 * The first is the three-register form and takes two barriers -- movs r0, r1,
 * r2 against shifts r2, r0, r1, so the two movs that need moving each get one.
 * The second is a four-argument fill where only r0 is out of place, and takes
 * one. That is the n-1 rule reading correctly in both directions: count the
 * movs whose position is wrong, not the arguments.
 *
 * `0x2015` is passed to two __Func_8093040 calls and the ROM reloads it from
 * the pool at each, so both sites take an r0 pin.
 */
extern void OvlFunc_969_2008894(int a);

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_Surprise(int slot, int n);
extern void __MessageID(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_969_200cb28(void)
{
    __MessageID(0x2829);
    OvlFunc_969_2008894(0x15);
    __PlaySound(0x3e);
    {
        PIN3;
        q0 = 0x80; __asm__ volatile ("" : : "r" (q0));
        q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
        q2 = 0x80; q2 <<= 9; q0 <<= 9; q1 <<= 9;
        __Func_8012330(q0, q1, q2);
    }
    __Func_80933d4(0x4cccc, 0x9999);
    { PIN2; q0 = 0x80; q1 = 0x80; q0 <<= 11; q1 <<= 8; __Func_80933d4(q0, q1); }
    {
        PIN4;
        q0 = 0xc0; __asm__ volatile ("" : : "r" (q0));
        q2 = 0xee; q3 = 1; q2 <<= 16; q1 = 0xffc00000; q0 <<= 16;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __Func_8093530();
    __CutsceneWait(0x28);
    __Func_80925cc(0x15, 1);
    { PIN3; q2 = 0x28; q0 = 0x2015; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 6; __Func_80925cc(q0, q1); }
    OvlFunc_969_2008894(6);
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0x15; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q2 = 0x50; q0 = 0x2015; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 6; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x28);
    __Func_809259c(6, 2);
    OvlFunc_969_2008894(6);
}
